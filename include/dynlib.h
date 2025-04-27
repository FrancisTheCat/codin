#pragma once

#include "codin.h"

typedef struct {
  Hash_Map(String, rawptr) symbols;
  rawptr mapping;
  isize  mapping_size;
} Dynlib;

typedef Maybe(Dynlib) Dynlib_Result;

extern Dynlib_Result dynlib_load(String path, Allocator allocator);
extern bool          dynlib_unload(Dynlib const *lib);
extern rawptr        dynlib_get_symbol(Dynlib const *lib, String name);
