#include "codin.h"

#define SLICE_LIT(name, T, ...)                                                \
  T name;                                                                      \
  type_of(*name.data) _##name##_data_values[] = __VA_ARGS__;                   \
  name.data = _##name##_data_values;                                           \
  name.len = count_of(_##name##_data_values);

#define SLICE_VAL(S, ...)                                                      \
  ({                                                                           \
    S slice;                                                                   \
    type_of(*slice.data) values[] = __VA_ARGS__;                               \
    slice.len = count_of(values);                                              \
    slice.data = alloca(size_of(values));                                      \
    mem_copy(slice.data, values, size_of(values));                             \
    slice;                                                                     \
  })

#define slice_init(_slice, length, ally)                                       \
  {                                                                            \
    type_of(_slice) __slice_init_slice = _slice;                               \
    __slice_init_slice->len = length;                                          \
    __slice_init_slice->data = (type_of(__slice_init_slice->data))unwrap_err(  \
        mem_alloc((length) * size_of(*(__slice_init_slice->data)), ally));     \
  }

#define slice_make(type, length, ally)                                         \
  ({                                                                           \
    type slice;                                                                \
    slice_init(&slice, length, ally);                                          \
    slice;                                                                     \
  })

#define slice_delete(slice, allocator)                                         \
  { mem_free(slice.data, slice.len * sizeof(slice.data[0]), allocator); }

#define slice_from_pointer(pointer, _len)                                      \
  ({                                                                           \
    Slice(type_of(*pointer)) slice;                                            \
    slice.data = pointer;                                                      \
    slice.len = _len;                                                          \
    slice;                                                                     \
  })

#define slice_range(_full, _start, _end)                                       \
  ({                                                                           \
    type_of(_start) start = _start;                                            \
    type_of(_end) end = _end;                                                  \
    type_of(_full) full = _full;                                               \
    type_of(_full) range;                                                      \
                                                                               \
    range.data = full.data + start;                                            \
    range.len = end - start;                                                   \
    range;                                                                     \
  })

#define slice_range_type(T, _full, _start, _end)                               \
  ({                                                                           \
    type_of(_start) start = _start;                                            \
    type_of(_end) end = _end;                                                  \
    type_of(_full) full = _full;                                               \
    T range;                                                                   \
                                                                               \
    range.data = full.data + start;                                            \
    range.len = end - start;                                                   \
    range;                                                                     \
  })

#define slice_start(_full, _start)                                             \
  ({                                                                           \
    type_of(_start) start = _start;                                            \
    type_of(_full) full = _full;                                               \
    type_of(_full) range = full;                                               \
                                                                               \
    range.data = full.data + start;                                            \
    range.len -= start;                                                        \
    range;                                                                     \
  })

#define slice_end(_full, _end)                                                 \
  ({                                                                           \
    type_of(_end) end = _end;                                                  \
    type_of(_full) full = _full;                                               \
    type_of(_full) range = full;                                               \
                                                                               \
    range.data = full.data;                                                    \
    range.len = end;                                                           \
    range;                                                                     \
  })

#define slice_zero(_slice) {                                                   \
  type_of(_slice) slice = _slice;                                              \
  mem_zero(slice.data, slice.len * size_of(*slice.data));                      \
}

#define linear_search(slice, value)                                            \
  ({                                                                           \
    slice_iter(slice, elem, i, {                                               \
      if (*elem == value) {                                                    \
        return i;                                                              \
      }                                                                        \
    });                                                                        \
  })

#define linear_search_by(slice, compare_value, compare)                        \
  ({                                                                           \
    isize ret = -1;                                                            \
    slice_iter(slice, elem, i, {                                               \
      type_of(*elem) compare_value = *elem;                                    \
      if (compare) {                                                           \
        ret = i;                                                               \
        break;                                                                 \
      }                                                                        \
    });                                                                        \
    ret;                                                                       \
  })

#define slice_reverse(_slice)                                                  \
  {                                                                            \
    type_of(_slice) slice = _slice;                                            \
    for_range(i, 0, slice.len / 2) {                                           \
      isize j = slice.len - i - 1;                                             \
      type_of(*slice.data) tmp = slice.data[i];                                \
      slice.data[i] = slice.data[j];                                           \
      slice.data[j] = tmp;                                                     \
    }                                                                          \
  }

internal Byte_Slice _slice_to_bytes(rawptr data, isize size, isize count) {
  return (Byte_Slice){
      (byte *)data,
      size * count,
  };
}
#define slice_to_bytes(slice)                                                  \
  _slice_to_bytes((rawptr)(slice).data, size_of((slice).data[0]), (slice).len)

#define slice_iter(slice, elem, i, BLOCK)                                      \
  for (isize i = 0; i < (slice).len; i++) {                                    \
    type_of((slice).data) elem = &(slice).data[i];                             \
    { BLOCK }                                                                  \
  }                                                                            \
  

internal Byte_Slice bytes_concatenate(Byte_Slice a, Byte_Slice b,
                                      Allocator allocator) {
  Byte_Slice data;
  slice_init(&data, a.len + b.len, allocator);
  mem_copy(data.data, a.data, a.len);
  mem_copy(&data.data[a.len], b.data, b.len);
  return data;
}

internal isize bytes_copy(Byte_Slice dst, Byte_Slice src) {
  isize n = min(dst.len, src.len);
  mem_copy(dst.data, src.data, n);
  return n;
}
