#pragma once

#include "runtime_linux.h"

#ifndef GB_STATIC_ASSERT
	#define GB_STATIC_ASSERT3(cond, msg) typedef char static_assertion_##msg[(!!(cond))*2-1]
	// NOTE(bill): Token pasting madness!!
	#define GB_STATIC_ASSERT2(cond, line) GB_STATIC_ASSERT3(cond, static_assertion_at_line_##line)
	#define GB_STATIC_ASSERT1(cond, line) GB_STATIC_ASSERT2(cond, line)
	#define GB_STATIC_ASSERT(cond)        GB_STATIC_ASSERT1(cond, __LINE__)
#endif

#define nil 0
#define false 0
#define true 1

#define MAX_ALIGN 8

#define local_persist static
#define internal static

#define thread_local __thread

#define diverging void

#define size_of(...) sizeof(__VA_ARGS__)
#define type_of(...) typeof(__VA_ARGS__)
#define count_of(...) (size_of(__VA_ARGS__) / size_of(*__VA_ARGS__))
#define align_of(...) __alignof__(__VA_ARGS__)

#define min(_A, _B)                                                            \
  ({                                                                           \
    type_of(_A) __A = (_A);                                                    \
    type_of(_B) __B = (_B);                                                    \
    __A < __B ? __A : __B;                                                     \
  })
#define max(_A, _B)                                                            \
  ({                                                                           \
    type_of(_A) __A = (_A);                                                    \
    type_of(_B) __B = (_B);                                                    \
    __A > __B ? __A : __B;                                                     \
  })
#define trap() __builtin_trap()
#define panic(message)                                                         \
  {                                                                            \
    __write_location(CALLER_LOCATION);                                         \
    __write_cstring(" Panic: '");                                              \
    __write_cstring(message);                                                  \
    __write_cstring("'\n");                                                    \
    trap();                                                                    \
  }
#define assert(cond)                                                           \
  {                                                                            \
    if (!(cond)) {                                                             \
      __write_location(CALLER_LOCATION);                                       \
      __write_cstring(" Assertion failure: (");                                \
      __write_cstring(#cond);                                                  \
      __write_cstring(")\n");                                                  \
      trap();                                                                  \
    }                                                                          \
  }
#define unreachable()                                                          \
  {                                                                            \
    __write_location(CALLER_LOCATION);                                         \
    __write_cstring(" Unreachable!\n");                                        \
    trap();                                                                    \
  }
#define unimplemented()                                                        \
  {                                                                            \
    __write_location(CALLER_LOCATION);                                         \
    __write_cstring(" Unimplemented!\n");                                      \
    trap();                                                                    \
  }

#define transmute(T, V) (*((T *)&(V)))

#define for_range(i, start, end) for (isize i = start; i < end; i += 1)
#define in_range(_v, start, end)                                               \
  ({                                                                           \
    type_of(_v) v = _v;                                                        \
    ((start <= v) && (v < end));                                               \
  })
#define loop while (1)

#define alloca(size) __builtin_alloca(size)

#define endianness_swap(_x)                                                    \
({                                                                             \
  union {                                                                      \
    type_of(_x) value;                                                         \
    byte bytes[size_of(_x)];                                                   \
  } x;                                                                         \
  x.value = (_x);                                                              \
                                                                               \
  for (int i = 0; i < (size_of(x) >> 1); i += 1) {                             \
    byte tmp = x.bytes[i];                                                     \
    x.bytes[i] = x.bytes[size_of(x) - 1 - i];                                  \
    x.bytes[size_of(x) - 1 - i] = tmp;                                         \
  }                                                                            \
                                                                               \
  x.value;                                                                     \
})

typedef signed long int i64;
typedef signed int      i32;
typedef signed short    i16;
typedef signed char     i8;

GB_STATIC_ASSERT(size_of(i64) == 8);
GB_STATIC_ASSERT(size_of(i32) == 4);
GB_STATIC_ASSERT(size_of(i16) == 2);
GB_STATIC_ASSERT(size_of(i8)  == 1);

typedef unsigned long int u64;
typedef unsigned int      u32;
typedef unsigned short    u16;
typedef unsigned char     u8;

GB_STATIC_ASSERT(size_of(u64) == 8);
GB_STATIC_ASSERT(size_of(u32) == 4);
GB_STATIC_ASSERT(size_of(u16) == 2);
GB_STATIC_ASSERT(size_of(u8)  == 1);

typedef u8 b8;
typedef u16 b16;
typedef u32 b32;
typedef u64 b64;

GB_STATIC_ASSERT(size_of(b64) == 8);
GB_STATIC_ASSERT(size_of(b32) == 4);
GB_STATIC_ASSERT(size_of(b16) == 2);
GB_STATIC_ASSERT(size_of(b8)  == 1);

typedef u64 usize;
typedef i64 isize;
typedef isize bsize;

GB_STATIC_ASSERT(size_of(usize) == size_of(void *));
GB_STATIC_ASSERT(size_of(isize) == size_of(void *));
GB_STATIC_ASSERT(size_of(bsize) == size_of(void *));

typedef double f64;
typedef float f32;

GB_STATIC_ASSERT(size_of(f64) == 8);
GB_STATIC_ASSERT(size_of(f32) == 4);

typedef unsigned char byte;

typedef i32 rune;

typedef usize uintptr;
typedef void *rawptr;
typedef const char *cstring;
typedef uintptr Fd;
typedef uintptr Pid;
typedef uintptr Tid;

u64 __get_fs_register();

#define Slice(T)                                                               \
  struct {                                                                     \
    T *data;                                                                   \
    isize len;                                                                 \
  }

typedef Slice(byte) Byte_Slice;
typedef Slice(void) Void_Slice;

#define LIT(str)                                                               \
  ({                                                                           \
    String s;                                                                  \
    s.data = str;                                                              \
    s.len = (sizeof str) - 1;                                                  \
    s;                                                                         \
  })

typedef Slice(const char) String;

typedef struct {
  String file;
  String proc;
  isize  line;
} Source_Code_Location;

#define CALLER_LOCATION                                                        \
  ({                                                                           \
    Source_Code_Location s;                                                    \
    s.file = LIT(__FILE__);                                                    \
    s.line = __LINE__;                                                         \
    s.proc = LIT(__func__);                                                    \
    s;                                                                         \
  })

internal String format_usize_to_buffer(usize value, Byte_Slice buffer) {
    isize is_negative = 0;
    isize i = 0;

    do {
        buffer.data[i] = (value % 10) + '0';
        i += 1;
        value /= 10;
    } while (value > 0);

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
      .len = i,
    };
}

