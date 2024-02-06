/*
#Copyright(C) 2023 Intel Corporation
#SPDX - License - Identifier : MIT License
*/

#include "pdhmm-serial.h"
#include "MathUtils.h"
#include "pdhmm-common.h"
#include <cstdio>

double qualToErrorProb(int8_t qual, const double *qualToErrorProbCache)
{
    return qualToErrorProbCache[(int)qual & 0xff]; // Map: 127 -> 127; -128 -> 128; -1 -> 255; etc.
}

double qualToProb(int8_t qual, const double *qualToErrorProbCache)
{
    return 1.0 - qualToErrorProb(qual, qualToErrorProbCache);
}

inline int32_t allocate(double *&matchMatrixVec, double *&insertionMatrixVec, double *&deletionMatrixVec, double *&branchMatchMatrixVec, double *&branchInsertionMatrixVec, double *&branchDeletionMatrixVec, double *&transitionVec, double *&priorVec, bool &constantsAreInitialized, bool &initialized, const int32_t maxReadLength, const int32_t maxHaplotypeLength)
{
    const int32_t paddedMaxReadLength = maxReadLength + 1;
    const int32_t paddedMaxHaplotypeLength = maxHaplotypeLength + 1;
    int32_t sizeOfTables = static_cast<int32_t>(paddedMaxHaplotypeLength * sizeof(double));

    matchMatrixVec = (double *)_mm_malloc(sizeOfTables, ALIGN_SIZE);
    insertionMatrixVec = (double *)_mm_malloc(sizeOfTables, ALIGN_SIZE);
    deletionMatrixVec = (double *)_mm_malloc(sizeOfTables, ALIGN_SIZE);
    branchMatchMatrixVec = (double *)_mm_malloc(sizeOfTables, ALIGN_SIZE);
    branchInsertionMatrixVec = (double *)_mm_malloc(sizeOfTables, ALIGN_SIZE);
    branchDeletionMatrixVec = (double *)_mm_malloc(sizeOfTables, ALIGN_SIZE);

    priorVec = (double *)_mm_malloc(paddedMaxHaplotypeLength * paddedMaxReadLength * sizeof(double), ALIGN_SIZE);

    transitionVec = (double *)_mm_malloc(TRANS_PROB_ARRAY_LENGTH * paddedMaxReadLength * sizeof(double), ALIGN_SIZE);

    if (matchMatrixVec == NULL || insertionMatrixVec == NULL || deletionMatrixVec == NULL || branchMatchMatrixVec == NULL || branchInsertionMatrixVec == NULL || branchDeletionMatrixVec == NULL || priorVec == NULL || transitionVec == NULL)
    {
        _mm_free(matchMatrixVec);
        _mm_free(insertionMatrixVec);
        _mm_free(deletionMatrixVec);
        _mm_free(branchMatchMatrixVec);
        _mm_free(branchInsertionMatrixVec);
        _mm_free(branchDeletionMatrixVec);
        _mm_free(priorVec);
        _mm_free(transitionVec);
        return PDHMM_MEMORY_ALLOCATION_FAILED;
    }

    for (int32_t i = 0; i < paddedMaxHaplotypeLength; i++)
    {
        matchMatrixVec[i] = 0;
        insertionMatrixVec[i] = 0;
        deletionMatrixVec[i] = 0;
        branchMatchMatrixVec[i] = 0;
        branchInsertionMatrixVec[i] = 0;
        branchDeletionMatrixVec[i] = 0;
    }

    for (int32_t i = 0; i < TRANS_PROB_ARRAY_LENGTH * paddedMaxReadLength; i++)
        transitionVec[i] = 0;

    for (int32_t i = 0; i < paddedMaxHaplotypeLength * paddedMaxReadLength; i++)
        priorVec[i] = 0;

    constantsAreInitialized = false;
    initialized = true;

    return PDHMM_SUCCESS;
}

inline void initialize(double *matchMatrixVec, double *insertionMatrixVec, double *deletionMatrixVec, double *branchMatchMatrixVec, double *branchInsertionMatrixVec, double *branchDeletionMatrixVec, bool &constantsAreInitialized, bool &initialized, const int32_t maxHaplotypeLength)
{
    const int32_t paddedMaxHaplotypeLength = maxHaplotypeLength + 1;

    for (int32_t i = 0; i < paddedMaxHaplotypeLength; i++)
    {
        matchMatrixVec[i] = 0;
        insertionMatrixVec[i] = 0;
        deletionMatrixVec[i] = 0;
        branchMatchMatrixVec[i] = 0;
        branchInsertionMatrixVec[i] = 0;
        branchDeletionMatrixVec[i] = 0;
    }

    constantsAreInitialized = false;
    initialized = true;
}

