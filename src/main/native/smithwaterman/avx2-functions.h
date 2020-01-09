#ifndef _AVX2_FUNCTIONS_H
#define _AVX2_FUNCTIONS_H

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

#define VEC_LENGTH 8

#define VEC_INT_TYPE __m256i
#define VEC_MASK_TYPE __m256i

#define VEC_LOADU(__addr) \
    _mm256_loadu_si256((__m256i *)(__addr))

#define VEC_STORE(__addr, __v) \
    _mm256_store_si256((__m256i *)(__addr), __v)

#define VEC_STOREU(__addr, __v) \
    _mm256_storeu_si256((__m256i *)(__addr), __v)

#define VEC_STREAM(__addr, __v) \
    _mm256_stream_si256((__m256i *)(__addr), __v)


#define VEC_SET_ZERO() \
    _mm256_setzero_si256()

#define VEC_SET1_VAL32(__val) \
    _mm256_set1_epi32(__val)

#define VEC_ADD(__v1, __v2) \
    _mm256_add_epi32(__v1, __v2)

#define VEC_MAX(__v1, __v2) \
    _mm256_max_epi32(__v1, __v2)
    
#define VEC_CMPGT(__v1, __v2) \
    _mm256_cmpgt_epi32(__v1, __v2)

#define VEC_CMPGT_MASK(__v1, __v2) \
    _mm256_cmpgt_epi32(__v1, __v2)

#define VEC_CMPEQ(__v1, __v2) \
    _mm256_cmpeq_epi32(__v1, __v2)

#define VEC_CMPEQ_MASK(__v1, __v2) \
    _mm256_cmpeq_epi32(__v1, __v2)

#define VEC_AND(__v1, __v2) \
    _mm256_and_si256(__v1, __v2)

#define VEC_OR(__v1, __v2) \
    _mm256_or_si256(__v1, __v2)

#define VEC_ANDNOT(__v1, __v2) \
    _mm256_andnot_si256(__v1, __v2)

#define VEC_BLEND(__v1, __v2, __mask) \
    (__m256i)_mm256_blendv_ps((__m256)__v1, (__m256)__v2, (__m256)__mask)


#define VEC_PERMUTE2x128_EVEN(__v1, __v2) \
    _mm256_permute2f128_si256(__v1, __v2, 0x20)    

#define VEC_PERMUTE2x128_ODD(__v1, __v2) \
    _mm256_permute2f128_si256(__v1, __v2, 0x31)    

#define VEC_PACKS_32(__v1, __v2) \
    _mm256_packs_epi32(__v1, __v2)

#define INIT_CONSTANTS 

#endif
