/*
#Copyright(C) 2023 Intel Corporation
#SPDX - License - Identifier : MIT License
*/

#include <stdio.h>
#include <omp.h>
#include "pdhmm-serial.h"

inline int32_t allocateVec(double *&matchMatrixVec, double *&insertionMatrixVec, double *&deletionMatrixVec, double *&branchMatchMatrixVec, double *&branchInsertionMatrixVec, double *&branchDeletionMatrixVec, double *&transitionVec, double *&priorVec, bool *&constantsAreInitialized, bool *&initialized, INT_TYPE *&prev_hap_bases_length, const int32_t maxReadLength, const int32_t maxHaplotypeLength, const int32_t totalThreads)
{
	const int32_t paddedMaxReadLength = maxReadLength + 1;
	const int32_t paddedMaxHaplotypeLength = maxHaplotypeLength + 1;
	int32_t sizeOfTables = paddedMaxHaplotypeLength * SIMD_WIDTH_DOUBLE * totalThreads * sizeof(double);

	matchMatrixVec = (double *)_mm_malloc(sizeOfTables, ALIGN_SIZE);
	insertionMatrixVec = (double *)_mm_malloc(sizeOfTables, ALIGN_SIZE);
	deletionMatrixVec = (double *)_mm_malloc(sizeOfTables, ALIGN_SIZE);
	branchMatchMatrixVec = (double *)_mm_malloc(sizeOfTables, ALIGN_SIZE);
	branchInsertionMatrixVec = (double *)_mm_malloc(sizeOfTables, ALIGN_SIZE);
	branchDeletionMatrixVec = (double *)_mm_malloc(sizeOfTables, ALIGN_SIZE);

	priorVec = (double *)_mm_malloc(paddedMaxHaplotypeLength * paddedMaxReadLength * SIMD_WIDTH_DOUBLE * totalThreads * sizeof(double), ALIGN_SIZE);

	transitionVec = (double *)_mm_malloc(TRANS_PROB_ARRAY_LENGTH * paddedMaxReadLength * SIMD_WIDTH_DOUBLE * totalThreads * sizeof(double), ALIGN_SIZE);

	constantsAreInitialized = (bool *)_mm_malloc(totalThreads * sizeof(bool), ALIGN_SIZE);
	initialized = (bool *)_mm_malloc(totalThreads * sizeof(bool), ALIGN_SIZE);

	prev_hap_bases_length = (INT_TYPE *)_mm_malloc(SIMD_WIDTH_DOUBLE * totalThreads * sizeof(INT_TYPE), ALIGN_SIZE);

	if (matchMatrixVec == NULL || insertionMatrixVec == NULL || deletionMatrixVec == NULL || branchMatchMatrixVec == NULL || branchInsertionMatrixVec == NULL || branchDeletionMatrixVec == NULL || priorVec == NULL || transitionVec == NULL || constantsAreInitialized == NULL || initialized == NULL || prev_hap_bases_length == NULL)
	{
		_mm_free(matchMatrixVec);
		_mm_free(insertionMatrixVec);
		_mm_free(deletionMatrixVec);
		_mm_free(branchMatchMatrixVec);
		_mm_free(branchInsertionMatrixVec);
		_mm_free(branchDeletionMatrixVec);
		_mm_free(priorVec);
		_mm_free(transitionVec);
		_mm_free(constantsAreInitialized);
		_mm_free(initialized);
		_mm_free(prev_hap_bases_length);
		return PDHMM_MEMORY_ALLOCATION_FAILED;
	}

	for (int32_t i = 0; i < paddedMaxHaplotypeLength * SIMD_WIDTH_DOUBLE * totalThreads; i++)
	{
		matchMatrixVec[i] = 0;
		insertionMatrixVec[i] = 0;
		deletionMatrixVec[i] = 0;
		branchMatchMatrixVec[i] = 0;
		branchInsertionMatrixVec[i] = 0;
		branchDeletionMatrixVec[i] = 0;
	}

	for (int32_t i = 0; i < TRANS_PROB_ARRAY_LENGTH * paddedMaxReadLength * SIMD_WIDTH_DOUBLE * totalThreads; i++)
		transitionVec[i] = 0;

	for (int32_t i = 0; i < paddedMaxHaplotypeLength * paddedMaxReadLength * SIMD_WIDTH_DOUBLE * totalThreads; i++)
		priorVec[i] = 0;

	for (int32_t i = 0; i < totalThreads; i++)
	{
		constantsAreInitialized[i] = false;
		initialized[i] = true;
	}
	return PDHMM_SUCCESS;
}

inline void freeVec(double *matchMatrixVec, double *insertionMatrixVec, double *deletionMatrixVec, double *branchMatchMatrixVec, double *branchInsertionMatrixVec, double *branchDeletionMatrixVec, double *transitionVec, double *priorVec, bool *&constantsAreInitialized, bool *&initialized, INT_TYPE *&prev_hap_bases_length)
{
	_mm_free(matchMatrixVec);
	_mm_free(insertionMatrixVec);
	_mm_free(deletionMatrixVec);
	_mm_free(branchMatchMatrixVec);
	_mm_free(branchInsertionMatrixVec);
	_mm_free(branchDeletionMatrixVec);
	_mm_free(transitionVec);
	_mm_free(priorVec);
	_mm_free(constantsAreInitialized);
	_mm_free(initialized);
	_mm_free(prev_hap_bases_length);
}

inline void initializeVec(double *matchMatrixVec, double *insertionMatrixVec, double *deletionMatrixVec, double *branchMatchMatrixVec, double *branchInsertionMatrixVec, double *branchDeletionMatrixVec, double *transitionVec, double *priorVec, bool &constantsAreInitialized, bool &initialized, const int32_t maxReadLength, const int32_t maxHaplotypeLength)
{
	const int32_t paddedMaxReadLength = maxReadLength + 1;
	const int32_t paddedMaxHaplotypeLength = maxHaplotypeLength + 1;
	int32_t sizeOfTables = paddedMaxHaplotypeLength * SIMD_WIDTH_DOUBLE * sizeof(double);

	for (int32_t i = 0; i < paddedMaxHaplotypeLength * SIMD_WIDTH_DOUBLE; i++)
	{
		matchMatrixVec[i] = 0;
		insertionMatrixVec[i] = 0;
		deletionMatrixVec[i] = 0;
		branchMatchMatrixVec[i] = 0;
		branchInsertionMatrixVec[i] = 0;
		branchDeletionMatrixVec[i] = 0;
	}

	constantsAreInitialized = false;
	initialized = true;
}

inline VEC_DOUBLE_TYPE qualToErrorProb(VEC_INT_TYPE qual, const double *qualToErrorProbCache)
{
	VEC_INT_TYPE vindex = VEC_AND_INT(qual, VEC_SET1_INT(0xff));
	VEC_DOUBLE_TYPE result = VEC_GATHER_PD(vindex, qualToErrorProbCache, 8);
	return result;
}

inline VEC_DOUBLE_TYPE qualToProb(VEC_INT_TYPE qual, const double *qualToErrorProbCache)
{
	return VEC_SUB_PD(VEC_SET1_PD(1), qualToErrorProb(qual, qualToErrorProbCache));
}

inline VEC_DOUBLE_TYPE matchToMatchProbFun(VEC_INT_TYPE insQual, VEC_INT_TYPE delQual, const double *matchToMatchProb, int32_t &status)
{
	VEC_INT_TYPE minQual = VEC_MIN_INT(insQual, delQual);
	VEC_INT_TYPE maxQual = VEC_MAX_INT(insQual, delQual);

	VEC_MASK_TYPE ltzero = VEC_CMP_LT_INT(minQual, VEC_SET0_INT());
	if (VEC_CMP_NE_MASK(ltzero, 0))
	{
		status = PDHMM_INPUT_DATA_ERROR;
		return VEC_SET1_PD(-1.0);
	}

	VEC_MASK_TYPE ltMaxQual = VEC_CMP_LT_INT(VEC_SET1_INT(MAX_QUAL), maxQual);

	// True value
	VEC_DOUBLE_TYPE a = VEC_SET1_PD(-0.1);
	VEC_DOUBLE_TYPE trueVal = approximateLog10SumLog10(VEC_MUL_PD(VEC_CVT_INT_PD(minQual), a), VEC_MUL_PD(VEC_CVT_INT_PD(maxQual), a));
	trueVal = pow_pd(10.0, trueVal, SIMD_WIDTH_DOUBLE, status);
	if (status != PDHMM_SUCCESS)
		return VEC_SET1_PD(-1.0);
	a = VEC_SET1_PD(1.0);
	trueVal = VEC_SUB_PD(a, trueVal);

	// False value

	// index = [((maxQual * (maxQual + 1)) >> 1) + minQual
	VEC_INT_TYPE vindex = VEC_ADD_INT(maxQual, VEC_SET1_INT(1));
	vindex = VEC_SRLI_INT(VEC_MULLO_INT(maxQual, vindex), 1);
	vindex = VEC_ADD_INT(vindex, minQual);
	VEC_DOUBLE_TYPE falseVal = VEC_GATHER_PD(vindex, matchToMatchProb, 8);
	return VEC_BLEND_PD(falseVal, trueVal, ltMaxQual);
}

