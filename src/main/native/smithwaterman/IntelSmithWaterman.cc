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
#include <vector>
#include <math.h>
#include <avx.h>
#include "IntelSmithWaterman.h"
#include "smithwaterman_common.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <immintrin.h>
#include <assert.h>
#include <debug.h>
#include "avx2_impl.h"
#ifndef __APPLE__
  #include "avx512_impl.h"
#endif


static jfieldID FID_reflength;
static jfieldID FID_altlength;

int32_t (*g_runSWOnePairBT)(int32_t match, int32_t mismatch, int32_t open, int32_t extend,uint8_t *seq1, uint8_t *seq2, int32_t len1, int32_t len2, int8_t overhangStrategy, char *cigarArray, int32_t cigarLen, int16_t *cigarCount, int32_t *offset);

JNIEXPORT void JNICALL Java_com_intel_gkl_smithwaterman_IntelSmithWaterman_initNative
  (JNIEnv * env, jclass obj )
{

if(is_avx512_supported())
      {
    #ifndef __APPLE__
        DBG("Using CPU-supported AVX-512 instructions");
        g_runSWOnePairBT = runSWOnePairBT_fp_avx512;

    #else
        assert(false);
    #endif
      }
      else
      {
        g_runSWOnePairBT = runSWOnePairBT_fp_avx2;
      }
      return;
}
/*
 * Class:     com_intel_gkl_smithwaterman_IntelSmithWaterman
 * Method:    alignNative
 */
JNIEXPORT jint JNICALL Java_com_intel_gkl_smithwaterman_IntelSmithWaterman_alignNative
  (JNIEnv * env, jclass obj, jbyteArray ref, jbyteArray alt, jbyteArray cigar, jint match, jint mismatch, jint open, jint extend, jint strategy)
{
    jint refLength = env->GetArrayLength(ref);
    jint altLength = env->GetArrayLength(alt);
    jint cigarLength = env->GetArrayLength(cigar);

    jbyte* reference = (jbyte*)env->GetPrimitiveArrayCritical(ref, 0);
    jbyte* alternate = (jbyte*)env->GetPrimitiveArrayCritical(alt, 0);
    jbyte* cigarArray = (jbyte*)env->GetPrimitiveArrayCritical(cigar, 0);

    if (reference == NULL || alternate == NULL || cigarArray == NULL) {
         DBG("GetPrimitiveArrayCritical failed from JAVA unable to continue.");
         if(env->ExceptionCheck())
             env->ExceptionClear();
         env->ThrowNew(env->FindClass("java/lang/IllegalArgumentException"), "Arrays aren't valid.");

         if (reference != NULL) {
           env->ReleasePrimitiveArrayCritical(ref, reference, 0);
         }

         if (alternate != NULL) {
           env->ReleasePrimitiveArrayCritical(alt, alternate, 0);
         }

         if (cigarArray != NULL) {
           env->ReleasePrimitiveArrayCritical(cigar, cigarArray, 0);
         }
         return -1;
    }

    jint count = 0;
    int32_t offset = 0;

    // call the low level routine
    int32_t result = g_runSWOnePairBT(match, mismatch, open, extend, (uint8_t*) reference, (uint8_t*) alternate, refLength, altLength, strategy, (char *) cigarArray, cigarLength, (int16_t*) &count, &offset);
    // release buffers
    env->ReleasePrimitiveArrayCritical(ref, reference, 0);
    env->ReleasePrimitiveArrayCritical(alt, alternate, 0);
    env->ReleasePrimitiveArrayCritical(cigar, cigarArray, 0);

    if(result == SW_MEMORY_ALLOCATION_FAILED) {
        env->ExceptionClear();
        env->ThrowNew(env->FindClass("java/lang/OutOfMemoryError"), "Memory allocation issue");
        return -1;
    }

    return offset;
}

/*
 * Class:     com_intel_gkl_smithwaterman_IntelSmithWaterman
 * Method:    doneNative
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_intel_gkl_smithwaterman_IntelSmithWaterman_doneNative
  (JNIEnv *, jclass)
{

}
