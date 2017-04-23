#ifndef SHACC_PAIRHMM_H
#define SHACC_PAIRHMM_H

#ifdef __APPLE__
#define WEAK __attribute__((weak_import))
#else
#define WEAK __attribute__((weak))
#endif

namespace shacc_pairhmm {

  struct Read {
    int length;
    const char* bases;
    const char* q;
    const char* i;
    const char* d;
    const char* c;
  };
  
  struct Haplotype {
    int length;
    const char* bases;
  };
  
  struct Batch {
    int num_reads;
    int num_haps;
    long num_cells;
    Read* reads;
    Haplotype* haps;
    float* results;
  };

  extern WEAK bool calculate(Batch& batch);
}

#endif
