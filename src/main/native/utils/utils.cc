#if defined(_MSC_VER)
  #include <intrin.h> // SIMD intrinsics for Windows
#else
  #include <x86intrin.h> // SIMD intrinsics for GCC
#endif

#ifdef linux
  #include <omp.h>
#endif

#include "utils.h"
#include <avx.h>

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
    if (value)
    {
        _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
    }
    else
    {
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
    return is_avx_supported();
}

/*
 * Class:     com_intel_gkl_IntelGKLUtils
 * Method:    isAvx512SupportedNative
 * Signature: (Z)V
 */
JNIEXPORT jboolean JNICALL Java_com_intel_gkl_IntelGKLUtils_isAvx512SupportedNative
  (JNIEnv *env, jobject obj)
{
    return is_avx512_supported();
}

/*
 * Class:     com_intel_gkl_IntelGKLUtils
 * Method:    getAvailableOmpThreadsNative
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_intel_gkl_IntelGKLUtils_getAvailableOmpThreadsNative
  (JNIEnv *env, jobject obj)
{
#ifdef _OPENMP
    int avail_threads = omp_get_max_threads();
#else
    int avail_threads = 0;
#endif

    return avail_threads;
}
