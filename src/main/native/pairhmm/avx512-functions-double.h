/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2021 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifdef PRECISION
#undef PRECISION
#undef MAIN_TYPE
#undef MAIN_TYPE_SIZE
#undef UNION_TYPE
#undef IF_256
#undef IF_MAIN_TYPE
#undef SHIFT_CONST1
#undef SHIFT_CONST2
#undef SHIFT_CONST3
#undef SHIFT_CONST4
#undef SHIFT_CONST5
#undef _256_TYPE
#undef SIMD_TYPE
#undef _512_INT_TYPE
#undef AVX_LENGTH
#undef HAP_TYPE
#undef MASK_TYPE
#undef MASK_ALL_ONES
#undef MASK_VEC
#undef MASK_CONST

#undef SET_VEC_ZERO
#undef VEC_PERMUTE
#undef VEC_AND
#undef VEC_OR
#undef VEC_ADD
#undef VEC_SUB
#undef VEC_MUL
#undef VEC_DIV
#undef VEC_BLEND
#undef VEC_BLENDV
#undef VEC_CAST_512_128
#undef VEC_EXTRACT_256
#undef VEC_EXTRACT_UNIT
#undef VEC_SET1_VAL128
#undef VEC_MOVE
#undef VEC_CAST_128_512
#undef VEC_INSERT_VAL
#undef VEC_INSERT_VEC
#undef VEC_CVT_128_512
#undef VEC_SET1_VAL
#undef VEC_SET1_VAL_32
#undef VEC_SET1
#undef VEC_SET2
#undef VEC_SET3
#undef VEC_POPCVT_CHAR
#undef VEC_LDPOPCVT_CHAR
#undef VEC_CMP_EQ
#undef VEC_SET_LSE
#undef SHIFT_HAP
#undef VEC_SSE_TO_AVX
#undef VEC_SHIFT_LEFT_1BIT
#undef VEC_SHIFT_LEFT
#undef VEC_XOR
#undef VEC_BSHIFT_LEFT
#undef VEC_BSHIFT_RIGHT
#undef VEC_MASK_TEST
#undef COMPARE_VECS
#undef BITMASK_VEC
#endif

#define PRECISION d

#define MAIN_TYPE double
#define MAIN_TYPE_SIZE 64
#define UNION_TYPE mix_D512
#define IF_256 IF_256d
#define IF_MAIN_TYPE IF_64
#define SHIFT_CONST1 1
#define SHIFT_CONST2 3
#define SHIFT_CONST3 2
#define SHIFT_CONST4 8
#define SHIFT_CONST5 8
#define _256_TYPE __m256d
#define SIMD_TYPE __m512d
#define _512_INT_TYPE __m512i
#define AVX_LENGTH 8
#define HAP_TYPE __m128i
#define MASK_TYPE uint64_t
#define MASK_ALL_ONES 0xFFFFFFFFFFFFFFFF
#define MASK_VEC MaskVec_D512
#define MASK_CONST __mmask8

#define SET_VEC_ZERO(__vec)                     \
    __vec= _mm512_setzero_pd()

#define VEC_PERMUTE(__v1, __v2)                 \
    _mm512_permutexvar_epi64(__v1, __v2)

#define VEC_AND(__v1, __v2)                     \
     _mm512_and_epi64(__v1, __v2)

#define VEC_OR(__v1, __v2)                      \
    _mm512_or_epi64(__v1, __v2)

#define VEC_ADD(__v1, __v2)                     \
    _mm512_add_pd(__v1, __v2)

#define VEC_SUB(__v1, __v2)                     \
    _mm512_sub_pd(__v1, __v2)

#define VEC_MUL(__v1, __v2)                     \
    _mm512_mul_pd(__v1, __v2)

#define VEC_DIV(__v1, __v2)                     \
    _mm512_div_pd(__v1, __v2)

#define VEC_BLEND(__v1, __v2, __mask)           \
    _mm512_mask_blend_pd(mask,__v1, __v2)

#define VEC_BLENDV(__distmChosen, __v1, __v2, __maskV)  \
    _512_INT_TYPE val = VEC_SET1_VAL_32();              \
    MASK_CONST mask = VEC_MASK_TEST(val, __maskV);      \
    __distmChosen = VEC_BLEND(__v1, __v2, mask);

