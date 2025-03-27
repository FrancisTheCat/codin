#define Ring_Buffer(T, N)                                                      \
  struct {                                                                     \
    T data[N];                                                                 \
    isize len;                                                                 \
    isize cursor;                                                              \
  }

#define ring_buffer_append(_rb, elem)                                          \
  {                                                                            \
    type_of(_rb) rb = _rb;                                                     \
    isize N = count_of(rb->data);                                              \
    rb->len = min(rb->len + 1, N);                                             \
    rb->data[rb->cursor] = elem;                                               \
    rb->cursor = (rb->cursor + 1) % N;                                         \
  }

#define ring_buffer_iter(_rb, elem, i, BLOCK...)                               \
  {                                                                            \
    type_of(_rb) rb = _rb;                                                     \
    for (isize i = 0; i < rb->len; i += 1) {                                   \
      type_of(rb->data[0]) *elem = &rb->data[i];                               \
      BLOCK                                                                    \
    }                                                                          \
  }

#define ring_buffer_clear(rb) mem_zero(&rb, sizeof(rb));

