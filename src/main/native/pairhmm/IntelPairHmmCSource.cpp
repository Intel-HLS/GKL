#include <avx.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>
#include <xmmintrin.h>
#include "IntelPairHmm.h"
#include "pairhmm_common.h"
#include "avx_impl.h"
#ifndef __APPLE__
  #include "avx512_impl.h"
#endif
#include "Context.h"
#include"debug.h"
bool g_use_double;
int g_max_threads;
bool g_use_fpga;

Context<float> g_ctxf;
Context<double> g_ctxd;

float (*g_compute_full_prob_float)(testcase *tc);
double (*g_compute_full_prob_double)(testcase *tc);

/*
 * Method:    initNative
 */
void initPairHMM()
{
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
}

/* Computelikelihoodsfloat method takes in one vector of input and computes the pHMM result float */

void computelikelihoodsfloat(testcase *testcases, float *expected_results)
{

    float result_final = 0;

    float result_float = g_compute_full_prob_float(testcases);

    result_final = (double)(log10f(result_float) - g_ctxf.LOG10_INITIAL_CONSTANT);

    (*expected_results) = (float)result_final;

    return;

}

/* Computelikelihoodsdouble method takes in one vector of input and computes the pHMM result double */

void computelikelihoodsdouble(testcase *testcases, double *expected_results)
{

    double result_final = 0;
    double result_double = g_compute_full_prob_double(testcases);
    result_final = log10(result_double) - g_ctxd.LOG10_INITIAL_CONSTANT;

    (*expected_results) = (double)result_final;

    return;

}

