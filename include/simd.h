#include "codin.h"

#include "immintrin.h"

// SSE

typedef __m128d f64x2;
typedef __m128  f32x4;

typedef __m128i i64x2;
typedef __m128i i32x4;
typedef __m128i i16x8;
typedef __m128i i8x16;


#define F64X2_ZERO _mm_setzero_pd()
#define F32X4_ZERO _mm_setzero_ps()

#define I64X2_ZERO _mm_setzero_si128()
#define I32X4_ZERO _mm_setzero_si128()
#define I16X8_ZERO _mm_setzero_si128()
#define I8X16_ZERO _mm_setzero_si128()


#define f64x2_broadcast(x) _mm_set1_pd(x)
#define f32x4_broadcast(x) _mm_set1_ps(x)

#define i64x2_broadcast(x) _mm_set1_epi64x(x)
#define i32x4_broadcast(x) _mm_set1_epi32(x)
#define i16x8_broadcast(x) _mm_set1_epi16(x)
#define i8x16_broadcast(x) _mm_set1_epi8(x)


#define f64x2_load(p) _mm_load_pd((f64x2 *)(p))
#define f32x4_load(p) _mm_load_ps((f32x4 *)(p))

#define i64x2_load(p) _mm_load_si64( (i64x2 *)(p))
#define i32x4_load(p) _mm_load_si32( (i32x4 *)(p))
#define i16x8_load(p) _mm_load_si16( (i16x8 *)(p))
#define i8x16_load(p) _mm_load_si128((i8x16 *)(p))


#define f64x2_load_unaligned(p) _mm_loadu_pd((f64x2 *)(p))
#define f32x4_load_unaligned(p) _mm_loadu_ps((f32x4 *)(p))

#define i64x2_load_unaligned(p) _mm_loadu_si64( (i64x2 *)(p))
#define i32x4_load_unaligned(p) _mm_loadu_si32( (i32x4 *)(p))
#define i16x8_load_unaligned(p) _mm_loadu_si16( (i16x8 *)(p))
#define i8x16_load_unaligned(p) _mm_loadu_si128((i8x16 *)(p))


#define f64x2_store(p, v) _mm_store_pd((f64 *)(p), (v))
#define f32x4_store(p, v) _mm_store_ps((f32 *)(p), (v))

#define i64x2_store(p, v) _mm_store_si128((i8x16 *)(p), (v))
#define i32x4_store(p, v) _mm_store_si128((i8x16 *)(p), (v))
#define i16x8_store(p, v) _mm_store_si128((i8x16 *)(p), (v))
#define i8x16_store(p, v) _mm_store_si128((i8x16 *)(p), (v))


#define f64x2_store_unaligned(p, v) _mm_storeu_pd((f64 *)(p), (v))
#define f32x4_store_unaligned(p, v) _mm_storeu_ps((f32 *)(p), (v))

#define i64x2_store_unaligned(p, v) _mm_storeu_si128((i8x16 *)(p), (v))
#define i32x4_store_unaligned(p, v) _mm_storeu_si128((i8x16 *)(p), (v))
#define i16x8_store_unaligned(p, v) _mm_storeu_si128((i8x16 *)(p), (v))
#define i8x16_store_unaligned(p, v) _mm_storeu_si128((i8x16 *)(p), (v))

// AVX-2

typedef __m256d f64x4;
typedef __m256  f32x8;

typedef __m256i i64x4;
typedef __m256i i32x8;
typedef __m256i i16x16;
typedef __m256i i8x32;


#define F64X4_ZERO _mm256_setzero_pd()
#define F32X8_ZERO _mm256_setzero_ps()

#define I64X4_ZERO  _mm256_setzero_si256()
#define I32X8_ZERO  _mm256_setzero_si256()
#define I16X16_ZERO _mm256_setzero_si256()
#define I8X32_ZERO  _mm256_setzero_si256()


#define f64x4_broadcast(x) _mm256_set1_pd(x)
#define f32x8_broadcast(x) _mm256_set1_ps(x)

#define i64x4_broadcast(x)  _mm256_set1_epi64x(x)
#define i32x8_broadcast(x)  _mm256_set1_epi32(x)
#define i16x16_broadcast(x) _mm256_set1_epi16(x)
#define i8x32_broadcast(x)  _mm256_set1_epi8(x)


#define f64x4_load(p) _mm256_load_pd((f64x2 *)(p))
#define f32x8_load(p) _mm256_load_ps((f32x4 *)(p))

#define i64x4_load(p)  _mm256_load_si64( (i64x2 *)(p))
#define i32x8_load(p)  _mm256_load_si32( (i32x4 *)(p))
#define i16x16_load(p) _mm256_load_si16( (i16x8 *)(p))
#define i8x32_load(p)  _mm256_load_si128((i8x16 *)(p))


#define f64x4_load_unaligned(p) _mm256_loadu_pd((f64x2 *)(p))
#define f32x8_load_unaligned(p) _mm256_loadu_ps((f32x4 *)(p))

