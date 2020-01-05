#include <stdint.h>
#include "pairhmm_common.h"
#include "Context.h"

#include "neon-types.h"

#undef SIMD_ENGINE
#define SIMD_ENGINE neon

#include "neon-functions-float.h"
#include "neon-vector-shift.h"
#include "neon-pairhmm-template.h"

#include "neon-functions-double.h"
#include "neon-vector-shift.h"
#include "neon-pairhmm-template.h"
