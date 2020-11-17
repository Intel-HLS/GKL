#include<stdio.h>

#define MAIN_CODE(bt_vec) \
            { \
            VEC_INT_TYPE e10 = VEC_LOADU(&E[inde]); \
            VEC_INT_TYPE ext_score_h = VEC_ADD(e10, w_extend_vec); \
            VEC_INT_TYPE h10 = VEC_LOADU(&H[hLeftInd]); \
            VEC_INT_TYPE open_score_h = VEC_ADD(h10, w_open_vec); \
            VEC_INT_TYPE e11 = VEC_MAX(open_score_h, ext_score_h); \
            VEC_INT_TYPE open_gt_ext_h = VEC_CMPGT(open_score_h, ext_score_h); \
            VEC_INT_TYPE ext_vec = VEC_ANDNOT(open_gt_ext_h, ins_ext_vec); \
            VEC_STOREU(&E[inde], e11); \
            VEC_INT_TYPE f01 = VEC_LOADU(&F[indf]); \
            VEC_INT_TYPE f11; \
            VEC_INT_TYPE ext_score_v = VEC_ADD(f01, w_extend_vec); \
            VEC_INT_TYPE h01 = VEC_LOADU(&H[hTopInd]); \
            VEC_INT_TYPE open_score_v = VEC_ADD(h01, w_open_vec); \
            f11 = VEC_MAX(ext_score_v, open_score_v); \
            VEC_INT_TYPE open_gt_ext_v = VEC_CMPGT(open_score_v, ext_score_v); \
            ext_vec = VEC_OR(ext_vec, VEC_ANDNOT(open_gt_ext_v, del_ext_vec)); \
            VEC_STOREU((VEC_INT_TYPE *)(&F[indf]), f11); \
            VEC_INT_TYPE h00 = VEC_LOADU((VEC_INT_TYPE *)(&H[hCurInd])); \
            VEC_INT_TYPE s1 = VEC_LOADU((VEC_INT_TYPE *)(seq1Rev + inde)); \
            VEC_INT_TYPE s2 = VEC_LOADU((VEC_INT_TYPE *)(seq2 + seq2Ind)); \
            VEC_MASK_TYPE cmp11 = VEC_CMPEQ_MASK(s1, s2); \
            VEC_INT_TYPE sbt11 = VEC_BLEND(w_mismatch_vec, w_match_vec, cmp11); \
            VEC_INT_TYPE m11 = VEC_ADD(h00, sbt11); \
            VEC_INT_TYPE h11 = VEC_MAX(minCutoff_vec, m11); \
            VEC_INT_TYPE e11_gt_h11 = VEC_CMPGT(e11, h11); \
            h11 = VEC_MAX(h11, e11); \
            bt_vec = VEC_AND(ins_vec, e11_gt_h11); \
            cmp11 = VEC_CMPGT_MASK(f11, h11); \
            h11 = VEC_MAX(h11, f11); \
            bt_vec = VEC_BLEND(bt_vec, del_vec, cmp11); \
            bt_vec = VEC_OR(bt_vec, ext_vec); \
            VEC_STOREU((VEC_INT_TYPE *)(&H[hCurInd]), h11); \
            }


