
#include <stdio.h>
#include <stdlib.h>
#include "PairWiseSW.h"

#ifdef IACA_ANALYSIS
#include "iacaMarks.h"
#endif

#ifdef SEP_ANALYSIS
#include "sampling.h"
#endif


#define MAIN_CODE(bt256) \
            { \
            __m256i e10 = _mm256_loadu_si256((__m256i *)(&E[inde])); \
            __m256i ext_score_h = _mm256_add_epi32(e10, w_extend_256); \
            __m256i h10 = _mm256_loadu_si256((__m256i *)(&H[hLeftInd])); \
            __m256i open_score_h = _mm256_add_epi32(h10, w_open_256); \
            __m256i e11 = _mm256_max_epi32(open_score_h, ext_score_h); \
            __m256i open_gt_ext_h = _mm256_cmpgt_epi32(open_score_h, ext_score_h); \
            __m256i ext256 = _mm256_andnot_si256(open_gt_ext_h, ins_ext256); \
            _mm256_storeu_si256((__m256i *)(&E[inde]), e11); \
            __m256i f01 = _mm256_loadu_si256((__m256i *)(&F[indf])); \
            __m256i f11; \
            __m256i ext_score_v = _mm256_add_epi32(f01, w_extend_256); \
            __m256i h01 = _mm256_loadu_si256((__m256i *)(&H[hTopInd])); \
            __m256i open_score_v = _mm256_add_epi32(h01, w_open_256); \
            f11 = _mm256_max_epi32(ext_score_v, open_score_v); \
            __m256i open_gt_ext_v = _mm256_cmpgt_epi32(open_score_v, ext_score_v); \
            ext256 = _mm256_or_si256(ext256, _mm256_andnot_si256(open_gt_ext_v, del_ext256)); \
            _mm256_storeu_si256((__m256i *)(&F[indf]), f11); \
            __m256i h00 = _mm256_loadu_si256((__m256i *)(&H[hCurInd])); \
            __m256i s1 = _mm256_loadu_si256((__m256i *)(seq1Rev + inde)); \
            __m256i s2 = _mm256_loadu_si256((__m256i *)(seq2 + seq2Ind)); \
            __m256i cmp11 = _mm256_cmpeq_epi32(s1, s2); \
            __m256i sbt11 = (__m256i)_mm256_blendv_ps((__m256)w_mismatch_256, (__m256)w_match_256, (__m256)cmp11); \
            __m256i m11 = _mm256_add_epi32(h00, sbt11); \
            __m256i h11 = _mm256_max_epi32(minCutoff256, m11); \
            cmp11 = _mm256_cmpgt_epi32(e11, h11); \
            h11 = _mm256_max_epi32(h11, e11); \
            bt256 = _mm256_and_si256(ins256, cmp11); \
            cmp11 = _mm256_cmpgt_epi32(f11, h11); \
            h11 = _mm256_max_epi32(h11, f11); \
            bt256 = (__m256i)_mm256_blendv_ps((__m256)bt256, (__m256)del256, (__m256)cmp11); \
            bt256 = _mm256_or_si256(bt256, ext256); \
            _mm256_storeu_si256((__m256i *)(&H[hCurInd]), h11); \
            }



PairWiseSW::PairWiseSW(int32_t w_match, int32_t w_mismatch, int32_t w_open, int32_t w_extend)
{
    this->w_match = w_match;
    this->w_mismatch = w_mismatch;
    this->w_open = w_open;
    this->w_extend = w_extend;
    totalTicks = 0;
    E_  = (int32_t *)_mm_malloc((6 * (MAX_SEQ_LEN + SIMD_WIDTH)) * sizeof(int32_t), 64);
    backTrack_ = (int16_t *)_mm_malloc((2 * MAX_SEQ_LEN * MAX_SEQ_LEN + 2 * SIMD_WIDTH) * sizeof(int16_t), 64);
    cigarBuf_  = (int16_t *)_mm_malloc(4 * MAX_SEQ_LEN * sizeof(int16_t), 64);
#ifdef PERF_DEBUG
	mainLoopTicks = 0;
	mainLoopCount = 0;
    totalCount = 0;
    overflowCount = 0;
#endif
}

