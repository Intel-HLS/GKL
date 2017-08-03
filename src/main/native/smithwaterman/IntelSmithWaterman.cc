
#include <vector>
#include <math.h>
#include "IntelSmithWaterman.h"
#include "PairWiseSW.h"


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <immintrin.h>
#include <omp.h>
#include <assert.h>

#define DBG(M, ...)  fprintf(stdout, "[DEBUG] (%s %s:%d) " M "\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define DEF_MEM_LEVEL 8

int64_t SW_cells = 0;


static jfieldID FID_w_match;
static jfieldID FID_w_mismatch;
static jfieldID FID_w_open;
static jfieldID FID_w_extend;
static jfieldID FID_strategy;

/**
 *  Cache the Java field IDs. Called once when the native library is loaded.
 */

JNIEXPORT void JNICALL Java_com_intel_gkl_smithwaterman_IntelSmithWaterman_initNative
(JNIEnv* env, jclass cls) {

  FID_w_match = env->GetFieldID(cls, "w_match", "I");
  FID_w_mismatch = env->GetFieldID(cls, "w_mismatch", "I");
  FID_w_extend = env->GetFieldID(cls, "w_extend", "I");
  FID_w_open = env->GetFieldID(cls, "w_open", "I");

}


/**
 *  Calls the native function to implement optimized smithwaterman on single pair
 */

JNIEXPORT int JNICALL Java_com_intel_gkl_smithwaterman_IntelSmithWaterman_alignNative
(JNIEnv* env, jobject obj,jbyteArray ref, jbyteArray alt, jbyteArray cigar, jint match, jint mismatch, jint open, jint extend, jint strategy)
{

    jbyte* reference = (jbyte*)env->GetPrimitiveArrayCritical(ref, 0);
    jbyte* alternate = (jbyte*)env->GetPrimitiveArrayCritical(alt, 0);
    jbyte* cigarArray = (jbyte*)env->GetPrimitiveArrayCritical(cigar, 0);

    jint count = 0;
    jint offset = 0;


    int len1 = strlen((char*)reference);
    int len2 = strlen((char*)alternate);

    // call the low level routine

    PairWiseSW *pwsw = new PairWiseSW(match, mismatch, open, extend);
    offset = pwsw->runSWOnePairBT((uint8_t*) reference, (uint8_t*) alternate,len1, len2, strategy, (int16_t*) cigarArray, (int16_t*) &count);


    // release buffers
    env->ReleasePrimitiveArrayCritical(ref, reference, 0);
    env->ReleasePrimitiveArrayCritical(alt, alternate, 0);
    env->ReleasePrimitiveArrayCritical(cigar, cigarArray, 0);


    free(pwsw);
    return offset;

}