inline int32_t qualToTransProbs(double *dest, VEC_INT_TYPE insQual, VEC_INT_TYPE delQual, VEC_INT_TYPE gcp, const double *matchToMatchProb, const double *qualToErrorProbCache)
{
	VEC_INT_TYPE zeroVec = VEC_SET0_INT();

	VEC_MASK_TYPE mask1 = VEC_CMP_LT_INT(insQual, zeroVec);
	if (VEC_CMP_NE_MASK(mask1, 0))
	{
		return PDHMM_INPUT_DATA_ERROR;
	}
	mask1 = VEC_CMP_LT_INT(delQual, zeroVec);
	if (VEC_CMP_NE_MASK(mask1, 0))
	{
		return PDHMM_INPUT_DATA_ERROR;
	}
	mask1 = VEC_CMP_LT_INT(gcp, zeroVec);
	if (VEC_CMP_NE_MASK(mask1, 0))
	{
		return PDHMM_INPUT_DATA_ERROR;
	}
	int32_t status = PDHMM_SUCCESS;
	VEC_DOUBLE_TYPE result = matchToMatchProbFun(insQual, delQual, matchToMatchProb, status);
	if (status != PDHMM_SUCCESS)
	{
		return status;
	}
	VEC_STORE_PD(dest + matchToMatch * SIMD_WIDTH_DOUBLE, result);

	result = qualToErrorProb(insQual, qualToErrorProbCache);
	VEC_STORE_PD(dest + matchToInsertion * SIMD_WIDTH_DOUBLE, result);

	result = qualToErrorProb(delQual, qualToErrorProbCache);
	VEC_STORE_PD(dest + matchToDeletion * SIMD_WIDTH_DOUBLE, result);

	result = qualToProb(gcp, qualToErrorProbCache);
	VEC_STORE_PD(dest + indelToMatch * SIMD_WIDTH_DOUBLE, result);

	result = qualToErrorProb(gcp, qualToErrorProbCache);
	VEC_STORE_PD(dest + insertionToInsertion * SIMD_WIDTH_DOUBLE, result);
	VEC_STORE_PD(dest + deletionToDeletion * SIMD_WIDTH_DOUBLE, result);

	return PDHMM_SUCCESS;
}

inline int32_t qualToTransProbsVec(double *dest, const INT_TYPE *insQuals, const INT_TYPE *delQuals, const INT_TYPE *gcps, INT_TYPE max_read_bases_length, const double *matchToMatchProb, const double *qualToErrorProbCache)
{
	INT_TYPE index, offset;
	VEC_INT_TYPE insQualsVec, delQualsVec, gcpVec;
	int32_t status = PDHMM_SUCCESS;

	for (INT_TYPE i = 0; i < max_read_bases_length; i++)
	{
		index = (i + 1) * TRANS_PROB_ARRAY_LENGTH * SIMD_WIDTH_DOUBLE;
		offset = i * SIMD_WIDTH_DOUBLE;
		insQualsVec = VEC_LOAD_INT(insQuals + offset);
		delQualsVec = VEC_LOAD_INT(delQuals + offset);
		gcpVec = VEC_LOAD_INT(gcps + offset);
		int32_t currStatus = qualToTransProbs(dest + index, insQualsVec, delQualsVec, gcpVec, matchToMatchProb, qualToErrorProbCache);
		if (currStatus != PDHMM_SUCCESS)
		{
			return currStatus;
		}
	}
	return status;
}

inline int32_t initializeProbabilitiesVec(double *dest, const INT_TYPE *insertionGOP, const INT_TYPE *deletionGOP, const INT_TYPE *overallGCP, INT_TYPE read_bases_length, const double *matchToMatchProb, const double *qualToErrorProbCache)
{
	return qualToTransProbsVec(dest, insertionGOP, deletionGOP, overallGCP, read_bases_length, matchToMatchProb, qualToErrorProbCache);
}

inline VEC_MASK_TYPE isBasePDMatching(VEC_INT_TYPE x, VEC_INT_TYPE hapPDBases)
{
	VEC_INT_TYPE SNPvec = VEC_SET1_INT(PartiallyDeterminedHaplotype::SNP);
	SNPvec = VEC_AND_INT(hapPDBases, SNPvec);
	VEC_INT_TYPE zeroVec = VEC_SET0_INT();
	VEC_MASK_TYPE mask1 = VEC_CMP_NE_INT(SNPvec, zeroVec);

	VEC_MASK_TYPE mask2 = VEC_CMP_EQ_INT(x, VEC_SET1_INT('A'));
	mask2 = VEC_OR_MASK(mask2, VEC_CMP_EQ_INT(x, VEC_SET1_INT('a')));
	SNPvec = VEC_SET1_INT(PartiallyDeterminedHaplotype::A);
	SNPvec = VEC_AND_INT(hapPDBases, SNPvec);
	VEC_MASK_TYPE ansA = VEC_CMP_NE_INT(SNPvec, zeroVec);
	ansA = VEC_AND_MASK(ansA, mask2);

	mask2 = VEC_CMP_EQ_INT(x, VEC_SET1_INT('C'));
	mask2 = VEC_OR_MASK(mask2, VEC_CMP_EQ_INT(x, VEC_SET1_INT('c')));
	SNPvec = VEC_SET1_INT(PartiallyDeterminedHaplotype::C);
	SNPvec = VEC_AND_INT(hapPDBases, SNPvec);
	VEC_MASK_TYPE ansC = VEC_CMP_NE_INT(SNPvec, zeroVec);
	ansC = VEC_AND_MASK(ansC, mask2);

	mask2 = VEC_CMP_EQ_INT(x, VEC_SET1_INT('T'));
	mask2 = VEC_OR_MASK(mask2, VEC_CMP_EQ_INT(x, VEC_SET1_INT('t')));
	SNPvec = VEC_SET1_INT(PartiallyDeterminedHaplotype::T);
	SNPvec = VEC_AND_INT(hapPDBases, SNPvec);
	VEC_MASK_TYPE ansT = VEC_CMP_NE_INT(SNPvec, zeroVec);
	ansT = VEC_AND_MASK(ansT, mask2);

	mask2 = VEC_CMP_EQ_INT(x, VEC_SET1_INT('G'));
	mask2 = VEC_OR_MASK(mask2, VEC_CMP_EQ_INT(x, VEC_SET1_INT('g')));
	SNPvec = VEC_SET1_INT(PartiallyDeterminedHaplotype::G);
	SNPvec = VEC_AND_INT(hapPDBases, SNPvec);
	VEC_MASK_TYPE ansG = VEC_CMP_NE_INT(SNPvec, zeroVec);
	ansG = VEC_AND_MASK(ansG, mask2);

	VEC_MASK_TYPE res = VEC_OR_MASK(ansA, ansC);
	res = VEC_OR_MASK(res, ansT);
	res = VEC_OR_MASK(res, ansG);
	res = VEC_AND_MASK(res, mask1);
	return res;
}

inline VEC_MASK_TYPE isBasePDMatchingPrime(VEC_INT_TYPE xPrime, VEC_INT_TYPE hapPDBases)
{
	VEC_INT_TYPE temp = VEC_AND_INT(hapPDBases, SNPVec);
	VEC_INT_TYPE zeroVec = VEC_SET0_INT();
	VEC_MASK_TYPE mask1 = VEC_CMP_NE_INT(temp, zeroVec);

	temp = VEC_AND_INT(hapPDBases, xPrime);
	VEC_MASK_TYPE mask2 = VEC_CMP_NE_INT(temp, zeroVec);
	mask2 = VEC_AND_MASK(mask1, mask2);
	return mask2;
}

inline VEC_INT_TYPE toUpperCase(VEC_INT_TYPE x)
{
	VEC_MASK_TYPE isLower = VEC_CMP_LE_INT(VEC_SET1_INT('a'), x);
	VEC_INT_TYPE xPrime = VEC_SUB_INT(x, VEC_SET1_INT(32));
	xPrime = VEC_BLEND_INT(x, xPrime, isLower);
	return xPrime;
}

inline VEC_INT_TYPE toPrime(VEC_INT_TYPE x)
{
	VEC_INT_TYPE result = AVec;
	VEC_MASK_TYPE mask = VEC_CMP_EQ_INT(x, VEC_SET1_INT('C'));
	result = VEC_BLEND_INT(result, CVec, mask);
	mask = VEC_CMP_EQ_INT(x, VEC_SET1_INT('G'));
	result = VEC_BLEND_INT(result, GVec, mask);
	mask = VEC_CMP_EQ_INT(x, VEC_SET1_INT('T'));
	result = VEC_BLEND_INT(result, TVec, mask);
	return result;
}

