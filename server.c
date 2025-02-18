#include "codin.h"

String content_dir;
Byte_Slice response_404;
Byte_Slice response_500;

void server_thread_proc(rawptr data) {
  Socket s             = (Socket)data;
  String request       = alloca_slice(String, 1024);
  Reader socket_reader = reader_from_socket(s);
  Writer socket_writer = writer_from_socket(s);
  isize  n             = or_goto(read_bytes(&socket_reader, slice_to_bytes(request)), error_500);

  if (request.len <= LIT("GET ").len) {
    goto error_500;
  }
  if (!string_equal(slice_range(request, 0, 4), LIT("GET "))) {
    goto error_500;
  }
  isize method_len = string_index_byte(request, ' ');
  if (method_len <= 0) {
    goto error_500;
  }

  String requested_file = slice_start(request, method_len + 1);
  requested_file        = slice_end(requested_file, string_index_byte(requested_file, ' '));
  if (requested_file.len <= 0) {
    goto error_500;
  }

  requested_file = strings_concatenate(content_dir, requested_file, context.temp_allocator);
  
  if_let_err(file_open(requested_file, FP_Read), file, {
    File_Info info;
    file_stat(file, &info);
    if (info.is_dir) {
      if (*IDX(requested_file, requested_file.len - 1) == '/') {
        requested_file = slice_end(requested_file, requested_file.len - 1);
      }
      requested_file = strings_concatenate(requested_file, LIT("/index.html"), context.temp_allocator);

      file_close(file);
      if_let_err(file_open(requested_file, FP_Read), index_file, {
        file = index_file;
      }, err, {
        goto error_404;
      });
    }

    // log_infof(LIT("Requested File: '%S'"), requested_file);

    Byte_Slice file_response = or_goto_err(read_entire_file_fd(file, context.temp_allocator), error_500);

    String header = fmt_tprintf(
      LIT("HTTP/1.0 200 OK\r\nContent-Length: %d\r\n\r\n"),
      file_response.len
    );
    // log_infof(LIT("'%S'"), header);
    isize n1 = or_goto(write_string(&socket_writer, header), end);
    isize n2 = or_else(write_bytes(&socket_writer, file_response), -1);
    // log_infof(LIT("Bytes written: %d, %d"), n1, n2);
  }, err, {
    goto error_404;
  });

end:
  socket_shutdown(s, Socket_Shutdown_Write);
  return;

error_404:
  // log_info(LIT("error_404"));
  write_bytes(&socket_writer, response_404);
  goto end;

error_500:
  // log_info(LIT("error_500"));
  write_bytes(&socket_writer, response_500);
  goto end;
}

void command_line_thread(rawptr data) {
  Byte_Slice buf = alloca_slice(Byte_Slice, 1024);

  b8 volatile *should_exit = data;

  loop {
    if_let(read_bytes(&stdin, buf), n, {
      String input;
      input.data = (char*)buf.data;
      input.len  = n - 1; // remove \n

      if (string_equal(input, LIT("quit"))) {
        log_info(LIT("Shutting down"));
        *should_exit = true;
        break;
      }
      log_info(LIT("Type in 'quit' to shut down the server"));
    }, {
      log_error(LIT("Failed to read from command line, terminating command line thread"));
      return;
    })
  }
}

int main() {
  Tracking_Allocator track = {0};
  context.allocator = tracking_allocator_init(&track, context.allocator);

  content_dir  = LIT("public");
  response_404 = string_to_bytes(LIT("HTTP/1.0 200 OK\r\nContent-Length: 9\r\n\r\nError 404"));
  response_500 = string_to_bytes(LIT("HTTP/1.0 200 OK\r\nContent-Length: 9\r\n\r\nError 500"));

  if (os_args.len > 1) {
    content_dir = *IDX(os_args, 1);
    if_let_err(file_open(content_dir, FP_Read), dir, {
      File_Info info;
      OS_Error err = file_stat(dir, &info);
      if (err) {
        log_errorf(
          LIT("Invalid content dir: %S (Error: %S)"),
          content_dir,
          enum_to_string(OS_Error, err)
        );
        return 1;
      } else if (!info.is_dir) {
        log_errorf(LIT("Invalid content dir: %S (Not a directory)"), content_dir);
        return 1;
      }
    }, err, {
      log_errorf(
        LIT("Invalid content dir: %S (Error: %S)"),
        content_dir,
        enum_to_string(OS_Error, err)
      );
    });
  }

  isize port = 25566;

  if (os_args.len > 2) {
    if_let(parse_isize(*IDX(os_args, 2)), custom_port, {
      if (port > 0 && (port < 1 << 16)) {
        port = custom_port;
      } else {
        log_error(strings_concatenate(LIT("Invalid port: %S"), *IDX(os_args, 2), context.temp_allocator));
        return 1;
      }
    }, {
      log_error(strings_concatenate(LIT("Invalid port: %S"), *IDX(os_args, 2), context.temp_allocator));
      return 1;
    });
  }

  log_infof(LIT("Starting Server on port %d with content dir '%S'"), port, content_dir);

  volatile b8 should_exit = false;
  thread_create(command_line_thread, (rawptr)&should_exit);

  Socket s = unwrap_err_msg(socket_create(port, false), "failed to create socket");

  while (!should_exit) {
    if_let_err(socket_accept(s), connection, {
      thread_create(server_thread_proc, (rawptr)connection.socket);
      // server_thread_proc((rawptr)connection.socket);
    }, err, {
      if (err == NE_Would_Block) {
        time_sleep(5 * Millisecond);
        continue;
      }
      log_error(enum_to_string(Net_Error, err));
    });
  }

  socket_close(s);
  tracking_allocator_fmt_results_w(&stdout, &track);
  os_exit(0);
}
