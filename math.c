#include "math.h"

extern f64 sqrt(f64);
extern f32 sqrtf(f32);

extern f32 sqrt_f32(f32 x) {
  return sqrtf(x);
  // return sqrt_f64(x);
}

extern f64 sqrt_f64(f64 x) {
  return sqrt(x);

  internal const double tiny = 1.0e-300;

  f64 z;
  i32 sign = (int)0x80000000;
  i32 ix0,s0,q,m,t,i;
  u32 r,t1,s1,ix1,q1;

  #define EXTRACT_WORDS(hi,lo,d)                                  \
  do {                                                            \
    union {u64 bits; f64 value; } __u;                            \
    __u.value = (d);                                              \
    (hi) = __u.bits >> 32;                                        \
    (lo) = (u32)__u.bits;                                         \
  } while (0)

  #define INSERT_WORDS(d,hi,lo)                                   \
  do {                                                            \
    union {u64 bits; f64 value; } __u;                            \
    __u.bits = ((u64)(hi) << 32) | (u32)(lo);                     \
    (d) = __u.value;                                              \
  } while (0)

  EXTRACT_WORDS(ix0, ix1, x);

  /* take care of Inf and NaN */
  if ((ix0&0x7ff00000) == 0x7ff00000) {
    return x*x + x;  /* sqrt(NaN)=NaN, sqrt(+inf)=+inf, sqrt(-inf)=sNaN */
  }
  /* take care of zero */
  if (ix0 <= 0) {
    if (((ix0&~sign)|ix1) == 0)
      return x;  /* sqrt(+-0) = +-0 */
    if (ix0 < 0)
      return (x-x)/(x-x);  /* sqrt(-ve) = sNaN */
  }
  /* normalize x */
  m = ix0>>20;
  if (m == 0) {  /* subnormal x */
    while (ix0 == 0) {
      m -= 21;
      ix0 |= (ix1>>11);
      ix1 <<= 21;
    }
    for (i=0; (ix0&0x00100000) == 0; i++)
      ix0<<=1;
    m -= i - 1;
    ix0 |= ix1>>(32-i);
    ix1 <<= i;
  }
  m -= 1023;    /* unbias exponent */
  ix0 = (ix0&0x000fffff)|0x00100000;
  if (m & 1) {  /* odd m, double x to make it even */
    ix0 += ix0 + ((ix1&sign)>>31);
    ix1 += ix1;
  }
  m >>= 1;      /* m = [m/2] */

  /* generate sqrt(x) bit by bit */
  ix0 += ix0 + ((ix1&sign)>>31);
  ix1 += ix1;
  q = q1 = s0 = s1 = 0;  /* [q,q1] = sqrt(x) */
  r = 0x00200000;        /* r = moving bit from right to left */

  while (r != 0) {
    t = s0 + r;
    if (t <= ix0) {
      s0   = t + r;
      ix0 -= t;
      q   += r;
    }
    ix0 += ix0 + ((ix1&sign)>>31);
    ix1 += ix1;
    r >>= 1;
  }

  r = sign;
  while (r != 0) {
    t1 = s1 + r;
    t  = s0;
    if (t < ix0 || (t == ix0 && t1 <= ix1)) {
      s1 = t1 + r;
      if ((t1&sign) == sign && (s1&sign) == 0)
        s0++;
      ix0 -= t;
      if (ix1 < t1)
        ix0--;
      ix1 -= t1;
      q1 += r;
    }
    ix0 += ix0 + ((ix1&sign)>>31);
    ix1 += ix1;
    r >>= 1;
  }

  /* use floating add to find out rounding direction */
  if ((ix0|ix1) != 0) {
    z = 1.0 - tiny; /* raise inexact flag */
    if (z >= 1.0) {
      z = 1.0 + tiny;
      if (q1 == (u32)0xffffffff) {
        q1 = 0;
        q++;
      } else if (z > 1.0) {
        if (q1 == (u32)0xfffffffe)
          q++;
        q1 += 2;
      } else
        q1 += q1 & 1;
    }
  }
  ix0 = (q>>1) + 0x3fe00000;
  ix1 = q1>>1;
  if (q&1)
    ix1 |= sign;
  ix0 += m << 20;
  INSERT_WORDS(z, ix0, ix1);
  return z;
}

extern f32 abs_f32(f32 x) { return x >= 0 ? x : -x; }
extern f64 abs_f64(f64 x) { return x >= 0 ? x : -x; }
extern i8  abs_i8 (i8  x) { return x >= 0 ? x : -x; }
extern i16 abs_i16(i16 x) { return x >= 0 ? x : -x; }
extern i32 abs_i32(i32 x) { return x >= 0 ? x : -x; }
extern i64 abs_i64(i64 x) { return x >= 0 ? x : -x; }
extern isize abs_isize(isize x) { return x >= 0 ? x : -x; }

extern isize log2_isize(isize x) {
  isize v = 1;
  for (; (1 << v) < x; v += 1);
  return v;
}

extern f32 fract_f32(f32 x) { return x - (isize)x; }
extern f64 fract_f64(f64 x) { return x - (isize)x; }

extern f32 sinf(f32);
extern f64 sin(f64);
extern f32 cosf(f32);
extern f64 cos(f64);
extern f32 tanf(f32);
extern f64 tan(f64);

extern f32 asinf(f32);
extern f64 asin(f64);
extern f32 acosf(f32);
extern f64 acos(f64);
extern f32 atanf(f32);
extern f64 atan(f64);
extern f32 atan2f(f32, f32);
extern f64 atan2(f64, f64);

extern f32 sin_f32(f32 x) {
  return sinf(x);
}
extern f64 sin_f64(f64 x) {
  return sin(x);
}

extern f32 cos_f32(f32 x) {
  return cosf(x);
}
extern f64 cos_f64(f64 x) {
  return cos(x);
}

extern f32 tan_f32(f32 x) {
  return tanf(x);
}
extern f64 tan_f64(f64 x) {
  return tan(x);
}

extern f32 asin_f32(f32 x) {
  return asinf(x);
}
extern f64 asin_f64(f64 x) {
  return asin(x);
}

extern f32 acos_f32(f32 x) {
  return acosf(x);
}
extern f64 acos_f64(f64 x) {
  return acos(x);
}

extern f32 atan_f32(f32 x) {
  return atanf(x);
}
extern f64 atan_f64(f64 x) {
  return atan(x);
}

extern f32 atan2_f32(f32 x, f32 y) {
  return atan2f(x, y);
}
extern f64 atan2_f64(f64 x, f64 y) {
  return atan2(x, y);
}

extern f32 powf(f32, f32);
extern f64 pow(f64, f64);

extern f32 pow_f32(f32 x, f32 y) {
  return powf(x, y);
}
extern f64 pow_f64(f64 x, f64 y) {
  return pow(x, y);
}