inline void initializePriorsVec(const INT_TYPE *haplotypeBases, const INT_TYPE *haplotypePDBases, const INT_TYPE *readBases, INT_TYPE *readQuals, INT_TYPE startIndex, INT_TYPE max_hap_bases_length, INT_TYPE max_read_bases_length, double *priorVec, const double *qualToErrorProbCache, int32_t maxHaplotypeLength)
{
	// initialize the prior matrix for all combinations of read x haplotype bases
	INT_TYPE paddedMaxHaplotypeLength = maxHaplotypeLength + 1;
	VEC_INT_TYPE nVec = VEC_SET1_INT((int8_t)'N');
	VEC_INT_TYPE aVec = VEC_SET1_INT('a');
	VEC_INT_TYPE caseDiff = VEC_SET1_INT(32);

	// manual unroll rows
	INT_TYPE n = (max_read_bases_length / 4) * 4;
	for (int32_t i = 0; i < n; i += 4)
	{
		INT_TYPE index = i * SIMD_WIDTH_DOUBLE;
		VEC_INT_TYPE x1 = VEC_LOAD_INT(readBases + index);
		VEC_INT_TYPE qual = VEC_LOAD_INT(readQuals + index);
		VEC_DOUBLE_TYPE trueVal1 = qualToProb(qual, qualToErrorProbCache);
		VEC_DOUBLE_TYPE falseVal1 = VEC_DIV_PD(qualToErrorProb(qual, qualToErrorProbCache), VEC_SET1_PD(3.0));
		VEC_MASK_TYPE maskx1 = VEC_CMP_EQ_INT(x1, nVec);
		VEC_MASK_TYPE isLower = VEC_CMP_LE_INT(aVec, x1);
		VEC_INT_TYPE x1Prime = VEC_SUB_INT(x1, caseDiff);
		x1Prime = VEC_BLEND_INT(x1, x1Prime, isLower);
		x1Prime = toPrime(x1Prime);

		index = (i + 1) * SIMD_WIDTH_DOUBLE;
		VEC_INT_TYPE x2 = VEC_LOAD_INT(readBases + index);
		qual = VEC_LOAD_INT(readQuals + index);
		VEC_DOUBLE_TYPE trueVal2 = qualToProb(qual, qualToErrorProbCache);
		VEC_DOUBLE_TYPE falseVal2 = VEC_DIV_PD(qualToErrorProb(qual, qualToErrorProbCache), VEC_SET1_PD(3.0));
		VEC_MASK_TYPE maskx2 = VEC_CMP_EQ_INT(x2, nVec);
		isLower = VEC_CMP_LE_INT(aVec, x2);
		VEC_INT_TYPE x2Prime = VEC_SUB_INT(x2, caseDiff);
		x2Prime = VEC_BLEND_INT(x2, x2Prime, isLower);
		x2Prime = toPrime(x2Prime);

		index = (i + 2) * SIMD_WIDTH_DOUBLE;
		VEC_INT_TYPE x3 = VEC_LOAD_INT(readBases + index);
		qual = VEC_LOAD_INT(readQuals + index);
		VEC_DOUBLE_TYPE trueVal3 = qualToProb(qual, qualToErrorProbCache);
		VEC_DOUBLE_TYPE falseVal3 = VEC_DIV_PD(qualToErrorProb(qual, qualToErrorProbCache), VEC_SET1_PD(3.0));
		VEC_MASK_TYPE maskx3 = VEC_CMP_EQ_INT(x3, nVec);
		isLower = VEC_CMP_LE_INT(aVec, x3);
		VEC_INT_TYPE x3Prime = VEC_SUB_INT(x3, caseDiff);
		x3Prime = VEC_BLEND_INT(x3, x3Prime, isLower);
		x3Prime = toPrime(x3Prime);

		index = (i + 3) * SIMD_WIDTH_DOUBLE;
		VEC_INT_TYPE x4 = VEC_LOAD_INT(readBases + index);
		qual = VEC_LOAD_INT(readQuals + index);
		VEC_DOUBLE_TYPE trueVal4 = qualToProb(qual, qualToErrorProbCache);
		VEC_DOUBLE_TYPE falseVal4 = VEC_DIV_PD(qualToErrorProb(qual, qualToErrorProbCache), VEC_SET1_PD(3.0));
		VEC_MASK_TYPE maskx4 = VEC_CMP_EQ_INT(x4, nVec);
		isLower = VEC_CMP_LE_INT(aVec, x4);
		VEC_INT_TYPE x4Prime = VEC_SUB_INT(x4, caseDiff);
		x4Prime = VEC_BLEND_INT(x4, x4Prime, isLower);
		x4Prime = toPrime(x4Prime);

		for (INT_TYPE j = startIndex; j < max_hap_bases_length; j++)
		{
			index = j * SIMD_WIDTH_DOUBLE;
			VEC_INT_TYPE y = VEC_LOAD_INT(haplotypeBases + index);
			VEC_INT_TYPE hapPDBase = VEC_LOAD_INT(haplotypePDBases + index);
			VEC_MASK_TYPE maskY = VEC_CMP_EQ_INT(y, nVec);

			VEC_MASK_TYPE mask1 = VEC_CMP_EQ_INT(x1, y);
			mask1 = VEC_OR_MASK(mask1, maskx1);
			mask1 = VEC_OR_MASK(mask1, maskY);
			mask1 = VEC_OR_MASK(mask1, isBasePDMatchingPrime(x1Prime, hapPDBase));
			VEC_DOUBLE_TYPE priorValue = VEC_BLEND_PD(falseVal1, trueVal1, mask1);
			index = (i + 1) * paddedMaxHaplotypeLength * SIMD_WIDTH_DOUBLE + (j + 1) * SIMD_WIDTH_DOUBLE;
			VEC_STORE_PD(priorVec + index, priorValue);

			mask1 = VEC_CMP_EQ_INT(x2, y);
			mask1 = VEC_OR_MASK(mask1, maskx2);
			mask1 = VEC_OR_MASK(mask1, maskY);
			mask1 = VEC_OR_MASK(mask1, isBasePDMatchingPrime(x2Prime, hapPDBase));
			priorValue = VEC_BLEND_PD(falseVal2, trueVal2, mask1);
			index = (i + 2) * paddedMaxHaplotypeLength * SIMD_WIDTH_DOUBLE + (j + 1) * SIMD_WIDTH_DOUBLE;
			VEC_STORE_PD(priorVec + index, priorValue);

			mask1 = VEC_CMP_EQ_INT(x3, y);
			mask1 = VEC_OR_MASK(mask1, maskx3);
			mask1 = VEC_OR_MASK(mask1, maskY);
			mask1 = VEC_OR_MASK(mask1, isBasePDMatchingPrime(x3Prime, hapPDBase));
			priorValue = VEC_BLEND_PD(falseVal3, trueVal3, mask1);
			index = (i + 3) * paddedMaxHaplotypeLength * SIMD_WIDTH_DOUBLE + (j + 1) * SIMD_WIDTH_DOUBLE;
			VEC_STORE_PD(priorVec + index, priorValue);

			mask1 = VEC_CMP_EQ_INT(x4, y);
			mask1 = VEC_OR_MASK(mask1, maskx4);
			mask1 = VEC_OR_MASK(mask1, maskY);
			mask1 = VEC_OR_MASK(mask1, isBasePDMatchingPrime(x4Prime, hapPDBase));
			priorValue = VEC_BLEND_PD(falseVal4, trueVal4, mask1);
			index = (i + 4) * paddedMaxHaplotypeLength * SIMD_WIDTH_DOUBLE + (j + 1) * SIMD_WIDTH_DOUBLE;
			VEC_STORE_PD(priorVec + index, priorValue);
		}
	}

	for (INT_TYPE i = n; i < max_read_bases_length; i++)
	{
		INT_TYPE index = i * SIMD_WIDTH_DOUBLE;
		VEC_INT_TYPE x1 = VEC_LOAD_INT(readBases + index);
		VEC_INT_TYPE qual = VEC_LOAD_INT(readQuals + index);
		VEC_DOUBLE_TYPE trueVal1 = qualToProb(qual, qualToErrorProbCache);
		VEC_DOUBLE_TYPE falseVal1 = VEC_DIV_PD(qualToErrorProb(qual, qualToErrorProbCache), VEC_SET1_PD(3.0));
		VEC_MASK_TYPE maskx1 = VEC_CMP_EQ_INT(x1, nVec);
		VEC_MASK_TYPE isLower = VEC_CMP_LE_INT(aVec, x1);
		VEC_INT_TYPE x1Prime = VEC_SUB_INT(x1, caseDiff);
		x1Prime = VEC_BLEND_INT(x1, x1Prime, isLower);
		x1Prime = toPrime(x1Prime);
		for (INT_TYPE j = startIndex; j < max_hap_bases_length; j++)
		{
			index = j * SIMD_WIDTH_DOUBLE;
			VEC_INT_TYPE y = VEC_LOAD_INT(haplotypeBases + index);
			VEC_INT_TYPE hapPDBase = VEC_LOAD_INT(haplotypePDBases + index);

			VEC_MASK_TYPE mask1 = VEC_CMP_EQ_INT(x1, y);
			mask1 = VEC_OR_MASK(mask1, maskx1);
			mask1 = VEC_OR_MASK(mask1, VEC_CMP_EQ_INT(y, nVec));
			mask1 = VEC_OR_MASK(mask1, isBasePDMatchingPrime(x1Prime, hapPDBase));
			VEC_DOUBLE_TYPE priorValue = VEC_BLEND_PD(falseVal1, trueVal1, mask1);
			index = (i + 1) * paddedMaxHaplotypeLength * SIMD_WIDTH_DOUBLE + (j + 1) * SIMD_WIDTH_DOUBLE;
			VEC_STORE_PD(priorVec + index, priorValue);
		}
	}
}

