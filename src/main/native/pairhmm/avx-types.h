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
#ifndef AVX_TEMPLATE_H
#define AVX_TEMPLATE_H


#define ALIGNED __attribute__((aligned(32)))

typedef union __attribute__((aligned(32))) {
        ALIGNED __m512 ALIGNED d;
        ALIGNED __m256i ALIGNED s[2];
        ALIGNED float  ALIGNED f[16];
        ALIGNED __m512i ALIGNED i;
} ALIGNED mix_F512 ALIGNED;

typedef union __attribute__((aligned(32))) {
        ALIGNED __m256 ALIGNED d;
        ALIGNED __m128i ALIGNED s[2];
        ALIGNED float  ALIGNED f[8];
        ALIGNED __m256i ALIGNED i;
} ALIGNED mix_F256 ALIGNED;

typedef union __attribute__((aligned(32))) {
        ALIGNED __m128 ALIGNED d;
        ALIGNED __m64 ALIGNED s[2];
        ALIGNED float  ALIGNED f[4];
        ALIGNED __m128i ALIGNED i;
} ALIGNED mix_F128 ALIGNED;

/* -------------------------------------------------------------------------- */

typedef union ALIGNED {
  __m256i vec;
  __m256 vecf;
  uint32_t masks[8];
} MaskVec_F512;

typedef union ALIGNED {
  __m128i vec;
  __m128 vecf;
  uint32_t masks[4];
} MaskVec_F256;

typedef union ALIGNED {
  __m64 vec;
  __m64 vecf;
  uint32_t masks[2];
} MaskVec_F128;

/* -------------------------------------------------------------------------- */

typedef union ALIGNED
{
        ALIGNED __m256i ALIGNED i;
        ALIGNED __m256 ALIGNED f;
} ALIGNED IF_256f ALIGNED;

typedef union ALIGNED
{
        ALIGNED __m128i ALIGNED i;
        ALIGNED __m128 ALIGNED f;
} ALIGNED IF_128f ALIGNED;

typedef union ALIGNED
{
        ALIGNED int    ALIGNED i;
        ALIGNED float  ALIGNED f;
} ALIGNED IF_32 ALIGNED;

/* -------------------------------------------------------------------------- */

typedef union __attribute__((aligned(32))) {
        ALIGNED __m512d ALIGNED d;
        ALIGNED __m256 ALIGNED s[2];
        ALIGNED double  ALIGNED f[8];
        ALIGNED __m512i ALIGNED i;
} ALIGNED mix_D512 ALIGNED;

typedef union __attribute__((aligned(32))) {
        ALIGNED __m256d ALIGNED d;
        ALIGNED __m128i ALIGNED s[2];
        ALIGNED double  ALIGNED f[4];
        ALIGNED __m256i ALIGNED i;
} ALIGNED mix_D256 ALIGNED;

typedef union __attribute__((aligned(32))) {
        ALIGNED __m128d ALIGNED d;
        ALIGNED __m64 ALIGNED s[2];
        ALIGNED double  ALIGNED f[2];
        ALIGNED __m128i ALIGNED i;
} ALIGNED mix_D128 ALIGNED;

/* -------------------------------------------------------------------------- */

typedef union ALIGNED {
  __m256i vec;
  __m256d vecf;
  uint64_t masks[4];
} MaskVec_D512;

typedef union ALIGNED {
  __m128i vec;
  __m128d vecf;
  uint64_t masks[2];
} MaskVec_D256;

typedef union ALIGNED {
  __m64 vec;
  __m64 vecf;
  uint64_t masks[1];
} MaskVec_D128;

/* -------------------------------------------------------------------------- */

typedef union ALIGNED
{
        ALIGNED __m256i ALIGNED i;
        ALIGNED __m256d ALIGNED f;
} ALIGNED IF_256d ALIGNED;

typedef union ALIGNED
{
        ALIGNED __m128i ALIGNED i;
        ALIGNED __m128d ALIGNED f;
} ALIGNED IF_128d ALIGNED;

typedef union ALIGNED
{
        ALIGNED int64_t ALIGNED i;
        ALIGNED double  ALIGNED f;
} ALIGNED IF_64 ALIGNED;

#endif // AVX_TEMPLATE_H
