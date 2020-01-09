#ifdef PRECISION
#undef PRECISION
#undef MAIN_TYPE
#undef MAIN_TYPE_SIZE
#undef UNION_TYPE
#undef IF_128
#undef IF_MAIN_TYPE
#undef SHIFT_CONST1
#undef SHIFT_CONST2
#undef SHIFT_CONST3
#undef _128_TYPE
#undef SIMD_TYPE
#undef NEON_LENGTH
#undef HAP_TYPE
#undef MASK_TYPE
#undef MASK_ALL_ONES
#undef MASK_VEC

#undef SET_VEC_ZERO
#undef VEC_OR
#undef VEC_ADD
#undef VEC_SUB
#undef VEC_MUL
#undef VEC_DIV
#undef VEC_BLEND
#undef VEC_BLENDV
#undef VEC_CAST_256_128
#undef VEC_EXTRACT_128
#undef VEC_EXTRACT_UNIT
#undef VEC_SET1_VAL128
#undef VEC_MOVE
#undef VEC_CAST_128_256
#undef VEC_INSERT_VAL
#undef VEC_INSERT_UNIT
#undef VEC_CVT_128_256
#undef VEC_SET1_VAL
#undef VEC_POPCVT_CHAR
#undef VEC_LDPOPCVT_CHAR
#undef VEC_CMP_EQ
#undef VEC_SET_LSE
#undef SHIFT_HAP
#undef VEC_SSE_TO_AVX
#undef VEC_SHIFT_LEFT_1BIT
#undef VEC_128_SHIFT_LEFT
#undef COMPARE_VECS
#undef BITMASK_VEC
#endif

#define PRECISION d
#define MAIN_TYPE double
#define MAIN_TYPE_SIZE 64
#define UNION_TYPE mix_D128
#define IF_128 IF_128d
#define IF_MAIN_TYPE IF_64
#define SHIFT_CONST1 1
#define SHIFT_CONST2 8
#define SHIFT_CONST3 0
#define _128_TYPE float64x2_t
#define SIMD_TYPE float64x2_t
#define NEON_LENGTH 2
#define HAP_TYPE UNION_TYPE
#define MASK_TYPE uint64_t
#define MASK_ALL_ONES 0xFFFFFFFFFFFFFFFFL
#define MASK_VEC MaskVec_D128

#define SET_VEC_ZERO(__vec)                     \
    __vec= _mm256_setzero_pd()

#define VEC_OR(__v1, __v2)                      \
    vorrq_s64(__v1, __v2)

#define VEC_ADD(__v1, __v2)                     \
    vaddq_f64(__v1, __v2)

#define VEC_SUB(__v1, __v2)                     \
    vsubq_f64(__v1, __v2)

#define VEC_MUL(__v1, __v2)                     \
    vmulq_f64(__v1, __v2)

#define VEC_DIV(__v1, __v2)                     \
    vdivq_f64(__v1, __v2)

#define VEC_BLEND(__v1, __v2, __mask)           \
    _mm256_blend_pd(__v1, __v2, __mask)

#define VEC_BLENDV(__v1, __v2, __maskV)      \
    vreinterpretq_f64_s64(vbslq_s64(vcgeq_u64(vreinterpretq_u64_f64(__maskV), vdupq_n_u64(0x8000000000000000)),  \
       vreinterpretq_s64_f64(__v2), vreinterpretq_s64_f64(__v1)))

#define VEC_CAST_256_128(__v1)                  \
    _mm256_castpd256_pd128 (__v1)

#define VEC_EXTRACT_128(__v1, __im)             \
    _mm256_extractf128_pd (__v1, __im)

#define VEC_EXTRACT_UNIT(__v1, __im)            \
    vgetq_lane_s64(__v1, __im)

#define VEC_SET1_VAL128(__val)                  \
    _mm_set1_pd(__val)

#define VEC_MOVE(__v1, __val)                   \
    _mm_move_sd(__v1, __val)

#define VEC_CAST_128_256(__v1)                  \
    _mm256_castpd128_pd256(__v1)

#define VEC_INSERT_VAL(__v1, __val, __pos)      \
    _mm256_insertf128_pd(__v1, __val, __pos)

#define VEC_INSERT_UNIT(__v1, __v2, __im)       \
    vsetq_lane_s64(__v2, __v1, __im)

#define VEC_CVT_128_256(__v1)                   \
    _mm256_cvtepi32_pd(__v1)

#define VEC_SET1_VAL(__val)                     \
    vdupq_n_f64(__val)

#define VEC_POPCVT_CHAR(__ch)                   \
    vcvtq_f64_s64(vdupq_n_s64(__ch))

#define VEC_LDPOPCVT_CHAR(__addr)               \
    _mm256_cvtepi32_pd(_mm_load_si128((__m128i const *)__addr))

#define VEC_CMP_EQ(__v1, __v2)                  \
    _mm256_cmp_pd(__v1, __v2, _CMP_EQ_OQ)

#define VEC_SET_LSE(__val)                      \
    vsetq_lane_f64(__val, vdupq_n_f64(zero), 0)

#define SHIFT_HAP(__v1, __val)                  \
    __v1 = _mm_insert_epi32(_mm_slli_si128(__v1, 4), __val.i, 0)

#define VEC_SSE_TO_AVX(__vsLow, __vsHigh, __vdst)       \
    __vdst = _mm256_castpd128_pd256(__vsLow) ;            \
__vdst = _mm256_insertf128_pd(__vdst, __vsHigh, 1) ;

#define VEC_SHIFT_LEFT_1BIT(__vs)               \
    __vs = vshlq_n_s64(__vs, 1)

#define VEC_128_SHIFT_LEFT(__vs, __im)          \
    vreinterpretq_s64_s8(vextq_s8(vdupq_n_s8(0), vreinterpretq_s8_s64(__vs), 16 - (__im)))

#define COMPARE_VECS(__v1, __v2, __first, __last) {                     \
    double* ptr1 = (double*) (&__v1) ;                                  \
    double* ptr2 = (double*) (&__v2) ;                                  \
    for (int ei=__first; ei <= __last; ++ei) {                          \
        if (ptr1[ei] != ptr2[ei]) {                                       \
            std::cout << "Double Mismatch at " << ei << ": "                \
            << ptr1[ei] << " vs. " << ptr2[ei] << std::endl ;     \
            exit(0) ;                                                       \
        }                                                                 \
    }                                                                   \
}

class BitMaskVec_double {

    MASK_VEC combined_ ;

    public:

    inline MASK_TYPE& getLowEntry(int index) {
        return combined_.masks[index] ;
    }

    inline MASK_TYPE& getHighEntry(int index) {
        return combined_.masks[NEON_LENGTH/2+index] ;
    }

    inline const SIMD_TYPE& getCombinedMask() {
        return combined_.vecf ;
    }

    inline void shift_left_1bit() {
        VEC_SHIFT_LEFT_1BIT(combined_.vec) ;
    }

} ;

#define BITMASK_VEC BitMaskVec_double
