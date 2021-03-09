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

#define PRECISION s

#define MAIN_TYPE float
#define MAIN_TYPE_SIZE 32
#define UNION_TYPE mix_F512
#define IF_256 IF_256f
#define IF_MAIN_TYPE IF_32
#define SHIFT_CONST1 3
#define SHIFT_CONST2 7
#define SHIFT_CONST3 4
#define SHIFT_CONST4 4
#define SHIFT_CONST5 12
#define _256_TYPE __m256
#define SIMD_TYPE __m512
#define _512_INT_TYPE __m512i
#define AVX_LENGTH 16
#define HAP_TYPE UNION_TYPE
#define MASK_TYPE uint32_t
#define MASK_ALL_ONES 0xFFFFFFFF
#define MASK_VEC MaskVec_F512
#define MASK_CONST __mmask16

#define SET_VEC_ZERO(__vec)                     \
    __vec= _mm512_setzero_ps()

#define VEC_PERMUTE(__v1, __v2)                 \
    _mm512_permutexvar_epi32(__v1, __v2)

#define VEC_AND(__v1, __v2)                     \
     _mm512_and_epi32(__v1, __v2)

#define VEC_OR(__v1, __v2)                      \
    _mm512_or_epi32(__v1, __v2)

#define VEC_ADD(__v1, __v2)                     \
    _mm512_add_ps(__v1, __v2)

#define VEC_SUB(__v1, __v2)                     \
    _mm512_sub_ps(__v1, __v2)

#define VEC_MUL(__v1, __v2)                     \
    _mm512_mul_ps(__v1, __v2)

#define VEC_DIV(__v1, __v2)                     \
    _mm512_div_ps(__v1, __v2)

#define VEC_BLEND(__v1, __v2, __mask)           \
    _mm512_mask_blend_ps(__mask, __v1, __v2)

#define VEC_BLENDV(__distmChosen, __v1, __v2, __maskV)  \
    _512_INT_TYPE val = VEC_SET1_VAL_32();              \
    MASK_CONST mask = VEC_MASK_TEST(val, __maskV);      \
    __distmChosen = VEC_BLEND(__v1, __v2, mask);

#define VEC_CAST_512_128(__v1)                  \
    _mm512_castps512_ps128 (__v1)

#define VEC_EXTRACT_256(__v1, __im)             \
    _mm512_extractf32x8_ps (__v1, __im)

#define VEC_EXTRACT_UNIT(__v1, __im)            \
    _mm256_extract_epi32(__v1, __im)

#define VEC_SET1_VAL128(__val)                  \
    _mm_set1_ps(__val)

#define VEC_MOVE(__v1, __val)                   \
    _mm_move_ss(__v1, __val)

#define VEC_CAST_128_512(__v1)                  \
    _mm512_castps128_ps512(__v1)

#define VEC_INSERT_VAL(__v1, __val, __pos)      \
    _mm256_insert_epi32(__v1, __val, __pos)

#define VEC_INSERT_VEC(__v1, __v2, __pos)       \
     _mm512_insertf32x8(__v1, __v2, __pos)

#define VEC_CVT_128_512(__v1)                   \
    _mm512_cvtepi32_ps(__v1.i)

#define VEC_SET1_VAL(__val)                     \
    _mm512_set1_ps(__val)

#define VEC_SET1_VAL_32()                       \
    _mm512_set1_epi32(0x80000000)

#define VEC_SET1()                              \
    _mm512_set_epi32(14,13,12,11,10,9,8,7,6,5,4,3,2,1,0,15)

#define VEC_SET2()                              \
    _mm512_set_epi32(0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0x0)

#define VEC_SET3(__val)                         \
    _mm512_set_epi32(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,__val)

#define VEC_POPCVT_CHAR(__ch)                   \
    _mm512_cvtepi32_ps(_mm512_set1_epi32(__ch))

#define VEC_LDPOPCVT_CHAR(__addr)               \
    _mm256_cvtepi32_ps(_mm256_loadu_si256((__m256i const *)__addr))

#define VEC_CMP_EQ(__v1, __v2)                  \
    _mm512_cmp_ps(__v1, __v2, _CMP_EQ_OQ)

#define VEC_SET_LSE(__val)                      \
    _mm512_set_ps(zero, zero, zero, zero, zero, zero, zero, zero, zero, zero, zero, zero, zero, zero, zero, __val);

#define SHIFT_HAP(__v1, __val)                  \
    _vector_shift_lastavxs(__v1, __val.f);

#define VEC_SSE_TO_AVX(__vsLow, __vsHigh, __vdst)       \
    __vdst = _mm512_inserti32x8(__vdst,__vsLow,0) ;     \
__vdst = _mm512_inserti32x8(__vdst, __vsHigh, 1) ;

#define VEC_SHIFT_LEFT_1BIT(__vs)               \
    __vs = _mm256_slli_epi32(__vs, 1)

#define VEC_SHIFT_LEFT(__vs, __val)             \
    __vs = _mm256_slli_si256(__vs, __val)

#define VEC_XOR(__v1, __v2)                     \
     _mm512_xor_ps(__v1, __v2)

#define VEC_BSHIFT_LEFT(__v1, __val)            \
     _mm512_bslli_epi128(__v1, __val)

#define VEC_BSHIFT_RIGHT(__v1, __val)           \
     _mm512_bsrli_epi128(__v1, __val)

#define VEC_MASK_TEST(__val, __mask)            \
     _mm512_test_epi32_mask(__val, __mask)

#define COMPARE_VECS(__v1, __v2, __first, __last) {                     \
    float* ptr1 = (float*) (&__v1) ;                                    \
    float* ptr2 = (float*) (&__v2) ;                                    \
    for (int ei=__first; ei <= __last; ++ei) {                          \
        if (ptr1[ei] != ptr2[ei]) {                                       \
            std::cout << "Float Mismatch at " << ei << ": "                 \
            << ptr1[ei] << " vs. " << ptr2[ei] << std::endl ;     \
            exit(0) ;                                                       \
        }                                                                 \
    }                                                                   \
}

class BitMaskVec_float512 {

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

#define BITMASK_VEC BitMaskVec_float512
