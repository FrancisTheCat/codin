#include "codin.h"

typedef struct {
  Hash_Map(String, rawptr) symbols;
  rawptr mapping;
  isize  size;
} Dynlib;

#ifdef linux
#include "dynlib_linux.h"
#endif

typedef Maybe(Dynlib) Dynlib_Result;

Dynlib_Result dynlib_load(String path, Allocator allocator) {
  Dynlib_Result result;
  result.value.mapping = _dynlib_load(path, &result.value.size);
  result.ok = result.value.mapping && _dynlib_load_symbols(&result.value, allocator);
  return result;
}

b8 dynlib_unload(Dynlib lib) {
  hash_map_delete(lib.symbols);
  return _dynlib_unload(lib.mapping, lib.size);
}

rawptr dynlib_get_symbol(Dynlib lib, String name) {
  rawptr *address = hash_map_get(lib.symbols, name);
  if (!address) {
    return nil;
  }
  return *address;
}
