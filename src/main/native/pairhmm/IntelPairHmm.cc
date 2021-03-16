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
#ifdef linux
#include <omp.h>
#endif
#include <vector>
#include <math.h>
#include <debug.h>
#include <avx.h>
#include <assert.h>
#include "IntelPairHmm.h"
#include "pairhmm_common.h"
#include "avx_impl.h"
#ifndef __APPLE__
  #include "avx512_impl.h"
#endif
#include "Context.h"
#include "JavaData.h"

bool g_use_double;
int g_max_threads;

Context<float> g_ctxf;
Context<double> g_ctxd;

float (*g_compute_full_prob_float)(testcase *tc);
double (*g_compute_full_prob_double)(testcase *tc);

/*
 * Class:     com_intel_gkl_pairhmm_IntelPairHmm
 * Method:    initNative
 * Signature: (ZI)V
 */
JNIEXPORT void JNICALL Java_com_intel_gkl_pairhmm_IntelPairHmm_initNative
(JNIEnv* env, jclass cls, jclass readDataHolder, jclass haplotypeDataHolder,
 jboolean use_double, jint max_threads)
{
  DBG("Enter");

  JavaData javaData(env);
  try {
    javaData.init(readDataHolder, haplotypeDataHolder);
  } catch (JavaException& e) {
    env->ExceptionClear();
    env->ThrowNew(env->FindClass(e.classPath), e.message);
    return;
  }

  g_use_double = use_double;
  
#ifdef _OPENMP
  int avail_threads = omp_get_max_threads();
  int req_threads = max_threads;
  g_max_threads = std::min(req_threads, avail_threads);

  DBG("Available threads: %d", avail_threads);
  DBG("Requested threads: %d", req_threads);
  if (req_threads > avail_threads) {
    DBG("Using %d available threads, but %d were requested", g_max_threads, req_threads);
  }
  else {
    DBG("Using %d threads", g_max_threads);
  }
#else
  if (max_threads != 1) {
    DBG("Ignoring request for %d threads; not using OpenMP implementation", max_threads);
  }
#endif


  // enable FTZ
  if (_MM_GET_FLUSH_ZERO_MODE() != _MM_FLUSH_ZERO_ON) {
    DBG("Flush-to-zero (FTZ) is enabled when running PairHMM");
  }
  _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);

  // set function pointers
  if(is_avx512_supported())
  {
#ifndef __APPLE__
    DBG("Using CPU-supported AVX-512 instructions");
    g_compute_full_prob_float = compute_fp_avx512s;
    g_compute_full_prob_double = compute_fp_avx512d;
#else
    assert(false);
#endif
  }
  else
  {
    g_compute_full_prob_float = compute_fp_avxs;
    g_compute_full_prob_double = compute_fp_avxd;
  }

  // init convert char table
  ConvertChar::init();
  DBG("Exit");
}

/*
 * Class:     com_intel_gkl_pairhmm_IntelPairHmm
 * Method:    computeLikelihoodsNative
 * Signature: (II[[B[[B[[B[[B[[B[[B[D)V
 */
JNIEXPORT void JNICALL Java_com_intel_gkl_pairhmm_IntelPairHmm_computeLikelihoodsNative
(JNIEnv* env, jobject obj,
 jobjectArray readDataArray, jobjectArray haplotypeDataArray, jdoubleArray likelihoodArray)
{
  DBG("Enter");

  //==================================================================
  // get Java data
  JavaData javaData(env);

  std::vector<testcase> testcases;
  double* javaResults;

  try {
    testcases = javaData.getData(readDataArray, haplotypeDataArray);
    javaResults = javaData.getOutputArray(likelihoodArray);
  } catch (JavaException& e) {
    env->ExceptionClear();
    env->ThrowNew(env->FindClass(e.classPath), e.message);
    return;
  }
  
  //==================================================================
  // calcutate pairHMM

    try {
        #ifdef _OPENMP
              #pragma omp parallel for schedule(dynamic, 1) num_threads(g_max_threads)
        #endif
        for (int i = 0; i < testcases.size(); i++) {

            double result_final = 0;
            float result_float = g_use_double ? 0.0f : g_compute_full_prob_float(&testcases[i]);

            if (result_float < MIN_ACCEPTED) {
              double result_double = g_compute_full_prob_double(&testcases[i]);
              result_final = log10(result_double) - g_ctxd.LOG10_INITIAL_CONSTANT;
            }
            else {
              result_final = (double)(log10f(result_float) - g_ctxf.LOG10_INITIAL_CONSTANT);
            }

            javaResults[i] = result_final;
            DBG("result = %e", result_final);
          }
    }
    catch (JavaException& e) {
       #ifdef _OPENMP
            #pragma omp barrier
       #endif
       env->ExceptionClear();
       env->ThrowNew(env->FindClass(e.classPath), "Error in pairhmm processing.");
       return;
    }

  DBG("Exit");
}


/*
 * Class:     com_intel_gkl_pairhmm_IntelPairHmm
 * Method:    doneNative
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_intel_gkl_pairhmm_IntelPairHmm_doneNative
(JNIEnv* env, jobject obj)
{
}
