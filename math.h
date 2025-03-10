#pragma once

#include "codin.h"

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
