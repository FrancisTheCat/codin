#include "codin.h"

#define enum_to_string(T, value)                                               \
  __##T##_to_string(value)                                                     \


#define enum_to_string_proc(T) __##T##_to_string

#define ENUM_TO_STRING_PROC_DECL(T, v)                                         \
  [[nodiscard]]                                                                \
  internal String enum_to_string_proc(T)(T v)
