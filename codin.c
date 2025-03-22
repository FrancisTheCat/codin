#include "codin.h"

#include "allocators.h"
#include "log.h"
#include "os.h"
#include "strings.h"

thread_local Growing_Arena_Allocator _default_temp_allocator_arena;
thread_local Default_Allocator       _default_heap_allocator;

extern rawptr memset(rawptr data, i32 c, usize n) {
  for_range(i, 0, n) {
    ((u8 *)data)[i] = (u8)c;
  }
  return data;
}

[[nodiscard]]
extern String format_usize_to_buffer(usize value, Byte_Slice buffer) {
  isize i = 0;

  do {
    buffer.data[i] = (value % 10) + '0';
    i     += 1;
    value /= 10;
  } while (value > 0);

  isize start = 0;
  isize end   = i - 1;
  while (start < end) {
    char temp = buffer.data[start];
    buffer.data[start] = buffer.data[end];
    buffer.data[end  ] = temp;
    start++;
    end--;
  }

  buffer.data[i] = 0;

  return (String) {
    .data = (char *)buffer.data,
    .len  = i,
  };
}

extern String format_usize_to_buffer_hex(usize value, Byte_Slice buffer, b8 uppercase) {
  isize i = 0;

  if (value == 0) {
    buffer.data[0] = '0';
    i += 1;
  }

  while (value > 0) {
    isize remainder = value % 16;

    buffer.data[i] = (remainder < 10) ? (remainder + '0') : (remainder - 10 + (uppercase ? 'A': 'a'));
    i += 1;

    value /= 16;
  }

  buffer.data[i] = 0;

  // Reverse the buffer to get the final result
  isize start = 0;
  isize end = i - 1;
  while (start < end) {
    char temp = buffer.data[start];
    buffer.data[start] = buffer.data[end];
    buffer.data[end] = temp;
    start++;
    end--;
  }

  return (String) {
    .data = (char *)buffer.data,
    .len  = i
  };
}

extern void __write_usize_hex(usize value) {
  byte buf[32] = {0};
  String str = format_usize_to_buffer_hex(value, slice_array(Byte_Slice, buf), true);
  __write_string(str);
}

extern void __write_usize(usize value) {
  byte buf[32] = {0};
  String str = format_usize_to_buffer(value, slice_array(Byte_Slice, buf));
  __write_string(str);
}

extern void __write_isize(isize value) {
  byte buf[33] = {0};
  Byte_Slice slice = slice_array(Byte_Slice, buf);
  Byte_Slice buf_ = slice;
  if (value < 0) {
    buf[0] = '-';
    buf_.data += 1;
    buf_.len  -= 1;
    value *= -1;
  }
  (void)format_usize_to_buffer(value, buf_);
  __write_cstring((cstring)buf);
}

extern void __write_location(Source_Code_Location location) {
  __write_string(location.file);
  __write_cstring(":");
  __write_string(location.proc);
  __write_cstring("(");
  __write_usize(location.line);
  __write_cstring(")");
}

extern void __thread_init();
extern void __runtime_init();
extern void __thread_cleanup();
extern void __runtime_cleanup();

extern void __thread_init() {
  context.allocator      = default_allocator_init(&_default_heap_allocator);
  context.temp_allocator = growing_arena_allocator_init(&_default_temp_allocator_arena, 1 << 20, context.allocator);
  context.logger         = create_file_logger(FD_STDERR);
}

extern void __runtime_init() {
  __thread_init();
  __os_init();
}

extern void __thread_cleanup() {
  growing_arena_allocator_destroy(_default_temp_allocator_arena);
}

extern void __runtime_cleanup() { __thread_cleanup(); }

extern i32 main(String_Slice args, String_Slice env);

i32 __codin_main(i32 arg_c, cstring *arg_v) {
  __runtime_init();
  slice_init(&os_args, arg_c, context.allocator);
  slice_iter(os_args, arg, i, {
    *arg = cstring_to_string(arg_v[i]);
  });
  for (cstring *e = &arg_v[arg_c + 1]; *e; e += 1) {
    os_env.len += 1;
  }
  slice_init(&os_env, os_env.len, context.allocator);
  slice_iter(os_env, e, i, {
    *e = cstring_to_string(arg_v[arg_c + 1 + i]);
  });

  i32 ret;
  CONTEXT_GUARD({
    ret = main(os_args, os_env);
  })
  __runtime_cleanup();

  return ret;
}
