#pragma once
#include "codin.h"

#include "math.h"

typedef union {
  struct {
    f32 x;
    f32 y;
  };
  f32 data[2];
} Vec2;

#define vec2(_x, _y)                                                           \
  (Vec2) { .x = _x, .y = _y }

typedef union {
  struct {
    f32 x;
    f32 y;
    f32 z;
  };
  struct {
    f32 r;
    f32 g;
    f32 b;
  };
  f32 data[3];
} Vec3;

#define vec3(_x, _y, _z)                                                       \
  (Vec3) { .x = _x, .y = _y, .z = _z }

typedef union {
  struct {
    f32 x;
    f32 y;
    f32 z;
    f32 w;
  };
  struct {
    f32 r;
    f32 g;
    f32 b;
    f32 a;
  };
  f32 data[4];
} Vec4;

typedef Vec4 Color4;
typedef Vec3 Color3;

#define vec4(_x, _y, _z, _w)                                                   \
  (Vec4) { .x = _x, .y = _y, .z = _z, .w = _w }

#define GENERATE_VECTOR_OPERATIONS(D)                                          \
  Vec##D vec##D##_add(Vec##D a, Vec##D b) {                                    \
    Vec##D c;                                                                  \
    for_range(i, 0, D) { c.data[i] = a.data[i] + b.data[i]; }                  \
    return c;                                                                  \
  }                                                                            \
                                                                               \
  Vec##D vec##D##_sub(Vec##D a, Vec##D b) {                                    \
    Vec##D c;                                                                  \
    for_range(i, 0, D) { c.data[i] = a.data[i] - b.data[i]; }                  \
    return c;                                                                  \
  }                                                                            \
                                                                               \
  Vec##D vec##D##_mul(Vec##D a, Vec##D b) {                                    \
    Vec##D c;                                                                  \
    for_range(i, 0, D) { c.data[i] = a.data[i] * b.data[i]; }                  \
    return c;                                                                  \
  }                                                                            \
                                                                               \
  Vec##D vec##D##_div(Vec##D a, Vec##D b) {                                    \
    Vec##D c;                                                                  \
    for_range(i, 0, D) { c.data[i] = a.data[i] / b.data[i]; }                  \
    return c;                                                                  \
  }                                                                            \
                                                                               \
  Vec##D vec##D##_scale(Vec##D a, f32 scale) {                                 \
    for_range(i, 0, D) { a.data[i] *= scale; }                                 \
    return a;                                                                  \
  }                                                                            \
                                                                               \
  f32 vec##D##_dot(Vec##D a, Vec##D b) {                                       \
    f32 ret = 0;                                                               \
    for_range(i, 0, D) { ret += a.data[i] * b.data[i]; }                       \
    return ret;                                                                \
  }                                                                            \
                                                                               \
  f32 vec##D##_length2(Vec##D v) { return vec##D##_dot(v, v); }                \
                                                                               \
  f32 vec##D##_length(Vec##D v) { return sqrt_f32(vec##D##_length2(v)); }      \
                                                                               \
  Vec##D vec##D##_normalize(Vec##D v) {                                        \
    f32 length = vec##D##_length(v);                                           \
    return vec##D##_scale(v, 1.0 / length);                                    \
  }                                                                            \
                                                                               \
  Vec##D vec##D##_reflect(Vec##D i, Vec##D n) {                                \
    f32 dot = vec##D##_dot(n, i);                                              \
    n = vec##D##_scale(n, 2 * dot);                                            \
    return vec##D##_sub(i, n);                                                 \
  }                                                                            \
                                                                               \
  Vec##D vec##D##_lerp(Vec##D a, Vec##D b, f32 t) {                            \
    a = vec##D##_scale(a, t);                                                  \
    b = vec##D##_scale(b, 1 - t);                                              \
                                                                               \
    return vec##D##_add(a, b);                                                 \
  }                                                                            \
                                                                               \
  Vec##D vec##D##_fract(Vec##D v) {                                            \
    for_range(i, 0, D) {                                                       \
      v.data[i] = fract_f32(v.data[i]);                                        \
    }                                                                          \
    return v;                                                                  \
  }                                                                            \
                                                                               \

GENERATE_VECTOR_OPERATIONS(2)
GENERATE_VECTOR_OPERATIONS(3)
GENERATE_VECTOR_OPERATIONS(4)

Vec3 vec3_cross(Vec3 a, Vec3 b) { unimplemented(); }
