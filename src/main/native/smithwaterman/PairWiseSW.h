/***********************************************
* # Copyright 2017. Sanchit Misra
* # Contact: Sanchit Misra
* #          sanchit.misra@intel.com
* #
* # GPL 2.0 applies.
* #
* ************************************************/

#ifndef _PAIRWISESW_H
#define _PAIRWISESW_H

#define __STDC_LIMIT_MACROS
#include <stdint.h>
#include <immintrin.h>
#include <omp.h>

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
        int16_t *cigar;
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
#define SIMD_WIDTH 8
#define DUMMY1 'B'
#define DUMMY2 'D'

class PairWiseSW
{
public:
    PairWiseSW(int32_t w_match, int32_t w_mismatch, int32_t w_open, int32_t w_extend);
	~PairWiseSW();
	void runSmithWaterman(SeqPair *pairArray, int32_t numPairs, int8_t bt, int32_t numThreads);
#ifdef DEBUG
    int32_t runSWOnePairBT(uint8_t *seq1, uint8_t *seq2, int32_t len1, int32_t len2, int8_t overhangStrategy, int16_t *cigarArray, int16_t *cigarCount, int32_t *score);
#else
    int32_t runSWOnePairBT(uint8_t *seq1, uint8_t *seq2, int32_t len1, int32_t len2, int8_t overhangStrategy, int16_t *cigarArray, int16_t *cigarCount);
#endif
	int64_t getTicks();

protected:
    void smithWatermanOnePair(SeqPair *p);
    void smithWatermanOnePairWrapper(SeqPair *pairArray, int32_t numPairs);
    void smithWatermanBackTrack(SeqPair *p, int32_t tid);
    void getCIGAR(SeqPair *p, int32_t tid);

private:

    int64_t totalTicks;
#ifdef PERF_DEBUG
    void getBTStats(SeqPair *p);
    int64_t mainLoopTicks;
    long unsigned int mainLoopCount, setupLoopCount;
    int64_t totalCount, overflowCount;
#endif
    int32_t w_match;
    int32_t w_mismatch;
    int32_t w_open;
    int32_t w_extend;

    int32_t *E_;
    int16_t *backTrack_;
};
#endif