inline void freeTables(double *matchMatrixVec, double *insertionMatrixVec, double *deletionMatrixVec, double *branchMatchMatrixVec, double *branchInsertionMatrixVec, double *branchDeletionMatrixVec, double *transitionVec, double *priorVec)
{
    _mm_free(matchMatrixVec);
    _mm_free(insertionMatrixVec);
    _mm_free(deletionMatrixVec);
    _mm_free(branchMatchMatrixVec);
    _mm_free(branchInsertionMatrixVec);
    _mm_free(branchDeletionMatrixVec);
    _mm_free(transitionVec);
    _mm_free(priorVec);
}

double matchToMatchProbFun(int32_t insQual, int32_t delQual, const double *matchToMatchProb, int32_t &status)
{
    status = PDHMM_SUCCESS;
    int32_t minQual;
    int32_t maxQual;
    if (insQual <= delQual)
    {
        minQual = insQual;
        maxQual = delQual;
    }
    else
    {
        minQual = delQual;
        maxQual = insQual;
    }

    if (minQual < 0)
    {
        status = PDHMM_INPUT_DATA_ERROR;
        DBG("quality cannot be negative: %d and %d\n", minQual, maxQual);
    }
    return (MAX_QUAL < maxQual) ? 1.0 - pow(10, approximateLog10SumLog10(-0.1 * minQual, -0.1 * maxQual)) : matchToMatchProb[((maxQual * (maxQual + 1)) >> 1) + minQual];
}

int32_t qualToTransProbs(double *dest, int8_t insQual, int8_t delQual, int8_t gcp, const double *matchToMatchProb, const double *qualToErrorProbCache)
{
    int32_t status = PDHMM_SUCCESS;
    if (insQual < 0)
    {
        DBG("insert quality cannot less than 0: %d\n", insQual);
        return PDHMM_INPUT_DATA_ERROR;
    }
    if (delQual < 0)
    {
        DBG("deletion quality cannot be less than 0: %d\n", delQual);
        return PDHMM_INPUT_DATA_ERROR;
    }
    if (gcp < 0)
    {
        DBG("gcp cannot be less than 0: %d\n", gcp);
        return PDHMM_INPUT_DATA_ERROR;
    }

    dest[matchToMatch] = matchToMatchProbFun(insQual & 0xFF, delQual & 0xFF, matchToMatchProb, status);
    dest[matchToInsertion] = qualToErrorProb(insQual, qualToErrorProbCache);
    dest[matchToDeletion] = qualToErrorProb(delQual, qualToErrorProbCache);
    dest[indelToMatch] = qualToProb(gcp, qualToErrorProbCache);
    dest[insertionToInsertion] = dest[deletionToDeletion] = qualToErrorProb(gcp, qualToErrorProbCache);
    return status;
}

int32_t qualToTransProbs(double *dest, const int8_t *insQuals, const int8_t *delQuals, const int8_t *gcps, int32_t read_bases_length, const double *matchToMatchProb, const double *qualToErrorProbCache)
{
    int32_t readLength = read_bases_length;
    int32_t index = 0;

    for (int32_t i = 0; i < readLength; i++)
    {
        index = (i + 1) * TRANS_PROB_ARRAY_LENGTH;
        int32_t currStatus = qualToTransProbs(dest + index, insQuals[i], delQuals[i], gcps[i], matchToMatchProb, qualToErrorProbCache);
        if (currStatus != PDHMM_SUCCESS)
            return currStatus;
    }
    return PDHMM_SUCCESS;
}

int32_t initializeProbabilities(double *dest, const int8_t *insertionGOP, const int8_t *deletionGOP, const int8_t *overallGCP, int32_t read_bases_length, const double *matchToMatchProb, const double *qualToErrorProbCache)
{
    return qualToTransProbs(dest, insertionGOP, deletionGOP, overallGCP, read_bases_length, matchToMatchProb, qualToErrorProbCache);
}