void inline smithWatermanBackTrack(SeqPair *p, int32_t match, int32_t mismatch, int32_t open, int32_t extend, int32_t* E_,int32_t tid)
{
    uint32_t seq1[MAX_SEQ_LEN];
    uint32_t seq1Rev[MAX_SEQ_LEN];
    uint32_t seq2[MAX_SEQ_LEN];
    int32_t nrow = p->len1;
    int32_t ncol = p->len2;
    int32_t overhangStrategy = p->overhangStrategy;

    VEC_INT_TYPE w_match_vec = VEC_SET1_VAL32(match);
    VEC_INT_TYPE w_mismatch_vec = VEC_SET1_VAL32(mismatch);
    VEC_INT_TYPE w_open_vec = VEC_SET1_VAL32(open);
    VEC_INT_TYPE w_extend_vec = VEC_SET1_VAL32(extend);

    int32_t i, j;
    int32_t lowInitValue = LOW_INIT_VALUE;
    VEC_INT_TYPE lowInitValue_vec = VEC_SET1_VAL32(lowInitValue);
    VEC_INT_TYPE minCutoff_vec = VEC_SET1_VAL32(MATRIX_MIN_CUTOFF);
    VEC_INT_TYPE zero_vec = VEC_SET_ZERO();
    VEC_INT_TYPE ins_vec = VEC_SET1_VAL32(INSERT);
    VEC_INT_TYPE del_vec = VEC_SET1_VAL32(DELETE);
    VEC_INT_TYPE ins_ext_vec = VEC_SET1_VAL32(INSERT_EXT);
    VEC_INT_TYPE del_ext_vec = VEC_SET1_VAL32(DELETE_EXT);
    INIT_CONSTANTS;
    int32_t hwidth = MAX_SEQ_LEN + AVX_LENGTH;
    int32_t ewidth = MAX_SEQ_LEN + AVX_LENGTH;

    int32_t *E  = E_ + tid * 6 * (MAX_SEQ_LEN + AVX_LENGTH);
    int32_t *F  = E + 1 * ewidth;
    int32_t *H  = E + 2 * ewidth;
    for(j = 0; j <= ncol; j+=AVX_LENGTH)
    {
        VEC_STORE(F + j, lowInitValue_vec);
    }
    for(i = 0; i <= nrow; i+=AVX_LENGTH)
    {
        VEC_STORE(E + i, lowInitValue_vec);
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
        for(j = (jlo + 1); j < (jhi - AVX_LENGTH);)
        {
#ifdef PERF_DEBUG
            mainLoopCount+=2;
#endif
#ifdef IACA_ANALYSIS
            IACA_START
#endif
            int32_t backTrackInd = j - jlo - 1;
            VEC_INT_TYPE bt_vec_0, bt_vec_1;
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
            MAIN_CODE(bt_vec_0)
            j = j + AVX_LENGTH;
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
            VEC_INT_TYPE bt_vec;
            MAIN_CODE(bt_vec_1)
            j = j + AVX_LENGTH;
            }
            VEC_INT_TYPE bt_vec_2 = VEC_PERMUTE2x128_EVEN(bt_vec_0, bt_vec_1);
            VEC_INT_TYPE bt_vec_3 = VEC_PERMUTE2x128_ODD(bt_vec_0, bt_vec_1);
            VEC_INT_TYPE bt_vec   = VEC_PACKS_32(bt_vec_2, bt_vec_3);
            VEC_STREAM(backTrack + antiDiag * MAX_SEQ_LEN + backTrackInd, bt_vec);
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
            VEC_INT_TYPE bt_vec_0;
            VEC_INT_TYPE bt_vec_1 = VEC_SET_ZERO();
            MAIN_CODE(bt_vec_0)
            VEC_INT_TYPE bt_vec_2 = VEC_PERMUTE2x128_EVEN(bt_vec_0, bt_vec_1);
            VEC_INT_TYPE bt_vec_3 = VEC_PERMUTE2x128_ODD(bt_vec_0, bt_vec_1);
            VEC_INT_TYPE bt_vec   = VEC_PACKS_32(bt_vec_2, bt_vec_3);
            int32_t backTrackInd = j - jlo - 1;
            VEC_STREAM(backTrack + antiDiag * MAX_SEQ_LEN + backTrackInd, bt_vec);
        }
#ifdef PERF_DEBUG
        endTick = __rdtsc();
        mainLoopTicks += endTick - startTick;
#endif
#endif
        if((overhangStrategy == INDEL) || (overhangStrategy == LEADING_INDEL))
        {
            H[cur + ((MAX_SEQ_LEN + 2 * jhi - antiDiag) >> 1)] = open + (jhi - 1) * extend;
            H[cur + ((MAX_SEQ_LEN + 2 * jlo - antiDiag) >> 1)] = open + (ilo - 1) * extend;
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

void inline getCIGAR(SeqPair *p, int16_t *cigarBuf_, int32_t tid)
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
    int maxSize =  sizeof(p->cigar);
    int curSize = 0;
    for(i = newId; i >= 0; i--)
    {
        char state;

        switch(cigarArray[2 * i])
        {
            case MATCH:
                state = 'M';
                break;
            case INSERT:
                state = 'I';
                break;
            case DELETE:
                state = 'D';
                break;
            case SOFTCLIP:
                state = 'S';
                break;
            default:
                state = 'R';
                break;
        }
	// 4 is u16 + Char + Null; look into how to handle the error state  
        if (curSize < (maxSize - 4)) 
		curSize += snprintf(p->cigar + curSize, 4, "%d%c", cigarArray[2 * i + 1], state);
    }
    p->cigarCount = strnlen(p->cigar, maxSize);
}


int32_t CONCAT(runSWOnePairBT_,SIMD_ENGINE)(int32_t match, int32_t mismatch, int32_t open, int32_t extend,uint8_t *seq1, uint8_t *seq2, int32_t len1, int32_t len2, int8_t overhangStrategy, char *cigarArray, int16_t *cigarCount)
{



    int32_t  w_match = match;
    int32_t  w_mismatch = mismatch;
    int32_t  w_open = open;
    int32_t  w_extend = extend;

    int32_t  *E_  = (int32_t *)_mm_malloc((6 * (MAX_SEQ_LEN+ AVX_LENGTH)) * sizeof(int32_t), 64);
    int16_t  *backTrack_ = (int16_t *)_mm_malloc((2 * MAX_SEQ_LEN * MAX_SEQ_LEN + 2 * AVX_LENGTH) * sizeof(int16_t), 64);
    int16_t  *cigarBuf_  = (int16_t *)_mm_malloc(4 * MAX_SEQ_LEN * sizeof(int16_t), 64);
   
    if (E_ == NULL  | backTrack_  == NULL | cigarBuf_ == NULL) {
         _mm_free(E_);
         _mm_free(backTrack_);
         _mm_free(cigarBuf_);
	 return -1; 
    } 

    SeqPair p;
    p.seq1 = seq1;
    p.seq2 = seq2;
    p.len1 = len1;
    p.len2 = len2;
    p.overhangStrategy = overhangStrategy;
    p.btrack = backTrack_;
    p.cigar = cigarArray;
    smithWatermanBackTrack(&p, match, mismatch,  open, extend, E_, 0);
    getCIGAR(&p, cigarBuf_, 0);

    (*cigarCount) = p.cigarCount;

    _mm_free(E_);
    _mm_free(backTrack_);
    _mm_free(cigarBuf_);
    return p.alignmentOffset;
    }
