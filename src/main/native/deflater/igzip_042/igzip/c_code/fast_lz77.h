/**********************************************************************
The MIT License

Copyright (c) 2014 Intel Corporation

	Permission is hereby granted, free of charge, to any person
	obtaining a copy of this software and associated documentation
	files (the "Software"), to deal in the Software without
	restriction, including without limitation the rights to use,
	copy, modify, merge, publish, distribute, sublicense, and/or
	sell copies of the Software, and to permit persons to whom the
	Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be
	included in all copies or substantial portions of the
	Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
	KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
	WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
	PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
	COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
	OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
	SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**********************************************************************/

#include "types.h"

extern "C" {

// copy D + LA bytes from src to dst
// dst is aligned
//void copy_D_LA(UINT8 *dst, UINT8 *src);

// copy x bytes (rounded up to 16 bytes) from src to dst
// src & dst are unaligned
//void copy_in(UINT8 *dst, UINT8 *src, UINT32 x);

//UINT32 compute_hash(UINT8 *src);
void check_hash(UINT32 val);

// find number of matching bytes at A and B, up to a max of 258
UINT32 compare258(const UINT8 *a, const UINT8 *b);

UINT32 compare(const UINT8 *a, const UINT8 *b, int len);
};