inline void recursionFunction(VEC_DOUBLE_TYPE mm_diag, VEC_DOUBLE_TYPE mm_top, VEC_DOUBLE_TYPE mm_left, VEC_DOUBLE_TYPE &mm_curr,
							  VEC_DOUBLE_TYPE im_diag, VEC_DOUBLE_TYPE im_top, VEC_DOUBLE_TYPE im_left, VEC_DOUBLE_TYPE &im_curr,
							  VEC_DOUBLE_TYPE dm_diag, VEC_DOUBLE_TYPE dm_top, VEC_DOUBLE_TYPE dm_left, VEC_DOUBLE_TYPE &dm_curr,
							  VEC_DOUBLE_TYPE bmm_diag, VEC_DOUBLE_TYPE bmm_top, VEC_DOUBLE_TYPE bmm_left, VEC_DOUBLE_TYPE &bmm_curr,
							  VEC_DOUBLE_TYPE bim_diag, VEC_DOUBLE_TYPE bim_top, VEC_DOUBLE_TYPE bim_left, VEC_DOUBLE_TYPE &bim_curr,
							  VEC_DOUBLE_TYPE bdm_diag, VEC_DOUBLE_TYPE bdm_top, VEC_DOUBLE_TYPE bdm_left, VEC_DOUBLE_TYPE &bdm_curr,
							  VEC_DOUBLE_TYPE tmm, VEC_DOUBLE_TYPE tim, VEC_DOUBLE_TYPE tmi, VEC_DOUBLE_TYPE tii, VEC_DOUBLE_TYPE tmd, VEC_DOUBLE_TYPE tdd,
							  VEC_INT_TYPE del_start, VEC_INT_TYPE del_end, VEC_INT_TYPE normalState, VEC_INT_TYPE insideState,
							  VEC_INT_TYPE afterState, VEC_INT_TYPE &currentState, VEC_MASK_TYPE rowWrite, const INT_TYPE *hap_pdbases_vec,
							  INT_TYPE i, INT_TYPE j, INT_TYPE paddedMaxHaplotypeLength, const double *priorVec)
{
	VEC_MASK_TYPE maskInside = VEC_CMP_EQ_INT(currentState, insideState);
	VEC_MASK_TYPE maskAfter = VEC_CMP_EQ_INT(currentState, afterState);
	VEC_DOUBLE_TYPE max_mm_left = VEC_MAX_PD(mm_left, bmm_left);
	VEC_DOUBLE_TYPE max_im_left = VEC_MAX_PD(im_left, bim_left);
	VEC_DOUBLE_TYPE max_dm_left = VEC_MAX_PD(dm_left, bdm_left);

	VEC_DOUBLE_TYPE max_mm_diag = VEC_MAX_PD(mm_diag, bmm_diag);
	VEC_DOUBLE_TYPE max_im_diag = VEC_MAX_PD(im_diag, bim_diag);
	VEC_DOUBLE_TYPE max_dm_diag = VEC_MAX_PD(dm_diag, bdm_diag);

	// Load the current values

	bmm_curr = VEC_BLEND_PD(mm_left, bmm_left, maskInside);
	bmm_curr = VEC_BLEND_PD(bmm_curr, max_mm_left, maskAfter);

	bim_curr = VEC_BLEND_PD(im_left, bim_left, maskInside);
	bim_curr = VEC_BLEND_PD(bim_curr, max_im_left, maskAfter);

	bdm_curr = VEC_BLEND_PD(dm_left, bdm_left, maskInside);
	bdm_curr = VEC_BLEND_PD(bdm_curr, max_dm_left, maskAfter);

	// Set values for After Del portion

	mm_diag = VEC_BLEND_PD(mm_diag, max_mm_diag, maskAfter);
	im_diag = VEC_BLEND_PD(im_diag, max_im_diag, maskAfter);
	dm_diag = VEC_BLEND_PD(dm_diag, max_dm_diag, maskAfter);
	mm_left = VEC_BLEND_PD(mm_left, max_mm_left, maskAfter);
	dm_left = VEC_BLEND_PD(dm_left, max_dm_left, maskAfter);

	INT_TYPE currIndex = i * paddedMaxHaplotypeLength * SIMD_WIDTH_DOUBLE + (j)*SIMD_WIDTH_DOUBLE;
	VEC_DOUBLE_TYPE pr = VEC_LOAD_PD(priorVec + currIndex);

	mm_curr = VEC_MUL_PD(pr,
						 VEC_ADD_PD(VEC_MUL_PD(mm_diag, tmm),
									VEC_ADD_PD(VEC_MUL_PD(im_diag, tim), VEC_MUL_PD(dm_diag, tim))));

	dm_curr = VEC_ADD_PD(VEC_MUL_PD(mm_left, tmd), VEC_MUL_PD(dm_left, tdd));

	// Special case since we MIGHT be at the deletion end and have to be handling jump states from above
	im_curr = VEC_ADD_PD(VEC_MUL_PD(mm_top, tmi), VEC_MUL_PD(im_top, tii));

	VEC_INT_TYPE hap_pb_j_off = VEC_LOAD_INT(hap_pdbases_vec + (j - OFF) * SIMD_WIDTH_DOUBLE);
	VEC_MASK_TYPE mask_hap_pb = VEC_CMP_EQ_INT(VEC_AND_INT(hap_pb_j_off, del_end), del_end);

	VEC_DOUBLE_TYPE im_curr_alt = VEC_ADD_PD(
		VEC_MUL_PD(VEC_MAX_PD(bmm_top, mm_top), tmi),
		VEC_MUL_PD(VEC_MAX_PD(bim_top, im_top), tii));

	im_curr = VEC_BLEND_PD(im_curr, im_curr_alt, mask_hap_pb);

	// State Change
	currentState = VEC_BLEND_INT(currentState, normalState, maskAfter);

	VEC_INT_TYPE hap_pb_j_minus_one = VEC_LOAD_INT(hap_pdbases_vec + (j - 1) * SIMD_WIDTH_DOUBLE);

	mask_hap_pb = VEC_CMP_EQ_INT(VEC_AND_INT(hap_pb_j_minus_one, del_start), del_start);

	currentState = VEC_BLEND_INT(currentState, insideState, mask_hap_pb);

	mask_hap_pb = VEC_CMP_EQ_INT(VEC_AND_INT(hap_pb_j_minus_one, del_end), del_end);

	currentState = VEC_BLEND_INT(currentState, afterState, mask_hap_pb);

	// check to write as per row number
	bmm_curr = VEC_BLEND_PD(bmm_top, bmm_curr, rowWrite);
	bim_curr = VEC_BLEND_PD(bim_top, bim_curr, rowWrite);
	bdm_curr = VEC_BLEND_PD(bdm_top, bdm_curr, rowWrite);

	mm_curr = VEC_BLEND_PD(mm_top, mm_curr, rowWrite);
	im_curr = VEC_BLEND_PD(im_top, im_curr, rowWrite);
	dm_curr = VEC_BLEND_PD(dm_top, dm_curr, rowWrite);
}

