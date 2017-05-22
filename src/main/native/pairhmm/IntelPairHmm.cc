#ifdef linux
#include <omp.h>
#endif
#include <vector>
#include <math.h>
#include "IntelPairHmm.h"
#include "pairhmm_common.h"
#include "avx-pairhmm.h"
#include "shacc_pairhmm.h"
#include "JavaData.h"
#include "debug.h"

bool g_use_double;
int g_max_threads;
bool g_use_fpga;

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
 jboolean use_double, jint max_threads, jboolean use_fpga)
{
  DBG("Enter");

  JavaData javaData;
  javaData.init(env, readDataHolder, haplotypeDataHolder);

  g_use_double = use_double;
  
#ifdef _OPENMP
  int avail_threads = omp_get_max_threads();
  int req_threads = (int)max_threads;
  g_max_threads = std::min(req_threads, avail_threads);

  INFO("Available threads: %d", avail_threads);
  INFO("Requested threads: %d", req_threads);
  if (req_threads > avail_threads) {
    WARN("Using %d available threads, but %d were requested", g_max_threads, req_threads);
  }
  else {
    INFO("Using %d threads", g_max_threads);
  }
#else
  if ((int)max_threads != 1) {
    WARN("Ignoring request for %d threads; not using OpenMP implementation", (int)max_threads);
  }
#endif

  g_use_fpga = use_fpga;

  // enable FTZ
  _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);

  // set function pointers
  g_compute_full_prob_float = compute_full_prob_avxs;
  g_compute_full_prob_double = compute_full_prob_avxd;

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
  JavaData javaData;
  std::vector<testcase> testcases = javaData.getData(env, readDataArray, haplotypeDataArray);
  double* javaResults = javaData.getOutputArray(env, likelihoodArray);
  
  //==================================================================
  // calcutate pairHMM
  shacc_pairhmm::Batch batch;
  bool batch_valid = false;
  if (g_use_fpga && !g_use_double) {
    batch = javaData.getBatch();
    batch_valid = shacc_pairhmm::calculate(batch);
  }

#ifdef _OPENMP
  #pragma omp parallel for schedule(dynamic, 1) num_threads(g_max_threads)
#endif
  for (int i = 0; i < testcases.size(); i++) {
    double result_final = 0;

    float result_float = g_use_double ? 0.0f : 
      batch_valid ? batch.results[i] : g_compute_full_prob_float(&testcases[i]);

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

  //==================================================================
  // release Java data
  javaData.releaseData(env);
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