#define VEC_CAST_512_128(__v1)                  \
    _mm512_castpd512_pd128 (__v1)

#define VEC_EXTRACT_256(__v1, __im)             \
    _mm512_extractf64x4_pd (__v1, __im)

#define VEC_EXTRACT_UNIT(__v1, __im)            \
    _mm256_extract_epi64(__v1, __im)

#define VEC_SET1_VAL128(__val)                  \
    _mm_set1_pd(__val)

#define VEC_MOVE(__v1, __val)                   \
    _mm_move_sd(__v1, __val)

#define VEC_CAST_128_512(__v1)                  \
    _mm512_castpd128_pd128(__v1)

#define VEC_INSERT_VAL(__v1, __val, __pos)      \
    _mm256_insert_epi64(__v1, __val, __pos)

#define VEC_INSERT_VEC(__v1, __v2, __pos)       \
     _mm512_insertf64x4(__v1, __v2, __pos)

#define VEC_CVT_128_512(__v1)                   \
    _mm512_cvtepi32_pd(__v1)

#define VEC_SET1_VAL(__val)                     \
    _mm512_set1_pd(__val)

#define VEC_SET1_VAL_32()                       \
     _mm512_set1_epi64(0x8000000000000000)

#define VEC_SET1()                              \
    _mm512_set_epi64(6,5,4,3,2,1,0,7)

#define VEC_SET2()                              \
    _mm512_set_epi64(0xFFFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFFFF,0x0)

#define VEC_SET3(__val)                         \
    _mm512_set_epi64(0,0,0,0,0,0,0,__val)

#define VEC_POPCVT_CHAR(__ch)                   \
    _mm512_cvtepi32_pd(_mm256_set1_epi32(__ch))

#define VEC_LDPOPCVT_CHAR(__addr)               \
    _mm512_cvtepi32_pd(_mm256_load_si256((__m256i const *)__addr))

#define VEC_CMP_EQ(__v1, __v2)                  \
    _mm512_cmp_pd(__v1, __v2, _CMP_EQ_OQ)

#define VEC_SET_LSE(__val)                      \
    _mm512_set_pd(zero, zero, zero, zero, zero, zero, zero, __val);

#define SHIFT_HAP(__v1, __val)                  \
    __v1 = _mm_insert_epi32(_mm_slli_si128(__v1, 4), __val.i, 0)

#define VEC_SSE_TO_AVX(__vsLow, __vsHigh, __vdst)       \
    __vdst = _mm512_inserti64x4(__vdst,__vsLow,0) ;     \
__vdst = _mm512_inserti64x4(__vdst, __vsHigh, 1) ;

#define VEC_SHIFT_LEFT_1BIT(__vs)               \
    __vs = _mm256_slli_epi64(__vs, 1)

#define VEC_SHIFT_LEFT(__vs, __val)             \
    __vs = _mm256_slli_si256(__vs, __val)

#define VEC_XOR(__v1, __v2)                     \
     _mm512_xor_pd(__v1, __v2)

#define VEC_BSHIFT_LEFT(__v1, __val)            \
     _mm512_bslli_epi128(__v1, __val)

#define VEC_BSHIFT_RIGHT(__v1, __val)           \
     _mm512_bsrli_epi128(__v1, __val)

#define VEC_MASK_TEST(__val, __mask)            \
     _mm512_test_epi64_mask(__val, __mask);

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

class BitMaskVec_double512 {

    MASK_VEC low_, high_ ;
    _512_INT_TYPE combined_ ;

    public:

    inline MASK_TYPE& getLowEntry(int index) {
        return low_.masks[index] ;
    }

    inline MASK_TYPE& getHighEntry(int index) {
        return high_.masks[index] ;
    }

    inline const _512_INT_TYPE& getCombinedMask() {
        VEC_SSE_TO_AVX(low_.vec, high_.vec, combined_) ;
        return combined_ ;
    }

    inline const _256_TYPE getLowVec() {
        return low_.vecf;
    }

     inline const _256_TYPE getHighVec() {
        return high_.vecf;
    }

    inline void shift_left_1bit() {
        VEC_SHIFT_LEFT_1BIT(low_.vec) ;
        VEC_SHIFT_LEFT_1BIT(high_.vec) ;
    }

} ;

#define BITMASK_VEC BitMaskVec_double512
