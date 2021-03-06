/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2021 Intel Corporation
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
#include <stdlib.h>
#include <string.h>

#include <sys/time.h>
#include "IntelDeflater.h"

extern "C" {
#include "zlib.h"
#include "zconf.h"
#include "igzip_lib.h"
}

#define DEF_MEM_LEVEL 8
//#define DEBUG
//#define profile


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
static jfieldID FID_level;
//static jfieldID FID_inputBufferOffset;



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
  FID_level = env->GetFieldID(cls,"level","I");
//  FID_inputBufferOffset = env->GetFieldID(cls, "inputBufferOffset", "I");

}

/**
 *  Reset the compressor to prepare for new input data. If the stream structure
 *  does not exists yet, it is allocated.
 */
JNIEXPORT void JNICALL Java_com_intel_gkl_compression_IntelDeflater_resetNative
(JNIEnv* env, jobject obj, jboolean nowrap) {
 
  jint level = env->GetIntField(obj, FID_level);

  if(level == 1 || level ==2) {

        isal_zstream* lz_stream = (isal_zstream*)env->GetLongField(obj, FID_lz_stream);
        if (lz_stream == 0) {
              lz_stream = (isal_zstream*)malloc(sizeof(isal_zstream));
              if ( lz_stream == NULL ) {
                if(env->ExceptionCheck())
                    env->ExceptionClear();
                env->ThrowNew(env->FindClass("java/lang/OutOfMemoryError"),"Memory allocation error");
                return;
              }

               isal_deflate_stateless_init(lz_stream);

               lz_stream->level = level;
               lz_stream->level_buf = (uint8_t*)malloc(ISAL_DEF_LVL2_DEFAULT);
               lz_stream->level_buf_size = ISAL_DEF_LVL2_DEFAULT;

              if (lz_stream->level_buf == NULL) {
                  if(env->ExceptionCheck()) {
                    env->ExceptionClear();
                  }
                  env->ThrowNew(env->FindClass("java/lang/OutOfMemoryError"),"Memory allocation error");
                  free(lz_stream);
                  return;
              }

              env->SetLongField(obj, FID_lz_stream, (jlong)lz_stream);
        }
        else {

          isal_deflate_stateless_init(lz_stream);

        }

        lz_stream->end_of_stream = 0;

       // DBG("lz_stream = 0x%lx", (long)lz_stream);
  }
  else {

        z_stream* lz_stream = (z_stream*)env->GetLongField(obj, FID_lz_stream);

        if (lz_stream == 0) {
              lz_stream = (z_stream*)calloc(1, sizeof(z_stream));
              if ( lz_stream == NULL ) {
                if(env->ExceptionCheck())
                    env->ExceptionClear();
                env->ThrowNew(env->FindClass("java/lang/OutOfMemoryError"),"Memory allocation error");
                return;
              }
              env->SetLongField(obj, FID_lz_stream, (jlong)lz_stream);

              lz_stream->zalloc = Z_NULL;
              lz_stream->zfree = Z_NULL;
              lz_stream->opaque = Z_NULL;
              int ret = deflateInit2(lz_stream, level,  Z_DEFLATED,
                                     nowrap ? -MAX_WBITS : MAX_WBITS,
                                     DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY);
              if (ret != Z_OK ) {
                if(env->ExceptionCheck())
                    env->ExceptionClear();
                env->ThrowNew(env->FindClass("java/lang/RuntimeException"),"IntelDeflater init error");
              }
        }
        else {
          deflateReset(lz_stream);
        }


  }
}


/**
* Generate Dynamic Huffman tables
* This function will be called only if we are implementing the fully dynamic huffman implementation which is not set as default in current implementation
* current implementation we use semi-dynamic huffman with tables generated using only first 64k block of stream
*/

