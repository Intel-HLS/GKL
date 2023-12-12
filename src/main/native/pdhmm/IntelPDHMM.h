/*
#Copyright(C) 2023 Intel Corporation
#SPDX - License - Identifier : MIT License
*/

#ifndef _Included_com_intel_gkl_pdhmm_IntelPDHMM
#define _Included_com_intel_gkl_pdhmm_IntelPDHMM
#include <jni.h>
#ifdef __cplusplus
extern "C"
{
#endif

	JNIEXPORT void JNICALL Java_com_intel_gkl_pdhmm_IntelPDHMM_initNative(JNIEnv *env, jclass obj);

	/*
	 * Class:     com_intel_gkl_pdhmm_IntelPDHMM
	 * Method:    computeReadLikelihoodGivenHaplotypeLog10Vec
	 * Signature: ()Z
	 */
	JNIEXPORT jdoubleArray JNICALL Java_com_intel_gkl_pdhmm_IntelPDHMM_computePDHMMNative(JNIEnv *env, jobject obj, jbyteArray jhap_bases, jbyteArray jhap_pdbases, jbyteArray jread_bases, jbyteArray jread_qual, jbyteArray jread_ins_qual, jbyteArray jread_del_qual, jbyteArray jgcp, jlongArray jhap_lengths, jlongArray jread_lengths, jint testcase, jint maxHapLength, jint maxReadLength);

#ifdef __cplusplus
}
#endif
#endif