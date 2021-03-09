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
#ifndef SMITHWATERMAN_COMMON_H
#define SMITHWATERMAN_COMMON_H

#if defined(_MSC_VER)
  #include <intrin.h> // SIMD intrinsics for Windows
#else
  #include <x86intrin.h> // SIMD intrinsics for GCC
#endif

#define __STDC_LIMIT_MACROS

#include <stdint.h>
#include <string.h>
#include <immintrin.h>

#define SW_SUCCESS 0
#define SW_MEMORY_ALLOCATION_FAILED 1

#define CAT(X,Y) X##Y
#define CONCAT(X,Y) CAT(X,Y)

#define MATCH 0
#define INSERT 1
#define DELETE 2
#define INSERT_EXT 4
#define DELETE_EXT 8
#define SOFTCLIP 9
#define INDEL 10
#define LEADING_INDEL 11
#define IGNORE 12

typedef struct dnaSeqPair
{
        int32_t id;
        uint8_t *seq1;
        uint8_t *seq2;
        int16_t len1, len2;
        int8_t overhangStrategy;
        int32_t score;
        int16_t max_i;
        int16_t max_j;
        int16_t *btrack;
        char *cigar;
        int16_t cigarCount;
        int16_t alignmentOffset;
}SeqPair;


/* Description : Function for converting integer number to a string.
 * Returns : Number of chars written 
 * Parameters :
 *    1. char * ptr : pointer to char array where the number will be written
 *    2. int number : number to convert 
 * @author : pawel.kolakowski@intel.com
 */
int32_t fast_itoa(char * ptr, int32_t number);

//the maximum DNA sequence length
#define MAX_SEQ_LEN 1024
#define MAX_NUM_PAIRS 800000
#define MATRIX_MIN_CUTOFF -100000000
#define LOW_INIT_VALUE (INT32_MIN/2)
#define max(x, y) ((x)>(y)?(x):(y))
#define min(x, y) ((x)<(y)?(x):(y))
#define DUMMY1 'B'
#define DUMMY2 'D'


#endif