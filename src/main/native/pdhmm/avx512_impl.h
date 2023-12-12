/*
#Copyright(C) 2023 Intel Corporation
#SPDX - License - Identifier : MIT License
*/

#ifndef AVX512_IMPL_H
#define AVX512_IMPL_H

#include "pdhmm-common.h"

extern int32_t (*computePDHMM_fp_avx512)(const int8_t *hap_bases, const int8_t *hap_pdbases, const int8_t *read_bases, const int8_t *read_qual, const int8_t *read_ins_qual, const int8_t *read_del_qual, const int8_t *gcp, double *result, int64_t testcase, const int64_t *hap_lengths, const int64_t *read_lengths, int32_t maxReadLength, int32_t maxHaplotypeLength);

#endif // AVX512_IMPL_H