PairWiseSW::~PairWiseSW()
{
   free(E_);
   free(backTrack_);
   free(cigarBuf_);
}

#ifdef DEBUG
int32_t PairWiseSW::runSWOnePairBT(uint8_t *seq1, uint8_t *seq2, int32_t len1, int32_t len2, int8_t overhangStrategy, char *cigarArray, int16_t *cigarCount, int32_t *score)
#else
int32_t PairWiseSW::runSWOnePairBT(uint8_t *seq1, uint8_t *seq2, int32_t len1, int32_t len2, int8_t overhangStrategy, char *cigarArray, int16_t *cigarCount)
#endif
{
    SeqPair p;
    p.seq1 = seq1;
    p.seq2 = seq2;
    p.len1 = len1;
    p.len2 = len2;
    p.overhangStrategy = overhangStrategy;
    p.btrack = backTrack_;
    p.cigar = cigarArray;
    smithWatermanBackTrack(&p, 0);
    getCIGAR(&p, 0);
#ifdef DEBUG
    (*score) = p.score;
#endif
    (*cigarCount) = p.cigarCount;
    return p.alignmentOffset;
}


#if 0
void PairWiseSW::runSmithWaterman(SeqPair *pairArray, int32_t numPairs, int8_t bt, int32_t numThreads)
{
    int i;
    int64_t startTick, endTick;
#ifdef SEP_ANALYSIS
	VTResumeSampling();
#endif
    E_  = (int32_t *)_mm_malloc((6 * (MAX_SEQ_LEN + SIMD_WIDTH)) * numThreads * sizeof(int32_t), 64);
    int16_t *backTrack = (int16_t *)_mm_malloc((2 * MAX_SEQ_LEN * MAX_SEQ_LEN + 2 * SIMD_WIDTH) * numThreads * sizeof(int16_t), 64);
    int16_t *cigarArray = (int16_t *)_mm_malloc(4 * MAX_SEQ_LEN * numPairs * sizeof(int16_t), 64);
    startTick = __rdtsc();
    if(bt)
    {
#pragma omp parallel num_threads(numThreads)
        {
            int64_t st, et;
            st = __rdtsc();
            int32_t tid = omp_get_thread_num();
            int16_t *myBTrack = backTrack + tid * (2 * MAX_SEQ_LEN * MAX_SEQ_LEN + 2 * SIMD_WIDTH);
#pragma omp for
            for(i = 0; i < numPairs; i++)
            {
                pairArray[i].btrack = myBTrack;
                pairArray[i].cigar  = cigarArray + i * 4 * MAX_SEQ_LEN;
                smithWatermanBackTrack(pairArray + i, tid);
#ifdef PERF_DEBUG
                getBTStats(pairArray + i);
#endif
                getCIGAR(pairArray + i, tid);
            }
            et = __rdtsc();
            //printf("%d] %ld ticks\n", tid, et - st);
        }
    }
    else
    {
        smithWatermanOnePairWrapper(pairArray, numPairs);
    }
    endTick = __rdtsc();
#ifdef SEP_ANALYSIS
	VTPauseSampling();
#endif
    totalTicks = endTick - startTick;
}
#endif

#ifdef PERF_DEBUG
void PairWiseSW::getBTStats(SeqPair *p)
{
    int32_t nrow = p->len1;
    int32_t ncol = p->len2;
    int16_t *btrack = p->btrack;

    int32_t i, j;
    for(i = 0; i <= nrow; i++)
    {
        for(j = 0; j <= ncol; j++)
        {
            int32_t antiDiag = i + j;
            int btrInd;
            if(antiDiag <= nrow)
            {
                btrInd = antiDiag * MAX_SEQ_LEN + j - 1;
            }
            else
            {
                int32_t jLo = antiDiag - nrow - 1;
                btrInd = antiDiag * MAX_SEQ_LEN + j - jLo - 1;
            }
            int32_t btr = btrack[btrInd];

            if((btr < -127) || (btr > 127))
                overflowCount++;
            totalCount++;
        }
    }
}
#endif

