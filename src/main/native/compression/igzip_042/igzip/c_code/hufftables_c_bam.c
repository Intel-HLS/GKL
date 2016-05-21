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

extern const UINT64 huff_hdr[13] = {
        0x9e565ae48007fded,
        0x9ee9eebcccf75401,
        0xbdd24955bba8d2d0,
        0x0e75e599bcd2aaa5,
        0x63c223046a8c03bb,
        0x92ec2d258f079961,
        0x8c2d859230e701b3,
        0x6c672739ce7392f1,
        0xd80c06d8c6309ce3,
        0x0ebdb5ebb01380c6,
        0x84f57752ab9dfcf0,
        0xc29556f9c5e9eaae,
        0xf39ee75f9f5eaa95,
};
extern const UINT64 huff_hdr_extra = 0x0007beecff9f;
extern const UINT32 huff_hdr_count = 13;
extern const UINT32 huff_hdr_extra_bits = 52-16;

#else

extern const UINT64 huff_hdr[14] = {
        0x0000000000088b1f,
        0x5ae48007fdedff00,
        0xeebcccf754019e56,
        0x4955bba8d2d09ee9,
        0xe599bcd2aaa5bdd2,
        0x23046a8c03bb0e75,
        0x2d258f07996163c2,
        0x859230e701b392ec,
        0x2739ce7392f18c2d,
        0x06d8c6309ce36c67,
        0xb5ebb01380c6d80c,
        0x7752ab9dfcf00ebd,
        0x56f9c5e9eaae84f5,
        0xe75f9f5eaa95c295,
};
extern const UINT64 huff_hdr_extra = 0x0007beecff9ff39e;
extern const UINT32 huff_hdr_count = 14;
extern const UINT32 huff_hdr_extra_bits = 52;

#endif
