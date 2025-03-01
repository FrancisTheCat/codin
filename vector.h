#pragma once

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
    assert((length) < (capacity));                                             \
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

internal void builder_init(Builder *builder, isize length, isize capacity, Allocator allocator) {
  vector_init(builder, length, capacity, allocator);
}

internal Builder builder_make(isize length, isize capacity, Allocator allocator) {
  return vector_make(Builder, length, capacity, allocator);
}

internal void byte_buffer_init(Byte_Buffer *byte_buffer, isize length, isize capacity, Allocator allocator) {
  vector_init(byte_buffer, length, capacity, allocator);
}

internal Byte_Buffer byte_buffer_make(isize length, isize capacity, Allocator allocator) {
  return vector_make(Byte_Buffer, length, capacity, allocator);
}

#define vector_delete(_vector)                                                 \
  {                                                                            \
    type_of(_vector) vector = _vector;                                         \
    mem_free(vector.data, vector.cap * sizeof(vector.data[0]),                 \
             vector.allocator);                                                \
  }

internal void byte_buffer_destroy(Byte_Buffer *bb) {
  vector_delete(*bb)
}

#define vector_append(_vector, elem)                                           \
  {                                                                            \
    type_of(_vector) vector = _vector;                                         \
    if (!vector->allocator.proc) {                                             \
      vector->allocator = context.allocator;                                   \
    }                                                                          \
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

#define vector_append_slice(_vector, _s)                                       \
  {                                                                            \
    type_of(_vector) vector = _vector;                                         \
    type_of(_s) s = _s;                                                        \
    if (!vector->allocator.proc) {                                             \
      vector->allocator = context.allocator;                                   \
    }                                                                          \
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
    if (!vector->allocator.proc) {                                             \
      vector->allocator = context.allocator;                                   \
    }                                                                          \
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

#define vector_insert(_vector, _index, value)                                  \
  ({                                                                           \
    type_of(_vector) vector = _vector;                                         \
    type_of(_index)  index  = _index;                                          \
    type_of(vector->data[0]) removed_item = vector->data[index];               \
    vector_append(vector, value);                                              \
    for (                                                                      \
      isize __vector_insert_i = vector->len - 1;                               \
      __vector_insert_i < index;                                               \
      __vector_insert_i++                                                      \
    ) {                                                                        \
      vector->data[__vector_insert_i] = vector->data[__vector_insert_i - 1];   \
    }                                                                          \
    vector->data[index] = value;                                               \
    removed_item;                                                              \
  })

#define vector_iter   slice_iter
#define vector_iter_v slice_iter_v

#define vector_to_slice(Type, _vector)                                         \
  ({                                                                           \
    type_of(_vector) vector = _vector;                                         \
    Type s;                                                                    \
    s.len = vector.len;                                                        \
    s.data = vector.data;                                                      \
    s;                                                                         \
  })

[[nodiscard]]
internal String builder_to_string(Builder b) {
  String s;
  s.data = b.data;
  s.len = b.len;
  return s;
}

[[nodiscard]]
internal Byte_Slice builder_to_bytes(Builder b) {
  Byte_Slice sb;
  sb.data = (byte *)b.data;
  sb.len = b.len;
  return sb;
}

[[nodiscard]]
internal cstring builder_to_cstring(Builder *b) {
  vector_append(b, 0);
  return b->data;
}

internal Byte_Buffer buffer_from_bytes(Byte_Slice data, Allocator allocator) {
  return (Byte_Buffer) {
    .data      = data.data,
    .len       = data.len,
    .cap       = data.len,
    .allocator = allocator,
  };
}

internal Byte_Slice buffer_to_bytes(Byte_Buffer bb) {
  return slice_to_bytes(bb);
}
