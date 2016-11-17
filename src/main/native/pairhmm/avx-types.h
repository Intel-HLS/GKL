#ifndef AVX_TEMPLATE_H
#define AVX_TEMPLATE_H

#ifdef _WIN32
#define ALIGNED __declspec(align(32))
#else
#define ALIGNED __attribute__((aligned(32)))
#endif

typedef ALIGNED union {
  __m256  ALIGNED d;
  __m128i ALIGNED s[2];
  float   ALIGNED f[8];
  __m256i ALIGNED i;
} mix_F;

typedef ALIGNED union {
  __m128  ALIGNED d;
  __m64   ALIGNED s[2];
  float   ALIGNED f[4];
  __m128i ALIGNED i;
} mix_F128;

typedef union ALIGNED {
  __m128i  vec;
  __m128   vecf;
  uint32_t masks[4];
} MaskVec_F;

typedef union ALIGNED {
  __m64    vec;
  __m64    vecf;
  uint32_t masks[2];
} MaskVec_F128;

typedef union ALIGNED {
  __m128i ALIGNED i;
  __m128  ALIGNED f;
} IF_128f;

typedef union ALIGNED {
  int   ALIGNED i;
  float ALIGNED f;
} IF_32;

typedef ALIGNED union {
  __m256d ALIGNED d;
  __m128i ALIGNED s[2];
  double  ALIGNED f[4];
  __m256i ALIGNED i;
} mix_D;

typedef ALIGNED union {
  __m128d ALIGNED d;
  __m64   ALIGNED s[2];
  double  ALIGNED f[2];
  __m128i ALIGNED i;
} mix_D128;

typedef union ALIGNED {
  __m128i  vec;
  __m128d  vecf;
  uint64_t masks[2];
} MaskVec_D;

typedef union ALIGNED {
  __m64    vec;
  __m64    vecf;
  uint64_t masks[1];
} MaskVec_D128;

typedef union ALIGNED {
  __m128i ALIGNED i;
  __m128d ALIGNED f;
} IF_128d;

typedef union ALIGNED {
  int64_t ALIGNED i;
  double  ALIGNED f;
} IF_64;

#endif // AVX_TEMPLATE_H
