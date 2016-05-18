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
#include "options.h"

#ifdef ONLY_DEFLATE

extern const UINT64 huff_hdr[9] = {
        0x9fb6b95c8005fded,
        0x1f1a6661ba93877d,
        0x555da6629edddb18,
        0x1b939274c09e438e,
        0x6663f7df0fe66666,
        0x6666666666666666,
        0xedb776eaa5ff7f7c,
        0x95577074f1ebedd3,
        0xb4b47ef76d24b4b6,
};
extern const UINT64 huff_hdr_extra = 0x00000074b4b4;
extern const UINT32 huff_hdr_count = 9;
extern const UINT32 huff_hdr_extra_bits = 45-16;

#else

extern const UINT64 huff_hdr[10] = {
        0x0000000000088b1f,
        0xb95c8005fdedff00,
        0x6661ba93877d9fb6,
        0xa6629edddb181f1a,
        0x9274c09e438e555d,
        0xf7df0fe666661b93,
        0x6666666666666663,
        0x76eaa5ff7f7c6666,
        0x7074f1ebedd3edb7,
        0x7ef76d24b4b69557,
};
extern const UINT64 huff_hdr_extra = 0x00000074b4b4b4b4;
extern const UINT32 huff_hdr_count = 10;
extern const UINT32 huff_hdr_extra_bits = 45;

#endif
