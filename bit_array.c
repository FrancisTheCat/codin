#include "bit_array.h"

// len and cap in bits
extern void bit_array_init(Bit_Array *ba, isize len, isize cap, Allocator allocator) {
  vector_init(&ba->data, (len + 7) / 8, (cap + 7) / 8, allocator);
}

extern Bit_Array bit_array_make(isize len, isize cap, Allocator allocator) {
  Bit_Array b = {0};
  bit_array_init(&b, len, cap, allocator);
  return b;
}

extern void bit_array_destroy(Bit_Array const *ba) {
  vector_delete(ba->data);
}

extern b8 bit_array_get(Bit_Array const *ba, isize index) {
  isize elem   = index >> 3;
  isize offset = index & 7;
  u8    value  = IDX(ba->data, elem) & (0x80 >> offset);

  return value != 0;
}

extern void bit_array_set(Bit_Array const *ba, isize index, b8 value) {
  assert(index >= 0);
  assert(index < ba->len);

  isize elem   = index >> 3;
  isize offset = index & 7;
  u8   *v      = &IDX(ba->data, elem);

  if (value) {
    u8 mask = 0x80 >> offset;
    *v |= mask;
  } else {
    u8 mask = ~(0x80 >> offset);
    *v &= mask;
  }
}

extern void bit_array_append(Bit_Array *ba, b8 value) {
  if ((ba->len >> 3) >= ba->data.len) {
    vector_append(&ba->data, 0);
  }
  value = !!value * 0x80;
  isize offset = ba->len & 7;
  IDX(ba->data, ba->len >> 3) |= (u64)value >> offset;
  ba->len += 1;
}

// append least significant n bits of value to bit array
extern void bit_array_append_n(Bit_Array *ba, u64 value, isize n) {
  u64 mask = 1 << (n - 1);
  for_range(i, 0, n) {
    bit_array_append(ba, !!(value & mask));
    mask >>= 1;
  }
}

extern void bit_array_append_u8(Bit_Array *ba, u8 value) {
  bit_array_append_n(ba, value, 8);
}

extern void bit_array_append_u16(Bit_Array *ba, u16 value) {
  bit_array_append_n(ba, value, 16);
}

extern void bit_array_append_u32(Bit_Array *ba, u32 value) {
  bit_array_append_n(ba, value, 32);
}

extern void bit_array_append_u64(Bit_Array *ba, u64 value) {
  bit_array_append_n(ba, value, 64);
}

extern Byte_Slice bit_array_to_bytes(Bit_Array *ba) {
  return slice_end(slice_to_bytes(ba->data), (ba->len + 7) / 8);
}
