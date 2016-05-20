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
#include "options.h"
#include "os.h"

const UINT32 K = 1024;
const UINT32 D = HIST_SIZE * K;        // Amount of history
const UINT32 LA = 17 * 16;     // Max look-ahead, rounded up to 32 byte boundary
const UINT32 BSIZE = 2*D + LA; // Nominal buffer size

//const UINT32 HASH_SIZE = 8 * K;
const UINT32 HASH_SIZE = D;
const UINT32 HASH_MASK = (HASH_SIZE - 1);

const UINT32 SHORTEST_MATCH = 3;

////////////////////////////////////////////////////////////////////////

enum LZ_State2_state {LZS2_HDR, LZS2_BODY, LZS2_TRL, LZS2_END};

// Variable prefixes:
// b_ : Measured wrt the start of the buffer
// f_ : Measured wrt the start of the file (aka file_start)

struct LZ_State2 {
    UINT32  b_bytes_valid;      // number of bytes of valid data in buffer
    UINT32  b_bytes_processed;
    UINT8  *file_start;         // pointer to where file would logically start
#if ((MAJOR_VERSION == IGZIP0) || (MAJOR_VERSION == IGZIP1))
    UINT32  crc;
#elif ((MAJOR_VERSION == IGZIP0C) || (MAJOR_VERSION == IGZIP1C))
    DECLARE_ALIGNED(UINT32  crc[16], 16);  // actually 4 128-bit integers
#else
 #error NO MAJOR VERSION SELECTED
#endif
    BitBuf2 bitbuf;
    LZ_State2_state state;
    UINT32  count;             // used for partial header/trailer writes
    UINT8   tmp_out_buff[16];
    UINT32  tmp_out_start;
    UINT32  tmp_out_end;  
    
//    UINT32  dummy;             // for alignment
    DECLARE_ALIGNED(UINT8   buffer[BSIZE + 16],    16);
#if ((MAJOR_VERSION == IGZIP0) || (MAJOR_VERSION == IGZIP0C))
    DECLARE_ALIGNED(UINT16  head[HASH_SIZE],       16);
#elif ((MAJOR_VERSION == IGZIP1) || (MAJOR_VERSION == IGZIP1C))
    DECLARE_ALIGNED(UINT64  head[HASH_SIZE],       16);
#else
 #error NO MAJOR VERSION SELECTED
#endif
};

struct LZ_Stream2 {
    UINT8 *next_in;   // Next input byte
    UINT32 avail_in;  // number of bytes available at next_in
    UINT32 total_in;  // total number of bytes read so far

    UINT8 *next_out;  // Next output byte
    UINT32 avail_out; // number of bytes available at next_out
    UINT32 total_out; // total number of bytes written so far
    UINT32 end_of_stream; // non-zero if this is the last input buffer

    LZ_State2 internal_state;
};

//static LZ_State2 state;