void PairWiseSW::smithWatermanOnePair(SeqPair *p)
{
#if 0
    uint8_t *seq1 = p->seq1;
    uint8_t *seq2 = p->seq2;
    int32_t nrow = p->len1;
    int32_t ncol = p->len2;

    int32_t w_match = this->w_match;
    int32_t w_mismatch = this->w_mismatch;
    int32_t w_open = this->w_open;
    int32_t w_extend = this->w_extend;

    int32_t lowInitValue = LOW_INIT_VALUE;
    int i, j;
    for(i = 0; i <= ncol; i++)
    {
        F[i] = lowInitValue;
        H[i] = 0;
    }
    int32_t maxScore = 0;
    for(i = 1; i < nrow; i++)
    {
        int32_t eij = lowInitValue;
        int32_t hdiag = 0; 
        int32_t hij;
        for(j = 1; j <=ncol; j++)
        {
            eij = max(eij + w_extend, H[j - 1] + w_open);
            int32_t fij = F[j] = max(F[j] + w_extend, H[j] + w_open);
            int32_t mij = hdiag + ((seq1[i - 1] == seq2[j - 1]) ? w_match: w_mismatch);
            hdiag = H[j];
            hij = MATRIX_MIN_CUTOFF;
            if(eij > hij)
            {
                hij = eij;
            }
            if(F[j] > hij)
            {
                hij = F[j];
            }
            if(mij > hij)
            {
                hij = mij;
            }
            H[j] = hij;
        }
        if(maxScore < hij)
        {
            maxScore = hij;
        }
    }
    {
        int32_t eij = lowInitValue;
        int32_t hdiag = 0; 
        int32_t hij;
        for(j = 1; j <=ncol; j++)
        {
            eij = max(eij + w_extend, H[j - 1] + w_open);
            int32_t fij = F[j] = max(F[j] + w_extend, H[j] + w_open);
            int32_t mij = hdiag + ((seq1[i - 1] == seq2[j - 1]) ? w_match: w_mismatch);
            hdiag = H[j];
            hij = MATRIX_MIN_CUTOFF;
            if(eij > hij)
            {
                hij = eij;
            }
            if(F[j] > hij)
            {
                hij = F[j];
            }
            if(mij > hij)
            {
                hij = mij;
            }
            H[j] = hij;
            if(maxScore < hij)
            {
                maxScore = hij;
            }
        }
    }
    p->score = maxScore;
#endif
    return;
}

void PairWiseSW::smithWatermanOnePairWrapper(SeqPair *pairArray, int32_t numPairs)
{
    int32_t i;

    for(i = 0; i < numPairs; i++)
    {
        smithWatermanOnePair(pairArray + i);
    }
    return;
}


