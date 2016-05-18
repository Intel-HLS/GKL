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

#ifdef GENOME_BAM
#include "hufftables_c_bam.c"
#elif defined(GENOME_SAM)
#include "hufftables_c_sam.c"
#else

#ifdef ONLY_DEFLATE

extern const UINT64 huff_hdr[25] = {
    0x34d76db6c76dfded,
    0x1fb1fb1c7ec7ef3d,
    0xfb1fb2f1fbec7ccb,
    0xb4c71fbec7ec71cb,
    0xebecbccb6fb1fbce,
    0xec76fb6c7aebceb2,
    0xb1c76cb6cb6c71c7,
    0xfb1dbadb6eb6eb6e,
    0x6db2ebafb6db6eb6,
    0x31cbedb6dbec7ac7,
    0xdb6f32cb2d3cf3af,
    0xb2cb2fcdb2f3ccbc,
    0xbfbc7ec7adb6c72e,
    0x1fb1c71ebfb1fbee,
    0xec7efbecbc72fec7,
    0x1fbeebc72fec71fb,
    0x1c71fbecbc72fec7,
    0xc71fbef1fbec71fb,
    0xfb6f1dbfb1c71fbe,
    0xc7efb1c7afedbc7e,
    0xcd34c631fbef1fb1,
    0xeb6ebaf32cbf32f3,
    0x1c7ec76fb1c76c7a,
    0x34f32dbcdbadb6fb,
    0xd318f47a31a6318d,
};
extern const UINT64 huff_hdr_extra = 0x0000000000af6d34;
extern const UINT32 huff_hdr_count = 25;
extern const UINT32 huff_hdr_extra_bits = 24;

#else

extern const UINT64 huff_hdr[26] = {
    0x0000000000088b1f,
    0x6db6c76dfdedff00,
    0xfb1c7ec7ef3d34d7,
    0xb2f1fbec7ccb1fb1,
    0x1fbec7ec71cbfb1f,
    0xbccb6fb1fbceb4c7,
    0xfb6c7aebceb2ebec,
    0x6cb6cb6c71c7ec76,
    0xbadb6eb6eb6eb1c7,
    0xebafb6db6eb6fb1d,
    0xedb6dbec7ac76db2,
    0x32cb2d3cf3af31cb,
    0x2fcdb2f3ccbcdb6f,
    0x7ec7adb6c72eb2cb,
    0xc71ebfb1fbeebfbc,
    0xfbecbc72fec71fb1,
    0xebc72fec71fbec7e,
    0xfbecbc72fec71fbe,
    0xbef1fbec71fb1c71,
    0x1dbfb1c71fbec71f,
    0xb1c7afedbc7efb6f,
    0xc631fbef1fb1c7ef,
    0xbaf32cbf32f3cd34,
    0xc76fb1c76c7aeb6e,
    0x2dbcdbadb6fb1c7e,
    0xf47a31a6318d34f3,
};
extern const UINT64 huff_hdr_extra = 0x000000af6d34d318;
extern const UINT32 huff_hdr_count = 26;
extern const UINT32 huff_hdr_extra_bits = 40;
#endif

#endif