void computationStep(const INT_TYPE *paddedReadLength, const INT_TYPE *hap_pdbases_vec, INT_TYPE currMaxHaplotypeLength, INT_TYPE maxReadLength, const INT_TYPE *paddedHaplotypeLength, double *matchMatrixVec, double *insertionMatrixVec, double *deletionMatrixVec, double *branchMatchMatrixVec, double *branchInsertionMatrixVec, double *branchDeletionMatrixVec, const double *transitionVec, const double *priorVec, double *result, int32_t maxHaplotypeLength)
{
	INT_TYPE paddedMaxHaplotypeLength = maxHaplotypeLength + 1;
	VEC_INT_TYPE currentState1 = VEC_SET1_INT(NORMAL);
	VEC_INT_TYPE currentState2 = VEC_SET1_INT(NORMAL);
	VEC_INT_TYPE currentState3 = VEC_SET1_INT(NORMAL);
	VEC_INT_TYPE currentState4 = VEC_SET1_INT(NORMAL);

	VEC_INT_TYPE paddedReadLenVec = VEC_LOAD_INT(paddedReadLength);
	VEC_INT_TYPE del_start = VEC_SET1_INT(PartiallyDeterminedHaplotype::DEL_START);
	VEC_INT_TYPE del_end = VEC_SET1_INT(PartiallyDeterminedHaplotype::DEL_END);
	VEC_INT_TYPE normalState = VEC_SET1_INT(NORMAL);
	VEC_INT_TYPE insideState = VEC_SET1_INT(INSIDE_DEL);
	VEC_INT_TYPE afterState = VEC_SET1_INT(AFTER_DEL);
	for (int32_t i = 1; i < maxReadLength; i += ROW_UNROLL)
	{
		// assumption made: current state can be set to normal at start of each row.
		currentState1 = VEC_SET1_INT(NORMAL);
		currentState2 = VEC_SET1_INT(NORMAL);
		currentState3 = VEC_SET1_INT(NORMAL);
		currentState4 = VEC_SET1_INT(NORMAL);

		VEC_INT_TYPE iVec1 = VEC_SET1_INT(i);
		VEC_MASK_TYPE rowWrite1 = VEC_CMP_LT_INT(iVec1, paddedReadLenVec);
		VEC_INT_TYPE iVec2 = VEC_SET1_INT(i + 1);
		VEC_MASK_TYPE rowWrite2 = VEC_CMP_LT_INT(iVec2, paddedReadLenVec);
		VEC_INT_TYPE iVec3 = VEC_SET1_INT(i + 2);
		VEC_MASK_TYPE rowWrite3 = VEC_CMP_LT_INT(iVec3, paddedReadLenVec);
		VEC_INT_TYPE iVec4 = VEC_SET1_INT(i + 3);
		VEC_MASK_TYPE rowWrite4 = VEC_CMP_LT_INT(iVec4, paddedReadLenVec);

		INT_TYPE index = i * TRANS_PROB_ARRAY_LENGTH * SIMD_WIDTH_DOUBLE;
		VEC_DOUBLE_TYPE tmm1 = VEC_LOAD_PD(transitionVec + index);
		VEC_DOUBLE_TYPE tim1 = VEC_LOAD_PD(transitionVec + index + SIMD_WIDTH_DOUBLE);
		VEC_DOUBLE_TYPE tmi1 = VEC_LOAD_PD(transitionVec + index + SIMD_WIDTH_DOUBLE * 2);
		VEC_DOUBLE_TYPE tii1 = VEC_LOAD_PD(transitionVec + index + SIMD_WIDTH_DOUBLE * 3);
		VEC_DOUBLE_TYPE tmd1 = VEC_LOAD_PD(transitionVec + index + SIMD_WIDTH_DOUBLE * 4);
		VEC_DOUBLE_TYPE tdd1 = VEC_LOAD_PD(transitionVec + index + SIMD_WIDTH_DOUBLE * 5);

		INT_TYPE index2 = (i + 1) * TRANS_PROB_ARRAY_LENGTH * SIMD_WIDTH_DOUBLE;
		VEC_DOUBLE_TYPE tmm2 = VEC_LOAD_PD(transitionVec + index2);
		VEC_DOUBLE_TYPE tim2 = VEC_LOAD_PD(transitionVec + index2 + SIMD_WIDTH_DOUBLE);
		VEC_DOUBLE_TYPE tmi2 = VEC_LOAD_PD(transitionVec + index2 + SIMD_WIDTH_DOUBLE * 2);
		VEC_DOUBLE_TYPE tii2 = VEC_LOAD_PD(transitionVec + index2 + SIMD_WIDTH_DOUBLE * 3);
		VEC_DOUBLE_TYPE tmd2 = VEC_LOAD_PD(transitionVec + index2 + SIMD_WIDTH_DOUBLE * 4);
		VEC_DOUBLE_TYPE tdd2 = VEC_LOAD_PD(transitionVec + index2 + SIMD_WIDTH_DOUBLE * 5);

		INT_TYPE index3 = (i + 2) * TRANS_PROB_ARRAY_LENGTH * SIMD_WIDTH_DOUBLE;
		VEC_DOUBLE_TYPE tmm3 = VEC_LOAD_PD(transitionVec + index3);
		VEC_DOUBLE_TYPE tim3 = VEC_LOAD_PD(transitionVec + index3 + SIMD_WIDTH_DOUBLE);
		VEC_DOUBLE_TYPE tmi3 = VEC_LOAD_PD(transitionVec + index3 + SIMD_WIDTH_DOUBLE * 2);
		VEC_DOUBLE_TYPE tii3 = VEC_LOAD_PD(transitionVec + index3 + SIMD_WIDTH_DOUBLE * 3);
		VEC_DOUBLE_TYPE tmd3 = VEC_LOAD_PD(transitionVec + index3 + SIMD_WIDTH_DOUBLE * 4);
		VEC_DOUBLE_TYPE tdd3 = VEC_LOAD_PD(transitionVec + index3 + SIMD_WIDTH_DOUBLE * 5);

		INT_TYPE index4 = (i + 3) * TRANS_PROB_ARRAY_LENGTH * SIMD_WIDTH_DOUBLE;
		VEC_DOUBLE_TYPE tmm4 = VEC_LOAD_PD(transitionVec + index4);
		VEC_DOUBLE_TYPE tim4 = VEC_LOAD_PD(transitionVec + index4 + SIMD_WIDTH_DOUBLE);
		VEC_DOUBLE_TYPE tmi4 = VEC_LOAD_PD(transitionVec + index4 + SIMD_WIDTH_DOUBLE * 2);
		VEC_DOUBLE_TYPE tii4 = VEC_LOAD_PD(transitionVec + index4 + SIMD_WIDTH_DOUBLE * 3);
		VEC_DOUBLE_TYPE tmd4 = VEC_LOAD_PD(transitionVec + index4 + SIMD_WIDTH_DOUBLE * 4);
		VEC_DOUBLE_TYPE tdd4 = VEC_LOAD_PD(transitionVec + index4 + SIMD_WIDTH_DOUBLE * 5);

		VEC_DOUBLE_TYPE mm_0 = VEC_SET1_PD(0); // mm_diag
		VEC_DOUBLE_TYPE mm_1;				   // mm_top
		VEC_DOUBLE_TYPE mm_2 = VEC_SET1_PD(0); // mm_left
		VEC_DOUBLE_TYPE mm_3;
		VEC_DOUBLE_TYPE mm_4 = VEC_SET1_PD(0); // mm_left
		VEC_DOUBLE_TYPE mm_5;
		VEC_DOUBLE_TYPE mm_6 = VEC_SET1_PD(0); // mm_left
		VEC_DOUBLE_TYPE mm_7;
		VEC_DOUBLE_TYPE mm_8 = VEC_SET1_PD(0); // mm_left
		VEC_DOUBLE_TYPE mm_9;

		VEC_DOUBLE_TYPE im_0 = VEC_SET1_PD(0);
		VEC_DOUBLE_TYPE im_1;
		VEC_DOUBLE_TYPE im_2 = VEC_SET1_PD(0);
		VEC_DOUBLE_TYPE im_3;
		VEC_DOUBLE_TYPE im_4 = VEC_SET1_PD(0);
		VEC_DOUBLE_TYPE im_5;
		VEC_DOUBLE_TYPE im_6 = VEC_SET1_PD(0);
		VEC_DOUBLE_TYPE im_7;
		VEC_DOUBLE_TYPE im_8 = VEC_SET1_PD(0);
		VEC_DOUBLE_TYPE im_9;

		VEC_DOUBLE_TYPE dm_0 = VEC_SET1_PD(0);
		if (i == 1)
			dm_0 = VEC_LOAD_PD(deletionMatrixVec);
		VEC_DOUBLE_TYPE dm_1;
		VEC_DOUBLE_TYPE dm_2 = VEC_SET1_PD(0);
		VEC_DOUBLE_TYPE dm_3;
		VEC_DOUBLE_TYPE dm_4 = VEC_SET1_PD(0);
		VEC_DOUBLE_TYPE dm_5;
		VEC_DOUBLE_TYPE dm_6 = VEC_SET1_PD(0);
		VEC_DOUBLE_TYPE dm_7;
		VEC_DOUBLE_TYPE dm_8 = VEC_SET1_PD(0);
		VEC_DOUBLE_TYPE dm_9;

		VEC_DOUBLE_TYPE bmm_0 = VEC_SET1_PD(0);
		VEC_DOUBLE_TYPE bmm_1;
		VEC_DOUBLE_TYPE bmm_2 = VEC_SET1_PD(0);
		VEC_DOUBLE_TYPE bmm_3;
		VEC_DOUBLE_TYPE bmm_4 = VEC_SET1_PD(0);
		VEC_DOUBLE_TYPE bmm_5;
		VEC_DOUBLE_TYPE bmm_6 = VEC_SET1_PD(0);
		VEC_DOUBLE_TYPE bmm_7;
		VEC_DOUBLE_TYPE bmm_8 = VEC_SET1_PD(0);
		VEC_DOUBLE_TYPE bmm_9;

		VEC_DOUBLE_TYPE bim_0 = VEC_SET1_PD(0);
		VEC_DOUBLE_TYPE bim_1;
		VEC_DOUBLE_TYPE bim_2 = VEC_SET1_PD(0);
		VEC_DOUBLE_TYPE bim_3;
		VEC_DOUBLE_TYPE bim_4 = VEC_SET1_PD(0);
		VEC_DOUBLE_TYPE bim_5;
		VEC_DOUBLE_TYPE bim_6 = VEC_SET1_PD(0);
		VEC_DOUBLE_TYPE bim_7;
		VEC_DOUBLE_TYPE bim_8 = VEC_SET1_PD(0);
		VEC_DOUBLE_TYPE bim_9;

		VEC_DOUBLE_TYPE bdm_0 = VEC_SET1_PD(0);
		VEC_DOUBLE_TYPE bdm_1;
		VEC_DOUBLE_TYPE bdm_2 = VEC_SET1_PD(0);
		VEC_DOUBLE_TYPE bdm_3;
		VEC_DOUBLE_TYPE bdm_4 = VEC_SET1_PD(0);
		VEC_DOUBLE_TYPE bdm_5;
		VEC_DOUBLE_TYPE bdm_6 = VEC_SET1_PD(0);
		VEC_DOUBLE_TYPE bdm_7;
		VEC_DOUBLE_TYPE bdm_8 = VEC_SET1_PD(0);
		VEC_DOUBLE_TYPE bdm_9;

		for (int32_t j = 1; j < currMaxHaplotypeLength; j++) // START INDEX SET TO 1
		{
			int32_t topIndex = (j)*SIMD_WIDTH_DOUBLE;

			// Load all data
			mm_1 = VEC_LOAD_PD(matchMatrixVec + topIndex);

			im_1 = VEC_LOAD_PD(insertionMatrixVec + topIndex);

			dm_1 = VEC_LOAD_PD(deletionMatrixVec + topIndex);

			bmm_1 = VEC_LOAD_PD(branchMatchMatrixVec + topIndex);

			bim_1 = VEC_LOAD_PD(branchInsertionMatrixVec + topIndex);

			bdm_1 = VEC_LOAD_PD(branchDeletionMatrixVec + topIndex);

			recursionFunction(mm_0, mm_1, mm_2, mm_3,
							  im_0, im_1, im_2, im_3,
							  dm_0, dm_1, dm_2, dm_3,
							  bmm_0, bmm_1, bmm_2, bmm_3,
							  bim_0, bim_1, bim_2, bim_3,
							  bdm_0, bdm_1, bdm_2, bdm_3,
							  tmm1, tim1, tmi1, tii1, tmd1, tdd1,
							  del_start, del_end, normalState, insideState,
							  afterState, currentState1, rowWrite1, hap_pdbases_vec,
							  i, j, paddedMaxHaplotypeLength, priorVec);

			recursionFunction(mm_2, mm_3, mm_4, mm_5,
							  im_2, im_3, im_4, im_5,
							  dm_2, dm_3, dm_4, dm_5,
							  bmm_2, bmm_3, bmm_4, bmm_5,
							  bim_2, bim_3, bim_4, bim_5,
							  bdm_2, bdm_3, bdm_4, bdm_5,
							  tmm2, tim2, tmi2, tii2, tmd2, tdd2,
							  del_start, del_end, normalState, insideState,
							  afterState, currentState2, rowWrite2, hap_pdbases_vec,
							  i + 1, j, paddedMaxHaplotypeLength, priorVec);

			recursionFunction(mm_4, mm_5, mm_6, mm_7,
							  im_4, im_5, im_6, im_7,
							  dm_4, dm_5, dm_6, dm_7,
							  bmm_4, bmm_5, bmm_6, bmm_7,
							  bim_4, bim_5, bim_6, bim_7,
							  bdm_4, bdm_5, bdm_6, bdm_7,
							  tmm3, tim3, tmi3, tii3, tmd3, tdd3,
							  del_start, del_end, normalState, insideState,
							  afterState, currentState3, rowWrite3, hap_pdbases_vec,
							  i + 2, j, paddedMaxHaplotypeLength, priorVec);

			recursionFunction(mm_6, mm_7, mm_8, mm_9,
							  im_6, im_7, im_8, im_9,
							  dm_6, dm_7, dm_8, dm_9,
							  bmm_6, bmm_7, bmm_8, bmm_9,
							  bim_6, bim_7, bim_8, bim_9,
							  bdm_6, bdm_7, bdm_8, bdm_9,
							  tmm4, tim4, tmi4, tii4, tmd4, tdd4,
							  del_start, del_end, normalState, insideState,
							  afterState, currentState4, rowWrite4, hap_pdbases_vec,
							  i + 3, j, paddedMaxHaplotypeLength, priorVec);

			INT_TYPE currIndex = (j)*SIMD_WIDTH_DOUBLE;
			VEC_STORE_PD(branchMatchMatrixVec + currIndex, bmm_9);
			VEC_STORE_PD(branchInsertionMatrixVec + currIndex, bim_9);
			VEC_STORE_PD(branchDeletionMatrixVec + currIndex, bdm_9);

			VEC_STORE_PD(matchMatrixVec + currIndex, mm_9);
			VEC_STORE_PD(insertionMatrixVec + currIndex, im_9);
			VEC_STORE_PD(deletionMatrixVec + currIndex, dm_9);

			mm_0 = mm_1;
			mm_2 = mm_3;
			mm_4 = mm_5;
			mm_6 = mm_7;
			mm_8 = mm_9;

			im_0 = im_1;
			im_2 = im_3;
			im_4 = im_5;
			im_6 = im_7;
			im_8 = im_9;

			dm_0 = dm_1;
			dm_2 = dm_3;
			dm_4 = dm_5;
			dm_6 = dm_7;
			dm_8 = dm_9;

			bmm_0 = bmm_1;
			bmm_2 = bmm_3;
			bmm_4 = bmm_5;
			bmm_6 = bmm_7;
			bmm_8 = bmm_9;

			bim_0 = bim_1;
			bim_2 = bim_3;
			bim_4 = bim_5;
			bim_6 = bim_7;
			bim_8 = bim_9;

			bdm_0 = bdm_1;
			bdm_2 = bdm_3;
			bdm_4 = bdm_5;
			bdm_6 = bdm_7;
			bdm_8 = bdm_9;
		}
	}

	// final log probability is the log10 sum of the last element in the Match and Insertion state arrays
	// this way we ignore all paths that ended in deletions! (huge)
	// but we have to sum all the paths ending in the M and I matrices, because they're no longer extended.
	for (int32_t i = 0; i < SIMD_WIDTH_DOUBLE; i++)
	{
		double finalSumProbabilities = 0.0;

		for (int32_t j = 1; j < paddedHaplotypeLength[i]; j++)
		{
			finalSumProbabilities += matchMatrixVec[j * SIMD_WIDTH_DOUBLE + i] + insertionMatrixVec[j * SIMD_WIDTH_DOUBLE + i];
		}
		result[i] = log10(finalSumProbabilities) - INITIAL_CONDITION_LOG10;
	}
}

