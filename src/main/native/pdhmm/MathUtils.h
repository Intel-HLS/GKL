/*
#Copyright(C) 2023 Intel Corporation
#SPDX - License - Identifier : MIT License
*/

#ifndef _MATHUTILS_H
#define _MATHUTILS_H
#include <cstdint>
#include <immintrin.h>

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

int32_t fastRound(double d);

bool isValidLog10Probability(const double result);

enum PartiallyDeterminedHaplotype
{
    SNP = 1,
    DEL_START = 2,
    DEL_END = 4,
    A = 8,
    C = 16,
    G = 32,
    T = 64,

};

#endif