JNIEXPORT void JNICALL Java_com_intel_gkl_compression_IntelDeflater_generateHuffman
(JNIEnv * env, jobject obj) {
      jint level = env->GetIntField(obj, FID_level);

      if(level == 1) {
          isal_zstream* lz_stream = (isal_zstream*)env->GetLongField(obj, FID_lz_stream);
          if (lz_stream == NULL) {
            if(env->ExceptionCheck())
                env->ExceptionClear();
            env->ThrowNew(env->FindClass("java/lang/NullPointerException"), "lz_stream is NULL.");
            return;
          }

          jbyteArray inputBuffer = (jbyteArray)env->GetObjectField(obj, FID_inputBuffer);
          jbyte* input = (jbyte*)env->GetPrimitiveArrayCritical(inputBuffer, 0);

          if (input == NULL) {
            if (env->ExceptionCheck())
              env->ExceptionClear();
            env->ThrowNew(env->FindClass("java/lang/NullPointerException"), "inputBuffer is null.");
            env->ReleasePrimitiveArrayCritical(inputBuffer, input, 0);
            return;
          }

          struct isal_huff_histogram *histogram = (struct isal_huff_histogram *) malloc(sizeof(*histogram));
          struct isal_hufftables *hufftables_custom;

          if (histogram == NULL) {
            DBG ("Malloc failed out of memory");
            if(env->ExceptionCheck())
                env->ExceptionClear();
            env->ThrowNew(env->FindClass("java/lang/OutOfMemoryError"),"Memory allocation error");
            env->ReleasePrimitiveArrayCritical(inputBuffer, input, 0);
            return;
          }

          memset(histogram, 0, sizeof(isal_huff_histogram));
          isal_update_histogram((unsigned char*)input, 64*1024, histogram);

          if (isal_create_hufftables(hufftables_custom, histogram) != 0) {
            env->ExceptionClear();
            env->ThrowNew(env->FindClass("java/lang/RuntimeException"), "Invalid huffman code was created.");
            env->ReleasePrimitiveArrayCritical(inputBuffer, input, 0);
            free(histogram);
            return;
          }

          lz_stream->hufftables = hufftables_custom;

          env->ReleasePrimitiveArrayCritical(inputBuffer, input, 0);

          free(histogram);
      }

}

/**
 *  Deflate the data.
 *  isa-l check isalExternal/igzip/igzip_rand_test.c
 *  zib check src/main/native/compression/otc_zlib/zutil.c
 */