void PairWiseSW::smithWatermanBackTrack(SeqPair *p, int32_t tid)
{
    uint32_t seq1[MAX_SEQ_LEN];
    uint32_t seq1Rev[MAX_SEQ_LEN];
    uint32_t seq2[MAX_SEQ_LEN];
    int32_t nrow = p->len1;
    int32_t ncol = p->len2;
    int32_t overhangStrategy = p->overhangStrategy;

#if 1
    __m256i w_match_256 = _mm256_set1_epi32(this->w_match);
    __m256i w_mismatch_256 = _mm256_set1_epi32(this->w_mismatch);
    __m256i w_open_256 = _mm256_set1_epi32(this->w_open);
    __m256i w_extend_256 = _mm256_set1_epi32(this->w_extend);
#endif


    int32_t i, j;
    int32_t lowInitValue = LOW_INIT_VALUE;
    __m256i lowInitValue256 = _mm256_set1_epi32(lowInitValue);
    __m256i minCutoff256 = _mm256_set1_epi32(MATRIX_MIN_CUTOFF);
    __m256i zero256 = _mm256_setzero_si256();
    __m256i ins256 = _mm256_set1_epi32(INSERT);
    __m256i del256 = _mm256_set1_epi32(DELETE);
    __m256i ins_ext256 = _mm256_set1_epi32(INSERT_EXT);
    __m256i del_ext256 = _mm256_set1_epi32(DELETE_EXT);
    int32_t hwidth = MAX_SEQ_LEN + SIMD_WIDTH;
    int32_t ewidth = MAX_SEQ_LEN + SIMD_WIDTH;

    int32_t *E  = E_ + tid * 6 * (MAX_SEQ_LEN + SIMD_WIDTH);
    int32_t *F  = E + 1 * ewidth;
    int32_t *H  = E + 2 * ewidth;
    for(j = 0; j <= ncol; j+=SIMD_WIDTH)
    {
        _mm256_store_si256((__m256i *)(F + j), lowInitValue256);
    }
    for(i = 0; i <= nrow; i+=SIMD_WIDTH)
    {
        _mm256_store_si256((__m256i *)(E + i), lowInitValue256);
    }

    H[MAX_SEQ_LEN >> 1] = 0;

    for(i = 0; i < nrow; i++)
    {
        seq1[i] = p->seq1[i];
        seq1Rev[MAX_SEQ_LEN - 1 - i] = p->seq1[i];
    }
    for(i = 0; i < ncol; i++)
    {
        seq2[i] = p->seq2[i];
    }

    int16_t *backTrack = p->btrack;

    int32_t maxScore = INT32_MIN;
    int32_t max_i = 0;
    int32_t max_j = 0;

    int32_t antiDiag;
    int32_t prev, cur;
    for(antiDiag = 1; antiDiag <= (nrow + ncol); antiDiag++)
    {
        int32_t ilo = min(antiDiag, nrow + 1);
        int32_t jhi = min(antiDiag, ncol + 1);
        int32_t ihi = antiDiag - jhi;
        int32_t jlo = antiDiag - ilo;

        prev = ((antiDiag - 1) & 1) * hwidth;
        cur  = (antiDiag & 1) * hwidth;

#if 1
#ifdef PERF_DEBUG
        int64_t startTick, endTick;
        startTick = __rdtsc();
#endif
        for(j = (jlo + 1); j < (jhi - SIMD_WIDTH);)
        {
#ifdef PERF_DEBUG
            mainLoopCount+=2;
#endif
#ifdef IACA_ANALYSIS
            IACA_START
#endif
            int32_t backTrackInd = j - jlo - 1;
            __m256i bt256_0, bt256_1;
            {
            i = antiDiag - j;
            int32_t diag = j - i;
            int32_t diagInd = MAX_SEQ_LEN + diag;
            int32_t inde = MAX_SEQ_LEN - i;
            int32_t indf = j;
            int32_t hLeftInd = prev + ((diagInd - 1) >> 1);
            int32_t hTopInd  = hLeftInd + 1;
            int32_t hCurInd = cur + (diagInd >> 1);
            int32_t seq2Ind = j - 1;
            MAIN_CODE(bt256_0)
            j = j + SIMD_WIDTH;
            }
            {
            i = antiDiag - j;
            int32_t diag = j - i;
            int32_t diagInd = MAX_SEQ_LEN + diag;
            int32_t inde = MAX_SEQ_LEN - i;
            int32_t indf = j;
            int32_t hLeftInd = prev + ((diagInd - 1) >> 1);
            int32_t hTopInd  = hLeftInd + 1;
            int32_t hCurInd = cur + (diagInd >> 1);
            int32_t seq2Ind = j - 1;
            __m256i bt256;
            MAIN_CODE(bt256_1)
            j = j + SIMD_WIDTH;
            }
            __m256i bt256_2 = _mm256_permute2f128_si256(bt256_0, bt256_1, 0x20);
            __m256i bt256_3 = _mm256_permute2f128_si256(bt256_0, bt256_1, 0x31);
            __m256i bt256   = _mm256_packs_epi32(bt256_2, bt256_3);
            _mm256_stream_si256((__m256i *)(backTrack + antiDiag * MAX_SEQ_LEN + backTrackInd), bt256);
        }
#ifdef IACA_ANALYSIS
        IACA_END
#endif
        if(j < jhi)
        {
#ifdef PERF_DEBUG
            mainLoopCount++;
#endif
            i = antiDiag - j;
            int32_t diag = j - i;
            int32_t diagInd = MAX_SEQ_LEN + diag;
            int32_t inde = MAX_SEQ_LEN - i;
            int32_t indf = j;
            int32_t hLeftInd = prev + ((diagInd - 1) >> 1);
            int32_t hTopInd  = hLeftInd + 1;
            int32_t hCurInd = cur + (diagInd >> 1);
            int32_t seq2Ind = j - 1;
            __m256i bt256_0;
            __m256i bt256_1 = _mm256_setzero_si256();
            MAIN_CODE(bt256_0)
            __m256i bt256_2 = _mm256_permute2f128_si256(bt256_0, bt256_1, 0x20);
            __m256i bt256_3 = _mm256_permute2f128_si256(bt256_0, bt256_1, 0x31);
            __m256i bt256   = _mm256_packs_epi32(bt256_2, bt256_3);
            int32_t backTrackInd = j - jlo - 1;
            _mm256_stream_si256((__m256i *)(backTrack + antiDiag * MAX_SEQ_LEN + backTrackInd), bt256);
        }
#ifdef PERF_DEBUG
        endTick = __rdtsc();
        mainLoopTicks += endTick - startTick;
#endif
#endif
        if((overhangStrategy == INDEL) || (overhangStrategy == LEADING_INDEL))
        {
            H[cur + ((MAX_SEQ_LEN + 2 * jhi - antiDiag) >> 1)] = w_open + (jhi - 1) * w_extend;
            H[cur + ((MAX_SEQ_LEN + 2 * jlo - antiDiag) >> 1)] = w_open + (ilo - 1) * w_extend;
        }
        else
        {
            H[cur + ((MAX_SEQ_LEN + 2 * jhi - antiDiag) >> 1)] = 0;
            H[cur + ((MAX_SEQ_LEN + 2 * jlo - antiDiag) >> 1)] = 0;
        }
        F[jhi] = lowInitValue;
        E[MAX_SEQ_LEN - ilo] = lowInitValue;

#if 1
        if(ilo == (nrow + 1))
        {
            int32_t score = H[cur + ((MAX_SEQ_LEN + jlo + 1 - (ilo - 1)) >> 1)];
            if((overhangStrategy == SOFTCLIP) || (overhangStrategy == IGNORE))
            {
                if((maxScore < score) || ((maxScore == score) && (abs(ilo - jlo - 2) < abs(max_i - max_j))))
                {
                    //printf("%d, %d) hij = %d\n", i, j, h11);
                    maxScore = score;
                    max_i = ilo - 1;
                    max_j = jlo + 1;
                }
            }
        }
        if(jhi == (ncol + 1))
        {
            int32_t score = H[cur + ((MAX_SEQ_LEN + jhi - 1 - (ihi + 1)) >> 1)];
            if((maxScore < score) || ((maxScore == score) && ((max_j == ncol) || (abs(ihi - jhi + 2) <= abs(max_i - max_j)))))
            {
                //printf("%d, %d) hij = %d\n", i, j, h11);
                maxScore = score;
                max_i = ihi + 1;
                max_j = jhi - 1;
            }
        }
#endif
    }
    //printf("maxScore = %d, max_i = %d, max_j = %d\n", maxScore, max_i, max_j);
    if(overhangStrategy == INDEL)
        p->score = H[cur + ((MAX_SEQ_LEN + ncol - nrow) >> 1)];
    else
        p->score = maxScore;
    p->score = maxScore;
    p->max_i = max_i;
    p->max_j = max_j;
    return;
}

