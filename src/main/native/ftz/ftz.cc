#if defined(_MSC_VER)
  #include <intrin.h> // SIMD intrinsics for Windows
#else
  #include <x86intrin.h> // SIMD intrinsics for GCC
#endif

#include "ftz.h"

//  _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);

/*
 * Class:     com_intel_gkl_ftz_FTZ
 * Method:    getFlushToZeroNative
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_intel_gkl_ftz_FTZ_getFlushToZeroNative
  (JNIEnv *env, jobject obj)
{
  jboolean value = _MM_GET_FLUSH_ZERO_MODE() == _MM_FLUSH_ZERO_ON ? 1 : 0;
  return value;
}

/*
 * Class:     com_intel_gkl_ftz_FTZ
 * Method:    setFlushToZeroNative
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL Java_com_intel_gkl_ftz_FTZ_setFlushToZeroNative
  (JNIEnv *env, jobject obj, jboolean value)
{
  if (value) {
    _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
  }
  else {
    _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_OFF);
  }
}
