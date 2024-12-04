#include "codin.h"

#define Iterator(V, I, D)                                                      \
  struct {                                                                     \
    b8 (*proc)(D *, V *, I *);                                                 \
    D data;                                                                    \
    V __type_info_value[0];                                                    \
    I __type_info_index[0];                                                    \
  }

#define iterate(iterator, _value_name, _index_name, block)                     \
  {                                                                            \
    type_of(*iterator.__type_info_index) _index_name = 0;                      \
    loop {                                                                     \
      type_of(*iterator.__type_info_value) _value_name;                        \
      if (iterator.proc(&iterator.data, &_value_name, &_index_name)) {         \
        block;                                                                 \
      } else {                                                                 \
        break;                                                                 \
      }                                                                        \
    }                                                                          \
  }

typedef Iterator(String, isize, String) Strings_Iterator;

internal b8 string_lines_iter_proc(String *data, String *value, isize *index) {
  if (data->len <= 0) {
    return false;
  }
  value->data = data->data;
  value->len = string_index_byte(*data, '\n');
  if (value->len < 0) {
    value->len = data->len;
  }
  data->len -= value->len + 1;
  data->data += value->len + 1;
  *index += 1;
  return true;
}

internal Strings_Iterator string_lines_iterator(String s) {
  return (Strings_Iterator){
      .proc = string_lines_iter_proc,
      .data = s,
  };
}