int32_t initializeStep1(const int8_t *read_ins_qual, const int8_t *read_del_qual, const int8_t *gcp, const INT_TYPE *hap_bases_length, const INT_TYPE *read_bases_length, const INT_TYPE *prev_hap_bases_length, const INT_TYPE *paddedHaplotypeLength, const INT_TYPE *paddedReadLength, INT_TYPE &currMaxHaplotypeLength, INT_TYPE &currMaxReadLength, double *transitionVec, double *deletionMatrixVec, const double *matchToMatchProb, const double *qualToErrorProbCache, int32_t maxReadLength)
{
	// Step 1 - Obtain max read and haplotype length
	currMaxHaplotypeLength = paddedHaplotypeLength[0];
	currMaxReadLength = paddedReadLength[0];

	for (int32_t i = 1; i < SIMD_WIDTH_DOUBLE; i++)
	{
		currMaxHaplotypeLength = std::max(currMaxHaplotypeLength, paddedHaplotypeLength[i]);
		currMaxReadLength = std::max(currMaxReadLength, paddedReadLength[i]);
	}

	// // *********** Initialize DeletionMatrix
	VEC_INT_TYPE prev_hap_bases_length_vec = VEC_LOAD_INT(prev_hap_bases_length);
	VEC_INT_TYPE hap_bases_length_vec = VEC_LOADU_INT(hap_bases_length);
	VEC_MASK_TYPE mask1 = VEC_CMP_EQ_INT(prev_hap_bases_length_vec, VEC_SET1_INT(-1));
	VEC_MASK_TYPE mask2 = VEC_CMP_NE_INT(prev_hap_bases_length_vec, hap_bases_length_vec);
	VEC_DOUBLE_TYPE initialValueVec = VEC_DIV_PD(VEC_SET1_PD(INITIAL_CONDITION), VEC_CVT_INT_PD(hap_bases_length_vec));
	VEC_DOUBLE_TYPE zeroVec = VEC_SET1_PD(0);
	VEC_DOUBLE_TYPE valueToStore = VEC_BLEND_PD(zeroVec, initialValueVec, mask1);
	valueToStore = VEC_BLEND_PD(valueToStore, initialValueVec, mask2);

	for (INT_TYPE i = 0; i < currMaxHaplotypeLength; i++)
	{
		VEC_STORE_PD(deletionMatrixVec + i * SIMD_WIDTH_DOUBLE, valueToStore);
	}

	INT_TYPE *read_ins_qual_vec = (INT_TYPE *)_mm_malloc(maxReadLength * SIMD_WIDTH_DOUBLE * sizeof(INT_TYPE), ALIGN_SIZE);
	INT_TYPE *read_del_qual_vec = (INT_TYPE *)_mm_malloc(maxReadLength * SIMD_WIDTH_DOUBLE * sizeof(INT_TYPE), ALIGN_SIZE);
	INT_TYPE *gcp_vec = (INT_TYPE *)_mm_malloc(maxReadLength * SIMD_WIDTH_DOUBLE * sizeof(INT_TYPE), ALIGN_SIZE);

	if (read_ins_qual_vec == NULL || read_del_qual_vec == NULL || gcp_vec == NULL)
	{
		_mm_free(read_ins_qual_vec);
		_mm_free(read_del_qual_vec);
		_mm_free(gcp_vec);
		return PDHMM_MEMORY_ALLOCATION_FAILED;
	}
	for (INT_TYPE i = 0; i < SIMD_WIDTH_DOUBLE; i++)
	{
		INT_TYPE index;
		const int8_t *read_ins_qual_i = read_ins_qual + i * maxReadLength;
		for (INT_TYPE j = 0; j < read_bases_length[i]; j++)
		{
			index = j * SIMD_WIDTH_DOUBLE + i;
			read_ins_qual_vec[index] = read_ins_qual_i[j];
		}
	}

	for (INT_TYPE i = 0; i < SIMD_WIDTH_DOUBLE; i++)
	{
		INT_TYPE index;
		const int8_t *read_del_qual_i = read_del_qual + i * maxReadLength;
		for (INT_TYPE j = 0; j < read_bases_length[i]; j++)
		{
			index = j * SIMD_WIDTH_DOUBLE + i;
			read_del_qual_vec[index] = read_del_qual_i[j];
		}
	}

	for (INT_TYPE i = 0; i < SIMD_WIDTH_DOUBLE; i++)
	{
		INT_TYPE index;
		const int8_t *gcp_i = gcp + i * maxReadLength;
		for (INT_TYPE j = 0; j < read_bases_length[i]; j++)
		{
			index = j * SIMD_WIDTH_DOUBLE + i;
			gcp_vec[index] = gcp_i[j];
		}
	}

	initializeProbabilitiesVec(transitionVec, read_ins_qual_vec, read_del_qual_vec, gcp_vec, currMaxReadLength - 1, matchToMatchProb, qualToErrorProbCache);

	_mm_free(read_ins_qual_vec);
	_mm_free(read_del_qual_vec);
	_mm_free(gcp_vec);
	return PDHMM_SUCCESS;
}

