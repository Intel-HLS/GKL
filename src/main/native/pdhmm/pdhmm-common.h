/*
#Copyright(C) 2023 Intel Corporation
#SPDX - License - Identifier : MIT License
*/

#ifndef AVX_PDHMM_COMMON
#define AVX_PDHMM_COMMON
#define TRANS_PROB_ARRAY_LENGTH 6
#define MAX_QUAL 254

#define INV_STEP_CONST 10000.0
#define OFF 1

#define NORMAL_STATE 0
#define INSIDE_STATE 1
#define AFTER_STATE 2

#define ROW_UNROLL 4
#define ALIGN_SIZE 64

#define CAT(X, Y) X##Y
#define CONCAT(X, Y) CAT(X, Y)

#define PDHMM_SUCCESS 0
#define PDHMM_MEMORY_ALLOCATION_FAILED 1
#define PDHMM_INPUT_DATA_ERROR 2
#define PDHMM_FAILURE 3

#include <stdint.h> /* int32_t */
#include <math.h>
#include <cstdio>
#include <immintrin.h>
#include "MathUtils.h"
#include <debug.h>

enum HMMState
{
	// The regular state
	NORMAL,

	// Idicating that we must be copying the array elements to the right
	INSIDE_DEL,

	// Indicating that we must handle the special case for merging events after the del
	AFTER_DEL,
};

const int matchToMatch = 0;
const int indelToMatch = 1;
const int matchToInsertion = 2;
const int insertionToInsertion = 3;
const int matchToDeletion = 4;
const int deletionToDeletion = 5;

inline double qualToErrorProb(double qual, int32_t &status)
{
	if (qual < 0.0)
	{
		status = PDHMM_INPUT_DATA_ERROR;
		DBG("deletion quality cannot be less than 0 \n");
	}
	return pow(10.0, qual / -10.0);
}

inline int32_t init(double *&matchToMatchLog10, double *&matchToMatchProb, double *&qualToErrorProbCache, double *&qualToProbLog10Cache)
{
	int32_t status = PDHMM_SUCCESS;
	/* Step 1  */
	JacobianLogTable::initCache();

	/* Step 2  */
	matchToMatchLog10 = (double *)_mm_malloc((((MAX_QUAL + 1) * (MAX_QUAL + 2)) >> 1) * sizeof(double), ALIGN_SIZE);
	matchToMatchProb = (double *)_mm_malloc((((MAX_QUAL + 1) * (MAX_QUAL + 2)) >> 1) * sizeof(double), ALIGN_SIZE);
	qualToErrorProbCache = (double *)_mm_malloc((MAX_QUAL + 1) * sizeof(double), ALIGN_SIZE);
	qualToProbLog10Cache = (double *)_mm_malloc((MAX_QUAL + 1) * sizeof(double), ALIGN_SIZE);

	if (matchToMatchLog10 == NULL || matchToMatchProb == NULL || qualToErrorProbCache == NULL || qualToProbLog10Cache == NULL)
	{
		_mm_free(matchToMatchLog10);
		_mm_free(matchToMatchProb);
		_mm_free(qualToErrorProbCache);
		_mm_free(qualToProbLog10Cache);
		return PDHMM_MEMORY_ALLOCATION_FAILED;
	}

	for (int32_t i = 0, offset = 0; i <= MAX_QUAL; offset += ++i)
	{
		for (int32_t j = 0; j <= i; j++)
		{
			double log10Sum = approximateLog10SumLog10(-0.1 * i, -0.1 * j);
			matchToMatchLog10[offset + j] = log1p(-std::min(1.0, pow(10, log10Sum))) * INV_LN10;
			matchToMatchProb[offset + j] = pow(10, matchToMatchLog10[offset + j]);
		}
	}

	/* Step 3 */

	for (int32_t i = 0; i <= MAX_QUAL; i++)
	{
		qualToErrorProbCache[i] = qualToErrorProb((double)i, status);
		if (status != PDHMM_SUCCESS)
		{
			_mm_free(matchToMatchLog10);
			_mm_free(matchToMatchProb);
			_mm_free(qualToErrorProbCache);
			_mm_free(qualToProbLog10Cache);
			return status;
		}
		qualToProbLog10Cache[i] = log10(1.0 - qualToErrorProbCache[i]);
	}
	return status;
}

inline void freeInit(double *matchToMatchLog10, double *matchToMatchProb, double *qualToErrorProbCache, double *qualToProbLog10Cache)
{
	_mm_free(matchToMatchLog10);
	_mm_free(matchToMatchProb);
	_mm_free(qualToErrorProbCache);
	_mm_free(qualToProbLog10Cache);
}
#endif