#ifdef __cplusplus
extern "C" {
#endif

void initNative();
void align(char* ref, char* alt, char*cigar, int match, int mismatch, int open, int extend, int strategy, int *score, int *offset);


#ifdef __cplusplus
}
#endif