#include "codin.h"

extern void mem_zero(rawptr data, isize len) {
  for (int i = 0; i < len; i += 1) {
    ((byte *)data)[i] = 0;
  }
}

extern void mem_copy(rawptr dst, rawptr src, isize len) {
  for (int i = 0; i < len; i += 1) {
    ((byte *)dst)[i] = ((byte *)src)[i];
  }
}

extern b8 mem_compare(rawptr a, rawptr b, isize len) {
  for (int i = 0; i < len; i += 1) {
    if (((byte *)a)[i] != ((byte *)b)[i]) {
      return false;
    }
  }
  return true;
}

extern isize required_align_offset(uintptr ptr, isize align) {
  uintptr diff = ptr & (align - 1);
  return (align - diff) * (diff != 0);
}

extern Allocator_Result _mem_alloc_aligned(
  isize                size,
  isize                align,
  Allocator            allocator,
  Source_Code_Location location
) {
  // if (!allocator.proc) {
  //   allocator = context.allocator;
  // }
  assert(allocator.proc);
  Allocator_Result ret = {0};
  ret = allocator.proc(allocator.data, AM_Alloc, size, align, nil, location);
  if (ret.err == AE_None && ret.value != nil) {
    for_range(i, 0, size) {
      assert(((byte *)ret.value)[i] == 0);
    }
  }
  return ret;
}

extern Allocator_Result _mem_alloc(isize size, Allocator allocator, Source_Code_Location location) {
  return _mem_alloc_aligned(size, MAX_ALIGN, allocator, location);
}

extern Allocator_Result _mem_clone(
  rawptr               data,
  isize                size,
  Allocator            allocator,
  Source_Code_Location location
) {
  // if (!allocator.proc) {
  //   allocator = context.allocator;
  // }
  assert(allocator.proc);
  Allocator_Result result = {0};
  result.value = or_return_err(_mem_alloc(size, allocator, location));
  mem_copy(result.value, data, size);
  return result;
}

extern Allocator_Error _mem_free(
  rawptr               ptr,
  isize                size,
  Allocator            allocator,
  Source_Code_Location location
) {
  if (!ptr) {
    return AE_None;
  }
  // if (!allocator.proc) {
  //   allocator = context.allocator;
  // }
  assert(allocator.proc);
  return allocator.proc(allocator.data, AM_Free, size, 0, ptr, location).err;
}

extern Allocator_Error _mem_resize(
  rawptr              *data,
  isize                old_size,
  isize                new_size,
  Allocator            allocator,
  Source_Code_Location location
) {
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

extern Allocator_Error _mem_free_all(Allocator allocator, Source_Code_Location location) {
  // if (!allocator.proc) {
  //   allocator = context.allocator;
  // }
  assert(allocator.proc);
  return allocator.proc(allocator.data, AM_Free_All, 0, 0, nil, location).err;
}
