#pragma once

//NOLINTBEGIN
#ifndef STATIC_ASSERT
  #define STATIC_ASSERT3(cond, msg) typedef char static_assertion_##msg[(!!(cond))*2-1]
  // NOTE(bill): Token pasting madness!!
  #define STATIC_ASSERT2(cond, line) STATIC_ASSERT3(cond, static_assertion_at_line_##line)
  #define STATIC_ASSERT1(cond, line) STATIC_ASSERT2(cond, line)
  #define STATIC_ASSERT(cond)        STATIC_ASSERT1(cond, __LINE__)
#endif
//NOLINTEND

#define nil 0
#define false 0
#define true 1

#define MAX_ALIGN 8

#define local_persist static
#define internal static

#define thread_local _Thread_local

#define diverging void

#define size_of(...) sizeof(__VA_ARGS__)
#define offset_of __builtin_offsetof
#define type_of(...) typeof(__VA_ARGS__)
#define count_of(...) (size_of(__VA_ARGS__) / size_of(*__VA_ARGS__))
#define align_of(...) __alignof__(__VA_ARGS__)

typedef __builtin_va_list va_list;
#define va_arg   __builtin_va_arg
#define va_start __builtin_va_start
#define va_end   __builtin_va_end

#define SWITCH(v) switch (v)
#define CASE    break; case
#define DEFAULT break; default

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
#define assert_msg(cond, msg)                                                  \
  {                                                                            \
    if (!(cond)) {                                                             \
      __write_location(CALLER_LOCATION);                                       \
      __write_cstring(" Assertion failure: (" #cond "): '" msg "'\n");         \
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

STATIC_ASSERT(size_of(i64) == 8);
STATIC_ASSERT(size_of(i32) == 4);
STATIC_ASSERT(size_of(i16) == 2);
STATIC_ASSERT(size_of(i8)  == 1);

typedef unsigned long int u64;
typedef unsigned int      u32;
typedef unsigned short    u16;
typedef unsigned char     u8;

STATIC_ASSERT(size_of(u64) == 8);
STATIC_ASSERT(size_of(u32) == 4);
STATIC_ASSERT(size_of(u16) == 2);
STATIC_ASSERT(size_of(u8)  == 1);

typedef u8 b8;
typedef u16 b16;
typedef u32 b32;
typedef u64 b64;

STATIC_ASSERT(size_of(b64) == 8);
STATIC_ASSERT(size_of(b32) == 4);
STATIC_ASSERT(size_of(b16) == 2);
STATIC_ASSERT(size_of(b8)  == 1);

typedef u64 usize;
typedef i64 isize;
typedef isize bsize;

STATIC_ASSERT(size_of(usize) == size_of(void *));
STATIC_ASSERT(size_of(isize) == size_of(void *));
STATIC_ASSERT(size_of(bsize) == size_of(void *));

typedef double f64;
typedef float f32;

STATIC_ASSERT(size_of(f64) == 8);
STATIC_ASSERT(size_of(f32) == 4);

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
  (String) {                                                                   \
    .data = str,                                                               \
    .len  = size_of(str) - 1,                                                  \
  }

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

extern rawptr memset(u8 *data, i32 c, isize n);

[[nodiscard]]
extern String format_usize_to_buffer(usize value, Byte_Slice buffer);

extern String format_usize_to_buffer_hex(usize value, Byte_Slice buffer, b8 uppercase);

#define slice_array(T, array) ({                                               \
   T slice;                                                                    \
   slice.data = &array[0];                                                     \
   slice.len = count_of(array);                                                \
   slice;                                                                      \
})

extern isize cstring_len(cstring s);
extern void __write_cstring(cstring str);
extern void __write_string(String str);
extern void __write_usize_hex(usize value);

extern void __write_usize(usize value);

extern void __write_isize(isize value);

extern void __write_location(Source_Code_Location location);

extern isize ilog2(isize x);

extern void __thread_init();
extern void __runtime_init();
extern void __thread_cleanup();
extern void __runtime_cleanup();

#include "result.h"

#include "enum.h"

#include "union.h"

#include "mem.h"

#include "slice.h"

#include "context.h"

#include "vector.h"

#include "hash_map.h"

i32 main();

i32 __codin_main(i32 arg_c, cstring *arg_v);

// should be `math.h` or smth
f64 sqrt_f64(f64 x);
f32 sqrt_f32(f32 x);
