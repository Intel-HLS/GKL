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

#define ASM

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

//#include "fast_lz77.h"
#include "bitbuf2.h"
//#include "huffman.h"
#include "crc.h"

#include "fast_lz77_2.h"

extern const UINT64 huff_hdr[];
extern const UINT64 huff_hdr_extra;
extern const UINT32 huff_hdr_count;
extern const UINT32 huff_hdr_extra_bits;

extern "C" void fast_lz2_body(LZ_Stream2 *stream);
extern "C" void fast_lz2_finish(LZ_Stream2 *stream);
extern "C" void _fast_lz(LZ_Stream2 *stream);
extern "C" void init_stream(LZ_Stream2 *stream);
extern "C" void dummy(UINT32 x);
extern "C" UINT32 crc_512to32(UINT32 *crc);
//extern "C" void init_crc(UINT32 *crc_ptr);
//extern "C" void copy_in_crc(UINT8 *dst, UINT8 *src, UINT32 size,
//                            UINT32 *crc_ptr);


////////////////////////////////////////////////////////////////////////

// Forward declarations
struct LZ_Stream2;
void write_header(LZ_Stream2 *stream);
void write_trailer(LZ_Stream2 *stream);
void fast_lz_body(LZ_Stream2 *stream);
void fast_lz_finish(LZ_Stream2 *stream);

////////////////////////////////////////////////////////////////////////

UINT32
file_size(LZ_State2 *state)
{
    return state->b_bytes_valid + (UINT32)(state->buffer - state->file_start);
}

static
void
fast_lz_int(LZ_Stream2 *stream)
{
    LZ_State2 *state = &stream->internal_state;

	if (state->state == LZS2_HDR) {
        write_header(stream);
        if (stream->avail_out < 8)
            return;
    }
    if (state->state == LZS2_BODY) {
        if ((stream->avail_in != 0) || !stream->end_of_stream) {
#ifndef ASM
            fast_lz_body(stream);
#else
            fast_lz2_body(stream);
#endif
            if (stream->avail_out < 8)
                return;
        }
        if ((stream->avail_in == 0) && stream->end_of_stream) {
#ifndef ASM
            fast_lz_finish(stream);
#else
            fast_lz2_finish(stream);
#endif
            if (stream->avail_out < 8)
                return;
        }
    }
    if (state->state == LZS2_TRL) {
        write_trailer(stream);
        if (stream->avail_out < 8)
            return;
    }
}


extern "C" 
void
fast_lz(LZ_Stream2 *stream)
{
    LZ_State2 *state = &stream->internal_state;
    UINT32 size;

    if (state->tmp_out_start != state->tmp_out_end) {
        size = state->tmp_out_end - state->tmp_out_start;
        if (size > stream->avail_out)
            size = stream->avail_out;
        memcpy(stream->next_out,
               state->tmp_out_buff + state->tmp_out_start,
               size);
        stream->next_out += size;
        stream->avail_out -= size;
        stream->total_out += size;
        state->tmp_out_start += size;
        if (stream->avail_out == 0)
            return;
    }
    assert(state->tmp_out_start == state->tmp_out_end);

    fast_lz_int(stream);

    if (stream->avail_out == 0)
        return;

    if (stream->avail_out < 8) {
       UINT8 *next_out;
       UINT32 avail_out;
       UINT32 total_out;

       next_out  = stream->next_out;
       avail_out = stream->avail_out;
       total_out = stream->total_out;

       stream->next_out = state->tmp_out_buff;
       stream->avail_out = sizeof(state->tmp_out_buff);
       stream->total_out = 0;

       fast_lz_int(stream);

       state->tmp_out_start = 0;
       state->tmp_out_end = stream->total_out;

       stream->next_out = next_out;
       stream->avail_out = avail_out;
       stream->total_out = total_out;
       if (state->tmp_out_end) {
           size = state->tmp_out_end;
           if (size > stream->avail_out)
               size = stream->avail_out;
           memcpy(stream->next_out,
                  state->tmp_out_buff,
                  size);
           stream->next_out += size;
           stream->avail_out -= size;
           stream->total_out += size;
           state->tmp_out_start += size;
       } 
    }
}

