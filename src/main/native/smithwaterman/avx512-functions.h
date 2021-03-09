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
#ifndef _AVX2_FUNCTIONS_H
#define _AVX2_FUNCTIONS_H

#undef AVX_LENGTH
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

#define AVX_LENGTH 16

#define VEC_INT_TYPE __m512i
#define VEC_MASK_TYPE __mmask16

#define VEC_LOADU(__addr) \
    _mm512_loadu_si512((__m512i *)(__addr))

#define VEC_STORE(__addr, __v) \
    _mm512_store_si512((__m512i *)(__addr), __v)

#define VEC_STOREU(__addr, __v) \
    _mm512_storeu_si512((__m512i *)(__addr), __v)

#define VEC_STREAM(__addr, __v) \
    _mm512_stream_si512((__m512i *)(__addr), __v)


#define VEC_SET_ZERO() \
    _mm512_setzero_si512()

#define VEC_SET1_VAL32(__val) \
    _mm512_set1_epi32(__val)

#define VEC_ADD(__v1, __v2) \
    _mm512_add_epi32(__v1, __v2)

#define VEC_MAX(__v1, __v2) \
    _mm512_max_epi32(__v1, __v2)
    
#define VEC_CMPGT(__v1, __v2) \
    _mm512_movm_epi32(_mm512_cmpgt_epi32_mask(__v1, __v2))

#define VEC_CMPGT_MASK(__v1, __v2) \
    _mm512_cmpgt_epi32_mask(__v1, __v2)

#define VEC_CMPEQ(__v1, __v2) \
    _mm512_movm_epi32(_mm512_cmpeq_epi32_mask(__v1, __v2))

#define VEC_CMPEQ_MASK(__v1, __v2) \
    _mm512_cmpeq_epi32_mask(__v1, __v2)

#define VEC_AND(__v1, __v2) \
    _mm512_and_si512(__v1, __v2)

#define VEC_OR(__v1, __v2) \
    _mm512_or_si512(__v1, __v2)

#define VEC_ANDNOT(__v1, __v2) \
    _mm512_andnot_si512(__v1, __v2)

#define VEC_BLEND(__v1, __v2, __mask) \
    _mm512_mask_blend_epi32(__mask, __v1, __v2)

#define VEC_PERMUTE2x128_EVEN(__v1, __v2) \
    _mm512_permutex2var_epi32(__v1, idx_even, __v2)    

#define VEC_PERMUTE2x128_ODD(__v1, __v2) \
    _mm512_permutex2var_epi32(__v1, idx_odd, __v2)    

#define VEC_PACKS_32(__v1, __v2) \
    _mm512_packs_epi32(__v1, __v2)

#define INIT_CONSTANTS \
    __m512i idx_even = _mm512_set_epi32(27, 26, 25, 24, 19, 18, 17, 16, 11, 10, 9, 8, 3, 2, 1, 0); \
    __m512i idx_odd = _mm512_set_epi32(31, 30, 29, 28, 23, 22, 21, 20, 15, 14, 13, 12, 7, 6, 5, 4);

#endif

