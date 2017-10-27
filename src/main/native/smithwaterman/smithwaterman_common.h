#ifndef SMITHWATERMAN_COMMON_H
#define SMITHWATERMAN_COMMON_H

#if defined(_MSC_VER)
  #include <intrin.h> // SIMD intrinsics for Windows
#else
  #include <x86intrin.h> // SIMD intrinsics for GCC
#endif



#include <stdint.h>
#include <string.h>
#include <immintrin.h>
#include <omp.h>


#define CAT(X,Y) X##Y
#define CONCAT(X,Y) CAT(X,Y)

#define MATCH 0
#define INSERT 1
#define DELETE 2
#define INSERT_EXT 4
#define DELETE_EXT 8
#define SOFTCLIP 9
#define INDEL 10
#define LEADING_INDEL 11
#define IGNORE 12

typedef struct dnaSeqPair
{
        int32_t id;
        uint8_t *seq1;
        uint8_t *seq2;
        int16_t len1, len2;
        int8_t overhangStrategy;
        int32_t score;
        int16_t max_i;
        int16_t max_j;
        int16_t *btrack;
        char *cigar;
        int16_t cigarCount;
        int16_t alignmentOffset;
}SeqPair;

//the maximum DNA sequence length
#define MAX_SEQ_LEN 1024
#define MAX_NUM_PAIRS 800000
#define MATRIX_MIN_CUTOFF -100000000
#define LOW_INIT_VALUE (INT32_MIN/2)
#define max(x, y) ((x)>(y)?(x):(y))
#define min(x, y) ((x)<(y)?(x):(y))
#define DUMMY1 'B'
#define DUMMY2 'D'

/*
    void PairWiseSWInit(int32_t w_match, int32_t w_mismatch, int32_t w_open, int32_t w_extend);
	void PairWiseSWClose();
	void runSmithWaterman(SeqPair *pairArray, int32_t numPairs, int8_t bt, int32_t numThreads);

    int32_t runSWOnePairBT(uint8_t *seq1, uint8_t *seq2, int32_t len1, int32_t len2, int8_t overhangStrategy, char *cigarArray, int16_t *cigarCount);


    void smithWatermanBackTrack(SeqPair *p, int32_t tid);
    void getCIGAR(SeqPair *p, int32_t tid);
  */


#endif