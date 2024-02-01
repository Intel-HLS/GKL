/*
#Copyright(C) 2023 Intel Corporation
#SPDX - License - Identifier : MIT License
*/

#include "MathUtils.h"
#include <cmath>
#include <cstdio>
#include <immintrin.h>
#include <unistd.h>

using namespace std;
double INITIAL_CONDITION = pow(2, 1020);
double INITIAL_CONDITION_LOG10 = log10(INITIAL_CONDITION);
const double neg_infinity = -1e10;
double JacobianLogTable::TABLE_STEP = 0.0001;
double JacobianLogTable::INV_STEP = 1.0 / TABLE_STEP;
double JacobianLogTable::MAX_TOLERANCE = 8.0;
double *JacobianLogTable::cache = NULL;
const double LN10 = log(10);
const double INV_LN10 = 1.0 / LN10;

int32_t fastRound(double d)
{
    return (d > 0.0) ? (int32_t)(d + 0.5) : (int32_t)(d - 0.5);
}

bool isValidLog10Probability(double result) { return result <= 0.0; }
double JacobianLogTable::get(double difference)
{
    int index = fastRound(difference * INV_STEP);
    return cache[index];
}

void JacobianLogTable::initCache()
{
    int from = 0;
    int to = (int)(MAX_TOLERANCE / TABLE_STEP) + 1;
    double *result = (double *)_mm_malloc((to - from) * sizeof(double), 64);
    for (int i = from; i < to; i++)
    {
        result[i - from] = cacheIntToDouble(i);
    }
    cache = result;
}

double JacobianLogTable::cacheIntToDouble(int k)
{
    return log10(1.0 + pow(10.0, -k * TABLE_STEP));
}

double approximateLog10SumLog10(double a, double b)
{
    // this code works only when a <= b so we flip them if the order is opposite
    if (a > b)
    {
        return approximateLog10SumLog10(b, a);
    }
    else if (a == neg_infinity)
    {
        return b;
    }

    // if |b-a| < tol we need to compute log(e^a + e^b) = log(e^b(1 + e^(a-b))) =
    // b + log(1 + e^(-(b-a))) we compute the second term as a table lookup with
    // integer quantization we have pre-stored correction for 0,0.1,0.2,... 10.0
    double diff = b - a;
    return b + (diff < JacobianLogTable::MAX_TOLERANCE
                    ? JacobianLogTable::get(diff)
                    : 0.0);
}
