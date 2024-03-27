/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2024 Intel Corporation
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
/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_intel_gkl_compression_IntelDeflater */

#ifndef _Included_com_intel_gkl_compression_IntelDeflater
#define _Included_com_intel_gkl_compression_IntelDeflater
#ifdef __cplusplus
extern "C" {
#endif
#undef com_intel_gkl_compression_IntelDeflater_DEFLATED
#define com_intel_gkl_compression_IntelDeflater_DEFLATED 8L
#undef com_intel_gkl_compression_IntelDeflater_NO_COMPRESSION
#define com_intel_gkl_compression_IntelDeflater_NO_COMPRESSION 0L
#undef com_intel_gkl_compression_IntelDeflater_BEST_SPEED
#define com_intel_gkl_compression_IntelDeflater_BEST_SPEED 1L
#undef com_intel_gkl_compression_IntelDeflater_BEST_COMPRESSION
#define com_intel_gkl_compression_IntelDeflater_BEST_COMPRESSION 9L
#undef com_intel_gkl_compression_IntelDeflater_DEFAULT_COMPRESSION
#define com_intel_gkl_compression_IntelDeflater_DEFAULT_COMPRESSION -1L
#undef com_intel_gkl_compression_IntelDeflater_FILTERED
#define com_intel_gkl_compression_IntelDeflater_FILTERED 1L
#undef com_intel_gkl_compression_IntelDeflater_HUFFMAN_ONLY
#define com_intel_gkl_compression_IntelDeflater_HUFFMAN_ONLY 2L
#undef com_intel_gkl_compression_IntelDeflater_DEFAULT_STRATEGY
#define com_intel_gkl_compression_IntelDeflater_DEFAULT_STRATEGY 0L
#undef com_intel_gkl_compression_IntelDeflater_NO_FLUSH
#define com_intel_gkl_compression_IntelDeflater_NO_FLUSH 0L
#undef com_intel_gkl_compression_IntelDeflater_SYNC_FLUSH
#define com_intel_gkl_compression_IntelDeflater_SYNC_FLUSH 2L
#undef com_intel_gkl_compression_IntelDeflater_FULL_FLUSH
#define com_intel_gkl_compression_IntelDeflater_FULL_FLUSH 3L
/*
 * Class:     com_intel_gkl_compression_IntelDeflater
 * Method:    initNative
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_intel_gkl_compression_IntelDeflater_initNative
  (JNIEnv *, jclass);

/*
 * Class:     com_intel_gkl_compression_IntelDeflater
 * Method:    resetNative
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL Java_com_intel_gkl_compression_IntelDeflater_resetNative
  (JNIEnv *, jobject, jboolean);

/*
 * Class:     com_intel_gkl_compression_IntelDeflater
 * Method:    deflateNative
 * Signature: ([BI)I
 */
JNIEXPORT jint JNICALL Java_com_intel_gkl_compression_IntelDeflater_deflateNative
  (JNIEnv *, jobject, jbyteArray, jint);

/*
 * Class:     com_intel_gkl_compression_IntelDeflater
 * Method:    endNative
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_intel_gkl_compression_IntelDeflater_endNative
  (JNIEnv *, jobject);

/*
 * Class:     com_intel_gkl_compression_IntelDeflater
 * Method:    generateHuffman
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_intel_gkl_compression_IntelDeflater_generateHuffman
  (JNIEnv *, jobject);

#ifdef __cplusplus
}
#endif
#endif
