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
#ifndef PAIRHMM_COMMON_H
#define PAIRHMM_COMMON_H

#if defined(_MSC_VER)
  #include <intrin.h> // SIMD intrinsics for Windows
#else
  #include <x86intrin.h> // SIMD intrinsics for GCC
#endif

#include <assert.h>
#include <stdint.h>

#define CAT(X,Y) X##Y
#define CONCAT(X,Y) CAT(X,Y)

#define MIN_ACCEPTED 1e-28f
#define NUM_DISTINCT_CHARS 5
#define AMBIG_CHAR 4

typedef struct {
  int rslen, haplen;
  const char *q, *i, *d, *c;
  const char *hap, *rs;
} testcase;

class ConvertChar {
  static uint8_t conversionTable[255] ;

 public:  
  static void init() {
    assert (NUM_DISTINCT_CHARS == 5) ;
    assert (AMBIG_CHAR == 4) ;
    
    conversionTable['A'] = 0 ;
    conversionTable['C'] = 1 ;
    conversionTable['T'] = 2 ;
    conversionTable['G'] = 3 ;
    conversionTable['N'] = 4 ;
  }
  
  static inline uint8_t get(uint8_t input) {
    return conversionTable[input] ;
  }
};

#endif // PAIRHMM_COMMON_H
