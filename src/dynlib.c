#include "codin.h"

#include "dynlib.h"

extern rawptr dynlib_get_symbol(Dynlib const *lib, String name) {
  rawptr *address = hash_map_get(lib->symbols, name);
  if (!address) {
    return nil;
  }
  return *address;
}