bool isBasePDMatching(int8_t x, int8_t hapPDBases, int32_t &status)
{
    if ((hapPDBases & PartiallyDeterminedHaplotype::SNP) != 0)
    {
        switch (x)
        {
        case 'A':
        case 'a':
            return ((hapPDBases & PartiallyDeterminedHaplotype::A) != 0);
        case 'C':
        case 'c':
            return ((hapPDBases & PartiallyDeterminedHaplotype::C) != 0);
        case 'T':
        case 't':
            return ((hapPDBases & PartiallyDeterminedHaplotype::T) != 0);
        case 'G':
        case 'g':
            return ((hapPDBases & PartiallyDeterminedHaplotype::G) != 0);
        default:
            status = PDHMM_INPUT_DATA_ERROR;
            DBG("Found unexpected base in alt alleles");
        }
    }
    return false;
}

int32_t initializePriors(const int8_t *haplotypeBases, const int8_t *haplotypePDBases, const int8_t *readBases, const int8_t *readQuals, int32_t startIndex, int32_t hap_bases_length, int32_t read_bases_length, double *prior, const double *qualToErrorProbCache, int32_t maxHaplotypeLength)
{
    int32_t paddedMaxHaplotypeLength = maxHaplotypeLength + 1;
    // initialize the prior matrix for all combinations of read x haplotype bases
    // Abusing the fact that java initializes arrays with 0.0, so no need to fill in rows and columns below 2.
    for (int32_t i = 0; i < read_bases_length; i++)
    {
        int8_t x = readBases[i];
        int8_t qual = readQuals[i];
        double *currPrior = prior + ((i + 1) * paddedMaxHaplotypeLength);
        for (int32_t j = startIndex; j < hap_bases_length; j++)
        {
            int8_t y = haplotypeBases[j];
            int8_t hapPDBase = haplotypePDBases[j];
            int32_t currStatus = PDHMM_SUCCESS;
            currPrior[j + 1] = (x == y || x == (int8_t)'N' || y == (int8_t)'N' || isBasePDMatching(x, hapPDBase, currStatus) ? qualToProb(qual, qualToErrorProbCache) : (qualToErrorProb(qual, qualToErrorProbCache) / (3.0)));
            if (currStatus != PDHMM_SUCCESS)
            {
                return currStatus;
            }
        }
    }
    return PDHMM_SUCCESS;
}

