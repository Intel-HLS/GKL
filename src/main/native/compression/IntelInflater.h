/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_intel_gkl_compression_IntelInflater */

#ifndef _Included_com_intel_gkl_compression_IntelInflater
#define _Included_com_intel_gkl_compression_IntelInflater
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_intel_gkl_compression_IntelInflater
 * Method:    initNative
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_intel_gkl_compression_IntelInflater_initNative
  (JNIEnv *, jclass);

/*
 * Class:     com_intel_gkl_compression_IntelInflater
 * Method:    resetNative
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL Java_com_intel_gkl_compression_IntelInflater_resetNative
  (JNIEnv *, jobject, jboolean);

/*
 * Class:     com_intel_gkl_compression_IntelInflater
 * Method:    inflateNative
 * Signature: ([BI)I
 */
JNIEXPORT jint JNICALL Java_com_intel_gkl_compression_IntelInflater_inflateNative
  (JNIEnv *, jobject, jbyteArray, jint);

/*
 * Class:     com_intel_gkl_compression_IntelInflater
 * Method:    endNative
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_intel_gkl_compression_IntelInflater_endNative
  (JNIEnv *, jobject);

#ifdef __cplusplus
}
#endif
#endif
