#pragma once

#include "codin.h"

#define ALLOCATOR_ERRORS(X)                                                    \
  X(AE_None)                                                                   \
  X(AE_Out_Of_Memory)                                                          \
  X(AE_Invalid_Arguments)                                                      \
  X(AE_Unimplemented)

X_ENUM(Allocator_Error, ALLOCATOR_ERRORS)

typedef Result(rawptr, Allocator_Error) Allocator_Result;

#define ALLOCATOR_MODES(X)                                                      \
  X(AM_Alloc)                                                                  \
  X(AM_Free)                                                                   \
  X(AM_Free_All)                                                               \

X_ENUM(Allocator_Mode, ALLOCATOR_MODES)

typedef Allocator_Result (*Allocator_Proc)(
  rawptr               data,
  Allocator_Mode       mode,
  isize                size,
  isize                align,
  rawptr               old_memory,
  Source_Code_Location loc
);

typedef struct {
  Allocator_Proc proc;
  rawptr         data;
} Allocator;

#define Kilobyte (1000)
#define Megabyte (1000 * Kilobyte)
#define Gigabyte (1000 * Megabyte)
#define Terabyte (1000 * Gigabyte)
#define Petabyte (1000 * Terrabyte)

#define Kibibyte (1024)
#define Mebibyte (1024 * Kibibyte)
#define Gibibyte (1024 * Mebibyte)
#define Tebibyte (1024 * Gibibyte)
#define Pebibyte (1024 * Tebibyte)

#define mem_tcopy(dst, src, len)                                               \
  ({                                                                           \
    STATIC_ASSERT(sizeof((dst)[0]) == sizeof((src)[0]));                       \
    mem_copy((rawptr)(dst), (rawptr)(src), len * size_of((dst)[0]));           \
  })

extern void  mem_zero(rawptr data, isize len);
extern void  mem_copy(rawptr dst, rawptr src, isize len);
extern bool  mem_compare(rawptr a, rawptr b, isize len);
extern isize required_align_offset(uintptr ptr, isize align);

#define mem_tnew(T, allocator)                                                 \
  (T*)unwrap_err(mem_alloc_aligned(size_of(T), align_of(T), (allocator)))

#define mem_alloc_aligned(size, align, allocator)                              \
  _mem_alloc_aligned(size, align, allocator, CALLER_LOCATION)
extern Allocator_Result _mem_alloc_aligned(
  isize                size,
  isize                align,
  Allocator            allocator,
  Source_Code_Location location
);

#define mem_alloc(size, allocator) _mem_alloc(size, allocator, CALLER_LOCATION)
extern Allocator_Result _mem_alloc(isize size, Allocator allocator, Source_Code_Location location);

#define mem_tclone(value, allocator)                                           \
  (type_of(*(value)) *)mem_clone(value, size_of(*(value)), allocator)

#define mem_clone(data, size, allocator)                                       \
  _mem_clone(data, size, allocator, CALLER_LOCATION)
extern Allocator_Result _mem_clone(
  rawptr               data,
  isize                size,
  Allocator            allocator,
  Source_Code_Location location
);

#define mem_tfree(ptr, allocator)                                              \
  _mem_free(ptr, sizeof((ptr)[0]), allocator, CALLER_LOCATION)

#define mem_free(ptr, size, allocator)                                         \
  _mem_free(ptr, size, allocator, CALLER_LOCATION)
extern Allocator_Error _mem_free(
  rawptr               ptr,
  isize                size,
  Allocator            allocator,
  Source_Code_Location location
);

#define mem_resize_aligned(data, old_size, new_size, align, allocator)         \
  _mem_resize_aligned(data, old_size, new_size, align, allocator, CALLER_LOCATION)
extern Allocator_Error _mem_resize_aligned(
  rawptr              *data,
  isize                old_size,
  isize                new_size,
  isize                align,
  Allocator            allocator,
  Source_Code_Location location
);

#define mem_resize(data, old_size, new_size, allocator)                        \
  _mem_resize(data, old_size, new_size, allocator, CALLER_LOCATION)
extern Allocator_Error _mem_resize(
  rawptr              *data,
  isize                old_size,
  isize                new_size,
  Allocator            allocator,
  Source_Code_Location location
);

#define mem_free_all(allocator) _mem_free_all(allocator, CALLER_LOCATION)
extern Allocator_Error _mem_free_all(Allocator allocator, Source_Code_Location location);

#define any_to_bytes(any) ((Byte_Slice) {.data = (byte *)any, .len = size_of(*any)})
