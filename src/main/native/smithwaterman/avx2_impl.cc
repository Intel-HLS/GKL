#include "avx2_impl.h"

#include "avx2-smithwaterman.h"

int32_t (*runSWOnePairBT_fp_avx2)(int32_t match, int32_t mismatch, int32_t open, int32_t extend,uint8_t *seq1, uint8_t *seq2, int32_t len1, int32_t len2, int8_t overhangStrategy, char *cigarArray, int32_t cigarLen, int16_t *cigarCount, int32_t *offset)= &runSWOnePairBT_avx2;
