#ifndef PAIRHMM_COMMON_H
#define PAIRHMM_COMMON_H

#if defined(_MSC_VER)
  #include <intrin.h> // SIMD intrinsics for Windows
#else
  #include <x86intrin.h> // SIMD intrinsics for GCC
#endif

#include <assert.h>
#include <stdint.h>

#define CAT(X,Y) X##Y
#define CONCAT(X,Y) CAT(X,Y)

#define MIN_ACCEPTED 1e-28f
#define NUM_DISTINCT_CHARS 5
#define AMBIG_CHAR 4

typedef struct {
  int rslen, haplen;
  const char *q, *i, *d, *c;
  const char *hap, *rs;
} testcase;

class ConvertChar {
  static uint8_t conversionTable[255] ;

 public:  
  static void init() {
    assert (NUM_DISTINCT_CHARS == 5) ;
    assert (AMBIG_CHAR == 4) ;
    
    conversionTable['A'] = 0 ;
    conversionTable['C'] = 1 ;
    conversionTable['T'] = 2 ;
    conversionTable['G'] = 3 ;
    conversionTable['N'] = 4 ;
  }
  
  static inline uint8_t get(uint8_t input) {
    return conversionTable[input] ;
  }
};

#endif // PAIRHMM_COMMON_H
