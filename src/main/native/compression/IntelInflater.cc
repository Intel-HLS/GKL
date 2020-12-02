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
#include <string.h>
#include <sys/time.h>
#include "IntelInflater.h"

extern "C" {
#include "zlib.h"
#include "zconf.h"
#include "igzip_lib.h"
}


//#define DEBUG
//#define profile

#ifdef DEBUG
#  define DBG(M, ...)  fprintf(stdout, "[DEBUG] (%s %s:%d) " M "\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#  define DBG(M, ...)
#endif

static jfieldID FID_inf_lz_stream;
static jfieldID FID_inf_inputBuffer;
static jfieldID FID_inf_inputBufferLength;
static jfieldID FID_inf_finished;
static jfieldID FID_inf_inputBufferOffset;



/**
 *  Cache the Java field IDs. Called once when the native library is loaded.
 */
JNIEXPORT void JNICALL Java_com_intel_gkl_compression_IntelInflater_initNative
(JNIEnv* env, jclass cls) {
  FID_inf_lz_stream = env->GetFieldID(cls, "lz_stream", "J");
  FID_inf_inputBuffer = env->GetFieldID(cls, "inputBuffer", "[B");
  FID_inf_inputBufferLength = env->GetFieldID(cls, "inputBufferLength", "I");
  FID_inf_finished = env->GetFieldID(cls, "finished", "Z");
  FID_inf_inputBufferOffset = env->GetFieldID(cls, "inputBufferOffset", "I");

}

JNIEXPORT void JNICALL Java_com_intel_gkl_compression_IntelInflater_resetNative
(JNIEnv* env, jobject obj, jboolean nowrap) {


    inflate_state* lz_stream = (inflate_state*)env->GetLongField(obj, FID_inf_lz_stream);

    if(lz_stream == 0) {

     lz_stream = (inflate_state*)calloc(1,sizeof(inflate_state));
      if ( lz_stream == NULL ) {
        if(env->ExceptionCheck())
            env->ExceptionClear();
        env->ThrowNew(env->FindClass("java/lang/OutOfMemoryError"),"Memory allocation error");
      }
      env->SetLongField(obj, FID_inf_lz_stream, (jlong)lz_stream);

    }
      isal_inflate_init(lz_stream);

      lz_stream->avail_in = 0;
      lz_stream->next_in = Z_NULL;
      lz_stream->avail_out = 0;
      lz_stream->next_out = Z_NULL;

}


JNIEXPORT jint JNICALL Java_com_intel_gkl_compression_IntelInflater_inflateNative
(JNIEnv * env, jobject obj, jbyteArray outputBuffer, jint outputBufferOffset, jint outputBufferLength)
{



   inflate_state* lz_stream = (inflate_state*)env->GetLongField(obj, FID_inf_lz_stream);

    jbyteArray inputBuffer = (jbyteArray)env->GetObjectField(obj, FID_inf_inputBuffer);
      jint inputBufferLength = env->GetIntField(obj, FID_inf_inputBufferLength);
      jint inputBufferOffset = env->GetIntField(obj, FID_inf_inputBufferOffset);


        jbyte* next_in = (jbyte*)env->GetPrimitiveArrayCritical(inputBuffer, 0);
        jbyte* next_out = (jbyte*)env->GetPrimitiveArrayCritical(outputBuffer, 0);


        lz_stream->next_in = (Bytef *) (next_in + inputBufferOffset);
        lz_stream->avail_in = (uInt) inputBufferLength;
        lz_stream->next_out = (Bytef *) (next_out + outputBufferOffset);
        lz_stream->avail_out = (uInt) outputBufferLength;

        int bytes_in = inputBufferLength;

        DBG("Decompressing");

        // compress and update lz_stream state

#ifdef profile
    struct timeval  tv1, tv2;
    gettimeofday(&tv1, NULL);
#endif


    int ret = isal_inflate(lz_stream);

#ifdef profile
    gettimeofday(&tv2, NULL);

    DBG ("Total time = %f seconds\n",
         (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
         (double) (tv2.tv_sec - tv1.tv_sec));
#endif
         int bytes_out = outputBufferLength - lz_stream->avail_out;


        DBG("%s", lz_stream->msg);
        DBG("%d", ret);
        DBG("avail_in = %d", lz_stream->avail_in);
        DBG("avail_out = %d", lz_stream->avail_out);
        DBG("total_out = %d", lz_stream->total_out);

        // release buffers
        env->ReleasePrimitiveArrayCritical(inputBuffer, next_in, 0);
        env->ReleasePrimitiveArrayCritical(outputBuffer, next_out, 0);

        if (ret == ISAL_END_INPUT && lz_stream->avail_in == 0) {
          env->SetBooleanField(obj, FID_inf_finished, true);
        }

        return bytes_out;

}

/*
**
 *  Close the compressor and reclaim memory
 */
JNIEXPORT void JNICALL
Java_com_intel_gkl_compression_IntelInflater_endNative(JNIEnv *env, jobject obj)
{

  inflate_state* lz_stream = (inflate_state*)env->GetLongField(obj, FID_inf_lz_stream);
  free(lz_stream);

}
