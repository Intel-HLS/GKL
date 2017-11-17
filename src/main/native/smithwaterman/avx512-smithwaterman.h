#include <stdint.h>

#undef SIMD_ENGINE
#define SIMD_ENGINE avx512

#include "avx512-functions.h"
#include "PairWiseSW.h"