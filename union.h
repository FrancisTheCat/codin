#pragma once

#include "codin.h"

#define X_UNION_FIELDS(f) f __##f;
#define X_UNION_FIELDS_BYTE(f) byte __##f;

#define X_UNION_VARIANT_NAME(f)                                                \
  case offset_of(type_of(**((u).__info)), __##f):                             \
    return LIT(#f);

#define union_to_string(T, v) (T##__to_string(v))

// __info exists to allow us to get the tag values
// it is an array of zero pointers to structs so it does not actually occupy space, nor does it change alignment since the tag is a usize anyway
#define X_UNION(Union, Variants)                                               \
  typedef struct {                                                             \
    union {                                                                    \
      Variants(X_UNION_FIELDS)                                                 \
    } variant;                                                                 \
    usize tag;                                                                 \
    struct {                                                                   \
      byte __nil_value;                                                        \
      Variants(X_UNION_FIELDS_BYTE)                                            \
    } *__info[0];                                                              \
  } Union;                                                                     \
  [[nodiscard]] internal String Union##__to_string(Union u) {                  \
    switch (u.tag) {                                                           \
      Variants(X_UNION_VARIANT_NAME)                                           \
    default:                                                                   \
      return LIT("Fuck");                                                      \
    }                                                                          \
  }

#define UNION_SET(u, T, v)                                                     \
  {                                                                            \
    (u)->variant.__##T = v;                                                    \
    (u)->tag = offset_of(type_of(**((u)->__info)), __##T);                     \
  }

#define UNION_GET(u, T)                                                        \
  ({                                                                           \
    if ((u)->tag != offset_of(type_of(**((u)->__info)), __##T)) {              \
      panic("Type assertion failure");                                         \
    }                                                                          \
    (u)->variant.__##T;                                                        \
  })

#define UNION_SWITCH(u, BLOCK)                                                 \
  {                                                                            \
    type_of(u) *__union_switch_T = &u;                                         \
    switch (__union_switch_T->tag) { BLOCK; }                                  \
  }

#define UNION_CASE(T, v, BLOCK)                                                \
  {                                                                            \
  case (offset_of(type_of(**((__union_switch_T)->__info)), __##T)): {                                                    \
    T *v = &__union_switch_T->variant.__##T;                                   \
    BLOCK;                                                                     \
  } break;                                                                     \
  }

#define UNION_IS_VARIANT(u, T)                                                 \
  ((u).tag == offset_of(type_of(**((u).__info)), __##T))