double subComputeReadLikelihoodGivenHaplotypeLog10(const int8_t *hap_bases, const int8_t *hap_pdbases, const int8_t *read_bases, const int8_t *read_qual, const int8_t *read_ins_qual, const int8_t *read_del_qual, const int8_t *gcp, int32_t hap_bases_length, int32_t read_bases_length, int32_t hapStartIndex, bool recacheReadValues, int32_t prev_hap_bases_length, int32_t paddedHaplotypeLength, int32_t paddedReadLength, double *matchMatrix, double *insertionMatrix, double *deletionMatrix, double *branchMatchMatrix, double *branchInsertionMatrix, double *branchDeletionMatrix, bool &constantsAreInitialized, double *transition, double *prior, const double *matchToMatchProb, const double *qualToErrorProbCache, int32_t &status, int32_t maxHaplotypeLength)
{
    status = PDHMM_SUCCESS;
    if (prev_hap_bases_length == -1 || prev_hap_bases_length != hap_bases_length)
    {
        double initialValue = INITIAL_CONDITION / hap_bases_length;
        // set the initial value (free deletions in the beginning) for the first row
        // in the deletion matrix
        for (int32_t j = 0; j < paddedHaplotypeLength; j++)
        {
            deletionMatrix[j] = initialValue;
        }
    }

    if (!constantsAreInitialized || recacheReadValues)
    {
        int32_t currStatus = initializeProbabilities(transition, read_ins_qual, read_del_qual, gcp, read_bases_length, matchToMatchProb, qualToErrorProbCache);
        if (currStatus != PDHMM_SUCCESS)
            status = currStatus;
        // note that we initialized the constants
        constantsAreInitialized = true;
    }

    int32_t currStatus = initializePriors(hap_bases, hap_pdbases, read_bases, read_qual, hapStartIndex, hap_bases_length, read_bases_length, prior, qualToErrorProbCache, maxHaplotypeLength);
    if (currStatus != PDHMM_SUCCESS)
        status = currStatus;

    enum HMMState currentState = NORMAL;
    double bmmTop, bmmDiag, bmmLeft, bimTop, bimDiag, bimLeft, bdmTop, bdmDiag, bdmLeft, mmTop, mmDiag, mmLeft, imTop, imDiag, imLeft, dmTop, dmDiag, dmLeft;
    double *currPrior, *currTransition;

    int32_t paddedMaxHaplotypeLength = maxHaplotypeLength + 1;
    for (int32_t i = 1; i < paddedReadLength; i++)
    {
        bmmLeft = bmmDiag = bimLeft = bimDiag = bdmLeft = bdmDiag = mmLeft = mmDiag = imLeft = imDiag = dmLeft = dmDiag = 0;

        if (i == 1)
            dmDiag = deletionMatrix[hapStartIndex];

        currPrior = prior + (i * paddedMaxHaplotypeLength);
        currTransition = transition + (i * TRANS_PROB_ARRAY_LENGTH);

        for (int32_t j = hapStartIndex + 1; j < paddedHaplotypeLength; j++)
        {
            bmmTop = branchMatchMatrix[j];
            bimTop = branchInsertionMatrix[j];
            bdmTop = branchDeletionMatrix[j];
            mmTop = matchMatrix[j];
            imTop = insertionMatrix[j];
            dmTop = deletionMatrix[j];

            // the following nested ifs are not replaced with switch case for performance reasons. Using switch case increases the number of instructions in the assembly code.
            if (currentState == NORMAL)
            {
                branchMatchMatrix[j] = mmLeft;
                branchDeletionMatrix[j] = dmLeft;
                branchInsertionMatrix[j] = imLeft;
            }
            else if (currentState == INSIDE_DEL)
            {
                branchMatchMatrix[j] = bmmLeft;
                branchDeletionMatrix[j] = bdmLeft;
                branchInsertionMatrix[j] = bimLeft;
            }
            else if (currentState == AFTER_DEL)
            {
                branchMatchMatrix[j] = std::max(bmmLeft, mmLeft);
                branchDeletionMatrix[j] = std::max(bdmLeft, dmLeft);
                branchInsertionMatrix[j] = std::max(bimLeft, imLeft);
                mmDiag = std::max(mmDiag, bmmDiag);
                imDiag = std::max(imDiag, bimDiag);
                dmDiag = std::max(dmDiag, bdmDiag);
                mmLeft = std::max(mmLeft, bmmLeft);
                dmLeft = std::max(dmLeft, bdmLeft);
            }

            matchMatrix[j] = currPrior[j] * (mmDiag * currTransition[matchToMatch] +
                                             imDiag * currTransition[indelToMatch] +
                                             dmDiag * currTransition[indelToMatch]);
            deletionMatrix[j] = mmLeft * currTransition[matchToDeletion] + dmLeft * currTransition[deletionToDeletion];

            // Special case since we MIGHT be at the deletion end and have to be handling jump states from above
            if ((hap_pdbases[j - OFF] & PartiallyDeterminedHaplotype::DEL_END) == PartiallyDeterminedHaplotype::DEL_END)
            {
                insertionMatrix[j] = std::max(bmmTop, mmTop) * currTransition[matchToInsertion] + std::max(bimTop, imTop) * currTransition[insertionToInsertion];
            }
            else
            {
                insertionMatrix[j] = mmTop * currTransition[matchToInsertion] + imTop * currTransition[insertionToInsertion];
            }

            if (currentState == AFTER_DEL)
            {
                currentState = NORMAL;
            }

            // If we are at a deletion start base, start copying the branch states
            if ((hap_pdbases[j - 1] & PartiallyDeterminedHaplotype::DEL_START) == PartiallyDeterminedHaplotype::DEL_START)
            {
                currentState = INSIDE_DEL;
            }
            // Being after a deletion overrides being inside a deletion by virtue of
            // the fact that we allow single element deletions
            if ((hap_pdbases[j - 1] & PartiallyDeterminedHaplotype::DEL_END) == PartiallyDeterminedHaplotype::DEL_END)
            {
                currentState = AFTER_DEL;
            }

            bmmDiag = bmmTop;
            bimDiag = bimTop;
            bdmDiag = bdmTop;
            mmDiag = mmTop;
            imDiag = imTop;
            dmDiag = dmTop;
            bmmLeft = branchMatchMatrix[j];
            bimLeft = branchInsertionMatrix[j];
            bdmLeft = branchDeletionMatrix[j];
            mmLeft = matchMatrix[j];
            imLeft = insertionMatrix[j];
            dmLeft = deletionMatrix[j];
        }
    }

    // final log probability is the log10 sum of the last element in the Match and
    // Insertion state arrays this way we ignore all paths that ended in
    // deletions! (huge) but we have to sum all the paths ending in the M and I
    // matrices, because they're no longer extended.
    double finalSumProbabilities = 0.0;
    for (int32_t j = 1; j < paddedHaplotypeLength; j++)
    {
        finalSumProbabilities += matchMatrix[j] + insertionMatrix[j];
    }
    return log10(finalSumProbabilities) - INITIAL_CONDITION_LOG10;
}

