#include <stdint.h>
#include "pairhmm_common.h"
#include "Context.h"

#include "avx-types.h"

#undef SIMD_ENGINE
#define SIMD_ENGINE avx512

#include "avx512-functions-float.h"
#include "avx512-vector-shift.h"
#include "avx-pairhmm-template.h"

#include "avx512-functions-double.h"
#include "avx512-vector-shift.h"
#include "avx-pairhmm-template.h"
