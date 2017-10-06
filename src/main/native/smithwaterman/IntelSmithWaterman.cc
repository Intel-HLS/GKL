#include <vector>
#include <math.h>
#include "IntelSmithWaterman.h"
#include "PairWiseSW.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <immintrin.h>
#include <assert.h>

#define DEF_MEM_LEVEL 8

static jfieldID FID_reflength;
static jfieldID FID_altlength;

/*
 * Class:     com_intel_gkl_smithwaterman_IntelSmithWaterman
 * Method:    alignNative
 */
JNIEXPORT jint JNICALL Java_com_intel_gkl_smithwaterman_IntelSmithWaterman_alignNative
  (JNIEnv * env, jclass obj, jbyteArray ref, jbyteArray alt, jbyteArray cigar, jint match, jint mismatch, jint open, jint extend, jint strategy)
{
    jbyte* reference = (jbyte*)env->GetPrimitiveArrayCritical(ref, 0);
    jbyte* alternate = (jbyte*)env->GetPrimitiveArrayCritical(alt, 0);
    jbyte* cigarArray = (jbyte*)env->GetPrimitiveArrayCritical(cigar, 0);

    jint count = 0;
    jint offset = 0;

    // call the low level routine
    jint refLength = env->GetArrayLength(ref);
    jint altLength = env->GetArrayLength(alt);

    PairWiseSW *pwsw = new PairWiseSW(match, mismatch, open, extend);
    offset = pwsw->runSWOnePairBT((uint8_t*) reference, (uint8_t*) alternate,refLength, altLength, strategy, (char *) cigarArray, (int16_t*) &count);

    // release buffers
    env->ReleasePrimitiveArrayCritical(ref, reference, 0);
    env->ReleasePrimitiveArrayCritical(alt, alternate, 0);
    env->ReleasePrimitiveArrayCritical(cigar, cigarArray, 0);

    delete pwsw;
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
