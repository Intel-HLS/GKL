/*
#Copyright(C) 2023 Intel Corporation
#SPDX - License - Identifier : MIT License
*/

#ifndef PDHMM_SERIAL_H
#define PDHMM_SERIAL_H

#include <cstdint>

int32_t computePDHMM_serial(const int8_t *hap_bases, const int8_t *hap_pdbases, const int8_t *read_bases, const int8_t *read_qual, const int8_t *read_ins_qual, const int8_t *read_del_qual, const int8_t *gcp, double *result, int64_t testcase, const int64_t *hap_lengths, const int64_t *read_lengths, int32_t maxReadLength, int32_t maxHaplotypeLength);

int32_t computePDHMM_serial(const double *matchToMatchProb, const double *qualToErrorProbCache, double *matchMatrix, double *insertionMatrix, double *deletionMatrix, double *branchMatchMatrix, double *branchInsertionMatrix, double *branchDeletionMatrix, double *transition, double *prior, const int8_t *hap_bases, const int8_t *hap_pdbases, const int8_t *read_bases, const int8_t *read_qual, const int8_t *read_ins_qual, const int8_t *read_del_qual, const int8_t *gcp, double *result, int64_t testcase, const int64_t *hap_lengths, const int64_t *read_lengths, int32_t maxReadLength, int32_t maxHaplotypeLength);

#endif
