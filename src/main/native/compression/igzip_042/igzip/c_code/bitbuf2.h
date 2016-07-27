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
#include <stdio.h>
#include <stddef.h> // for offsetof
#include "types.h"

#ifdef LINUX
 #define _mm_stream_si64x(dst, src) *((UINT64*)dst) = src
//__inline__ void _mm_stream_si64x(INT64 *dst, UINT64 src) {
//    __asm__ ("mov %1, %0" : "=r" (dst) : "r" (src));
//}
#else
#include <intrin.h>
#endif

//#pragma warning(disable: 1684)
#pragma warning(disable: 4996)

struct BitBuf2
{
    UINT64 m_bits;
    UINT32 m_bit_count;
    UINT8 *m_out_buf;
    UINT8 *m_out_end;
    UINT8 *m_out_start;
};
static 
void 
construct(BitBuf2 *me)
{
    me->m_bits = 0;
    me->m_bit_count = 0;
    me->m_out_buf = me->m_out_start = me->m_out_end = NULL;
}

static
void
init(BitBuf2 *me)
{
    me->m_bits = 0;
    me->m_bit_count = 0;
}

static
void
set_buf(BitBuf2 *me, unsigned char *buf, unsigned int len, unsigned int slop = 8)
{
    me->m_out_buf = me->m_out_start = buf;
    me->m_out_end = buf + len - slop;
}

static
bool
is_full(BitBuf2 *me)
{
    return (me->m_out_buf > me->m_out_end);
}

static
UINT8*
buffer_ptr(BitBuf2 *me) 
{
    return me->m_out_buf;
}

static
UINT32
buffer_used(BitBuf2 *me) 
{
    return (UINT32)(me->m_out_buf - me->m_out_start);
}

static
void
check_space(BitBuf2 *me, UINT32 num_bits)
{
    if (63 - me->m_bit_count < num_bits) {
        _mm_stream_si64x((INT64*)me->m_out_buf, me->m_bits);
        UINT32 bytes = me->m_bit_count / 8;
        me->m_out_buf += bytes;
        bytes *= 8;
        me->m_bit_count -= bytes;
        me->m_bits >>= bytes;
    }
}

static
void
write_bits(BitBuf2 *me, UINT64 code, UINT32 count) 
{
    me->m_bits |= code << me->m_bit_count;
    me->m_bit_count += count;
}

// fill bitbuffer and write 8 bytes if necessary
static
void 
write_bits_safe(BitBuf2 *me, UINT64 code, UINT32 count)
{
    me->m_bits |= code << me->m_bit_count;
    me->m_bit_count += count;
    if (me->m_bit_count >= 64) {
        _mm_stream_si64x((INT64*)me->m_out_buf, me->m_bits);
        me->m_out_buf += 8;
        me->m_bit_count -= 64;
        me->m_bits = code >> (count - me->m_bit_count);
    }
}

// returns number of padding bits used
static
UINT32 
pad_to_byte(BitBuf2 *me)
{
    UINT32 extra = ((-(int)me->m_bit_count) & 7);
    me->m_bit_count += extra;
    return extra;
}

//   Can write up to 8 bytes to output buffer
static
void
flush(BitBuf2 *me)
{
    if (me->m_bit_count) {
        _mm_stream_si64x((INT64*)me->m_out_buf, me->m_bits);
        UINT32 bytes = (me->m_bit_count + 7) / 8;
        me->m_out_buf += bytes;
    }
    me->m_bits = 0;
    me->m_bit_count = 0;
}


///////////////////
static 
void 
print_offsets(FILE *fp) 
{
#ifndef LINUX
    fprintf(fp, "bitbuf.m_bits = 0x%X\n", (UINT32) offsetof(BitBuf2, m_bits));
    fprintf(fp, "bitbuf.m_bit_count = 0x%X\n", (UINT32) offsetof(BitBuf2, m_bit_count));
    fprintf(fp, "bitbuf.m_out_buf = 0x%X\n", (UINT32) offsetof(BitBuf2, m_out_buf));
    fprintf(fp, "bitbuf.m_out_end = 0x%X\n", (UINT32) offsetof(BitBuf2, m_out_end));
    fprintf(fp, "bitbuf.m_out_start = 0x%X\n", (UINT32) offsetof(BitBuf2, m_out_start));
#endif
}


