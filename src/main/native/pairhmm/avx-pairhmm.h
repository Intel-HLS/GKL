#include <stdint.h>
#include "pairhmm_common.h"
#include "Context.h"

#define SIMD_ENGINE avx

#include "avx-types.h"

#include "avx-functions-float.h"
#include "avx-vector-shift.h"
#include "avx-pairhmm-template.h"

#include "avx-functions-double.h"
#include "avx-vector-shift.h"
#include "avx-pairhmm-template.h"

template double compute_full_prob_avxd<double>(testcase* tc);
template float compute_full_prob_avxs<float>(testcase* tc);
