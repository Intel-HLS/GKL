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

int32_t (*g_runSWOnePairBT)(int32_t match, int32_t mismatch, int32_t open, int32_t extend,uint8_t *seq1, uint8_t *seq2, int32_t len1, int32_t len2, int8_t overhangStrategy, char *cigarArray, int16_t *cigarCount);

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

    jbyte* reference = (jbyte*)env->GetPrimitiveArrayCritical(ref, 0);
    jbyte* alternate = (jbyte*)env->GetPrimitiveArrayCritical(alt, 0);
    jbyte* cigarArray = (jbyte*)env->GetPrimitiveArrayCritical(cigar, 0);

    if (reference == NULL || alternate == NULL || cigarArray == NULL) {
         DBG("GetPrimitiveArrayCritical failed from JAVA unable to contiune"); 
         return -1;
    }

    jint count = 0;
    jint offset = 0;

    // call the low level routine
    offset = g_runSWOnePairBT(match, mismatch, open, extend,(uint8_t*) reference, (uint8_t*) alternate,refLength, altLength, strategy, (char *) cigarArray, (int16_t*) &count);

    // release buffers
    env->ReleasePrimitiveArrayCritical(ref, reference, 0);
    env->ReleasePrimitiveArrayCritical(alt, alternate, 0);
    env->ReleasePrimitiveArrayCritical(cigar, cigarArray, 0);

    if(offset == -1 || env->ExceptionCheck()) {
        env->ExceptionClear();
        env->ThrowNew(env->FindClass("java/lang/IllegalArgumentException"), "Invalid offset value");
    }

    if(offset == -2) {
        env->ThrowNew(env->FindClass("java/lang/OutOfMemoryError"), "Memory allocation issue");
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
