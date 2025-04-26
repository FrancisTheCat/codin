#include "codin.h"

internal isize parse_int(String *str) {
  i32  value    = 0;
  bool negative = false;
  while (str->len) {
    char c = str->data[0];
    switch (c) {
    CASE '0' ... '9':
      value *= 10;
      value += c - '0';
    CASE '-':
      negative = true;
    DEFAULT:
      return negative ? -value : value;
    }
    str->data += 1;
    str->len  -= 1;
  }
  return negative ? -value : value;
}

internal f64 parse_float(String *str) {
  f32  value    = 0;
  bool negative = false;
  while (str->len) {
    char c = str->data[0];
    switch (c) {
    CASE '0' ... '9':
      value *= 10;
      value += c - '0';
    CASE '.':
      str->data += 1;
      str->len  -= 1;

      f32 factor = 0.1f;
      while (str->len) {
        char c = str->data[0];
        switch (c) {
        CASE '0' ... '9':
          value  += (c - '0') * factor;
          factor *= 0.1f;
        DEFAULT:
          return negative ? -value : value;
        }
        str->data += 1;
        str->len  -= 1;
      }

      goto end;
    CASE '-':
      negative = true;
    DEFAULT:
  return negative ? -value : value;
    }
    str->data += 1;
    str->len  -= 1;
  }

end:
  return negative ? -value : value;
}
