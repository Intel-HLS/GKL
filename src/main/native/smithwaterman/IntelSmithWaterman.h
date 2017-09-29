#include <jni.h>

#ifndef _Included_com_intel_gkl_pairhmm_IntelSmithWaterman
#define _Included_com_intel_gkl_pairhmm_IntelSmithWaterman

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT int JNICALL Java_com_intel_gkl_smithwaterman_IntelSmithWaterman_alignNative
    (JNIEnv* env, jobject obj,jbyteArray ref, jbyteArray alt, jbyteArray cigar, jint match, jint mismatch, jint open, jint extend, jint strategy);

JNIEXPORT void JNICALL Java_com_intel_gkl_smithwaterman_IntelSmithWaterman_initNative
    (JNIEnv* env, jclass cls);

JNIEXPORT void JNICALL Java_com_intel_gkl_smithwaterman_IntelSmithWaterman_doneNative
    (JNIEnv* env, jclass cls);

#ifdef __cplusplus
}
#endif
#endif