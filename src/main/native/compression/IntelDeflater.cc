/*
 * The MIT License
 *
 * Copyright (c) 2016 Intel Corporation
 *
 *  Permission is hereby granted, free of charge, to any person
 *  obtaining a copy of this software and associated documentation
 *  files (the "Software"), to deal in the Software without
 *  restriction, including without limitation the rights to use,
 *  copy, modify, merge, publish, distribute, sublicense, and/or
 *  sell copies of the Software, and to permit persons to whom the
 *  Software is furnished to do so, subject to the following
 *  conditions:
 *
 *  The above copyright notice and this permission notice shall be
 *  included in all copies or substantial portions of the
 *  Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
 *  KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 *  PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 *  COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 *  OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 *  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <stdlib.h>
#include "IntelDeflater.h"

extern "C" {
#include "zlib.h"
#include "zconf.h"
#include "igzip_lib.h"
}

#define DEF_MEM_LEVEL 8

#ifdef DEBUG
#  define DBG(M, ...)  fprintf(stdout, "[DEBUG] (%s %s:%d) " M "\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#  define DBG(M, ...)
#endif

static jfieldID FID_lz_stream;
static jfieldID FID_inputBuffer;
static jfieldID FID_inputBufferLength;
static jfieldID FID_endOfStream;
static jfieldID FID_finished;
static jfieldID FID_finish;
static jfieldID FID_level;

/**
 *  Cache the Java field IDs. Called once when the native library is loaded. 
 */
JNIEXPORT void JNICALL Java_com_intel_gkl_compression_IntelDeflater_initNative
(JNIEnv* env, jclass cls) {
  FID_lz_stream = env->GetFieldID(cls, "lz_stream", "J");
  FID_inputBuffer = env->GetFieldID(cls, "inputBuffer", "[B");
  FID_inputBufferLength = env->GetFieldID(cls, "inputBufferLength", "I");
  FID_endOfStream = env->GetFieldID(cls, "endOfStream", "Z");
  FID_finished = env->GetFieldID(cls, "finished", "Z");
  FID_finish = env->GetFieldID(cls, "finish", "Z");
  FID_level = env->GetFieldID(cls,"level","I");
}

/**
 *  Reset the compressor to prepare for new input data. If the stream structure
 *  does not exists yet, it is allocated.
 */
JNIEXPORT void JNICALL Java_com_intel_gkl_compression_IntelDeflater_resetNative
(JNIEnv* env, jobject obj, jboolean nowrap) {
 
  jint level = env->GetIntField(obj, FID_level);

  if(level == 1) {
    isal_zstream* lz_stream = (isal_zstream*)env->GetLongField(obj, FID_lz_stream);

    if (lz_stream == 0) {
      lz_stream = (isal_zstream*)malloc(sizeof(isal_zstream));
      if ( lz_stream == NULL ) {
        jclass Exception = env->FindClass("java/lang/Exception");
        env->ThrowNew(Exception,"Memory allocation error");
      }
      env->SetLongField(obj, FID_lz_stream, (jlong)lz_stream);
    }
  
    isal_deflate_init(lz_stream);
    lz_stream->end_of_stream = 0;
  
    DBG("lz_stream = 0x%lx", (long)lz_stream);
  }
  else {
    
    z_stream* lz_stream = (z_stream*)env->GetLongField(obj, FID_lz_stream);

    if (lz_stream == 0) {
      lz_stream = (z_stream*)calloc(1, sizeof(z_stream));
      if ( lz_stream == NULL ) {
        jclass Exception = env->FindClass("java/lang/Exception");
        env->ThrowNew(Exception,"Memory allocation error");
      }
      env->SetLongField(obj, FID_lz_stream, (jlong)lz_stream);
      
      lz_stream->zalloc = Z_NULL;
      lz_stream->zfree = Z_NULL;
      lz_stream->opaque = Z_NULL;
      int ret = deflateInit2(lz_stream, level,  Z_DEFLATED,
                             nowrap ? -MAX_WBITS : MAX_WBITS,
                             DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY);
      if (ret != Z_OK) {
        jclass Exception = env->FindClass("java/lang/Exception");
        env->ThrowNew(Exception,"IntelDeflater init error");
      }
    }
    else {
      deflateReset(lz_stream);
    }

    DBG("lz_stream = 0x%lx", (long)lz_stream);  
  }
}


/**
 *  Deflate the data.
 */