void
write_header(LZ_Stream2 *stream)
{
    LZ_State2 *state = &stream->internal_state;

    UINT32 count_end, count;
    UINT64 *p64;

    count = huff_hdr_count - state->count;
    if (count != 0) {
        count_end = stream->avail_out / 8;
        if (count < count_end)
            count_end = count;

        p64 = (UINT64*) stream->next_out;
        count = state->count;

        stream->avail_out -= count_end * 8;
        stream->total_out += count_end * 8;
        state->count += count_end;

        count_end += count;
        while (count < count_end) {
            *p64++ = huff_hdr[count++];
        }

        stream->next_out = (UINT8*) p64;
        count = huff_hdr_count - state->count;
    }
    if ((count == 0) && (stream->avail_out >= 8)) {
        set_buf(&state->bitbuf,stream->next_out, stream->avail_out);
        write_bits(&state->bitbuf,huff_hdr_extra, huff_hdr_extra_bits);
        check_space(&state->bitbuf,64);  // force flush
        state->state = LZS2_BODY;
        state->count = 0;

        count = buffer_used(&state->bitbuf);
        stream->next_out = buffer_ptr(&state->bitbuf);
        stream->avail_out -= count;
        stream->total_out += count;
    }
}

void
write_trailer(LZ_Stream2 *stream)
{
    LZ_State2 *state = &stream->internal_state;
    unsigned int bytes;

    if (stream->avail_out >= 8) {
        set_buf(&state->bitbuf,stream->next_out, stream->avail_out);

        // the flush() will pad to the next byte and write up to 8 bytes
        // to the output stream/buffer
        flush(&state->bitbuf);
#ifndef ONLY_DEFLATE
        if (! is_full(&state->bitbuf)) {
#if ((MAJOR_VERSION == IGZIP0C) || (MAJOR_VERSION == IGZIP1C))
            write_bits(&state->bitbuf,crc_512to32(state->crc), 32);
#elif ((MAJOR_VERSION == IGZIP0) || (MAJOR_VERSION == IGZIP1))
            write_bits(&state->bitbuf,get_crc(state->crc), 32);
#else
  #error UNKNWON MAJOR VERSION MAJOR_VERSION
#endif
            write_bits(&state->bitbuf,file_size(state), 32);
            flush(&state->bitbuf);
            state->state = LZS2_END;
        }
#else
        state->state = LZS2_END;
#endif // ifndef ONLY_DEFLATE
        // update output buffer

        bytes = buffer_used(&state->bitbuf);
		stream->next_out = buffer_ptr(&state->bitbuf);
        stream->avail_out -= bytes;
        stream->total_out += bytes;
    }
}

////////////////////////////////////////////////////////////////////////

#if 0 // not needed with asm igzip functions
UINT32
compare258( const UINT8 *pSrc0, const UINT8 *pSrc1)
{
    int i;
    const int len = 258;

    for( i = 0; i < (len - 7); i += 8 )
        if ( (*((UINT64*)(pSrc0 + i)) - *((UINT64*)(pSrc1 + i))) ) {
            if ( (*((UINT32*)(pSrc0 + i)) - *((UINT32*)(pSrc1 + i))) ) {
                if (pSrc0[i+0] != pSrc1[i+0])
                    return i+0;
                if (pSrc0[i+1] != pSrc1[i+1])
                    return i+1;
                if (pSrc0[i+2] != pSrc1[i+2])
                    return i+2;
                return i+3;
            } else {
                if (pSrc0[i+4] != pSrc1[i+4])
                    return i+4;
                if (pSrc0[i+5] != pSrc1[i+5])
                    return i+5;
                if (pSrc0[i+6] != pSrc1[i+6])
                    return i+6;
                return i+7;
            }
        }
    for( ; i < len; i++ )
        if (pSrc0[i] - pSrc1[i])
            break;

    return i;
}


UINT32
compare( const UINT8 *pSrc0, const UINT8 *pSrc1, int len)
{
    int i;

    for( i = 0; i < (len - 7); i += 8 )
        if ( (*((UINT64*)(pSrc0 + i)) - *((UINT64*)(pSrc1 + i))) ) {
            if ( (*((UINT32*)(pSrc0 + i)) - *((UINT32*)(pSrc1 + i))) ) {
                if (pSrc0[i+0] != pSrc1[i+0])
                    return i+0;
                if (pSrc0[i+1] != pSrc1[i+1])
                    return i+1;
                if (pSrc0[i+2] != pSrc1[i+2])
                    return i+2;
                return i+3;
            } else {
                if (pSrc0[i+4] != pSrc1[i+4])
                    return i+4;
                if (pSrc0[i+5] != pSrc1[i+5])
                    return i+5;
                if (pSrc0[i+6] != pSrc1[i+6])
                    return i+6;
                return i+7;
            }
        }
    for( ; i < len; i++ )
        if (pSrc0[i] - pSrc1[i])
            break;

    return i;
}
#endif
