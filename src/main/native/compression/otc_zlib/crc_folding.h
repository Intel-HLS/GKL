#ifndef _CRC_FOLDING_H_
#define _CRC_FOLDING_H_

#ifdef HAVE_PCLMULQDQ

#include "x86.h"

extern void ZLIB_INTERNAL crc_fold_init(unsigned crc[4 * 5]);
extern void ZLIB_INTERNAL crc_fold_copy(unsigned crc[4 * 5],
	unsigned char *dst, z_const unsigned char *src, long len);
extern unsigned ZLIB_INTERNAL crc_fold_512to32(unsigned crc[4 * 5]);

#endif

#endif