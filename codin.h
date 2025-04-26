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

#define MAX_ALIGN 8

#define local_persist static
#define internal      static

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

#define _MACRO_CONCAT_IMPL(x, y) x##y
#define MACRO_CONCAT(x, y) _MACRO_CONCAT_IMPL(x, y)

#define min(A, B) _min((A), (B), __COUNTER__)
#define _min(A, B, COUNTER)                                                    \
  ({                                                                           \
    type_of(A) MACRO_CONCAT(__A_, COUNTER) = (A);                              \
    type_of(B) MACRO_CONCAT(__B_, COUNTER) = (B);                              \
    MACRO_CONCAT(__A_, COUNTER) < MACRO_CONCAT(__B_, COUNTER) ?                \
    MACRO_CONCAT(__A_, COUNTER) : MACRO_CONCAT(__B_, COUNTER);                 \
  })

#define max(A, B) _max((A), (B), __COUNTER__)
#define _max(A, B, COUNTER)                                                    \
  ({                                                                           \
    type_of(A) MACRO_CONCAT(__A_, COUNTER) = (A);                              \
    type_of(B) MACRO_CONCAT(__B_, COUNTER) = (B);                              \
    MACRO_CONCAT(__A_, COUNTER) > MACRO_CONCAT(__B_, COUNTER) ?                \
    MACRO_CONCAT(__A_, COUNTER) : MACRO_CONCAT(__B_, COUNTER);                 \
  })

#define clamp(X, A, B) _clamp((X), (A), (B), __COUNTER__)
#define _clamp(X, A, B, COUNTER)                                               \
  ({                                                                           \
    type_of(X) MACRO_CONCAT(__X_, COUNTER) = (X);                              \
    type_of(A) MACRO_CONCAT(__A_, COUNTER) = (A);                              \
    type_of(B) MACRO_CONCAT(__B_, COUNTER) = (B);                              \
                                                                               \
    assert(MACRO_CONCAT(__A_, COUNTER) < MACRO_CONCAT(__B_, COUNTER))          \
                                                                               \
    if (MACRO_CONCAT(__X_, COUNTER) < MACRO_CONCAT(__A_, COUNTER)) {           \
      MACRO_CONCAT(__X_, COUNTER) = MACRO_CONCAT(__A_, COUNTER);               \
    }                                                                          \
    if (MACRO_CONCAT(__X_, COUNTER) > MACRO_CONCAT(__B_, COUNTER)) {           \
      MACRO_CONCAT(__X_, COUNTER) = MACRO_CONCAT(__B_, COUNTER);               \
    }                                                                          \
                                                                               \
    MACRO_CONCAT(__X_, COUNTER);                                               \
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

#define endianness_swap(x) _endianness_swap(x, __COUNTER__)
#define _endianness_swap(_x, COUNTER)                                          \
  ({                                                                           \
    union {                                                                    \
      type_of(_x) value;                                                       \
      byte        bytes[size_of(_x)];                                          \
    } MACRO_CONCAT(__x_, COUNTER);                                             \
    MACRO_CONCAT(__x_, COUNTER).value = (_x);                                  \
                                                                               \
    for (int i = 0; i < (size_of(MACRO_CONCAT(__x_, COUNTER)) >> 1); i += 1) { \
      byte tmp = MACRO_CONCAT(__x_, COUNTER).bytes[i];                         \
      MACRO_CONCAT(__x_, COUNTER).bytes[i] =                                   \
          MACRO_CONCAT(__x_, COUNTER)                                          \
              .bytes[size_of(MACRO_CONCAT(__x_, COUNTER)) - 1 - i];            \
      MACRO_CONCAT(__x_, COUNTER)                                              \
          .bytes[size_of(MACRO_CONCAT(__x_, COUNTER)) - 1 - i] = tmp;          \
    }                                                                          \
                                                                               \
    MACRO_CONCAT(__x_, COUNTER).value;                                         \
  })

typedef signed long int i64;
typedef signed int      i32;
typedef signed short    i16;
typedef signed char     i8;

#define I64_MIN -9223372036854775808ll
#define I64_MAX  9223372036854775807ll

#define I32_MIN -2147483648
#define I32_MAX  2147483647

#define I16_MIN -32768
#define I16_MAX  32767

#define I8_MIN -128
#define I8_MAX  127

STATIC_ASSERT(size_of(i64) == 8);
STATIC_ASSERT(size_of(i32) == 4);
STATIC_ASSERT(size_of(i16) == 2);
STATIC_ASSERT(size_of(i8)  == 1);

typedef unsigned long int u64;
typedef unsigned int      u32;
typedef unsigned short    u16;
typedef unsigned char     u8;

#define U64_MAX  18446744073709551615ll
#define U32_MAX  4294967295
#define U16_MAX  65535
#define U8_MAX   255

STATIC_ASSERT(size_of(u64) == 8);
STATIC_ASSERT(size_of(u32) == 4);
STATIC_ASSERT(size_of(u16) == 2);
STATIC_ASSERT(size_of(u8)  == 1);

#if !defined(__cplusplus)
  #if !defined(__STDC_VERSION__) || (__STDC_VERSION__ < 202000L)
    typedef _Bool bool;
    #define true 1
    #define false 0
  #endif
#endif

typedef bool b8;
typedef u16  b16;
typedef u32  b32;
typedef u64  b64;

STATIC_ASSERT(size_of(b64) == 8);
STATIC_ASSERT(size_of(b32) == 4);
STATIC_ASSERT(size_of(b16) == 2);
STATIC_ASSERT(size_of(b8)  == 1);

typedef u64   usize;
typedef i64   isize;
typedef usize bsize;

STATIC_ASSERT(size_of(usize) == size_of(void *));
STATIC_ASSERT(size_of(isize) == size_of(void *));
STATIC_ASSERT(size_of(bsize) == size_of(void *));

typedef float  f32;
typedef double f64;

#define F32_INFINITY __builtin_inff()
#define F64_INFINITY __builtin_inf()

STATIC_ASSERT(size_of(f64) == 8);
STATIC_ASSERT(size_of(f32) == 4);

typedef u8 byte;

typedef i32 rune;

typedef usize uintptr;
typedef void *rawptr;
typedef const char *cstring;
typedef uintptr Fd;
typedef uintptr Pid;
typedef uintptr Tid;

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
typedef Slice(String) String_Slice;

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

extern rawptr memset(rawptr data, i32 c, usize n);

[[nodiscard]]
extern String format_usize_to_buffer(usize value, Byte_Slice buffer);
[[nodiscard]]
extern String format_usize_to_buffer_hex(usize value, Byte_Slice buffer, bool uppercase);

#define slice_array(T, array) ((T) {                                           \
 .data = &array[0],                                                            \
 .len  = count_of(array),                                                      \
})

extern void __write_cstring(cstring str);
extern void __write_string(String str);
extern void __write_usize(usize value);
extern void __write_usize_hex(usize value);
extern void __write_isize(isize value);
extern void __write_location(Source_Code_Location location);

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
