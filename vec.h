#pragma once
#include "codin.h"

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

Vec3 vec3_add(Vec3 a, Vec3 b) {
  Vec3 c;
  for_range(i, 0, 3) { c.data[i] = a.data[i] + b.data[i]; }
  return c;
}

Vec3 vec3_sub(Vec3 a, Vec3 b) {
  Vec3 c;
  for_range(i, 0, 3) { c.data[i] = a.data[i] - b.data[i]; }
  return c;
}

Vec3 vec3_mul(Vec3 a, Vec3 b) {
  Vec3 c;
  for_range(i, 0, 3) { c.data[i] = a.data[i] * b.data[i]; }
  return c;
}

Vec3 vec3_div(Vec3 a, Vec3 b) {
  Vec3 c;
  for_range(i, 0, 3) { c.data[i] = a.data[i] / b.data[i]; }
  return c;
}

Vec3 vec3_scale(Vec3 a, f32 scale) {
  for_range(i, 0, 3) { a.data[i] *= scale; }
  return a;
}

f32 vec3_dot(Vec3 a, Vec3 b) {
  f32 ret = 0;
  for_range(i, 0, 3) { ret += a.data[i] * b.data[i]; }
  return ret;
}

Vec3 vec3_cross(Vec3 a, Vec3 b) { unimplemented(); }

f32 sqrt_f32(f32 x) { return __builtin_sqrtf(x); }

f32 vec3_length2(Vec3 v) { return vec3_dot(v, v); }

f32 vec3_length(Vec3 v) { return sqrt_f32(vec3_length2(v)); }

Vec3 vec3_normalize(Vec3 v) {
  f32 length = vec3_length(v);
  return vec3_scale(v, 1.0 / length);
}

Vec3 vec3_reflect(Vec3 i, Vec3 n) {
  f32 dot = vec3_dot(n, i);
  n = vec3_scale(n, 2 * dot);
  return vec3_sub(i, n);
}

Vec3 vec3_lerp(Vec3 a, Vec3 b, f32 t) {
  a = vec3_scale(a, t);
  b = vec3_scale(b, 1 - t);

  return vec3_add(a, b);
}

f32 fract_f32(f32 x) { return x - (f32)(isize)x; }

Vec3 vec3_fract(Vec3 v) {
  for_range(i, 0, 3) {
    v.data[i] = fract_f32(v.data[i]);
  }
  return v;
}
