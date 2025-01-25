#include "codin.h"

internal rune utf8_first_rune(String str, isize *n) {
  if (str.len < 1) {
    if (n) {
      *n = -1;
    }
    return 0;
  }
  u8 first = IDX(str, 0);
  if ((first & 0x80) == 0) {
    if (n) {
    *n = 1;
    }
    return first;
  }

  isize len = 0;
  u8    len_mask = 0x80;
  while (len_mask & first) {
    len_mask >>= 1;
    len += 1;
  }

  if (len > 4) {
    if (n) {
      *n = -1;
    }
    return 0;
  }

  if (n) {
    *n = len;
  }
  if (len > str.len) {
    return 0;
  }

  u8   mask = 0x1F >> (len - 3);
  rune value = IDX(str, 0) & mask;

  mask = 0x3F;
  for_range(i, 1, len) {
    value <<= 6;
    value |= IDX(str, i) & mask;
  }
  return value;
}

internal rune utf8_rune_at(String str, isize index, isize *n) {
  return utf8_first_rune(slice_start(str, index), n);
}
