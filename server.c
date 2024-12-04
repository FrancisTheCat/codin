#include "codin.h"

String content_dir;
Byte_Slice response_404;
Byte_Slice response_500;

void server_thread_proc(rawptr data) {
  Socket s = (Socket)data;

  String request = slice_make(String, 1024, context.temp_allocator);
  Reader socket_reader = reader_from_handle(s);
  isize n = or_goto(read_bytes(&socket_reader, slice_to_bytes(request)), error_500);

  Writer socket_writer = writer_from_handle(s);

  if (request.len <= LIT("GET ").len) {
    goto error_500;
  }
  if (!string_equal(slice_range(request, 0, 4), LIT("GET "))) {
    goto error_500;
  }
  isize method_len = string_index_byte(request, ' ') + 1;
  if (method_len < 1) {
    goto error_500;
  }

  String requested_file = request;
  requested_file.data += method_len;
  requested_file.len = string_index_byte(requested_file, ' ');

  if (requested_file.len < 0) {
    goto error_500;
  }

  requested_file = strings_concatenate(content_dir, requested_file,
                                       context.temp_allocator);

  if_let_err(file_open(requested_file, FP_Read), file, {
    File_Info info;
    file_stat(file, &info);
    if (info.is_dir) {
      if (requested_file.data[requested_file.len - 1] == '/') {
        requested_file = strings_concatenate(
            requested_file, LIT("index.html"), context.temp_allocator);
      } else {
        requested_file = strings_concatenate(
            requested_file, LIT("/index.html"), context.temp_allocator);
      }

      file_close(file);
      if_let_err(file_open(requested_file, FP_Read), index_file, {
        file = index_file;
      }, err, {
        goto error_404;
      });
    }

    log_infof(LIT("Requested File: '%S'"), requested_file);

    Byte_Slice file_response = or_goto_err(read_entire_file_fd(file, context.temp_allocator), error_500);

    String header = fmt_aprintf(
      context.temp_allocator,
      LIT("HTTP/1.0 200 OK\r\nContent-Length: %d\r\n\r\n"),
      file_response.len
    );
    log_infof(LIT("'%S'"), header);
    isize n1 = or_goto(write_string(&socket_writer, header), end);
    isize n2 = or_else(write_bytes(&socket_writer, file_response), -1);
    log_infof(LIT("Bytes written: %d, %d"), n1, n2);
  }, err, {
    goto error_404;
  });

end:
  socket_shutdown(s, Socket_Shutdown_Write);
  return;

error_404:
  write_bytes(&socket_writer, response_404);
  goto end;

error_500:
  write_bytes(&socket_writer, response_500);
  goto end;
}

void command_line_thread(rawptr data) {
  Byte_Slice buf = slice_make(Byte_Slice, 1024, context.allocator);

  b8 *should_exit = data;

  loop {
    if_let(read_bytes(&stdin, buf), n, {
      String input;
      input.data = (char*)buf.data;
      input.len = n - 1; // remove \n

      if (string_equal(input, LIT("quit"))) {
        log_info(LIT("Shutting down"));
        slice_delete(buf, context.allocator);
        *should_exit = true;
      }
      log_info(LIT("Type in 'quit' to shut down the server"));
    }, {
      log_error(LIT("Failed to read from command line, terminating command line thread"));
      slice_delete(buf, context.allocator);
      return;
    })
  }
}

int main(Arg_Slice args) {
  Tracking_Allocator track;
  context.allocator = tracking_allocator_init(&track, context.allocator);

  content_dir = LIT("public");
  response_404 = slice_to_bytes(LIT("Error 404"));
  response_500 = slice_to_bytes(LIT("Error 500"));

  if (args.len > 1) {
    content_dir = args.data[1];
    if_let_err(file_open(content_dir, FP_Read), dir, {
      File_Info info;
      OS_Error err = file_stat(dir, &info);
      if (err) {
        String error_message = fmt_aprintf(
            context.temp_allocator, LIT("Invalid content dir: %S (Error: %S)"),
            content_dir, err);
        log_error(error_message);
        return 1;
      } else if (!info.is_dir) {
        String error_message = fmt_aprintf(
            context.temp_allocator,
            LIT("Invalid content dir: %S (Not a directory)"), content_dir);
        log_error(error_message);
        return 1;
      }
    }, err, {
      String error_message = fmt_aprintf(
          context.temp_allocator, LIT("Invalid content dir: %S (Error: %S)"),
          content_dir, os_error_string(err));
      log_error(error_message);
      return 1;
    });
  }

  isize port = 25566;

  if (args.len > 2) {
    if_let(parse_isize(args.data[2]), custom_port, {
      if (port > 0 && port < 2 << 16) {
        port = custom_port;
      } else {
        log_error(strings_concatenate(LIT("Invalid port: %S"), args.data[2],
                                      context.temp_allocator));
        return 1;
      }
    },
    {
      log_error(strings_concatenate(LIT("Invalid port: %S"), args.data[2],
                                    context.temp_allocator));
      return 1;
    });
  }

  String info_string = fmt_tprintf(LIT("Starting Server on port %d with content dir '%S'"), port, content_dir);
  log_info(info_string);

  b8 should_exit = false;

  create_thread(command_line_thread, &should_exit);

  Socket s = socket_create(port);

  while(!should_exit) {
    if_let_err(socket_accept(s), connection, {
      create_thread(server_thread_proc, (rawptr)connection.socket);
    }, err, {
      log_error(net_error_string(err));
    });
  }

  socket_close(s);

  tracking_allocator_fmt_results_w(&stdout, &track);

  os_exit(0);

  unreachable();
}
