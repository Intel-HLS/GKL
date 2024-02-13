/*
#Copyright(C) 2023 Intel Corporation
#SPDX - License - Identifier : MIT License
*/

#ifndef MATHUTILS_AVX
#define MATHUTILS_AVX
#include "MathUtils.h"

static VEC_INT_TYPE SNPVec, AVec, CVec, GVec, TVec;

inline void initVec()
{
    SNPVec = VEC_SET1_INT(PartiallyDeterminedHaplotype::SNP);
    AVec = VEC_SET1_INT(PartiallyDeterminedHaplotype::A);
    CVec = VEC_SET1_INT(PartiallyDeterminedHaplotype::C);
    GVec = VEC_SET1_INT(PartiallyDeterminedHaplotype::G);
    TVec = VEC_SET1_INT(PartiallyDeterminedHaplotype::T);
}

inline VEC_LONG_TYPE fastRound(VEC_DOUBLE_TYPE d)
{
    VEC_DOUBLE_MASK_TYPE mask1 = VEC_CMP_GT_PD(d, VEC_SET0_PD());
    VEC_DOUBLE_TYPE half = VEC_SET1_PD(0.5);
    VEC_LONG_TYPE d_plus = VEC_CVT_ROUND_PD_LONG(VEC_ADD_PD(d, half));
    VEC_LONG_TYPE d_minus = VEC_CVT_ROUND_PD_LONG(VEC_SUB_PD(d, half));

    return VEC_BLEND_LONG_DOUBLE_MASK(d_minus, d_plus, mask1);
}

class CONCAT(JacobianLogTable_vec_, SIMD_ENGINE) : public JacobianLogTable
{
public:
    inline static VEC_DOUBLE_TYPE get(VEC_DOUBLE_TYPE difference)
    {
        VEC_LONG_TYPE index = fastRound(VEC_MUL_PD(difference, VEC_SET1_PD(INV_STEP)));
        VEC_DOUBLE_TYPE result = VEC_GATHER_PD_LONG_INDEX(index, cache, 8);
        return result;
    }
};

inline VEC_DOUBLE_TYPE approximateLog10SumLog10(VEC_DOUBLE_TYPE a, VEC_DOUBLE_TYPE b)
{
    // this code works only when a <= b so we flip them if the order is opposite
    VEC_DOUBLE_MASK_TYPE mask1 = VEC_CMP_GT_PD(a, b);
    if (VEC_CMP_NE_DOUBLE_MASK(mask1, 0))
    {
        VEC_DOUBLE_TYPE newA = VEC_MIN_PD(a, b);
        VEC_DOUBLE_TYPE newB = VEC_MAX_PD(a, b);
        return approximateLog10SumLog10(newA, newB);
    }

    mask1 = VEC_CMP_EQ_PD(a, VEC_SET1_PD(neg_infinity));
    VEC_DOUBLE_TYPE result;

    // if |b-a| < tol we need to compute log(e^a + e^b) = log(e^b(1 + e^(a-b))) = b + log(1 + e^(-(b-a)))
    // we compute the second term as a table lookup with integer quantization
    // we have pre-stored correction for 0,0.1,0.2,... 10.0
    VEC_DOUBLE_TYPE diff = VEC_SUB_PD(b, a);
    VEC_DOUBLE_TYPE zeroVec = VEC_SET0_PD();
    VEC_DOUBLE_MASK_TYPE mask2 = VEC_CMP_LT_PD(diff, VEC_SET1_PD(JacobianLogTable::MAX_TOLERANCE));
    diff = VEC_BLEND_PD_DOUBLE_MASK(zeroVec, diff, mask2);
    VEC_DOUBLE_TYPE getDiff = CONCAT(JacobianLogTable_vec_, SIMD_ENGINE)::get(diff);
    result = VEC_BLEND_PD_DOUBLE_MASK(zeroVec, getDiff, mask2);
    result = VEC_ADD_PD(result, b);
    result = VEC_BLEND_PD_DOUBLE_MASK(result, b, mask1);
    return result;
}

VEC_DOUBLE_TYPE pow_pd(double a, VEC_DOUBLE_TYPE b, int32_t size, int32_t &status)
{
    double *elements = (double *)_mm_malloc(size * sizeof(double), 64);
    if (elements == NULL)
    {
        status = PDHMM_MEMORY_ALLOCATION_FAILED;
        return b;
    }
    VEC_STORE_PD(elements, b);

    for (int32_t i = 0; i < size; i++)
    {
        elements[i] = pow(a, elements[i]);
    }
    b = VEC_LOAD_PD(elements);
    _mm_free(elements);
    return b;
}

#endif
