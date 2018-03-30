#include<stdio.h>
#include"SmithWatermanUnitTest.h"

#define DEFAULT_MATCH 1
#define DEFAULT_MISMATCH -1
#define DEFAULT_OPEN 1
#define DEFAULT_EXTEND 1

int32_t w_match, w_mismatch, w_open, w_extend;
int8_t bt = 0;
char *pairFileName;
int64_t SW_cells = 0;

void parseCmdLine(int argc, char *argv[])
{
    int i;
    w_match = DEFAULT_MATCH;
    w_mismatch = DEFAULT_MISMATCH;
    w_open = DEFAULT_OPEN;
    w_extend = DEFAULT_EXTEND;

    int pairFlag = 0;
    for(i = 1; i < argc; i+=2)
    {
        if(strcmp(argv[i], "-match") == 0)
        {
            w_match = atoi(argv[i + 1]);
        }
        if(strcmp(argv[i], "-mismatch") == 0)
        {
            w_mismatch = atoi(argv[i + 1]);
        }
        if(strcmp(argv[i], "-gapo") == 0)
        {
            w_open = atoi(argv[i + 1]);
        }
        if(strcmp(argv[i], "-gape") == 0)
        {
            w_extend = atoi(argv[i + 1]);
        }
        if(strcmp(argv[i], "-pairs") == 0)
        {
            pairFileName = argv[i + 1];
            pairFlag = 1;
        }
        if(strcmp(argv[i], "-bt") == 0)
        {
            bt = atoi(argv[i + 1]);
        }
    }
    if(pairFlag == 0)
    {
        printf("ERROR! pairFileName not specified.\n");
        exit(0);
    }
}

int loadPairs(SeqPair *seqPairArray)
 {
     FILE *pairFile = fopen(pairFileName, "r");

     if(pairFile == NULL)
     {
         fprintf(stderr, "Could not open file: %s\n", pairFileName);
         exit(0);
     }

     uint8_t *seqBuf = (uint8_t *)_mm_malloc(MAX_SEQ_LEN * 2 * MAX_NUM_PAIRS * sizeof(int8_t), 64);
     char overhangStrategy[20];

     int32_t numPairs = 0;
     while(numPairs < MAX_NUM_PAIRS)
     {
         if(!fgets((char *)(seqBuf + numPairs * 2 * MAX_SEQ_LEN), MAX_SEQ_LEN, pairFile))
         {
             break;
         }
         if(!fgets((char *)(seqBuf + (numPairs * 2 + 1) * MAX_SEQ_LEN), MAX_SEQ_LEN, pairFile))
         {
             printf("ERROR! Odd number of sequences in %s\n", pairFileName);
             break;
         }
         if(!fgets(overhangStrategy, 20, pairFile))
         {
             printf("ERROR! Overhang Strategy value of the last sequence is not available %s\n", pairFileName);
             break;
         }

         SeqPair sp;
        // sp.id = numPairs;
         sp.seq1 = seqBuf + numPairs * 2 * MAX_SEQ_LEN;
         sp.seq2 = seqBuf + (numPairs * 2 + 1) * MAX_SEQ_LEN;
         sp.len1 = strnlen((char *)(seqBuf + numPairs * 2 * MAX_SEQ_LEN), MAX_SEQ_LEN) - 1;
         sp.len2 = strnlen((char *)(seqBuf + (numPairs * 2 + 1) * MAX_SEQ_LEN), MAX_SEQ_LEN) - 1;

         if(strncmp(overhangStrategy, "SOFTCLIP", 8) == 0)
         {
             sp.overhangStrategy = SOFTCLIP;
         }
         else if(strncmp(overhangStrategy, "INDEL", 5) == 0)
         {
             sp.overhangStrategy = INDEL;
         }
         else if(strncmp(overhangStrategy, "LEADING_INDEL", 13) == 0)
         {
             sp.overhangStrategy = LEADING_INDEL;
         }
         else if(strncmp(overhangStrategy, "IGNORE", 8) == 0)
         {
             sp.overhangStrategy = IGNORE;
         }
         else
         {
             sp.overhangStrategy = IGNORE;
         }
         //printf("len1 = %d, len2 = %d\n", sp.len1, sp.len2);
         //printf("%s\n", sp.seq1);
         sp.score = 0;
         seqPairArray[numPairs] = sp;
         numPairs++;
         SW_cells += (sp.len1 * sp.len2);
     }
     if(numPairs == MAX_NUM_PAIRS)
     {
         printf("Reached max limit of number of pairs that can be processed."
                 "\nPotentially there are few more pairs left to be processed\n");
     }
     fclose(pairFile);
     return numPairs;
 }


int main(int argc, char *argv[])
{

    parseCmdLine(argc, argv);
    SeqPair *seqPairArray = (SeqPair *)_mm_malloc(MAX_NUM_PAIRS * sizeof(SeqPair), 64);
    int32_t numPairs = loadPairs(seqPairArray);
    char *cigarArray = (char *)_mm_malloc(4 * MAX_SEQ_LEN * numPairs * sizeof(char), 64);
    int i;
    initNative();

    for(i = 0; i < numPairs; i++)
    {
        char *myCigarArray = cigarArray + i * 4 * MAX_SEQ_LEN;
        SeqPair *p = seqPairArray + i;
        p->cigar = myCigarArray;

        align(p->seq1, p->seq2, p->cigar, w_match, w_mismatch, w_open, w_extend, p->overhangStrategy, &(p->score), &(p->alignmentOffset));

    }
#if 1
    if(bt)
    {
        for(i = 0; i < numPairs; i++)
        {
            printf("%s", seqPairArray[i].cigar);
#if 0
            int32_t cigarCount = seqPairArray[i].cigarCount;
            int16_t *cigarArray = seqPairArray[i].cigar;
            int j;
            for(j = cigarCount; j >= 0; j--)
            {
                printf("%d", cigarArray[2 * j + 1]);
                switch(cigarArray[2 * j])
                {
                    case MATCH:
                        printf("M");
                        break;
                    case INSERT:
                        printf("I");
                        break;
                    case DELETE:
                        printf("D");
                        break;
                    case SOFTCLIP:
                        printf("S");
                        break;
                    default:
                        printf("R");
                        break;
                }
            }
#endif
            printf(" %d\n", seqPairArray[i].alignmentOffset);
        }
    }
#endif
}