int32_t initializeStep2(const int8_t *hap_bases, const int8_t *hap_pdbases, const int8_t *read_bases, const int8_t *read_qual, const INT_TYPE *hap_bases_length, const INT_TYPE *read_bases_length, INT_TYPE *&hap_pdbases_vec, INT_TYPE currMaxHaplotypeLength, INT_TYPE currMaxReadLength, bool &constantsAreInitialized, double *priorVec, const double *qualToErrorProbCache, int maxReadLength, int maxHaplotypeLength)
{
	INT_TYPE *hap_bases_vec = (INT_TYPE *)_mm_malloc(maxHaplotypeLength * SIMD_WIDTH_DOUBLE * sizeof(INT_TYPE), ALIGN_SIZE);
	hap_pdbases_vec = (INT_TYPE *)_mm_malloc(maxHaplotypeLength * SIMD_WIDTH_DOUBLE * sizeof(INT_TYPE), ALIGN_SIZE);
	INT_TYPE *read_bases_vec = (INT_TYPE *)_mm_malloc(maxReadLength * SIMD_WIDTH_DOUBLE * sizeof(INT_TYPE), ALIGN_SIZE);
	INT_TYPE *read_qual_vec = (INT_TYPE *)_mm_malloc(maxReadLength * SIMD_WIDTH_DOUBLE * sizeof(INT_TYPE), ALIGN_SIZE);

	if (hap_bases_vec == NULL || hap_pdbases_vec == NULL || read_bases_vec == NULL || read_qual_vec == NULL)
	{
		_mm_free(hap_bases_vec);
		_mm_free(hap_pdbases_vec);
		_mm_free(read_bases_vec);
		_mm_free(read_qual_vec);
		return PDHMM_MEMORY_ALLOCATION_FAILED;
	}
	for (INT_TYPE i = 0; i < SIMD_WIDTH_DOUBLE; i++)
	{
		INT_TYPE index;
		const int8_t *hap_bases_i = hap_bases + i * maxHaplotypeLength;
		for (INT_TYPE j = 0; j < hap_bases_length[i]; j++)
		{
			index = j * SIMD_WIDTH_DOUBLE + i;
			hap_bases_vec[index] = hap_bases_i[j];
		}
	}

	for (INT_TYPE i = 0; i < SIMD_WIDTH_DOUBLE; i++)
	{
		INT_TYPE index;
		const int8_t *hap_pdbases_i = hap_pdbases + i * maxHaplotypeLength;
		for (INT_TYPE j = 0; j < hap_bases_length[i]; j++)
		{
			index = j * SIMD_WIDTH_DOUBLE + i;
			hap_pdbases_vec[index] = hap_pdbases_i[j];
		}
	}

	for (INT_TYPE i = 0; i < SIMD_WIDTH_DOUBLE; i++)
	{
		INT_TYPE index;
		const int8_t *read_bases_i = read_bases + i * maxReadLength;
		for (INT_TYPE j = 0; j < read_bases_length[i]; j++)
		{
			index = j * SIMD_WIDTH_DOUBLE + i;
			read_bases_vec[index] = read_bases_i[j];
		}
	}

	for (INT_TYPE i = 0; i < SIMD_WIDTH_DOUBLE; i++)
	{
		INT_TYPE index;
		const int8_t *read_qual_i = read_qual + i * maxReadLength;
		for (INT_TYPE j = 0; j < read_bases_length[i]; j++)
		{
			index = j * SIMD_WIDTH_DOUBLE + i;
			read_qual_vec[index] = read_qual_i[j];
		}
	}

	initializePriorsVec(hap_bases_vec, hap_pdbases_vec, read_bases_vec, read_qual_vec, 0, currMaxHaplotypeLength - 1, currMaxReadLength - 1, priorVec, qualToErrorProbCache, maxHaplotypeLength);

	constantsAreInitialized = true;

	_mm_free(hap_bases_vec);
	_mm_free(read_bases_vec);
	_mm_free(read_qual_vec);
	return PDHMM_SUCCESS;
}

int32_t computeReadLikelihoodGivenHaplotypeLog10Vec(const int8_t *hap_bases, const int8_t *hap_pdbases, const int8_t *read_bases, const int8_t *read_qual, const int8_t *read_ins_qual, const int8_t *read_del_qual, const int8_t *gcp, const INT_TYPE *hap_bases_length, const INT_TYPE *read_bases_length, double *matchMatrixVec, double *insertionMatrixVec, double *deletionMatrixVec, double *branchMatchMatrixVec, double *branchInsertionMatrixVec, double *branchDeletionMatrixVec, double *transitionVec, double *priorVec, bool &constantsAreInitialized, INT_TYPE *prev_hap_bases_length, double *result, const double *matchToMatchProb, const double *qualToErrorProbCache, int32_t maxReadLength, int32_t maxHaplotypeLength)
{
	bool *recacheReadValues;
	recacheReadValues = (bool *)_mm_malloc(SIMD_WIDTH_DOUBLE * sizeof(bool), ALIGN_SIZE);
	INT_TYPE *hapStartIndex;
	hapStartIndex = (INT_TYPE *)_mm_malloc(SIMD_WIDTH_DOUBLE * sizeof(INT_TYPE), ALIGN_SIZE);
	INT_TYPE *nextHapStartIndex;
	nextHapStartIndex = (INT_TYPE *)_mm_malloc(SIMD_WIDTH_DOUBLE * sizeof(INT_TYPE), ALIGN_SIZE);

	for (INT_TYPE i = 0; i < SIMD_WIDTH_DOUBLE; i++)
	{
		recacheReadValues[i] = false;
		hapStartIndex[i] = 0;
		nextHapStartIndex[i] = 0;
	}

	INT_TYPE *paddedReadLength = (INT_TYPE *)_mm_malloc(SIMD_WIDTH_DOUBLE * sizeof(INT_TYPE), ALIGN_SIZE);
	INT_TYPE *paddedHaplotypeLength = (INT_TYPE *)_mm_malloc(SIMD_WIDTH_DOUBLE * sizeof(INT_TYPE), ALIGN_SIZE);

	for (INT_TYPE i = 0; i < SIMD_WIDTH_DOUBLE; i++)
	{
		paddedReadLength[i] = read_bases_length[i] + 1;
		paddedHaplotypeLength[i] = hap_bases_length[i] + 1;
		hapStartIndex[i] = (recacheReadValues[i]) ? 0 : hapStartIndex[i];
	}

	// Pre-compute the difference between the current haplotype and the next one to be run
	// Looking ahead is necessary for the ArrayLoglessPairHMM implementation
	// TODO this optimization is very dangerous if we have undetermined haps that could have the same bases but mean different things
	// final int nextHapStartIndex =  (nextHaplotypeBases == null || haplotypeBases.length != nextHaplotypeBases.length) ? 0 : findFirstPositionWhereHaplotypesDiffer(haplotypeBases, haplotypePDBases, nextHaplotypeBases, nextHaplotypePDBases);
	// int32_t nextHapStartIndex[SIMD_WIDTH_DOUBLE] = {0}; // disable the optimization for now until its confirmed to be correct
	INT_TYPE *hap_pdbases_vec, currMaxHaplotypeLength, currMaxReadLength;

	int32_t status = initializeStep1(read_ins_qual, read_del_qual, gcp, hap_bases_length, read_bases_length, prev_hap_bases_length, paddedHaplotypeLength, paddedReadLength, currMaxHaplotypeLength, currMaxReadLength, transitionVec, deletionMatrixVec, matchToMatchProb, qualToErrorProbCache, maxReadLength);

	if (status != PDHMM_SUCCESS)
	{
		_mm_free(hap_pdbases_vec);
		_mm_free(paddedReadLength);
		_mm_free(paddedHaplotypeLength);
		_mm_free(recacheReadValues);
		_mm_free(hapStartIndex);
		_mm_free(nextHapStartIndex);
		return status;
	}

	status = initializeStep2(hap_bases, hap_pdbases, read_bases, read_qual, hap_bases_length, read_bases_length, hap_pdbases_vec, currMaxHaplotypeLength, currMaxReadLength, constantsAreInitialized, priorVec, qualToErrorProbCache, maxReadLength, maxHaplotypeLength);

	if (status != PDHMM_SUCCESS)
	{
		_mm_free(hap_pdbases_vec);
		_mm_free(paddedReadLength);
		_mm_free(paddedHaplotypeLength);
		_mm_free(recacheReadValues);
		_mm_free(hapStartIndex);
		_mm_free(nextHapStartIndex);
		return status;
	}

	computationStep(paddedReadLength, hap_pdbases_vec, currMaxHaplotypeLength, currMaxReadLength, paddedHaplotypeLength, matchMatrixVec, insertionMatrixVec, deletionMatrixVec, branchMatchMatrixVec, branchInsertionMatrixVec, branchDeletionMatrixVec, transitionVec, priorVec, result, maxHaplotypeLength);

	// Utils.validate(result <= 0.0, () -> "PairHMM Log Probability cannot be greater than 0: " + String.format("haplotype: %s, read: %s, result: %f, PairHMM: %s", new String(haplotypeBases), new String(readBases), result, this.getClass().getSimpleName()));

	// Utils.validate(MathUtils.isValidLog10Probability(result), () -> "Invalid Log Probability: " + result);

	// Warning: This assumes no downstream modification of the haplotype bases (saves us from copying the array). It is okay for the haplotype caller.
	for (INT_TYPE i = 0; i < SIMD_WIDTH_DOUBLE; i++)
	{
		prev_hap_bases_length[i] = hap_bases_length[i];

		// For the next iteration, the hapStartIndex for the next haploytpe becomes the index for the current haplotype
		// The array implementation has to look ahead to the next haplotype to store caching info. It cannot do this if nextHapStart is before hapStart
		hapStartIndex[i] = (nextHapStartIndex[i] < hapStartIndex[i]) ? 0 : nextHapStartIndex[i];
	}
	_mm_free(hap_pdbases_vec);
	_mm_free(paddedReadLength);
	_mm_free(paddedHaplotypeLength);
	_mm_free(recacheReadValues);
	_mm_free(hapStartIndex);
	_mm_free(nextHapStartIndex);
	return PDHMM_SUCCESS;
}

