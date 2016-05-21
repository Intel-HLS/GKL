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

enum {DIST_TABLE_SIZE = 1024};

// DECODE_OFFSET is dist code index corresponding to DIST_TABLE_SIZE + 1
enum {DECODE_OFFSET = 20};

extern "C" const struct HuffTables {

    // bits 7:0 are the length
    // bits 31:8 are the code
    UINT32 dist_table[DIST_TABLE_SIZE];

    // bits 7:0 are the length
    // bits 31:8 are the code
    UINT32 len_table[256];

    // bits 3:0 are the length
    // bits 15:4 are the code
    UINT16 lit_table[257];

    // bits 3:0 are the length
    // bits 15:4 are the code
    UINT16 dcodes[30 - DECODE_OFFSET];

} huff_tables;

////////////////////////////////////////////////////////////////////////
// Function Prototypes

void get_dist_code(UINT32 dist, UINT32 *code, UINT32 *code_len);
void get_len_code(UINT32 len,   UINT32 *code, UINT32 *code_len);
void get_lit_code(UINT32 lit,   UINT32 *code, UINT32 *code_len);

