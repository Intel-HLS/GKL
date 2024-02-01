/*
#Copyright(C) 2023 Intel Corporation
#SPDX - License - Identifier : MIT License
*/

#include "IntelPDHMM.h"
#include <debug.h>
#include "pdhmm-serial.h"
#include "avx2_impl.h"
#ifndef __APPLE__
#include "avx512_impl.h"
#endif
#include <avx.h>

int32_t (*g_computePDHMM)(const int8_t *hap_bases, const int8_t *hap_pdbases, const int8_t *read_bases, const int8_t *read_qual, const int8_t *read_ins_qual, const int8_t *read_del_qual, const int8_t *gcp, double *result, int64_t t, const int64_t *hap_lengths, const int64_t *read_lengths, int32_t maxReadLength, int32_t maxHaplotypeLength);

inline bool is_sse_supported()
{
    uint32_t a, b, c, d;
    uint32_t sse_mask = (1 << 27) | bit_SSE2 | bit_SSE4_1;

    __cpuid_count(1, 0, a, b, c, d);
    if ((c & sse_mask) != sse_mask)
    {
        return false;
    }

    if (!check_xcr0_ymm())
    {
        return false;
    }

    return true;
}

JNIEXPORT void JNICALL Java_com_intel_gkl_pdhmm_IntelPDHMM_initNative(JNIEnv *env, jclass obj)
{
    if (is_avx512_supported())
    {
#ifndef __APPLE__
        INFO("Using CPU-supported AVX-512 instructions.");
        g_computePDHMM = computePDHMM_fp_avx512;

#else
        assert(false);
#endif
    }
    else if (is_avx_supported() && is_avx2_supported() && is_sse_supported())
    {
        INFO("Using CPU-supported AVX-2, AVX and SSE instructions.");
        g_computePDHMM = computePDHMM_fp_avx2;
    }
    else
    {
        INFO("Using Serial Implementation.");
        g_computePDHMM = computePDHMM_serial;
    }
    return;
}

/*
 * Class:     com_intel_gkl_pdhmm_IntelPDHMM
 * Method:    computePDHMM
 * Signature: (Z)V
 */
