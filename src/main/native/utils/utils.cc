#if defined(__aarch64__)
  #include <arm_neon.h>
#else
#if defined(_MSC_VER)
  #include <intrin.h> // SIMD intrinsics for Windows
#else
  #include <x86intrin.h> // SIMD intrinsics for GCC
#endif
#endif

#ifdef linux
  #include <omp.h>
#endif

#include "utils.h"

#if defined(__aarch64__)
#include <neon.h>
#else
#include <avx.h>
#endif

/*
 * Class:     com_intel_gkl_IntelGKLUtils
 * Method:    getFlushToZeroNative
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_intel_gkl_IntelGKLUtils_getFlushToZeroNative
  (JNIEnv *env, jobject obj)
{
#if defined(__aarch64__)
    jboolean value = _AA64_GET_FLUSH_ZERO_MODE() == _AA64_FLUSH_ZERO_ON ? 1 : 0;
#else
    jboolean value = _MM_GET_FLUSH_ZERO_MODE() == _MM_FLUSH_ZERO_ON ? 1 : 0;
#endif
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
#if defined (__aarch64__)
    if (value)
    {
      _AA64_SET_FLUSH_ZERO_MODE(_AA64_FLUSH_ZERO_ON);
    }
    else
    {
      _AA64_SET_FLUSH_ZERO_MODE(_AA64_FLUSH_ZERO_OFF);
    }
#else
    if (value)
    {
        _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
    }
    else
    {
        _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_OFF);
    }
#endif
}

/*
 * Class:     com_intel_gkl_IntelGKLUtils
 * Method:    isAvxSupportedNative
 * Signature: (Z)V
 */
#if !defined (__aarch64__)
JNIEXPORT jboolean JNICALL Java_com_intel_gkl_IntelGKLUtils_isAvxSupportedNative
  (JNIEnv *env, jobject obj)
{
    return is_avx_supported();
}

/*
 * Class:     com_intel_gkl_IntelGKLUtils
 * Method:    isAvx2SupportedNative
 * Signature: (Z)V
 */
JNIEXPORT jboolean JNICALL Java_com_intel_gkl_IntelGKLUtils_isAvx2SupportedNative
  (JNIEnv *env, jobject obj)
{
    return is_avx2_supported();
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
#else
/*
 * Class:     com_intel_gkl_IntelGKLUtils
 * Method:    isNeonSupportedNative
 * Signature: (Z)V
 */
JNIEXPORT jboolean JNICALL Java_com_intel_gkl_IntelGKLUtils_isNeonSupportedNative
  (JNIEnv *env, jobject obj)
{
    return is_neon_supported();
}
#endif
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
