#include "codin.h"

#include "fmt.h"
#include "image.h"
#include "os.h"
#include "spall.h"
#include "strings.h"
#include "time.h"

#define SPALL_PROFILING

#ifdef SPALL_PROFILING
  thread_local SpallBuffer  spall_buffer;
  thread_local SpallProfile spall_ctx;

  internal f64 get_time_in_nanos() {
    return time_now();
  }
#else
  #define spall_buffer_begin(...)
  #define spall_buffer_end(...)
#endif

#define spall_begin(name) spall_buffer_begin(&spall_ctx, &spall_buffer, LIT(name),         get_time_in_nanos())
#define spall_begin_fn()  spall_buffer_begin(&spall_ctx, &spall_buffer, LIT(__FUNCTION__), get_time_in_nanos())
#define spall_end()       spall_buffer_end(  &spall_ctx, &spall_buffer,                    get_time_in_nanos())

#include "qr.h"

b8 spall_write_callback(SpallProfile *self, const void *data, isize length) {
  return file_write((Fd)self->data, (Byte_Slice) {.data = (byte *)data, .len = length}).err == OSE_None;
}

void spall_close_callback(SpallProfile *self) {
  file_close((Fd)self->data);
}

i32 main() {
#ifdef SPALL_PROFILING
  Fd spall_fd = unwrap_err(file_open(LIT("trace.spall"), FP_Create | FP_Read_Write | FP_Truncate));
  spall_ctx   = spall_init_callbacks(0.001, spall_write_callback, nil, spall_close_callback, (rawptr)spall_fd);
  Byte_Slice spall_buffer_backing = slice_make(Byte_Slice, 1024 * 4, context.temp_allocator);
  spall_buffer = (SpallBuffer) {
    .length = spall_buffer_backing.len,
    .data   = spall_buffer_backing.data,
  };
  spall_buffer_init(&spall_ctx, &spall_buffer);
#endif

  if (os_args.len < 2) {
    fmt_eprintlnc("Usage:");
    fmt_eprintlnc("\tFile:  qr_gen 'file' > qr_code.png");
    fmt_eprintlnc("\tValue: qr_gen -v 'value' > qr_code.png");
    return 1;
  }

  Byte_Slice data = {0};

  if (os_args.len == 2) {
    data = unwrap_err_msg(read_entire_file_path(IDX(os_args, 1), context.allocator), "Failed to read input file");
  } else {
    if (!string_equal(IDX(os_args, 1), LIT("-v"))) {
      fmt_eprintlnc("Usage:");
      fmt_eprintlnc("\tFile:  qr_gen 'file' > qr_code.png");
      fmt_eprintlnc("\tValue: qr_gen -v 'value' > qr_code.png");
      return 1;
    }

    data = slice_to_bytes(IDX(os_args, 2));
  }

  data = slice_to_bytes(LIT("Hello World"));

  Image image;

  spall_begin("qr_code_generate_image");
  b8 ok = qr_code_generate_image(
    data,
    &image,
    QR_Error_Correction_L,
    QR_MASK_AUTOMATIC,
    context.allocator
  );
  spall_end();

  if (!ok) {
    fmt_eprintln(LIT("Failed to generate QR-Code"));
    return 1;
  }

  spall_begin("save_png");
  png_save_writer(&std_out, &image);
  spall_end();

#ifdef SPALL_PROFILING
  spall_buffer_quit(&spall_ctx, &spall_buffer);
  spall_quit(&spall_ctx);
#endif
}
