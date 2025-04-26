#include "hash_map.h"

#include "strings.h"

#define INITIAL_HASH_SEED 0xcbf29ce484222325

#define HASH_MASK (u64)(((u64)1 << (u64)(8 * sizeof(uintptr) - 1)) - 1)

extern u64 default_hasher(rawptr data, uintptr seed, isize N) {
  u64 h = seed + INITIAL_HASH_SEED;
  byte *p = (byte *)data;
  for (int i = 0; i < N; i += 1) {
    h = (h ^ (u64)p[0]) * 0x100000001b3;
    p += 1;
  }
  h &= HASH_MASK;
  if (h < 2) {
    h += 2;
  }
  return (u64)h;
}

extern u64 cstring_hash(cstring value) {
  isize len = cstring_len(value);
  return default_hasher((rawptr)value, INITIAL_HASH_SEED, len);
}

extern u64 string_hash(String value) {
  return default_hasher((rawptr)value.data, INITIAL_HASH_SEED, value.len);
}
