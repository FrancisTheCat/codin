#include "codin.h"

typedef struct {
  isize      len; // in bits
  Vector(u8) data;
} Bit_Array;

extern Bit_Array bit_array_make(isize len, isize cap, Allocator allocator);
extern void      bit_array_init(Bit_Array *ba, isize len, isize cap, Allocator allocator);
extern void      bit_array_destroy(Bit_Array const *ba);

extern Byte_Slice bit_array_to_bytes(Bit_Array *ba);

extern b8   bit_array_get(Bit_Array const *ba, isize index);
extern void bit_array_set(Bit_Array const *ba, isize index, b8 value);

extern void bit_array_append  (Bit_Array *ba, b8  value);
extern void bit_array_append_n(Bit_Array *ba, u64 value, isize n);

extern void bit_array_append_u8 (Bit_Array *ba, u8  value);
extern void bit_array_append_u16(Bit_Array *ba, u16 value);
extern void bit_array_append_u32(Bit_Array *ba, u32 value);
extern void bit_array_append_u64(Bit_Array *ba, u64 value);

#define bit_array_iter(ba, b, i, BLOCK) {                                      \
  for (isize i = 0; i < (ba).len; i += 1) {                                    \
    b8 b = bit_array_get(&ba, i);                                              \
    { BLOCK; }                                                                 \
  }                                                                            \
}                                                                              \

#define bit_array_iter_set(ba, i, BLOCK) {                                     \
  for (isize i = 0; i < (ba).len; i += 1) {                                    \
    if (bit_array_get(&ba, i)) {                                               \
      BLOCK;                                                                   \
    }                                                                          \
  }                                                                            \
}                                                                              \

#define bit_array_iter_unset(ba, i, BLOCK) {                                   \
  for (isize i = 0; i < (ba).len; i += 1) {                                    \
    if (!bit_array_get(&ba, i)) {                                              \
      BLOCK;                                                                   \
    }                                                                          \
  }                                                                            \
}