void PairWiseSW::getCIGAR(SeqPair *p, int32_t tid)
{
    int16_t *btrack = p->btrack;
    int32_t max_i = p->max_i;
    int32_t max_j = p->max_j;
    int32_t nrow = p->len1;
    int32_t ncol = p->len2;
    int32_t overhangStrategy = p->overhangStrategy;

    int32_t i, j;
    int16_t *cigarArray = cigarBuf_;

    int32_t cigarId = 0;
    
    if (overhangStrategy == INDEL)
    {
        i = nrow;
        j = ncol;
    }
    else if(overhangStrategy == LEADING_INDEL)
    {
        i = max_i;
        j = ncol;
    }
    else
    {
        i = max_i;
        j = max_j;
    }

    if(j < ncol)
    {
        cigarArray[cigarId * 2] = SOFTCLIP;
        cigarArray[cigarId * 2 + 1] = ncol - j;
        cigarId++;
    }
    int state = 0;
    while((i > 0) && (j > 0))
    {
        int32_t antiDiag = i + j;
        int32_t btrInd;
        if(antiDiag <= nrow)
        {
            btrInd = antiDiag * MAX_SEQ_LEN + j - 1;
        }
        else
        {
            int32_t jLo = antiDiag - nrow - 1;
            btrInd = antiDiag * MAX_SEQ_LEN + j - jLo - 1;
        }
        int32_t btr = btrack[btrInd];
        if(state == INSERT_EXT)
        {
            j--;
            cigarArray[cigarId * 2 - 1]++;
            state = btr & INSERT_EXT;
        }
        else if(state == DELETE_EXT)
        {
            i--;
            cigarArray[cigarId * 2 - 1]++;
            state = btr & DELETE_EXT;
        }
        else
        {
            switch(btr & 3)
            {
                case MATCH:
                    i--;
                    j--;
                    cigarArray[cigarId * 2] = MATCH;
                    cigarArray[cigarId * 2 + 1] = 1;
                    state = 0;
                    cigarId++;
                    break;
                case INSERT:
                    j--;
                    cigarArray[cigarId * 2] = INSERT;
                    cigarArray[cigarId * 2 + 1] = 1;
                    state = btr & INSERT_EXT;
                    cigarId++;
                    break;
                case DELETE:
                    i--;
                    cigarArray[cigarId * 2] = DELETE;
                    cigarArray[cigarId * 2 + 1] = 1;
                    state = btr & DELETE_EXT;
                    cigarId++;
                    break;
            }
        }
    }
    if(overhangStrategy == SOFTCLIP)
    {
        if(j > 0)
        {
            cigarArray[cigarId * 2] = SOFTCLIP;
            cigarArray[cigarId * 2 + 1] = j;
            cigarId++;
        }
        p->alignmentOffset = i;
    }
    else if(overhangStrategy == IGNORE)
    {
        if(j > 0)
        {
            cigarArray[cigarId * 2] = cigarArray[(cigarId - 1) * 2];
            cigarArray[cigarId * 2 + 1] = j;
            cigarId++;
        }
        p->alignmentOffset = i - j;
    }
    else // overhangStrategy == INDEL || overhangStrategy == LEADING_INDEL
    {
        if (i > 0)
        {
            cigarArray[cigarId * 2] = DELETE;
            cigarArray[cigarId * 2 + 1] = i;
            cigarId++;
        }
        else if(j > 0)
        {
            cigarArray[cigarId * 2] = INSERT;
            cigarArray[cigarId * 2 + 1] = j;
            cigarId++;
        }
        p->alignmentOffset = 0;
    }
    int32_t newId = 0;
    int32_t prev = cigarArray[newId * 2];
    //printf("cigarId = %d\n", cigarId);
    for(i = 1; i < cigarId; i++)
    {
        int32_t cur = cigarArray[i * 2];
        if(cur == prev)
        {
            cigarArray[newId * 2 + 1] += cigarArray[i * 2 + 1];
        }
        else
        {
            newId++;
            cigarArray[newId * 2] = cur;
            cigarArray[newId * 2 + 1] = cigarArray[i * 2 + 1];
            prev = cur;
        }
             
    }

    int curSize = 0;
    for(i = newId; i >= 0; i--)
    {
        int bytesWritten = sprintf(p->cigar + curSize, "%d", cigarArray[2 * i + 1]);
        curSize += bytesWritten;

        switch(cigarArray[2 * i])
        {
            case MATCH:
                sprintf(p->cigar + curSize, "M");
                break;
            case INSERT:
                sprintf(p->cigar + curSize, "I");
                break;
            case DELETE:
                sprintf(p->cigar + curSize, "D");
                break;
            case SOFTCLIP:
                sprintf(p->cigar + curSize, "S");
                break;
            default:
                sprintf(p->cigar + curSize, "R");
                break;
        }
        curSize++;
    }
    p->cigarCount = strlen(p->cigar);
}


int64_t PairWiseSW::getTicks()
{
#ifdef PERF_DEBUG
    printf("mainLoopCount = %ld, ticks = %ld, per iter = %lf\n", mainLoopCount, mainLoopTicks, mainLoopTicks * 1.0/mainLoopCount);
    printf("totalCount = %ld, overflowCount = %ld, frac = %lf\n", totalCount, overflowCount, overflowCount * 1.0 / totalCount);
#endif
    return totalTicks;
}
