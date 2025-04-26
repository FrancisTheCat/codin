#include "hash.h"

#define BASE 65521

internal u32 _update_adler32(u32 adler, Byte_Slice data) {
  u32 s1 = adler & 0xffff;
  u32 s2 = (adler >> 16) & 0xffff;
  isize n;

  for (n = 0; n < data.len; n++) {
    s1 = (s1 + data.data[n]) % BASE;
    s2 = (s2 + s1          ) % BASE;
  }
  return (s2 << 16) + s1;
}

#undef BASE

extern u32 hash_adler_32(u32 initial, Reader const *reader) {
  static byte _buf[1 << 12];
  Byte_Slice buf = {.data = _buf, .len = 1 << 12};
  u32 ret = initial;
  loop {
    isize n = or_break(read_bytes(reader, buf));
    ret = _update_adler32(ret, slice_end(buf, n));
    if (n != 1 << 12) {
      break;
    }
  }
  return ret;
}

/* Table of CRCs of all 8-bit messages. */
internal u32 crc_table[256];

/* Flag: has the table been computed? Initially false. */
internal bool crc_table_computed = false;

/* Make the table for a fast CRC. */
internal void init_crc_table() {
  u32 c;
  i32 n, k;

  for (n = 0; n < 256; n++) {
    c = (u32) n;
    for (k = 0; k < 8; k++) {
      if (c & 1) {
        c = 0xedb88320L ^ (c >> 1);
      } else {
        c = c >> 1;
      }
    }
    crc_table[n] = c;
  }
  crc_table_computed = true;
}

internal u32 update_crc(u32 crc, Reader const *reader) {
 u32 c = crc;
 i32 n;

#define BUF_SIZE 1024
  byte buf[BUF_SIZE];

 if (!crc_table_computed) {
   init_crc_table();
 }

 loop {
   isize len = or_break(read_bytes(reader, (Byte_Slice){.data = buf, .len = BUF_SIZE}));
   if (len == 0) {
     break;
   }
   for (n = 0; n < len; n++) {
     c = crc_table[(c ^ buf[n]) & 0xff] ^ (c >> 8);
   }
 }

#undef BUF_SIZE
 return c;
}

extern u32 hash_crc_32(u32 initial, Reader const *reader) {
 return update_crc(initial, reader);
}
