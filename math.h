#pragma once

#include "codin.h"

#define TAU         6.28318530717958647692528676655900576
#define PI          3.14159265358979323846264338327950288
#define E           2.71828182845904523536

#define SQRT_TWO    1.41421356237309504880168872420969808
#define SQRT_THREE  1.73205080756887729352744634150587236
#define SQRT_FIVE   2.23606797749978969640917366873127623

#define LN2         0.693147180559945309417232121458176568
#define LN10        2.30258509299404568401799145468436421

#define RAD_PER_DEG TAU/360.0
#define DEG_PER_RAD 360.0/TAU

#define deg_to_rad(d) ((d) * RAD_PER_DEG)
#define rad_to_deg(r) ((r) * DEG_PER_RAD)

[[nodiscard]]
extern f32 sqrt_f32(f32 x);
[[nodiscard]]
extern f64 sqrt_f64(f64 x);

[[nodiscard]]
extern f32 sin_f32(f32 x);
[[nodiscard]]
extern f64 sin_f64(f64 x);

[[nodiscard]]
extern f32 cos_f32(f32 x);
[[nodiscard]]
extern f64 cos_f64(f64 x);

[[nodiscard]]
extern f32 pow_f32(f32 x, f64 e);
[[nodiscard]]
extern f64 pow_f64(f64 x, f64 e);

[[nodiscard]]
extern f32 log_f32(f32 x, f64 base);
[[nodiscard]]
extern f64 log_f64(f64 x, f64 base);

[[nodiscard]]
extern f32 mod_f32(f32 x, f64 y);
[[nodiscard]]
extern f64 mod_f64(f64 x, f64 y);

[[nodiscard]]
extern f32 fract_f32(f32 x);
[[nodiscard]]
extern f64 fract_f64(f64 x);

[[nodiscard]]
extern f32 abs_f32(f32 x);
[[nodiscard]]
extern f64 abs_f64(f64 x);
[[nodiscard]]
extern i8 abs_i8(i8 x);
[[nodiscard]]
extern i16 abs_i16(i16 x);
[[nodiscard]]
extern i32 abs_i32(i32 x);
[[nodiscard]]
extern i64 abs_i64(i64 x);
[[nodiscard]]
extern isize abs_isize(isize x);

[[nodiscard]]
extern isize log2_isize(isize x);
