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
  Vec2 xy;
  struct {
    f32 _padding;
    Vec2 yz;
  };
  f32 data[3];
} Vec3;

#define vec3(...)                                                              \
  (Vec3) { {__VA_ARGS__} }

typedef union {
  struct { f32 x, y, z, w; };
  struct { f32 r, g, b, a; };
  struct {
    f32  _padding;
    Vec2 yz;
  };
  struct {
    Vec2 xy;
    Vec2 zw;
  };
  Vec3 xyz;
  struct {
    f32  _padding3;
    Vec3 yzw;
  };
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

typedef union {
  struct {
    f32 rows[3][3];
  };
  struct {
    Vec3 row_vectors[3];
  };
  f32 data[3 * 3];
} Matrix_3x3;

#define MATRIX_3X3_IDENTITY (Matrix_3x3) { \
  .rows = {                                \
    {1, 0, 0},                             \
    {0, 1, 0},                             \
    {0, 0, 1},                             \
  },                                       \
}

internal inline Matrix_3x3 matrix_3x3_from_basis(Vec3 a, Vec3 b, Vec3 c) {
  return (Matrix_3x3) {
    .rows = {
      { a.x, b.x, c.x, },
      { a.y, b.y, c.y, },
      { a.z, b.z, c.z, },
    },
  };
}

internal inline Matrix_3x3 matrix_3x3_transpose(Matrix_3x3 m) {
  return (Matrix_3x3) {
    .rows = {
      { m.rows[0][0], m.rows[1][0], m.rows[2][0], },
      { m.rows[0][1], m.rows[1][1], m.rows[2][1], },
      { m.rows[0][2], m.rows[1][2], m.rows[2][2], },
    },
  };
}

internal inline Matrix_3x3 matrix_3x3_inverse(Matrix_3x3 m) {
  f32 a00 = m.rows[0][0], a01 = m.rows[0][1], a02 = m.rows[0][2];
  f32 a10 = m.rows[1][0], a11 = m.rows[1][1], a12 = m.rows[1][2];
  f32 a20 = m.rows[2][0], a21 = m.rows[2][1], a22 = m.rows[2][2];

  f32 det =
    a00 * (a11 * a22 - a12 * a21) -
    a01 * (a10 * a22 - a12 * a20) +
    a02 * (a10 * a21 - a11 * a20);

  if (abs_f32(det) < 0.0001f) {
    return MATRIX_3X3_IDENTITY;
  }

  f32 inv_det = 1.0f / det;

  Matrix_3x3 inv;
  inv.rows[0][0] =  (a11 * a22 - a12 * a21) * inv_det;
  inv.rows[0][1] = -(a01 * a22 - a02 * a21) * inv_det;
  inv.rows[0][2] =  (a01 * a12 - a02 * a11) * inv_det;

  inv.rows[1][0] = -(a10 * a22 - a12 * a20) * inv_det;
  inv.rows[1][1] =  (a00 * a22 - a02 * a20) * inv_det;
  inv.rows[1][2] = -(a00 * a12 - a02 * a10) * inv_det;

  inv.rows[2][0] =  (a10 * a21 - a11 * a20) * inv_det;
  inv.rows[2][1] = -(a00 * a21 - a01 * a20) * inv_det;
  inv.rows[2][2] =  (a00 * a11 - a01 * a10) * inv_det;

  return inv;
}

internal inline Vec3 matrix_3x3_mul_vec3(Matrix_3x3 m, Vec3 v) {
  return vec3(
    v.x * m.rows[0][0] + v.y * m.rows[0][1] + v.z * m.rows[0][2],
    v.x * m.rows[1][0] + v.y * m.rows[1][1] + v.z * m.rows[1][2],
    v.x * m.rows[2][0] + v.y * m.rows[2][1] + v.z * m.rows[2][2],
  );
}

