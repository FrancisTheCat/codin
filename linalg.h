#pragma once
#include "codin.h"

#include "math.h"

typedef union {
  struct { f32 x, y; };
  f32 data[2];
} Vec2;

#define vec2(...)                                                              \
  (Vec2) { {__VA_ARGS__} }

typedef union {
  struct { f32 x, y, z; };
  struct { f32 r, g, b; };
  f32 data[3];
} Vec3;

#define vec3(...)                                                              \
  (Vec3) { {__VA_ARGS__} }

typedef union {
  struct { f32 x, y, z, w; };
  struct { f32 r, g, b, a; };
  f32 data[4];
} Vec4;

typedef Vec4 Color4;
typedef Vec3 Color3;

#define vec4(...)                                                              \
  (Vec4) { {__VA_ARGS__} }

#define GENERATE_VECTOR_OPERATIONS(D)                                          \
  internal Vec##D vec##D##_add(Vec##D a, Vec##D b) {                           \
    Vec##D c;                                                                  \
    for_range(i, 0, D) { c.data[i] = a.data[i] + b.data[i]; }                  \
    return c;                                                                  \
  }                                                                            \
                                                                               \
  internal Vec##D vec##D##_sub(Vec##D a, Vec##D b) {                           \
    Vec##D c;                                                                  \
    for_range(i, 0, D) { c.data[i] = a.data[i] - b.data[i]; }                  \
    return c;                                                                  \
  }                                                                            \
                                                                               \
  internal Vec##D vec##D##_mul(Vec##D a, Vec##D b) {                           \
    Vec##D c;                                                                  \
    for_range(i, 0, D) { c.data[i] = a.data[i] * b.data[i]; }                  \
    return c;                                                                  \
  }                                                                            \
                                                                               \
  internal Vec##D vec##D##_div(Vec##D a, Vec##D b) {                           \
    Vec##D c;                                                                  \
    for_range(i, 0, D) { c.data[i] = a.data[i] / b.data[i]; }                  \
    return c;                                                                  \
  }                                                                            \
                                                                               \
  internal Vec##D vec##D##_scale(Vec##D a, f32 scale) {                        \
    for_range(i, 0, D) { a.data[i] *= scale; }                                 \
    return a;                                                                  \
  }                                                                            \
                                                                               \
  internal f32 vec##D##_dot(Vec##D a, Vec##D b) {                              \
    f32 ret = 0;                                                               \
    for_range(i, 0, D) { ret += a.data[i] * b.data[i]; }                       \
    return ret;                                                                \
  }                                                                            \
                                                                               \
  internal f32 vec##D##_length2(Vec##D v) {                                    \
    return vec##D##_dot(v, v);                                                 \
  }                                                                            \
                                                                               \
  internal f32 vec##D##_length(Vec##D v) {                                     \
    return sqrt_f32(vec##D##_length2(v));                                      \
  }                                                                            \
                                                                               \
  internal Vec##D vec##D##_normalize(Vec##D v) {                               \
    f32 length = vec##D##_length(v);                                           \
    return vec##D##_scale(v, 1.0 / length);                                    \
  }                                                                            \
                                                                               \
  internal Vec##D vec##D##_reflect(Vec##D i, Vec##D n) {                       \
    f32 dot = vec##D##_dot(n, i);                                              \
    n = vec##D##_scale(n, 2 * dot);                                            \
    return vec##D##_sub(i, n);                                                 \
  }                                                                            \
                                                                               \
  internal Vec##D vec##D##_lerp(Vec##D a, Vec##D b, f32 t) {                   \
    a = vec##D##_scale(a, 1 - t);                                              \
    b = vec##D##_scale(b,     t);                                              \
                                                                               \
    return vec##D##_add(a, b);                                                 \
  }                                                                            \
                                                                               \
  internal Vec##D vec##D##_fract(Vec##D v) {                                   \
    for_range(i, 0, D) {                                                       \
      v.data[i] = fract_f32(v.data[i]);                                        \
    }                                                                          \
    return v;                                                                  \
  }                                                                            \
                                                                               \
  internal Vec##D vec##D##_clamp(Vec##D x, Vec##D a, Vec##D b) {               \
    for_range(i, 0, D) {                                                       \
      x.data[i] = clamp(x.data[i], a.data[i], b.data[i]);                      \
    }                                                                          \
    return x;                                                                  \
  }                                                                            \
                                                                               \
  internal Vec##D vec##D##_broadcast(f32 a) {                                  \
    Vec##D v;                                                                  \
    for_range(i, 0, D) {                                                       \
      v.data[i] = a;                                                           \
    }                                                                          \
    return v;                                                                  \
  }                                                                            \
                                                                               \

// NOLINTBEGIN
GENERATE_VECTOR_OPERATIONS(2)
GENERATE_VECTOR_OPERATIONS(3)
GENERATE_VECTOR_OPERATIONS(4)
// NOLINTEND

internal Vec3 vec3_cross(Vec3 a, Vec3 b) {
  return (Vec3) {
    .x = a.y * b.z - a.z * b.y,
    .y = a.z * b.x - a.x * b.z,
    .z = a.x * b.y - a.y * b.x
  };
}
