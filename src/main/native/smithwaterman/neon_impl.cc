#include "neon_impl.h"

#include "neon-smithwaterman.h"

int32_t (*runSWOnePairBT_fp_neon)(int32_t match, int32_t mismatch, int32_t open, int32_t extend,uint8_t *seq1, uint8_t *seq2, int32_t len1, int32_t len2, int8_t overhangStrategy, char *cigarArray, int16_t *cigarCount)= &runSWOnePairBT_neon;


