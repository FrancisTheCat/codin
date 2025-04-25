#include "base64.h"

#include "allocators.h"
#include "bit_array.h"

extern String base64_encode(Byte_Slice data, Allocator allocator) {
  Builder b;
  builder_init(&b, 0, (data.len + 7) * 8 / 6, allocator);
  b.allocator = panic_allocator();

  Bit_Array ba = {
    .data = {
      .data      = data.data,
      .len       = data.len,
      .cap       = data.len,
      .allocator = panic_allocator(),
    },
    .len = data.len * 8,
  };

  isize offset = 0;
  while (offset < data.len * 8) {
    u8 byte = bit_array_get_n(&ba, offset, 6);
    offset += 6;

    u8 c;
    switch (byte) {
    case 0  ... 25:
      c = 'A' + byte - 0;
      break;
    case 26 ... 51:
      c = 'a' + byte - 26;
      break;
    case 52 ... 61:
      c = '0' + byte - 52;
      break;
    case 62:
      c = '+';
      break;
    case 63:
      c = '/';
      break;
    }

    vector_append(&b, c);
  }

  return builder_to_string(b);
}

extern b8 base64_decode(String data, Byte_Slice *out, Allocator allocator) {
  Bit_Array ba = bit_array_make(0, (data.len + 5) * 6, allocator);
  ba.data.allocator = panic_allocator();
  slice_iter_v(data, c, i, {
    u8 value;
    switch (c) {
    CASE 'A' ... 'Z':
      value = c - 'A';
    CASE 'a' ... 'z':
      value = c - 'a' + 26;
    CASE '0' ... '9':
      value = c - '0' + 52;
    CASE '+':
      value = 62;
    CASE '/':
      value = 63;
    DEFAULT:
      bit_array_destroy(&ba);
      return false;
    }
    bit_array_append_n(&ba, value, 6);
  });

  *out = bit_array_to_bytes(&ba);
  return true;
}