double computeReadLikelihoodGivenHaplotypeLog10(const int8_t *hap_bases, const int8_t *hap_pdbases, const int8_t *read_bases, const int8_t *read_qual, const int8_t *read_ins_qual, const int8_t *read_del_qual, const int8_t *gcp, int64_t hap_bases_length, int64_t read_bases_length, int32_t &prev_hap_bases_length, double *matchMatrix, double *insertionMatrix, double *deletionMatrix, double *branchMatchMatrix, double *branchInsertionMatrix, double *branchDeletionMatrix, bool &constantsAreInitialized, double *transition, double *prior, const double *matchToMatchProb, const double *qualToErrorProbCache, int32_t &status, int32_t maxHaplotypeLength)
{
    status = PDHMM_SUCCESS;
    bool recacheReadValues = false;
    int32_t hapStartIndex = 0;

    int32_t paddedReadLength = (int32_t)read_bases_length + 1;
    int32_t paddedHaplotypeLength = (int32_t)hap_bases_length + 1;

    // The following ternary operator always executes the false case similar to PDHMM implementation in GATK.
    // We have intentionally kept this instruction to ensure that any performance gains achieved in PDHMM GKL are not merely due to the absence
    // of this ternary operator but are a result of our optimizations.
    hapStartIndex = (recacheReadValues) ? 0 : hapStartIndex;

    // Pre-compute the difference between the current haplotype and the next one to be run
    // Looking ahead is necessary for the ArrayLoglessPairHMM implementation
    // TODO this optimization is very dangerous if we have undetermined haps that could have the same bases but mean different things
    // final int nextHapStartIndex =  (nextHaplotypeBases == null || haplotypeBases.length != nextHaplotypeBases.length) ? 0 : findFirstPositionWhereHaplotypesDiffer(haplotypeBases, haplotypePDBases, nextHaplotypeBases, nextHaplotypePDBases);
    int32_t nextHapStartIndex = 0; // disable the optimization for now until it's confirmed to be correct

    double result = subComputeReadLikelihoodGivenHaplotypeLog10(hap_bases, hap_pdbases, read_bases, read_qual, read_ins_qual, read_del_qual, gcp, (int32_t)hap_bases_length, (int32_t)read_bases_length, hapStartIndex, recacheReadValues, prev_hap_bases_length, paddedHaplotypeLength, paddedReadLength, matchMatrix, insertionMatrix, deletionMatrix, branchMatchMatrix, branchInsertionMatrix, branchDeletionMatrix, constantsAreInitialized, transition, prior, matchToMatchProb, qualToErrorProbCache, status, maxHaplotypeLength);

    if (result > 0.0)
    {
        status = PDHMM_FAILURE;
        DBG("PairHMM Log Probability cannot be greater than 0.\n");
    }

    if (!isValidLog10Probability(result))
    {
        status = PDHMM_FAILURE;
        DBG("Invalid Log Probability: %lf \n", result);
    }

    // Warning: This assumes no downstream modification of the haplotype bases (saves us from copying the array). It is okay for the haplotype caller.
    prev_hap_bases_length = (int32_t)hap_bases_length;

    // For the next iteration, the hapStartIndex for the next haplotype becomes the index for the current haplotype
    // The array implementation has to look ahead to the next haplotype to store caching info. It cannot do this if nextHapStart is before hapStart
    // The following ternary operator always executes the false case similar to PDHMM implementation in GATK.
    // We have intentionally kept this instruction to ensure that any performance gains achieved in PDHMM GKL are not merely due to the absence
    // of this ternary operator but are a result of our optimizations.
    hapStartIndex = (nextHapStartIndex < hapStartIndex) ? 0 : nextHapStartIndex;

    return result;
}

