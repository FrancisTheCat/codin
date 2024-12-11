#include "codin.h"

#define Vector(T)                                                              \
  struct {                                                                     \
    T *data;                                                                   \
    isize len;                                                                 \
    isize cap;                                                                 \
    Allocator allocator;                                                       \
  }

typedef Vector(char) Builder;
typedef Vector(byte) Byte_Buffer;

// NOLINTBEGIN
#define vector_init(_vector, length, capacity, ally)                           \
  {                                                                            \
    type_of(*_vector) *vector = _vector;                                       \
    vector->len = length;                                                      \
    vector->cap = capacity;                                                    \
    vector->allocator = ally;                                                  \
    vector->data = (type_of(vector->data))unwrap_err(                          \
        mem_alloc(capacity * sizeof(vector->data[0]), ally));                  \
  }
#define vector_make(type, length, capacity, ally)                              \
  ({                                                                           \
    type __make_vector;                                                        \
    vector_init(&__make_vector, length, capacity, ally);                       \
    __make_vector;                                                             \
  })

#define builder_init(builder, length, capacity, ally)                          \
  vector_init(builder, length, capacity, ally)

#define vector_delete(_vector)                                                 \
  {                                                                            \
    type_of(_vector) vector = _vector;                                         \
    mem_free(vector.data, vector.cap * sizeof(vector.data[0]),                 \
             vector.allocator);                                                \
  }

#define vector_append(_vector, elem)                                           \
  {                                                                            \
    type_of(_vector) vector = _vector;                                         \
    if (vector->len == vector->cap) {                                          \
      isize new_cap = max(vector->cap * 2, 8);                                 \
      mem_resize((rawptr *)&vector->data,                                      \
                 vector->cap * sizeof(vector->data[0]),                        \
                 new_cap * sizeof(vector->data[0]), vector->allocator);        \
      vector->cap = new_cap;                                                   \
    }                                                                          \
    vector->data[vector->len] = (elem);                                        \
    vector->len += 1;                                                          \
  }
// NOLINTEND

#define vector_append_slice(_vector, s)                                        \
  {                                                                            \
    type_of(_vector) vector = _vector;                                         \
    assert(sizeof((s).data[0]) == sizeof(vector->data[0]));                    \
    if (vector->len + (s).len >= vector->cap) {                                \
      isize new_cap = max(vector->len + (s).len, max(vector->cap * 2, 8));     \
      mem_resize((rawptr *)&vector->data,                                      \
                 vector->cap * sizeof(vector->data[0]),                        \
                 new_cap * sizeof(vector->data[0]), vector->allocator);        \
      vector->cap = new_cap;                                                   \
    }                                                                          \
    mem_copy(&vector->data[vector->len], (s).data, s.len);                     \
    vector->len += (s).len;                                                    \
  }

#define vector_reserve(_vector, _cap)                                          \
  {                                                                            \
    type_of(_vector) vector = _vector;                                         \
    if (_cap > vector->cap) {                                                  \
      isize new_cap = _cap;                                                    \
      mem_resize((rawptr *)&vector->data,                                      \
                 vector->cap * sizeof(vector->data[0]),                        \
                 new_cap * sizeof(vector->data[0]), vector->allocator);        \
      vector->cap = new_cap;                                                   \
    }                                                                          \
  }

#define vector_clear(vector)                                                   \
  { (vector)->len = 0; }

#define vector_pop(_vector)                                                    \
  ({                                                                           \
    type_of(_vector) vector = _vector;                                         \
    vector->len -= 1;                                                          \
    vector->data[vector->len];                                                 \
  })

/* swaps with the tail to achieve O(1) time complexity
   if you require order to be retained use 'vector_remove_ordered'  */
#define vector_remove(_vector, index)                                          \
  ({                                                                           \
    type_of(_vector) vector = _vector;                                         \
    type_of(vector->data[0]) removed_item = vector->data[index];               \
    vector->len -= 1;                                                          \
    vector->data[index] = vector->data[vector->len];                           \
    removed_item;                                                              \
  })

// O(N)
#define vector_remove_ordered(_vector, index)                                  \
  ({                                                                           \
    type_of(_vector) vector = _vector;                                         \
    type_of(vector->data[0]) removed_item = vector->data[index];               \
    vector->len -= 1;                                                          \
    for (isize i = index; i < vector->len; i++) {                              \
      vector->data[i] = vector->data[i + 1];                                   \
    }                                                                          \
    removed_item;                                                              \
  })

#define vector_iter slice_iter

#define vector_to_slice(Type, _vector)                                         \
  ({                                                                           \
    type_of(_vector) vector = _vector;                                         \
    Type s;                                                                    \
    s.len = vector.len;                                                        \
    s.data = vector.data;                                                      \
    s;                                                                         \
  })

internal String builder_to_string(Builder b) {
  String s;
  s.data = b.data;
  s.len = b.len;
  return s;
}

internal Byte_Slice builder_to_bytes(Builder b) {
  Byte_Slice sb;
  sb.data = (byte *)b.data;
  sb.len = b.len;
  return sb;
}

internal cstring builder_to_cstring(Builder *b) {
  vector_append(b, 0);
  return b->data;
}
