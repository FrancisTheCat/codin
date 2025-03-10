#include "codin.h"

#define sort_slice(_slice)                                                     \
  {                                                                            \
    type_of(_slice) __sort_slice_slice = _slice;                               \
    sort_slice_by(__sort_slice_slice, index_i, index_j, ({                     \
      __sort_slice_slice.data[index_i] <=                                      \
      __sort_slice_slice.data[index_j];                                        \
    }));                                                                       \
  }

#define sort_slice_by(_slice, index_i, index_j, is_sorted)                     \
  {                                                                            \
    type_of(_slice) __sort_slice_by_slice = _slice;                            \
    sort_interface(index_i, index_j, is_sorted,                                \
        {                                                                      \
          type_of(*__sort_slice_by_slice.data) tmp =                           \
              __sort_slice_by_slice.data[index_i];                             \
          __sort_slice_by_slice.data[index_i] =                                \
              __sort_slice_by_slice.data[index_j];                             \
          __sort_slice_by_slice.data[index_j] = tmp;                           \
        },                                                                     \
        __sort_slice_by_slice.len);                                            \
  }

#define sort_interface(index_i, index_j, is_sorted, swap, _len)                \
  {                                                                            \
    typedef struct {                                                           \
      isize start;                                                             \
      isize end;                                                               \
    } _Sort_Range;                                                             \
                                                                               \
    Vector(_Sort_Range) __stack;                                               \
    vector_init(&__stack, 0, 8, context.temp_allocator);                       \
                                                                               \
    vector_append(&__stack, ((_Sort_Range){.start = 0, .end = _len}));         \
    while (__stack.len) {                                                      \
      _Sort_Range __range = vector_pop(&__stack);                              \
                                                                               \
      if (__range.end - __range.start < 2) {                                   \
        continue;                                                              \
      }                                                                        \
                                                                               \
      isize __p = __range.end - 1;                                             \
      isize __i = __range.start;                                               \
      for_range(__j, __range.start, __range.end - 1) {                         \
        isize index_i = __j;                                                   \
        isize index_j = __p;                                                   \
        assert(__i >= 0);                                                      \
        assert(__i <  _len);                                                   \
        assert(__j >= 0);                                                      \
        assert(__j <  _len);                                                   \
        assert(__p >= 0);                                                      \
        assert(__p <  _len);                                                   \
        if (is_sorted) {                                                       \
          isize index_i = __i;                                                 \
          isize index_j = __j;                                                 \
          swap;                                                                \
          __i += 1;                                                            \
        }                                                                      \
      }                                                                        \
                                                                               \
      assert(__i >= 0);                                                        \
      assert(__i <  _len);                                                     \
      assert(__p >= 0);                                                        \
      assert(__p <  _len);                                                     \
                                                                               \
      {                                                                        \
        isize index_i = __p;                                                   \
        isize index_j = __i;                                                   \
        swap;                                                                  \
      }                                                                        \
                                                                               \
      if (__range.start > __range.end) {                                       \
        vector_append(&__stack, ((_Sort_Range){__range.start, __i}));          \
        vector_append(&__stack, ((_Sort_Range){__i + 1, __range.end}));        \
      } else {                                                                 \
        vector_append(&__stack, ((_Sort_Range){__i + 1, __range.end}));        \
        vector_append(&__stack, ((_Sort_Range){__range.start, __i}));          \
      }                                                                        \
    }                                                                          \
  }
