#ifndef AVX2_IMPL_H
#define AVX2_IMPL_H

#include "smithwaterman_common.h"

extern int32_t (*runSWOnePairBT_fp_avx2)(int32_t match, int32_t mismatch, int32_t open, int32_t extend,uint8_t *seq1, uint8_t *seq2, int16_t len1, int16_t len2, int8_t overhangStrategy, char *cigarArray, int32_t cigarLen, uint32_t *cigarCount, int32_t *offset);

#endif //AVX2_IMPL_H

