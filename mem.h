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
  rawptr data;
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

internal void mem_zero(rawptr data, isize len) {
  for (int i = 0; i < len; i += 1) {
    ((byte *)data)[i] = 0;
  }
}

#define mem_tcopy(dst, src, len)                                               \
  ({                                                                           \
    assert(sizeof(dst[0]) == sizeof(src[0]));                                  \
    mem_copy((rawptr)dst, (rawptr)src, len * size_of(dst[0]));                 \
  })

internal void mem_copy(rawptr dst, rawptr src, isize len) {
  for (int i = 0; i < len; i += 1) {
    ((byte *)dst)[i] = ((byte *)src)[i];
  }
}

internal bsize mem_compare(rawptr a, rawptr b, isize len) {
  for (int i = 0; i < len; i += 1) {
    if (((byte *)a)[i] != ((byte *)b)[i]) {
      return false;
    }
  }
  return true;
}
internal isize required_align_offset(uintptr ptr, isize align) {
  uintptr diff = ptr & (align - 1);
  return (align - diff) * (diff != 0);
}

#define mem_tnew(T, allocator)                                                 \
  (T*)unwrap_err(mem_alloc_aligned(size_of(T), align_of(T), (allocator)))

#define mem_alloc_aligned(size, align, allocator)                              \
  _mem_alloc_aligned(size, align, allocator, CALLER_LOCATION)
internal Allocator_Result _mem_alloc_aligned(isize size, isize align,
                                             Allocator allocator,
                                             Source_Code_Location location) {
  // if (!allocator.proc) {
  //   allocator = context.allocator;
  // }
  assert(allocator.proc);
  Allocator_Result ret = {0};
  ret = allocator.proc(allocator.data, AM_Alloc, size, align, nil, location);
  if (ret.err == AE_None && ret.value != nil) {
    mem_zero(ret.value, size);
  }
  return ret;
}

#define mem_alloc(size, allocator) _mem_alloc(size, allocator, CALLER_LOCATION)
internal Allocator_Result _mem_alloc(isize size, Allocator allocator,
                                     Source_Code_Location location) {
  return _mem_alloc_aligned(size, MAX_ALIGN, allocator, location);
}

#define mem_tclone(value, allocator)                                           \
  (type_of(*(value)) *)mem_clone(value, size_of(*(value)), allocator)

#define mem_clone(data, size, allocator)                                       \
  _mem_clone(data, size, allocator, CALLER_LOCATION)
internal Allocator_Result _mem_clone(rawptr data, isize size,
                                     Allocator allocator,
                                     Source_Code_Location location) {
  // if (!allocator.proc) {
  //   allocator = context.allocator;
  // }
  assert(allocator.proc);
  Allocator_Result result = {0};
  result.value = or_return_err(_mem_alloc(size, allocator, location));
  mem_copy(result.value, data, size);
  return result;
}

#define mem_alloc_clone(value_, allocator)                                     \
  ({                                                                           \
    type_of(value_) value = value_;                                            \
    type_of(value) *p = (type_of(value) *)_mem_alloc(sizeof(value), allocator, \
                                                     CALLER_LOCATION);         \
    *p = value;                                                                \
    p;                                                                         \
  })

#define mem_tfree(ptr, allocator)                                              \
  _mem_free(ptr, sizeof((ptr)[0]), allocator, CALLER_LOCATION)

#define mem_free(ptr, size, allocator)                                         \
  _mem_free(ptr, size, allocator, CALLER_LOCATION)
internal Allocator_Error _mem_free(rawptr ptr, isize size, Allocator allocator,
                                   Source_Code_Location location) {
  // if (!allocator.proc) {
  //   allocator = context.allocator;
  // }
  assert(allocator.proc);
  return allocator.proc(allocator.data, AM_Free, size, 0, ptr, location).err;
}

#define mem_resize(data, old_size, new_size, allocator)                        \
  _mem_resize(data, old_size, new_size, allocator, CALLER_LOCATION)
internal Allocator_Error _mem_resize(rawptr *data, isize old_size,
                                     isize new_size, Allocator allocator,
                                     Source_Code_Location location) {
  Allocator_Error err;
  // if (!allocator.proc) {
  //   allocator = context.allocator;
  // }
  assert(allocator.proc);
  if (new_size == 0) {
    err = _mem_free(*data, old_size, allocator, location);
    if (!err) {
      *data = nil;
    }
    return err;
  }
  rawptr new_data = or_return_err_v(_mem_alloc(new_size, allocator, location));
  mem_copy(new_data, *data, old_size);
  if (old_size) {
    err = _mem_free(*data, old_size, allocator, location);
  }
  *data = new_data;

  return err;
}

#define mem_free_all(allocator) _mem_free_all(allocator, CALLER_LOCATION)
internal Allocator_Error _mem_free_all(Allocator allocator,
                                       Source_Code_Location location) {
  // if (!allocator.proc) {
  //   allocator = context.allocator;
  // }
  assert(allocator.proc);
  return allocator.proc(allocator.data, AM_Free_All, 0, 0, nil, location).err;
}

#define any_to_bytes(any) ((Byte_Slice) {.data = (byte *)any, .len = size_of(*any)})
