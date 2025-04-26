#pragma once

#include "codin.h"

#include "io.h"

#define hash_bytes(INITIAL, PROC, BYTES)                                       \
  ({                                                                           \
    Byte_Slice __hashed_bytes = (BYTES);                                       \
    Reader __hash_reader = buffer_reader(&__hashed_bytes);                     \
    (PROC)((INITIAL), &__hash_reader);                                         \
  })

typedef u32 (*hash_proc_32)(u32, Reader const *);
typedef u64 (*hash_proc_64)(u64, Reader const *);

typedef struct {
  u32          *hash;
  hash_proc_32  proc;
  Writer const *next;
} Hashing_Writer_32;

typedef struct {
  u64          *hash;
  hash_proc_64  proc;
  Writer const *next;
} Hashing_Writer_64;

internal Maybe_Int hashing_writer_proc_32(rawptr data, Byte_Slice buf) {
  Hashing_Writer_32 *hasher = (Hashing_Writer_32 *)data;
  Maybe_Int ret = write_bytes(hasher->next, buf);
  if (ret.ok) {
    buf.len = ret.value;
    Reader reader = buffer_reader(&buf);
    *hasher->hash = hasher->proc(*hasher->hash, &reader);
  }
  return ret;
}

[[nodiscard]]
internal Writer hashing_writer_32(Hashing_Writer_32 *hasher) {
  assert(hasher->hash);
  assert(hasher->proc);
  return (Writer) {
    .data = hasher,
    .proc = hashing_writer_proc_32,
  };
}

internal Maybe_Int hashing_writer_proc_64(rawptr data, Byte_Slice buf) {
  Hashing_Writer_64 *hasher = (Hashing_Writer_64 *)data;
  Maybe_Int ret = write_bytes(hasher->next, buf);
  if (ret.ok) {
    buf.len = ret.value;
    Reader reader = buffer_reader(&buf);
    *hasher->hash = hasher->proc(*hasher->hash, &reader);
  }
  return ret;
}

[[nodiscard]]
internal Writer hashing_writer_64(Hashing_Writer_64 *hasher) {
  return (Writer) {
    .data = hasher,
    .proc = hashing_writer_proc_64,
  };
}

extern u32 hash_crc_32(u32 initial, Reader const *r);
extern u32 hash_adler_32(u32 initial, Reader const *r);