JNIEXPORT jdoubleArray JNICALL Java_com_intel_gkl_pdhmm_IntelPDHMM_computePDHMMNative(JNIEnv *env, jobject obj, jbyteArray jhap_bases, jbyteArray jhap_pdbases, jbyteArray jread_bases, jbyteArray jread_qual, jbyteArray jread_ins_qual, jbyteArray jread_del_qual, jbyteArray jgcp, jlongArray jhap_lengths, jlongArray jread_lengths, jint testcase, jint maxHapLength, jint maxReadLength)
{
    jdoubleArray jresult;
    jresult = env->NewDoubleArray(testcase);
    if (jresult == NULL)
    {
        env->ExceptionClear();
        env->ThrowNew(env->FindClass("java/lang/OutOfMemoryError"), "Memory allocation issue.");
        return NULL; /* out of memory error thrown */
    }

    jbyte *hap_bases = (jbyte *)env->GetPrimitiveArrayCritical(jhap_bases, 0);
    jbyte *hap_pdbases = (jbyte *)env->GetPrimitiveArrayCritical(jhap_pdbases, 0);
    jbyte *read_bases = (jbyte *)env->GetPrimitiveArrayCritical(jread_bases, 0);
    jbyte *read_qual = (jbyte *)env->GetPrimitiveArrayCritical(jread_qual, 0);
    jbyte *read_ins_qual = (jbyte *)env->GetPrimitiveArrayCritical(jread_ins_qual, 0);
    jbyte *read_del_qual = (jbyte *)env->GetPrimitiveArrayCritical(jread_del_qual, 0);
    jbyte *gcp = (jbyte *)env->GetPrimitiveArrayCritical(jgcp, 0);
    jlong *hap_lengths = (jlong *)env->GetPrimitiveArrayCritical(jhap_lengths, 0);
    jlong *read_lengths = (jlong *)env->GetPrimitiveArrayCritical(jread_lengths, 0);

    if (hap_bases == NULL || hap_pdbases == NULL || read_bases == NULL || read_qual == NULL || read_ins_qual == NULL || read_del_qual == NULL || gcp == NULL || hap_lengths == NULL || read_lengths == NULL)
    {
        DBG("GetPrimitiveArrayCritical failed from JAVA unable to continue.");
        if (env->ExceptionCheck())
            env->ExceptionClear();
        env->ThrowNew(env->FindClass("java/lang/IllegalArgumentException"), "Input arrays aren't valid.");
        if (hap_bases != NULL)
            env->ReleasePrimitiveArrayCritical(jhap_bases, hap_bases, 0);
        if (hap_pdbases != NULL)
            env->ReleasePrimitiveArrayCritical(jhap_pdbases, hap_pdbases, 0);
        if (read_bases != NULL)
            env->ReleasePrimitiveArrayCritical(jread_bases, read_bases, 0);
        if (read_qual != NULL)
            env->ReleasePrimitiveArrayCritical(jread_qual, read_qual, 0);
        if (read_ins_qual != NULL)
            env->ReleasePrimitiveArrayCritical(jread_ins_qual, read_ins_qual, 0);
        if (read_del_qual != NULL)
            env->ReleasePrimitiveArrayCritical(jread_del_qual, read_del_qual, 0);
        if (gcp != NULL)
            env->ReleasePrimitiveArrayCritical(jgcp, gcp, 0);
        if (hap_lengths != NULL)
            env->ReleasePrimitiveArrayCritical(jhap_lengths, hap_lengths, 0);
        if (read_lengths != NULL)
            env->ReleasePrimitiveArrayCritical(jread_lengths, read_lengths, 0);
        return NULL;
    }

    double *result = (double *)_mm_malloc(testcase * sizeof(double), ALIGN_SIZE);
    if (result == NULL)
    {
        env->ExceptionClear();
        env->ThrowNew(env->FindClass("java/lang/OutOfMemoryError"), "Memory allocation issue.");
        return NULL;
    }
    int32_t status = g_computePDHMM(hap_bases, hap_pdbases, read_bases, read_qual, read_ins_qual, read_del_qual, gcp, result, testcase, hap_lengths, read_lengths, maxReadLength, maxHapLength);

    // release buffers
    env->ReleasePrimitiveArrayCritical(jhap_bases, hap_bases, 0);
    env->ReleasePrimitiveArrayCritical(jhap_pdbases, hap_pdbases, 0);
    env->ReleasePrimitiveArrayCritical(jread_bases, read_bases, 0);
    env->ReleasePrimitiveArrayCritical(jread_qual, read_qual, 0);
    env->ReleasePrimitiveArrayCritical(jread_ins_qual, read_ins_qual, 0);
    env->ReleasePrimitiveArrayCritical(jread_del_qual, read_del_qual, 0);
    env->ReleasePrimitiveArrayCritical(jgcp, gcp, 0);
    env->ReleasePrimitiveArrayCritical(jhap_lengths, hap_lengths, 0);
    env->ReleasePrimitiveArrayCritical(jread_lengths, read_lengths, 0);

    if (status != PDHMM_SUCCESS)
    {
        if (status == PDHMM_MEMORY_ALLOCATION_FAILED)
        {
            env->ExceptionClear();
            env->ThrowNew(env->FindClass("java/lang/OutOfMemoryError"), "Memory allocation issue.");
        }
        if (status == PDHMM_INPUT_DATA_ERROR)
        {
            env->ExceptionClear();
            env->ThrowNew(env->FindClass("java/lang/IllegalArgumentException"), "Error while calculating pdhmm. Input arrays aren't valid.");
        }
        if (status == PDHMM_FAILURE)
        {
            env->ExceptionClear();
            env->ThrowNew(env->FindClass("java/lang/RuntimeException"), "Failure while computing PDHMM.");
        }
    }
    else
    {
        env->SetDoubleArrayRegion(jresult, 0, testcase, result);
    }
    _mm_free(result);
    return jresult;
}
