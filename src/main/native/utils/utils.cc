#if defined(_MSC_VER)
  #include <intrin.h> // SIMD intrinsics for Windows
#else
  #include <x86intrin.h> // SIMD intrinsics for GCC
#endif

#include "utils.h"
#include <cpuid.h>
#include <stdio.h>
#include <stdlib.h>

#define OSXSAVEFlag (1UL<<27)
#define AVXFlag     ((1UL<<28)|OSXSAVEFlag)


#define DEBUG

#ifdef DEBUG
#  define DBG(M, ...)  fprintf(stdout, "[DEBUG] (%s %s:%d) " M "\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#  define DBG(M, ...)
#endif



/*
 * Class:     com_intel_gkl_IntelGKLUtils
 * Method:    getFlushToZeroNative
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_intel_gkl_IntelGKLUtils_getFlushToZeroNative
  (JNIEnv *env, jobject obj)
{
  jboolean value = _MM_GET_FLUSH_ZERO_MODE() == _MM_FLUSH_ZERO_ON ? 1 : 0;
  return value;
}

/*
 * Class:     com_intel_gkl_IntelGKLUtils
 * Method:    setFlushToZeroNative
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL Java_com_intel_gkl_IntelGKLUtils_setFlushToZeroNative
  (JNIEnv *env, jobject obj, jboolean value)
{
  if (value) {
    _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
  }
  else {
    _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_OFF);
  }
}

/*
 * Class:     com_intel_gkl_IntelGKLUtils
 * Method:    isAvxSupportedNative
 * Signature: (Z)V
 */

JNIEXPORT jboolean JNICALL Java_com_intel_gkl_IntelGKLUtils_isAvxSupportedNative
  (JNIEnv *env, jobject obj)
{

        jint eax, ebx, ecx, edx;

        __cpuid(0, eax, ebx, ecx, edx);

        if ((ecx && AVXFlag) == 0 ) return false;

        return true;

}