#define i64x4_load_unaligned(p)  _mm256_loadu_si64( (i64x2 *)(p))
#define i32x8_load_unaligned(p)  _mm256_loadu_si32( (i32x4 *)(p))
#define i16x16_load_unaligned(p) _mm256_loadu_si16( (i16x8 *)(p))
#define i8x32_load_unaligned(p)  _mm256_loadu_si128((i8x16 *)(p))


#define f64x4_store(p, v) _mm256_store_pd((f64 *)(p), (v))
#define f32x8_store(p, v) _mm256_store_ps((f32 *)(p), (v))

#define i64x4_store(p, v)  _mm256_store_si64((i64 *)(p), (v))
#define i32x8_store(p, v)  _mm256_store_si32((i32 *)(p), (v))
#define i16x16_store(p, v) _mm256_store_si16((i16 *)(p), (v))
#define i8x32_store(p, v)  _mm256_store_si128((i8 *)(p), (v))


#define f64x4_store_unaligned(p, v) _mm256_storeu_pd((f64 *)(p), (v))
#define f32x8_store_unaligned(p, v) _mm256_storeu_ps((f32 *)(p), (v))

#define i64x4_store_unaligned(p, v)  _mm256_storeu_si64((i64 *)(p), (v))
#define i32x8_store_unaligned(p, v)  _mm256_storeu_si32((i32 *)(p), (v))
#define i16x16_store_unaligned(p, v) _mm256_storeu_si16((i16 *)(p), (v))
#define i8x32_store_unaligned(p, v)  _mm256_storeu_si128((i8 *)(p), (v))

// AVX-512

typedef __m512d f64x8;
typedef __m512  f32x16;

typedef __m512i i64x8;
typedef __m512i i32x16;
typedef __m512i i16x32;
typedef __m512i i8x64;


#define F64X8_ZERO  _mm512_setzero_pd()
#define F32X16_ZERO _mm512_setzero_ps()

#define I64X8_ZERO  _mm512_setzero_si512()
#define I32X16_ZERO _mm512_setzero_si512()
#define I16X32_ZERO _mm512_setzero_si512()
#define I8X64_ZERO  _mm512_setzero_si512()


#define f64x8_broadcast(x)  _mm512_set1_pd(x)
#define f32x16_broadcast(x) _mm512_set1_ps(x)

#define i64x8_broadcast(x)  _mm512_set1_epi64x(x)
#define i32x16_broadcast(x) _mm512_set1_epi32(x)
#define i16x32_broadcast(x) _mm512_set1_epi16(x)
#define i8x64_broadcast(x)  _mm512_set1_epi8(x)


#define f64x8_load(p)  _mm512_load_pd((f64x4 *)(p))
#define f32x16_load(p) _mm512_load_ps((f32x8 *)(p))

#define i64x8_load(p)  _mm512_load_si64( (i64x4  *)(p))
#define i32x16_load(p) _mm512_load_si32( (i32x8  *)(p))
#define i16x32_load(p) _mm512_load_si16( (i16x16 *)(p))
#define i8x64_load(p)  _mm512_load_si128((i8x32  *)(p))


#define f64x8_load_unaligned(p)  _mm512_loadu_pd((f64x4 *)(p))
#define f32x16_load_unaligned(p) _mm512_loadu_ps((f32x8 *)(p))

#define i64x8_load_unaligned(p)  _mm512_loadu_si64( (i64x4  *)(p))
#define i32x16_load_unaligned(p) _mm512_loadu_si32( (i32x8  *)(p))
#define i16x32_load_unaligned(p) _mm512_loadu_si16( (i16x16 *)(p))
#define i8x64_load_unaligned(p)  _mm512_loadu_si128((i8x32  *)(p))


#define f64x8_store(p, v)  _mm512_store_pd((f64 *)(p), (v))
#define f32x16_store(p, v) _mm512_store_ps((f32 *)(p), (v))

#define i64x8_store(p, v)  _mm512_store_si128((i8x64 *)(p), (v))
#define i32x16_store(p, v) _mm512_store_si128((i8x64 *)(p), (v))
#define i16x32_store(p, v) _mm512_store_si128((i8x64 *)(p), (v))
#define i8x64_store(p, v)  _mm512_store_si128((i8x64 *)(p), (v))


#define f64x8_store_unaligned(p, v)  _mm512_storeu_pd((f64 *)(p), (v))
#define f32x16_store_unaligned(p, v) _mm512_storeu_ps((f32 *)(p), (v))

#define i64x8_store_unaligned(p, v)  _mm512_storeu_i128((i8x64 *)(p), (v))
#define i32x16_store_unaligned(p, v) _mm512_storeu_i128((i8x64 *)(p), (v))
#define i16x32_store_unaligned(p, v) _mm512_storeu_i128((i8x64 *)(p), (v))
#define i8x64_store_unaligned(p, v)  _mm512_storeu_i128((i8x64 *)(p), (v))
