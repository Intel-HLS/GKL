
#ifndef _ISAL_H_
#define _ISAL_H_

#define ISAL_MAJOR_VERSION 2
#define ISAL_MINOR_VERSION 16
#define ISAL_PATCH_VERSION 0
#define ISAL_MAKE_VERSION(maj, min, patch)  ((maj) * 0x10000 + (min) * 0x100 + (patch))
#define ISAL_VERSION ISAL_MAKE_VERSION(ISAL_MAJOR_VERSION, ISAL_MINOR_VERSION, ISAL_PATCH_VERSION)

#include <isa-l/crc.h>
#include <isa-l/erasure_code.h>
#include <isa-l/gf_vect_mul.h>
#include <isa-l/igzip_lib.h>
#include <isa-l/raid.h>
#endif //_ISAL_H_
