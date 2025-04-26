#pragma once

#include "codin.h"

#define Queue(T) struct {                                                      \
  T        *data;                                                              \
  isize     len, cap, read, write;                                             \
  Allocator allocator;                                                         \
}

#define queue_init(queue, cap, allocator) {                                    \
  vector_init((queue), 0, (cap), (allocator));                                 \
  (queue)->read  = 0;                                                          \
  (queue)->write = 0;                                                          \
}

#define queue_destroy(queue)                                                   \
  vector_delete((queue))

#define queue_append(queue, elem) {                                            \
  if ((queue)->len == (queue)->cap) {                                          \
    assert((queue)->read == (queue)->write);                                   \
    isize new_cap = max((queue)->cap * 2, 8);                                  \
                                                                               \
    type_of((queue)->data) new_data = (unwrap_err(                             \
        mem_alloc_aligned(new_cap * size_of(*(queue)->data),                   \
                          align_of(*new_data), (queue)->allocator)));          \
                                                                               \
    mem_copy(new_data, (queue)->data + (queue)->read,                          \
      ((queue)->cap - (queue)->read) * size_of(*new_data));                    \
    mem_copy(new_data + (queue)->cap - (queue)->read,                          \
      (queue)->data, ((queue)->read) * size_of(*new_data));                    \
                                                                               \
    mem_free((queue)->data, (queue)->cap *size_of(*(queue)->data),             \
             (queue)->allocator);                                              \
                                                                               \
    (queue)->read  = 0;                                                        \
    (queue)->write = (queue)->len;                                             \
    (queue)->data  = new_data;                                                 \
    (queue)->cap   = new_cap;                                                  \
  }                                                                            \
  (queue)->data[(queue)->write] = (elem);                                      \
  (queue)->write = (1 + (queue)->write) % (queue)->cap;                        \
  (queue)->len += 1;                                                           \
}

#define queue_pop(queue) ({                                                    \
  assert((queue)->len);                                                        \
  type_of(*(queue)->data) ret = (queue)->data[(queue)->read];                  \
  (queue)->read = (1 + (queue)->read) % (queue)->cap;                          \
  (queue)->len -= 1;                                                           \
  ret;                                                                         \
})

