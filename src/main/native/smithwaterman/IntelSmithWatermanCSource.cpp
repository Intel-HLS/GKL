

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <immintrin.h>
#include <assert.h>
#include <debug.h>
#include <avx.h>
#include "avx2_impl.h"
#ifndef __APPLE__
  #include "avx512_impl.h"
#endif

#include "IntelSmithWatermanCSource.h"
#include "smithwaterman_common.h"

int32_t (*g_runSWOnePairBT)(int32_t match, int32_t mismatch, int32_t open, int32_t extend,uint8_t *seq1, uint8_t *seq2, int32_t len1, int32_t len2, int8_t overhangStrategy, char *cigarArray, int16_t *cigarCount, int16_t *score);


void initNative()
{

if(is_avx512_supported())
      {
    #ifndef __APPLE__
        DBG("Using CPU-supported AVX-512 instructions");
        g_runSWOnePairBT = runSWOnePairBT_fp_avx512;

    #else
        assert(false);
    #endif
      }
      else
      {
        g_runSWOnePairBT = runSWOnePairBT_fp_avx2;
      }
      return;
}

/* performns smithwaterman alignment on one pair of ref and alt vector and computes, score and cigar string */

void align(char* ref, char* alt, char*cigar, int match, int mismatch, int open, int extend, int strategy, int *score, int *offset)
{

    int count = 0;
    int alignmentoffset =0;

    int refLength = strlen(ref);
    int altLength = strlen(alt);

    (*offset) = g_runSWOnePairBT(match, mismatch, open, extend,(uint8_t*) ref, (uint8_t*) alt,refLength, altLength, strategy, (char *) cigar, (int16_t*) &count, (int16_t*) &score);


    return;
}



