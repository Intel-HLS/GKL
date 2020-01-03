#include "neon_impl.h"
#include "neon-pairhmm.h"

float (*compute_fp_neons)(testcase*) = &compute_full_prob_neons;
double (*compute_fp_neond)(testcase*) = &compute_full_prob_neond;
