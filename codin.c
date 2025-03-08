#include "codin.h"

#include "allocators.h"
#include "log.h"
#include "os.h"
#include "runtime_linux.h"
#include "strings.h"

thread_local Growing_Arena_Allocator _default_temp_allocator_arena;
thread_local Default_Allocator       _default_heap_allocator;

extern rawptr memset(u8 *data, i32 c, isize n) {
  for_range(i, 0, n) {
    data[i] = (u8)c;
  }
  return data;
}

[[nodiscard]]
extern String format_usize_to_buffer(usize value, Byte_Slice buffer) {
    isize i = 0;

    do {
        buffer.data[i] = (value % 10) + '0';
        i += 1;
        value /= 10;
    } while (value > 0);

    isize start = 0;
    isize end = i - 1;
    while (start < end) {
        char temp = buffer.data[start];
        buffer.data[start] = buffer.data[end];
        buffer.data[end] = temp;
        start++;
        end--;
    }

    buffer.data[i] = 0;

    return (String) {
      .data = (char *)buffer.data,
      .len = i,
    };
}

extern String format_usize_to_buffer_hex(usize value, Byte_Slice buffer, b8 uppercase) {
    isize i = 0;

    if (value == 0) {
        buffer.data[0] = '0';
        i += 1;
    }

    while (value > 0) {
        isize remainder = value % 16;

        buffer.data[i] = (remainder < 10) ? (remainder + '0') : (remainder - 10 + (uppercase ? 'A': 'a'));
        i += 1;

        value /= 16;
    }

    buffer.data[i] = 0;

    // Reverse the buffer to get the final result
    isize start = 0;
    isize end = i - 1;
    while (start < end) {
        char temp = buffer.data[start];
        buffer.data[start] = buffer.data[end];
        buffer.data[end] = temp;
        start++;
        end--;
    }

    return (String) {
      .data = (char *)buffer.data,
      .len  = i
  };
}

#define slice_array(T, array) ({                                               \
   T slice;                                                                    \
   slice.data = &array[0];                                                     \
   slice.len = count_of(array);                                                \
   slice;                                                                      \
})

extern isize cstring_len(cstring s);
extern void __write_cstring(cstring str) { syscall(SYS_write, 2, str, cstring_len(str)); }
extern void __write_string(String str) { syscall(SYS_write, 2, str.data, str.len); }
extern void __write_usize_hex(usize value) {
#define N 32
  byte buf[N] = {0};
  String str = format_usize_to_buffer_hex(value, slice_array(Byte_Slice, buf), true);
  __write_string(str);
#undef N
}

extern void __write_usize(usize value) {
#define N 32
  byte buf[N] = {0};
  String str = format_usize_to_buffer(value, slice_array(Byte_Slice, buf));
  __write_string(str);
#undef N
}

extern void __write_isize(isize value) {
#define N 32
  byte buf[N + 1] = {0};
  Byte_Slice slice = slice_array(Byte_Slice, buf);
  Byte_Slice buf_ = slice;
  if (value < 0) {
    buf[0] = '-';
    buf_.data += 1;
    buf_.len  -= 1;
    value *= -1;
  }
  (void)format_usize_to_buffer(value, buf_);
  __write_cstring((cstring)buf);
#undef N
}

extern void __write_location(Source_Code_Location location) {
  __write_string(location.file);
  __write_cstring(":");
  __write_string(location.proc);
  __write_cstring("(");
  __write_usize(location.line);
  __write_cstring(")");
}

extern isize ilog2(isize x) {
  isize v = 1;
  for (; (1 << v) < x; v += 1) {
  }
  return v;
}

extern void __thread_init();
extern void __runtime_init();
extern void __thread_cleanup();
extern void __runtime_cleanup();

extern void __thread_init() {
  context.allocator      = default_allocator_init(&_default_heap_allocator);
  context.temp_allocator = growing_arena_allocator_init(&_default_temp_allocator_arena, 1 << 20, context.allocator);
  context.logger         = create_file_logger(FD_STDERR);
}

extern void __runtime_init() {
  __thread_init();
  __os_init();
}

extern void __thread_cleanup() {
  growing_arena_allocator_destroy(_default_temp_allocator_arena);
}

extern void __runtime_cleanup() { __thread_cleanup(); }

i32 main();

i32 __codin_main(i32 arg_c, cstring *arg_v) {
  __runtime_init();
  slice_init(&os_args, arg_c, context.allocator);
  slice_iter(os_args, arg, i, {
    *arg = cstring_to_string(arg_v[i]);
  });
  for (cstring *e = &arg_v[arg_c + 1]; *e; e += 1) {
    os_env.len += 1;
  }
  slice_init(&os_env, os_env.len, context.allocator);
  slice_iter(os_env, e, i, {
    *e = cstring_to_string(arg_v[arg_c + 1 + i]);
  });

  i32 ret;
  CONTEXT_GUARD({
    ret = main();
  })
  __runtime_cleanup();

  return ret;
}

f32 sqrt_f32(f32 x) {
  return sqrt_f64(x);
}

f64 sqrt_f64(f64 x) {
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
