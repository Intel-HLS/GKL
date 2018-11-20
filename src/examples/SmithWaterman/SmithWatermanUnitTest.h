#ifndef SMITHWATERMAN_COMMON_H
#define SMITHWATERMAN_COMMON_H

#if defined(_MSC_VER)
  #include <intrin.h> // SIMD intrinsics for Windows
#else
  #include <x86intrin.h> // SIMD intrinsics for GCC
#endif

#define __STDC_LIMIT_MACROS

#include <stdint.h>
#include <string.h>
#include <immintrin.h>



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
        char *cigar;
        int16_t cigarCount;
        int16_t alignmentOffset;
}SeqPair;

//the maximum DNA sequence length
#define MAX_SEQ_LEN 1024
#define MAX_NUM_PAIRS 80000
#define MATRIX_MIN_CUTOFF -100000000
#define LOW_INIT_VALUE (INT32_MIN/2)
#define max(x, y) ((x)>(y)?(x):(y))
#define min(x, y) ((x)<(y)?(x):(y))
#define DUMMY1 'B'
#define DUMMY2 'D'

int32_t w_match;
    int32_t w_mismatch;
    int32_t w_open;
    int32_t w_extend;

    int32_t *E_;
    int16_t *backTrack_;
    int16_t *cigarBuf_;


//extern void align(char* ref, char* alt, char*cigar, int match, int mismatch, int open, int extend, int strategy, int score, int offset);
#endif