internal inline Matrix_3x3 matrix_3x3_rotate(Vec3 v, f32 radians) {
  f32 c = cos_f32(radians);
  f32 s = sin_f32(radians);

  Vec3 a = vec3_normalize(v);
  Vec3 t = vec3_scale(a, 1 - c);

  Matrix_3x3 rot = MATRIX_3X3_IDENTITY;

  rot.rows[0][0] = c + t.data[0] * a.data[0];
  rot.rows[1][0] = 0 + t.data[0] * a.data[1] + s * a.data[2];
  rot.rows[2][0] = 0 + t.data[0] * a.data[2] - s * a.data[1];

  rot.rows[0][1] = 0 + t.data[1] * a.data[0] - s * a.data[2];
  rot.rows[1][1] = c + t.data[1] * a.data[1];
  rot.rows[2][1] = 0 + t.data[1] * a.data[2] + s * a.data[0];

  rot.rows[0][2] = 0 + t.data[2] * a.data[0] + s * a.data[1];
  rot.rows[1][2] = 0 + t.data[2] * a.data[1] - s * a.data[0];
  rot.rows[2][2] = c + t.data[2] * a.data[2];

  return rot;
}

typedef union {
  struct {
    f32 rows[4][4];
  };
  struct {
    Vec4 row_vectors[4];
  };
  f32 data[4 * 4];
} Matrix_4x4;

#define MATRIX_4X4_IDENTITY (Matrix_4x4) { \
  .rows = {                                \
    {1, 0, 0, 0},                          \
    {0, 1, 0, 0},                          \
    {0, 0, 1, 0},                          \
    {0, 0, 0, 1},                          \
  },                                       \
}

internal inline Vec4 matrix_4x4_mul_vec4(Matrix_4x4 m, Vec4 v) {
  return vec4(
    v.x * m.rows[0][0] + v.y * m.rows[0][1] + v.z * m.rows[0][2] + v.w * m.rows[0][3],
    v.x * m.rows[1][0] + v.y * m.rows[1][1] + v.z * m.rows[1][2] + v.w * m.rows[1][3],
    v.x * m.rows[2][0] + v.y * m.rows[2][1] + v.z * m.rows[2][2] + v.w * m.rows[2][3],
    v.x * m.rows[3][0] + v.y * m.rows[3][1] + v.z * m.rows[3][2] + v.w * m.rows[3][3],
  );
}

internal inline Matrix_4x4 matrix_4x4_mul(Matrix_4x4 a, Matrix_4x4 b) {
  Matrix_4x4 result = {0};

  for_range(row, 0, 4) {
    for_range(col, 0, 4) {
      result.rows[row][col] =
        a.rows[row][0] * b.rows[0][col] +
        a.rows[row][1] * b.rows[1][col] +
        a.rows[row][2] * b.rows[2][col] +
        a.rows[row][3] * b.rows[3][col];
    }
  }

  return result;
}

internal inline Matrix_3x3 matrix_4x4_to_3x3(Matrix_4x4 m) {
  return (Matrix_3x3) {
    .row_vectors = {
      m.row_vectors[0].xyz,
      m.row_vectors[1].xyz,
      m.row_vectors[2].xyz,
    },
  };
}

internal inline Matrix_4x4 matrix_4x4_translation_rotation_scale(Vec3 t, Vec4 q, Vec3 s) {
  f32 xx = q.x * q.x;
  f32 yy = q.y * q.y;
  f32 zz = q.z * q.z;
  
  f32 xy = q.x * q.y;
  f32 xz = q.x * q.z;
  f32 yz = q.y * q.z;
  
  f32 wx = q.w * q.x;
  f32 wy = q.w * q.y;
  f32 wz = q.w * q.z;
  
  Matrix_4x4 m = {0};

  m.rows[0][0] = (1.0f - 2.0f * (yy + zz)) * s.x;
  m.rows[0][1] = (2.0f * (xy - wz)) * s.x;
  m.rows[0][2] = (2.0f * (xz + wy)) * s.x;

  m.rows[1][0] = (2.0f * (xy + wz)) * s.y;
  m.rows[1][1] = (1.0f - 2.0f * (xx + zz)) * s.y;
  m.rows[1][2] = (2.0f * (yz - wx)) * s.y;

  m.rows[2][0] = (2.0f * (xz - wy)) * s.z;
  m.rows[2][1] = (2.0f * (yz + wx)) * s.z;
  m.rows[2][2] = (1.0f - 2.0f * (xx + yy)) * s.z;

  m.rows[0][3] = t.x;
  m.rows[1][3] = t.y;
  m.rows[2][3] = t.z;
  m.rows[3][3] = 1.0f;

  return m;
}
