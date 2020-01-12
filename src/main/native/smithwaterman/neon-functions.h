#ifndef _NEON_FUNCTIONS_H
#define _NEON_FUNCTIONS_H

#undef VEC_LENGTH
#undef VEC_INT_TYPE
#undef VEC_LOADU
#undef VEC_STORE
#undef VEC_STOREU
#undef VEC_STREAM
#undef VEC_SET_ZERO
#undef VEC_SET1_VAL32
#undef VEC_ADD
#undef VEC_MAX
#undef VEC_CMPGT
#undef VEC_CMPEQ
#undef VEC_AND
#undef VEC_OR
#undef VEC_ANDNOT
#undef VEC_BLEND
#undef VEC_PERMUTE2x128
#undef VEC_PACKS_32

#define VEC_LENGTH 4

#define VEC_INT_TYPE int32x4_t
#define VEC_MASK_TYPE uint32x4_t
#define VEC_INT_TYPE2 int16x8_t

#define VEC_LOADU(__addr) \
    vld1q_s32((const int32_t *) __addr)
//  __m128i _mm_loadu_si128 (__m128i const* __addr)
//  _mm256_loadu_si256((__m256i *)(__addr))

#define VEC_STORE(__addr, __v) \
    vst1q_s32((int32_t *) __addr, __v)
//  void _mm_store_si128 (__m128i* __addr, __m128i __v)
//  _mm256_store_si256((__m256i *)(__addr), __v)

#define VEC_STOREU(__addr, __v) \
    vst1q_s32((int32_t *) __addr, __v)
//  void _mm_storeu_si128 (__m128i* __addr, __m128i __v)
//  _mm256_storeu_si256((__m256i *)(__addr), __v)

#define VEC_STREAM(__addr, __v) \
    vst1q_s16((int16_t *) __addr, __v)
//  void _mm_stream_si128 (__m128i* __addr, __m128i __v)
//  _mm256_stream_si256((__m256i *)(__addr), __v)

#define VEC_SET_ZERO() \
    vdupq_n_s32(0)
//  __m128i _mm_setzero_si128 ()
//  _mm256_setzero_si256()

#define VEC_SET1_VAL32(__val) \
    vdupq_n_s32(__val)
//  __m128i _mm_set1_epi32 (int __val)
//  _mm256_set1_epi32(__val)

#define VEC_ADD(__v1, __v2) \
    vaddq_s32(__v1, __v2)
//  __m128i _mm_add_epi32 (__m128i __v1, __m128i __v2)
//  _mm256_add_epi32(__v1, __v2)

#define VEC_MAX(__v1, __v2) \
    vmaxq_s32(__v1, __v2)
//  __m128i _mm_max_epi32 (__m128i __v1, __m128i __v2)
//  _mm256_max_epi32(__v1, __v2)

#define VEC_CMPGT(__v1, __v2) \
    vreinterpretq_s32_u32(vcgtq_s32(__v1, __v2))
//  __m128i _mm_cmpgt_epi32 (__m128i __v1, __m128i __v2)
//  _mm256_cmpgt_epi32(__v1, __v2)

#define VEC_CMPGT_MASK(__v1, __v2) \
    vcgtq_s32(__v1, __v2)
//  __m128i _mm_cmpgt_epi32 (__m128i __v1, __m128i __v2)
//  _mm256_cmpgt_epi32(__v1, __v2)

#define VEC_CMPEQ(__v1, __v2) \
    vreinterpretq_s32_u32(vceqq_s32(__v1, __v2))
//  __m128i _mm_cmpeq_epi32 (__m128i __v1, __m128i __v2)
//  _mm256_cmpeq_epi32(__v1, __v2)

#define VEC_CMPEQ_MASK(__v1, __v2) \
    vceqq_s32(__v1, __v2)
//  __m128i _mm_cmpeq_epi32 (__m128i __v1, __m128i __v2)
//  _mm256_cmpeq_epi32(__v1, __v2)

#define VEC_AND(__v1, __v2) \
    vandq_s32(__v1, __v2)
//  __m128i _mm_and_si128 (__m128i __v1, __m128i __v2)
//  _mm256_and_si256(__v1, __v2)

#define VEC_OR(__v1, __v2) \
    vorrq_s32(__v1, __v2)
//  __m128i _mm_or_si128 (__m128i __v1, __m128i __v2)
//  _mm256_or_si256(__v1, __v2)

#define VEC_ANDNOT(__v1, __v2) \
    vbicq_s32(__v2, __v1)
//  __m128i _mm_andnot_si128 (__m128i __v1, __m128i __v2)
//  _mm256_andnot_si256(__v1, __v2)

#define VEC_BLEND(__v1, __v2, __mask) \
    vbslq_s32(__mask, __v2, __v1)
//  __m128 _mm_blendv_ps (__m128 __v1, __m128 __v2, __m128 mask)
//  (__m256i)_mm256_blendv_ps((__m256)__v1, (__m256)__v2, (__m256)__mask)

#define VEC_PERMUTE2x128_EVEN(__v1, __v2) \
    vreinterpretq_s32_s64(vtrn1q_s64(vreinterpretq_s64_s32(__v1), vreinterpretq_s64_s32(__v2)))
//  _mm256_permute2f128_si256(__v1, __v2, 0x20)

#define VEC_PERMUTE2x128_ODD(__v1, __v2) \
    vreinterpretq_s32_s64(vtrn2q_s64(vreinterpretq_s64_s32(__v1), vreinterpretq_s64_s32(__v2)))
//  _mm256_permute2f128_si256(__v1, __v2, 0x31)

#define VEC_PACKS_32(__v1, __v2) \
    vcombine_s16(vqmovn_s32(__v1), vqmovn_s32(__v2))
//  __m128i _mm_packs_epi32 (__m128i __v1, __m128i __v2)
//  _mm256_packs_epi32(__v1, __v2)

#define INIT_CONSTANTS

#endif
