#pragma once

#include "codin.h"

#define enum_to_string(T, value)                                               \
  enum_to_string_proc(T)(value)                                                \

#define enum_len(T)                                                            \
  T##__Max_Value                                                               \

#define enum_is_valid(T, v) enum_is_valid_proc(T)(v)

#define enum_iter(T, i)                                                        \
  for (                                                                        \
    T i = (T)({                                                                \
      T start = (T)0;                                                          \
      while (                                                                  \
        !enum_is_valid(T, start) &&                                            \
        (i32)start < (isize)enum_len(T)                                        \
      ) {                                                                      \
        *(i32 *)&start += 1;                                                   \
      }                                                                        \
      start;                                                                   \
    });                                                                        \
    (isize)i < (isize)enum_len(T);                                             \
    ({                                                                         \
      do {                                                                     \
        *(i32 *)&i += 1;                                                       \
      } while (!enum_is_valid(T, i) && (i32)i < (i32)enum_len(T));             \
    })                                                                         \
  )


#define enum_to_string_proc(T) T##__to_string
#define enum_is_valid_proc(T) T##__is_valid

#define ENUM_IS_VALID_PROC_DECL(T, v)                                          \
  [[nodiscard]]                                                                \
  internal b8 enum_is_valid_proc(T)(T v)

#define ENUM_TO_STRING_PROC_DECL(T, v)                                         \
  [[nodiscard]]                                                                \
  internal String enum_to_string_proc(T)(T v)

#define X_ENUM_TO_STRING_CASES(v) case v: return LIT(#v);
#define X_ENUM_TO_STRING(Enum, Variants) ENUM_TO_STRING_PROC_DECL(Enum, v) {   \
  switch (v) {                                                                 \
    Variants(X_ENUM_TO_STRING_CASES);                                          \
  default:                                                                     \
    return LIT(#Enum"_Invalid");                                               \
  }                                                                            \
}

#define X_ENUM_IS_VALID_CASES(v) case v: return true;
#define X_ENUM_IS_VALID(Enum, Variants)                                        \
  ENUM_IS_VALID_PROC_DECL(Enum, v) {                                           \
    switch (v) {                                                               \
      Variants(X_ENUM_IS_VALID_CASES);                                         \
    default:                                                                   \
      return false;                                                            \
    }                                                                          \
  }

#define X_ENUM_VARIANTS(v) v,
#define X_ENUM(Enum, Variants)                                                 \
typedef enum {                                                                 \
  Variants(X_ENUM_VARIANTS)                                                    \
  Enum##__Max_Value,                                                           \
} Enum;                                                                        \
                                                                               \
X_ENUM_TO_STRING(Enum, Variants)                                               \
X_ENUM_IS_VALID(Enum, Variants)

#define X_ENUM_TO_STRING_CASES_EXPLICIT(v, _n) case v: return LIT(#v);
#define X_ENUM_TO_STRING_EXPLICIT(Enum, Variants)                              \
  ENUM_TO_STRING_PROC_DECL(Enum, v) {                                          \
    switch (v) {                                                               \
      Variants(X_ENUM_TO_STRING_CASES_EXPLICIT);                               \
    default:                                                                   \
      return LIT(#Enum"_Invalid");                                             \
    }                                                                          \
  }

#define X_ENUM_IS_VALID_CASES_EXPLICIT(v, _n) case v: return true;
#define X_ENUM_IS_VALID_EXPLICIT(Enum, Variants)                               \
  ENUM_IS_VALID_PROC_DECL(Enum, v) {                                           \
    switch (v) {                                                               \
      Variants(X_ENUM_IS_VALID_CASES_EXPLICIT);                                \
    default:                                                                   \
      return false;                                                            \
    }                                                                          \
  }

#define X_ENUM_VARIANTS_EXPLICIT(v, n) v = n,
#define X_ENUM_EXPLICIT(Enum, Variants)                                        \
typedef enum {                                                                 \
  Variants(X_ENUM_VARIANTS_EXPLICIT)                                           \
  __##Enum##_Max_Value,                                                        \
} Enum;                                                                        \
                                                                               \
X_ENUM_TO_STRING_EXPLICIT(Enum, Variants)                                      \
X_ENUM_IS_VALID_EXPLICIT(Enum, Variants)
