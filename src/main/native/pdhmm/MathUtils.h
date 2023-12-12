/*
#Copyright(C) 2023 Intel Corporation
#SPDX - License - Identifier : MIT License
*/

#ifndef _MATHUTILS_H
#define _MATHUTILS_H
#include <limits>
#include <stdint.h>
#include <immintrin.h>
#include <math.h>

extern double INITIAL_CONDITION;
extern double INITIAL_CONDITION_LOG10;
extern const double neg_infinity;
extern const double LN10;
extern const double INV_LN10;

class JacobianLogTable
{
public:
	static double *cache;
	static double TABLE_STEP;
	static double INV_STEP;
	static double MAX_TOLERANCE;
	static double get(double difference);

	static void initCache();

private:
	static double cacheIntToDouble(int i);
};

double approximateLog10SumLog10(double a, double b);

bool isSame(double a, double b, double epsilon);
int32_t fastRound(double d);
int64_t getFreq();
bool isValidLog10Probability(const double result);

class PartiallyDeterminedHaplotype
{
public:
	static int8_t SNP, DEL_START, DEL_END, A, C, G, T, N;
};

#endif