int32_t computePDHMM_serial(const int8_t *hap_bases, const int8_t *hap_pdbases, const int8_t *read_bases, const int8_t *read_qual, const int8_t *read_ins_qual, const int8_t *read_del_qual, const int8_t *gcp, double *result, int64_t testcase, const int64_t *hap_lengths, const int64_t *read_lengths, int32_t maxReadLength, int32_t maxHaplotypeLength)
{
    int32_t status = PDHMM_SUCCESS;
    double *matchToMatchLog10, *matchToMatchProb, *qualToErrorProbCache, *qualToProbLog10Cache;
    status = init(matchToMatchLog10, matchToMatchProb, qualToErrorProbCache, qualToProbLog10Cache);
    if (status != PDHMM_SUCCESS)
        return status;

    double *matchMatrix, *insertionMatrix, *deletionMatrix, *branchMatchMatrix, *branchInsertionMatrix, *branchDeletionMatrix, *transition, *prior;
    bool constantsAreInitialized = false, initialized = true;
    int32_t prev_hap_bases_length = -1;

    allocate(matchMatrix, insertionMatrix, deletionMatrix, branchMatchMatrix, branchInsertionMatrix, branchDeletionMatrix, transition, prior, constantsAreInitialized, initialized, maxReadLength, maxHaplotypeLength);

    for (int32_t i = 0; i < testcase; i++)
    {
        initialize(matchMatrix, insertionMatrix, deletionMatrix, branchMatchMatrix, branchInsertionMatrix, branchDeletionMatrix, constantsAreInitialized, initialized, maxHaplotypeLength);
        prev_hap_bases_length = -1;
        int32_t hapIndexOffset = i * maxHaplotypeLength;
        int32_t readIndexOffset = i * maxReadLength;
        int32_t currStatus = PDHMM_SUCCESS;

        result[i] = computeReadLikelihoodGivenHaplotypeLog10(hap_bases + hapIndexOffset, hap_pdbases + hapIndexOffset, read_bases + readIndexOffset, read_qual + readIndexOffset, read_ins_qual + readIndexOffset, read_del_qual + readIndexOffset, gcp + readIndexOffset, hap_lengths[i], read_lengths[i], prev_hap_bases_length, matchMatrix, insertionMatrix, deletionMatrix, branchMatchMatrix, branchInsertionMatrix, branchDeletionMatrix, constantsAreInitialized, transition, prior, matchToMatchProb, qualToErrorProbCache, currStatus, maxHaplotypeLength);

        if (currStatus != PDHMM_SUCCESS)
            status = currStatus;
    }
    freeTables(matchMatrix, insertionMatrix, deletionMatrix, branchMatchMatrix, branchInsertionMatrix, branchDeletionMatrix, transition, prior);
    freeInit(matchToMatchLog10, matchToMatchProb, qualToErrorProbCache, qualToProbLog10Cache);
    return status;
}

int32_t computePDHMM_serial(const double *matchToMatchProb, const double *qualToErrorProbCache, double *matchMatrix, double *insertionMatrix, double *deletionMatrix, double *branchMatchMatrix, double *branchInsertionMatrix, double *branchDeletionMatrix, double *transition, double *prior, const int8_t *hap_bases, const int8_t *hap_pdbases, const int8_t *read_bases, const int8_t *read_qual, const int8_t *read_ins_qual, const int8_t *read_del_qual, const int8_t *gcp, double *result, int64_t testcase, const int64_t *hap_lengths, const int64_t *read_lengths, int32_t maxReadLength, int32_t maxHaplotypeLength)
{
    int32_t status = PDHMM_SUCCESS;

    bool constantsAreInitialized = false, initialized = true;

    int32_t prev_hap_bases_length = -1;

    for (int32_t i = 0; i < testcase; i++)
    {
        initialize(matchMatrix, insertionMatrix, deletionMatrix, branchMatchMatrix, branchInsertionMatrix, branchDeletionMatrix, constantsAreInitialized, initialized, maxHaplotypeLength);
        prev_hap_bases_length = -1;
        int32_t hapIndexOffset = i * maxHaplotypeLength;
        int32_t readIndexOffset = i * maxReadLength;
        int32_t currStatus = PDHMM_SUCCESS;

        result[i] = computeReadLikelihoodGivenHaplotypeLog10(hap_bases + hapIndexOffset, hap_pdbases + hapIndexOffset, read_bases + readIndexOffset, read_qual + readIndexOffset, read_ins_qual + readIndexOffset, read_del_qual + readIndexOffset, gcp + readIndexOffset, hap_lengths[i], read_lengths[i], prev_hap_bases_length, matchMatrix, insertionMatrix, deletionMatrix, branchMatchMatrix, branchInsertionMatrix, branchDeletionMatrix, constantsAreInitialized, transition, prior, matchToMatchProb, qualToErrorProbCache, currStatus, maxHaplotypeLength);

        if (currStatus != PDHMM_SUCCESS)
            status = currStatus;
    }
    return status;
}
