#include "codin.h"

typedef struct {
  Hash_Map(String, rawptr) symbols;
  rawptr mapping;
  isize  mapping_size;
} Dynlib;

#ifdef linux
#include "dynlib_linux.h"
#endif

typedef Maybe(Dynlib) Dynlib_Result;

Dynlib_Result dynlib_load(String path, Allocator allocator) {
  Dynlib_Result result;
  result.ok = _dynlib_load(path, allocator, &result.value);
  return result;
}

bool dynlib_unload(Dynlib lib) {
  hash_map_delete(lib.symbols);
  return _dynlib_unload(lib.mapping, lib.mapping_size);
}

rawptr dynlib_get_symbol(Dynlib lib, String name) {
  rawptr *address = hash_map_get(lib.symbols, name);
  if (!address) {
    return nil;
  }
  return *address;
}
