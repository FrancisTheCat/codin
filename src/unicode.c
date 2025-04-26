#include "unicode.h"

extern rune utf8_first_rune(String str, isize *n) {
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

  isize len      = 0;
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

  u8   mask  = 0x1F >> (len - 2);
  rune value = first & mask;

  mask = 0x3F;
  for_range(i, 1, len) {
    value <<= 6;
    value |= IDX(str, i) & mask;
  }
  return value;
}

extern rune utf8_rune_at(String str, isize index, isize *n) {
  return utf8_first_rune(slice_start(str, index), n);
}

/*
First    | Last     | Byte 1   | Byte 2   | Byte 3   | Byte 4   
U+0000   | U+007F   | 0yyyzzzz |          |          |          
U+0080   | U+07FF   | 110xxxyy | 10yyzzzz |          |          
U+0800   | U+FFFF   | 1110wwww | 10xxxxyy | 10yyzzzz |          
U+010000 | U+10FFFF | 11110uvv | 10vvwwww | 10xxxxyy | 10yyzzzz 
*/

extern isize utf8_rune_encode(rune r, char *buf) {
  if (r < 0x80) {
    buf[0] = r;
    return 1;
  } else if (r < 0x800) {
    buf[1] = 0x80 | (r & 0x3F);
    buf[0] = 0xC0 | ((r >> 6) & 0x1F);
    return 2;
  } else if (r < 0x10000) {
    buf[2] = 0x80 | (r & 0x3F);
    buf[1] = 0x80 | ((r >> 6 ) & 0x3F);
    buf[0] = 0xE0 | ((r >> 12) & 0x0F);
    return 3;
  } else if (r < 0x110000) {
    buf[3] = 0x80 | (r & 0x3F);
    buf[2] = 0x80 | ((r >> 6 ) & 0x3F);
    buf[1] = 0x80 | ((r >> 12) & 0x3F);
    buf[0] = 0xF0 | ((r >> 18) & 0x07);
    return 4;
  } else {
    return -1;
  }
}