JNIEXPORT jint JNICALL Java_com_intel_gkl_compression_IntelDeflater_deflateNative
(JNIEnv * env, jobject obj, jbyteArray outputBuffer, jint outputBufferLength) {
 
  jbyteArray inputBuffer = (jbyteArray)env->GetObjectField(obj, FID_inputBuffer);
  jint inputBufferLength = env->GetIntField(obj, FID_inputBufferLength);
  jboolean endOfStream = env->GetBooleanField(obj, FID_endOfStream);
  jint level = env->GetIntField(obj, FID_level);
  jboolean finish = env->GetBooleanField(obj, FID_finish);

  if(level == 1) {
  
    isal_zstream* lz_stream = (isal_zstream*)env->GetLongField(obj, FID_lz_stream);
  
    DBG("lz_stream = 0x%lx", (long)lz_stream);

    jbyte* next_in = (jbyte*)env->GetPrimitiveArrayCritical(inputBuffer, 0);
    jbyte* next_out = (jbyte*)env->GetPrimitiveArrayCritical(outputBuffer, 0);

    lz_stream->next_in = (UINT8*)next_in;
    lz_stream->avail_in = inputBufferLength;
    lz_stream->end_of_stream = endOfStream;
    lz_stream->next_out = (UINT8*)next_out;
    lz_stream->avail_out = outputBufferLength;

    DBG("end_of_stream = %d", lz_stream->end_of_stream);
    DBG("avail_in = %d", lz_stream->avail_in);
    DBG("avail_out = %d", lz_stream->avail_out);
    DBG("total_out = %d", lz_stream->total_out);

    int bytes_in = inputBufferLength;

    DBG("Compressing");

    // compress and update lz_stream state
    isal_deflate_stateless(lz_stream);
    long bytes_out = outputBufferLength - lz_stream->avail_out;

    DBG("Compression ratio = %2.2f", 100.0 - (100.0 * bytes_out / bytes_in));
    DBG("avail_in = %d", lz_stream->avail_in);
    DBG("avail_out = %d", lz_stream->avail_out);
    DBG("total_out = %d", lz_stream->total_out);

    // release buffers
    env->ReleasePrimitiveArrayCritical(inputBuffer, next_in, 0);
    env->ReleasePrimitiveArrayCritical(outputBuffer, next_out, 0);

    // set finished if endOfStream was set and all input processed
    if (endOfStream && lz_stream->avail_in == 0) {
      env->SetLongField(obj, FID_finished, true);
    }

    // return number of bytes written to output buffer
    return bytes_out;
  }
  else {
      
    z_stream* lz_stream = (z_stream*)env->GetLongField(obj, FID_lz_stream);

    jbyte* next_in = (jbyte*)env->GetPrimitiveArrayCritical(inputBuffer, 0);
    jbyte* next_out = (jbyte*)env->GetPrimitiveArrayCritical(outputBuffer, 0);

    lz_stream->next_in = (Bytef *) next_in;
    lz_stream->avail_in = (uInt) inputBufferLength;
    lz_stream->next_out = (Bytef *) next_out;
    lz_stream->avail_out = (uInt) outputBufferLength;
      
    int bytes_in = inputBufferLength;

    DBG("Compressing");

    // compress and update lz_stream state
    int ret = deflate(lz_stream, Z_FINISH);
    int bytes_out = outputBufferLength - lz_stream->avail_out;

    DBG("Compression ratio = %2.2f", 100.0 - (100.0 * bytes_out / bytes_in));
    DBG("avail_in = %d", lz_stream->avail_in);
    DBG("avail_out = %d", lz_stream->avail_out);
    DBG("total_out = %d", lz_stream->total_out);

    // release buffers
    env->ReleasePrimitiveArrayCritical(inputBuffer, next_in, 0);
    env->ReleasePrimitiveArrayCritical(outputBuffer, next_out, 0);

    if (ret == Z_STREAM_END && lz_stream->avail_in == 0) { 
      env->SetLongField(obj, FID_finished, true);
    }

    return bytes_out;
  }
}

/**
 *  Close the compressor and reclaim memory
 */
JNIEXPORT void JNICALL
Java_com_intel_gkl_compression_IntelDeflater_endNative(JNIEnv *env, jobject obj)
{
  jint level = env->GetIntField(obj, FID_level);
  z_stream* lz_stream = (z_stream*)env->GetLongField(obj, FID_lz_stream);
  if (level != 1) {
    deflateEnd(lz_stream);
  }
}