int32_t CONCAT(computePDHMM_, SIMD_ENGINE)(const int8_t *hap_bases, const int8_t *hap_pdbases, const int8_t *read_bases, const int8_t *read_qual, const int8_t *read_ins_qual, const int8_t *read_del_qual, const int8_t *gcp, double *result, int64_t t, const int64_t *hap_lengths, const int64_t *read_lengths, int32_t maxReadLength, int32_t maxHaplotypeLength)
{
	int32_t totalThreads = 1;
#pragma omp parallel
	{
		totalThreads = omp_get_num_threads();
	}
	double *matchToMatchLog10, *matchToMatchProb, *qualToErrorProbCache, *qualToProbLog10Cache;
	INT_TYPE testcase, roundedTestcase;

#if defined(IS_INT32) && IS_INT32
	testcase = (INT_TYPE)(t);
	INT_TYPE *hap_bases_lengths = (INT_TYPE *)_mm_malloc(testcase * sizeof(INT_TYPE), ALIGN_SIZE);
	INT_TYPE *read_bases_length = (INT_TYPE *)_mm_malloc(testcase * sizeof(INT_TYPE), ALIGN_SIZE);
	if (hap_bases_lengths == NULL || read_bases_length == NULL)
	{
		_mm_free(hap_bases_lengths);
		_mm_free(read_bases_length);
		return PDHMM_MEMORY_ALLOCATION_FAILED;
	}
	for (int i = 0; i < testcase; i++)
	{
		hap_bases_lengths[i] = (INT_TYPE)(hap_lengths[i]);
		read_bases_length[i] = (INT_TYPE)(read_lengths[i]);
	}
#else
	testcase = (INT_TYPE)t;
	const INT_TYPE *hap_bases_lengths = hap_lengths;
	const INT_TYPE *read_bases_length = read_lengths;
#endif

	int32_t initStatus = init(matchToMatchLog10, matchToMatchProb, qualToErrorProbCache, qualToProbLog10Cache);
	initVec();

	if (initStatus != PDHMM_SUCCESS)
	{
#if defined(IS_INT32) && IS_INT32
		_mm_free(hap_bases_lengths);
		_mm_free(read_bases_length);
#endif
		freeInit(matchToMatchLog10, matchToMatchProb, qualToErrorProbCache, qualToProbLog10Cache);
		return initStatus;
	}

	double *g_matchMatrixVec, *g_insertionMatrixVec, *g_deletionMatrixVec, *g_branchMatchMatrixVec, *g_branchInsertionMatrixVec, *g_branchDeletionMatrixVec, *g_transitionVec, *g_priorVec;

	bool *g_constantsAreInitialized, *g_initialized;
	INT_TYPE *g_prev_hap_bases_length;

	int32_t allocateStatus = allocateVec(g_matchMatrixVec, g_insertionMatrixVec, g_deletionMatrixVec, g_branchMatchMatrixVec, g_branchInsertionMatrixVec, g_branchDeletionMatrixVec, g_transitionVec, g_priorVec, g_constantsAreInitialized, g_initialized, g_prev_hap_bases_length, maxReadLength, maxHaplotypeLength, totalThreads); // array allocation
	int32_t *status = (int32_t *)_mm_malloc(totalThreads * sizeof(int32_t), ALIGN_SIZE);

	if (allocateStatus != PDHMM_SUCCESS || status == NULL)
	{
#if defined(IS_INT32) && IS_INT32
		_mm_free(hap_bases_lengths);
		_mm_free(read_bases_length);
#endif
		freeInit(matchToMatchLog10, matchToMatchProb, qualToErrorProbCache, qualToProbLog10Cache);
		freeVec(g_matchMatrixVec, g_insertionMatrixVec, g_deletionMatrixVec, g_branchMatchMatrixVec, g_branchInsertionMatrixVec, g_branchDeletionMatrixVec, g_transitionVec, g_priorVec, g_constantsAreInitialized, g_initialized, g_prev_hap_bases_length);

		return allocateStatus;
	}

	const int32_t paddedMaxReadLength = maxReadLength + 1;
	const int32_t paddedMaxHaplotypeLength = maxHaplotypeLength + 1;
	int32_t sizeOfIntermTables = paddedMaxHaplotypeLength * SIMD_WIDTH_DOUBLE;

	for (int32_t i = 0; i < totalThreads; i++)
	{
		status[i] = PDHMM_SUCCESS;
	}

	roundedTestcase = (testcase / SIMD_WIDTH_DOUBLE) * SIMD_WIDTH_DOUBLE;

#pragma omp parallel
	{
		int32_t tid = omp_get_thread_num();

		double *matchMatrixVec, *insertionMatrixVec, *deletionMatrixVec, *branchMatchMatrixVec, *branchInsertionMatrixVec, *branchDeletionMatrixVec, *transitionVec, *priorVec;

		matchMatrixVec = g_matchMatrixVec + (sizeOfIntermTables * tid);
		insertionMatrixVec = g_insertionMatrixVec + (sizeOfIntermTables * tid);
		deletionMatrixVec = g_deletionMatrixVec + (sizeOfIntermTables * tid);
		branchMatchMatrixVec = g_branchMatchMatrixVec + (sizeOfIntermTables * tid);
		branchInsertionMatrixVec = g_branchInsertionMatrixVec + (sizeOfIntermTables * tid);
		branchDeletionMatrixVec = g_branchDeletionMatrixVec + (sizeOfIntermTables * tid);

		transitionVec = g_transitionVec + (TRANS_PROB_ARRAY_LENGTH * paddedMaxReadLength * SIMD_WIDTH_DOUBLE * tid);
		priorVec = g_priorVec + (paddedMaxHaplotypeLength * paddedMaxReadLength * SIMD_WIDTH_DOUBLE * tid);

		bool *constantsAreInitialized, *initialized;
		constantsAreInitialized = g_constantsAreInitialized + tid;
		initialized = g_initialized + tid;

		INT_TYPE *prev_hap_bases_length = g_prev_hap_bases_length + (SIMD_WIDTH_DOUBLE * tid);

#pragma omp for
		for (int32_t i = 0; i < roundedTestcase; i += SIMD_WIDTH_DOUBLE)
		{
			initializeVec(matchMatrixVec, insertionMatrixVec, deletionMatrixVec, branchMatchMatrixVec, branchInsertionMatrixVec, branchDeletionMatrixVec, transitionVec, priorVec, *constantsAreInitialized, *initialized, maxReadLength, maxHaplotypeLength); // array allocation
			for (int32_t j = 0; j < SIMD_WIDTH_DOUBLE; j++)
			{
				prev_hap_bases_length[j] = -1;
			}

			int32_t hapIndexOffset = i * maxHaplotypeLength;
			int32_t readIndexOffset = i * maxReadLength;

			int32_t currStatus = computeReadLikelihoodGivenHaplotypeLog10Vec(hap_bases + hapIndexOffset, hap_pdbases + hapIndexOffset, read_bases + readIndexOffset, read_qual + readIndexOffset, read_ins_qual + readIndexOffset, read_del_qual + readIndexOffset, gcp + readIndexOffset, hap_bases_lengths + i, read_bases_length + i, matchMatrixVec, insertionMatrixVec, deletionMatrixVec, branchMatchMatrixVec, branchInsertionMatrixVec, branchDeletionMatrixVec, transitionVec, priorVec, *constantsAreInitialized, prev_hap_bases_length, result + i, matchToMatchProb, qualToErrorProbCache, maxReadLength, maxHaplotypeLength);

			if (currStatus != PDHMM_SUCCESS)
				status[tid] = currStatus;
		}
	}

	/* remaining testcases */

	int32_t hapIndexOffset = (int32_t)roundedTestcase * maxHaplotypeLength;
	int32_t readIndexOffset = (int32_t)roundedTestcase * maxReadLength;
	computePDHMM_serial(matchToMatchLog10, matchToMatchProb, qualToErrorProbCache, qualToProbLog10Cache, g_matchMatrixVec, g_insertionMatrixVec, g_deletionMatrixVec, g_branchMatchMatrixVec, g_branchInsertionMatrixVec, g_branchDeletionMatrixVec, g_transitionVec, g_priorVec, hap_bases + hapIndexOffset, hap_pdbases + hapIndexOffset, read_bases + readIndexOffset, read_qual + readIndexOffset, read_ins_qual + readIndexOffset, read_del_qual + readIndexOffset, gcp + readIndexOffset, result + roundedTestcase, testcase - roundedTestcase, hap_lengths + roundedTestcase, read_lengths + roundedTestcase, maxReadLength, maxHaplotypeLength);
	INFO("Total Tests performed serially = %ld\n", int64_t(testcase - roundedTestcase));

	freeVec(g_matchMatrixVec, g_insertionMatrixVec, g_deletionMatrixVec, g_branchMatchMatrixVec, g_branchInsertionMatrixVec, g_branchDeletionMatrixVec, g_transitionVec, g_priorVec, g_constantsAreInitialized, g_initialized, g_prev_hap_bases_length);
	freeInit(matchToMatchLog10, matchToMatchProb, qualToErrorProbCache, qualToProbLog10Cache);

	int32_t outputStatus = PDHMM_SUCCESS;
	for (int32_t i = 0; i < totalThreads; i++)
	{
		if (status[i] != PDHMM_SUCCESS)
			outputStatus = status[i];
	}
	_mm_free(status);

#if defined(IS_INT32) && IS_INT32
	_mm_free(hap_bases_lengths);
	_mm_free(read_bases_length);
#endif

	return outputStatus;
}
