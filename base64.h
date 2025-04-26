#pragma once

#include "codin.h"

extern String base64_encode(Byte_Slice data, Allocator allocator);
extern bool   base64_decode(String data, Byte_Slice *out, Allocator allocator);