internal String format_usize_to_buffer_hex(usize value, Byte_Slice buffer, b8 uppercase) {
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

#define slice_array(T, array) ({                                               \
   T slice;                                                                    \
   slice.data = &array[0];                                                     \
   slice.len = count_of(array);                                                \
   slice;                                                                      \
})

internal isize cstring_len(cstring s);
internal void __write_cstring(cstring str) { syscall(1, 2, str, cstring_len(str)); }
internal void __write_string(String str) { syscall(1, 2, str.data, str.len); }
internal void __write_usize_hex(usize value) {
#define N 32
  byte buf[N] = {0};
  String str = format_usize_to_buffer_hex(value, slice_array(Byte_Slice, buf), true);
  __write_string(str);
#undef N
}
internal void __write_usize(usize value) {
#define N 32
  byte buf[N] = {0};
  String str = format_usize_to_buffer(value, slice_array(Byte_Slice, buf));
  __write_string(str);
#undef N
}
internal void __write_location(Source_Code_Location location) {
  __write_string(location.file);
  __write_cstring(":");
  __write_string(location.proc);
  __write_cstring("(");
  __write_usize(location.line);
  __write_cstring(")");
}

internal isize ilog2(isize x) {
  isize v = 1;
  for (; (1 << v) < x; v += 1) {
  }
  return v;
}

internal void __thread_init();
internal void __runtime_init();
internal void __thread_cleanup();
internal void __runtime_cleanup();

#include "result.h"

#include "mem.h"

#include "slice.h"

#include "strings.h"

#include "path.h"

#include "context.h"

#include "vector.h"

#include "ring_buffer.h"

#include "hash_map.h"

#include "time_.h"

#include "sort.h"

#include "allocators.h"

#include "io.h"

#include "os.h"

#include "thread.h"

#include "fmt.h"

#include "log.h"

#include "test.h"

#include "net.h"

#include "dynlib.h"

internal _Thread_local Growing_Arena_Allocator __default_temp_allocator_arena;
internal _Thread_local Default_Allocator       __default_heap_allocator;

internal void __thread_init() {
  context.allocator = default_allocator_init(&__default_heap_allocator);
  context.temp_allocator = growing_arena_allocator_init(
      &__default_temp_allocator_arena, 1 << 12, context.allocator);
  context.logger = create_file_logger(1);
}

internal void __runtime_init() {
  __thread_init();
  __os_init();
}

internal void __thread_cleanup() {
  growing_arena_allocator_destroy(__default_temp_allocator_arena);
}

internal void __runtime_cleanup() { __thread_cleanup(); }

typedef Slice(String) Arg_Slice;

int main(Arg_Slice);

int __codin_main(i32 __arg_c, cstring *__arg_v) {
    __runtime_init();
    Arg_Slice __args = slice_make(Arg_Slice, __arg_c, context.allocator);
    slice_iter(__args, __arg, i, {
      *__arg = cstring_to_string(__arg_v[i]);
    });
    CONTEXT_GUARD({
      main(__args);
    })
    __runtime_cleanup();
}