JNIEXPORT jint JNICALL Java_com_intel_gkl_compression_IntelDeflater_deflateNative
(JNIEnv * env, jobject obj, jbyteArray outputBuffer, jint outputBufferLength) {



  jbyteArray inputBuffer = (jbyteArray)env->GetObjectField(obj, FID_inputBuffer);
  jint inputBufferLength = env->GetIntField(obj, FID_inputBufferLength);
  //jint inputBufferOffset = env->GetIntField(obj, FID_inputBufferOffset);
  jboolean endOfStream = env->GetBooleanField(obj, FID_endOfStream);
  jint level = env->GetIntField(obj, FID_level);
  const char* err_msg;

  if( outputBufferLength == 0 || inputBufferLength == 0 )
  {
          if (env->ExceptionCheck())
              env->ExceptionClear();
          env->ThrowNew(env->FindClass("java/lang/NullPointerException"), " Buffer size not right.");
          return -1;
  }

  if(level == 1 || level == 2 ) {
  
        isal_zstream* lz_stream = (isal_zstream*)env->GetLongField(obj, FID_lz_stream);

        jbyte* next_in = (jbyte*)env->GetPrimitiveArrayCritical(inputBuffer, 0);
        if (next_in == NULL ) {
          if (env->ExceptionCheck())
            env->ExceptionClear();
          env->ThrowNew(env->FindClass("java/lang/NullPointerException"), "inputBuffer is null.");
          return -1;
        }

        jbyte* next_out = (jbyte*)env->GetPrimitiveArrayCritical(outputBuffer, 0);
        if (next_out == NULL ) {
          if (env->ExceptionCheck())
            env->ExceptionClear();
          env->ThrowNew(env->FindClass("java/lang/NullPointerException"), "outputBuffer is null.");
          env->ReleasePrimitiveArrayCritical(inputBuffer, next_in, 0);
          return -1;
        }

        lz_stream->next_in = (uint8_t*) (next_in);
        lz_stream->avail_in = inputBufferLength;
        lz_stream->end_of_stream = endOfStream;
        lz_stream->next_out = (uint8_t*) (next_out);
        lz_stream->avail_out = outputBufferLength ;

        DBG("Compressing");

        #ifdef profile
            struct timeval  tv1, tv2;
            gettimeofday(&tv1, NULL);
        #endif

        // compress and update lz_stream state
        int ret = isal_deflate_stateless(lz_stream);

        #ifdef profile
            gettimeofday(&tv2, NULL);
            DBG ("Total time = %f seconds\n",
                     (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
                     (double) (tv2.tv_sec - tv1.tv_sec));
        #endif

        DBG ("avail_out = %d \n",lz_stream->avail_out );

        // release buffers
        env->ReleasePrimitiveArrayCritical(inputBuffer, next_in, 0);
        env->ReleasePrimitiveArrayCritical(outputBuffer, next_out, 0);

        // set finished if endOfStream was set and all input processed
        if (endOfStream && lz_stream->avail_in == 0) {
          env->SetBooleanField(obj, FID_finished, true);
        }

        if (ret != COMP_OK) {
              
              switch (ret) {
                case INVALID_FLUSH:
                  err_msg = "Invalid FLUSH selected.";
                  break;
                case ISAL_INVALID_LEVEL:
                  err_msg = "Invalid compression level set.";
                  break;
                case ISAL_INVALID_LEVEL_BUF:
                  err_msg = "Level buffer is not large enough.";
                  break;
                case STATELESS_OVERFLOW:
                  err_msg = "Output buffer too small.";
                  break;
                default:
                  err_msg = "isal_deflate_stateless returned an unknown return code.";
                  DBG("isal_deflate_stateless returned %d", ret);
              }

              env->ExceptionClear();
              env->ThrowNew(env->FindClass("java/lang/RuntimeException"), err_msg);
              return -1;
        }

        // return number of bytes written to output buffer
        long bytes_out = outputBufferLength - lz_stream->avail_out;
        if(bytes_out == 0 )
        {
            env->ExceptionClear();
            env->ThrowNew(env->FindClass("java/lang/RuntimeException"), "No bytes written");
            return -1;
        }
        
        DBG ("bytes_out = %d \n", bytes_out);
        return bytes_out;
  }
  else {
      
        z_stream* lz_stream = (z_stream*)env->GetLongField(obj, FID_lz_stream);
        if (lz_stream == NULL) {
          if(env->ExceptionCheck())
              env->ExceptionClear();
          env->ThrowNew(env->FindClass("java/lang/NullPointerException"), "lz_stream is NULL.");
          return -1;
        }

        jbyte* next_in = (jbyte*)env->GetPrimitiveArrayCritical(inputBuffer, 0);
        if (next_in == NULL ) {
          if (env->ExceptionCheck())
            env->ExceptionClear();
          env->ThrowNew(env->FindClass("java/lang/NullPointerException"), "inputBuffer is null.");
          return -1;
        }

        jbyte* next_out = (jbyte*)env->GetPrimitiveArrayCritical(outputBuffer, 0);
        if (next_out == NULL ) {
          if (env->ExceptionCheck())
            env->ExceptionClear();
          env->ThrowNew(env->FindClass("java/lang/NullPointerException"), "outputBuffer is null.");
          env->ReleasePrimitiveArrayCritical(inputBuffer, next_in, 0);
          return -1;
        }

        lz_stream->next_in = (Bytef *) next_in;
        lz_stream->avail_in = (uInt) inputBufferLength;
        lz_stream->next_out = (Bytef *) next_out;
        lz_stream->avail_out = (uInt) outputBufferLength;

        DBG("Decompressing");

        #ifdef profile
            struct timeval  tv1, tv2;
            gettimeofday(&tv1, NULL);
        #endif

        // compress and update lz_stream state
        int ret = deflate(lz_stream, Z_FINISH);

        #ifdef profile
             gettimeofday(&tv2, NULL);
             DBG ("Total time = %f seconds\n",
                      (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
                      (double) (tv2.tv_sec - tv1.tv_sec));
        #endif

        // release buffers
        env->ReleasePrimitiveArrayCritical(inputBuffer, next_in, 0);
        env->ReleasePrimitiveArrayCritical(outputBuffer, next_out, 0);

        if (ret == Z_STREAM_END && lz_stream->avail_in == 0) {
          env->SetBooleanField(obj, FID_finished, true);
        }
        else if (ret != Z_OK) {
          const char* err_msg;
          switch (ret) {
            case Z_STREAM_ERROR:
              err_msg = "Stream state is inconsistent.";
              break;
            case Z_BUF_ERROR:
              err_msg = "No progress possible. Buffer error.";
              break;
            default:
              err_msg = zError(ret);
              DBG("deflate returned %d", err_msg);
          }

          env->ExceptionClear();
          env->ThrowNew(env->FindClass("java/lang/RuntimeException"), err_msg);
          return -1;
        }

         int bytes_out = outputBufferLength - lz_stream->avail_out;
         if(bytes_out == 0)
         {
            env->ExceptionClear();
            env->ThrowNew(env->FindClass("java/lang/RuntimeException"), "No bytes written");
            return -1;
         }
         DBG ("bytes_out = %d \n", bytes_out);
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

   if (level == 1 || level == 2 ) {
      isal_zstream* lz_stream = (isal_zstream*)env->GetLongField(obj, FID_lz_stream);
      isal_deflate_reset(lz_stream);
      free(lz_stream->level_buf);
      free(lz_stream);
   }
   else {
      z_stream* lz_stream = (z_stream*)env->GetLongField(obj, FID_lz_stream);
      deflateReset(lz_stream);
      free(lz_stream);
  }

  env->SetLongField(obj, FID_lz_stream, 0);

}