#if 0
class BitBuf2 {
protected:
    UINT64 m_bits;
    UINT32 m_bit_count;
    UINT8 *m_out_buf;
    UINT8 *m_out_end;
    UINT8 *m_out_start;

public:
    BitBuf2() {
        m_bits = 0;
        m_bit_count = 0;
        m_out_buf = m_out_start = m_out_end = NULL;
    }
    void init() {
        m_bits = 0;
        m_bit_count = 0;
    }
    void set_buf(unsigned char *buf, unsigned int len,
                 unsigned int slop = 8) {
        m_out_buf = m_out_start = buf;
        m_out_end = buf + len - slop;
    }
    bool is_full() {
        return (m_out_buf > m_out_end);
    }
    UINT8* buffer_ptr() {return m_out_buf;}
    UINT32 buffer_used() {return (UINT32)(m_out_buf - m_out_start);}
    // flush bit buffer if it can't hold num_bits bits
    // we never want to fill it completely
    // num_bits should not exceed 56
    //   Can write up to 7 bytes to output buffer
    void check_space(UINT32 num_bits) {
        if (63 - m_bit_count < num_bits) {
            _mm_stream_si64x((INT64*)m_out_buf, m_bits);
            UINT32 bytes = m_bit_count / 8;
            m_out_buf += bytes;
            bytes *= 8;
            m_bit_count -= bytes;
            m_bits >>= bytes;
        }
    }
    // assume that the bit buffer has enough space
    void write_bits(UINT64 code, UINT32 count) {
        m_bits |= code << m_bit_count;
        m_bit_count += count;
    }
    // fill bitbuffer and write 8 bytes if necessary
    void write_bits_safe(UINT64 code, UINT32 count) {
        m_bits |= code << m_bit_count;
        m_bit_count += count;
        if (m_bit_count >= 64) {
            _mm_stream_si64x((INT64*)m_out_buf, m_bits);
            m_out_buf += 8;
            m_bit_count -= 64;
            m_bits = code >> (count - m_bit_count);
        }
    }
    // returns number of padding bits used
    UINT32 pad_to_byte() {
        UINT32 extra = ((-(int)m_bit_count) & 7);
        m_bit_count += extra;
        return extra;
    }
    //   Can write up to 8 bytes to output buffer
    void flush() {
        if (m_bit_count) {
            _mm_stream_si64x((INT64*)m_out_buf, m_bits);
            UINT32 bytes = (m_bit_count + 7) / 8;
            m_out_buf += bytes;
        }
        m_bits = 0;
        m_bit_count = 0;
    }


    ///////////////////
    static void print_offsets(FILE *fp) {
#ifndef LINUX
        fprintf(fp, "bitbuf.m_bits = 0x%X\n", (UINT32) offsetof(BitBuf2, m_bits));
        fprintf(fp, "bitbuf.m_bit_count = 0x%X\n", (UINT32) offsetof(BitBuf2, m_bit_count));
        fprintf(fp, "bitbuf.m_out_buf = 0x%X\n", (UINT32) offsetof(BitBuf2, m_out_buf));
        fprintf(fp, "bitbuf.m_out_end = 0x%X\n", (UINT32) offsetof(BitBuf2, m_out_end));
        fprintf(fp, "bitbuf.m_out_start = 0x%X\n", (UINT32) offsetof(BitBuf2, m_out_start));
#endif
    }
    ///////////////////

/*
    // Following writes directly into buffer and bypasses bit-buffer
    // It assumes that the bit-buffer is empty
    void fast_write64(const UINT64 *data, UINT32 num_words) {
        UINT32 num;
        while (num_words != 0) {
            num = (UINT32) (m_out_end + SLOP - 8 - m_out_buf) / 8;
            if (num_words < num)
                num = num_words;
            num_words -= num;
            while (num != 0) {
                *(UINT64*)m_out_buf = *data;
                m_out_buf += 8;
                data++;
                num--;
            }
            if (num_words)
                do_callback();
        } // end while num_words != 0        
    }
*/
};
#endif #if 0
