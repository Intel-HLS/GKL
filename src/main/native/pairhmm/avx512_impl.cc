#include "avx512_impl.h"

#ifndef __APPLE__

#include "avx512-pairhmm.h"

float (*compute_fp_avx512s)(testcase*) = &compute_full_prob_avx512s;
double (*compute_fp_avx512d)(testcase*) = &compute_full_prob_avx512d;

#else

float (*compute_fp_avx512s)(testcase*) = NULL;
double (*compute_fp_avx512d)(testcase*) = NULL;

#endif
