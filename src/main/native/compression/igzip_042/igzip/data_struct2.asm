;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; The MIT License
; 
; Copyright (c) 2014 Intel Corporation
; 
; 	Permission is hereby granted, free of charge, to any person
; 	obtaining a copy of this software and associated documentation
; 	files (the "Software"), to deal in the Software without
; 	restriction, including without limitation the rights to use,
; 	copy, modify, merge, publish, distribute, sublicense, and/or
; 	sell copies of the Software, and to permit persons to whom the
; 	Software is furnished to do so, subject to the following
; 	conditions:
; 
; 	The above copyright notice and this permission notice shall be
; 	included in all copies or substantial portions of the
; 	Software.
; 
; 	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
; 	KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
; 	WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
; 	PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
; 	COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
; 	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
; 	OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
; 	SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%ifndef __DATA_STRUCT2_ASM__
%define __DATA_STRUCT2_ASM__

;; START_FIELDS
%macro START_FIELDS 0
%assign _FIELD_OFFSET 0
%assign _STRUCT_ALIGN 0
%endm

;; FIELD name size align
%macro FIELD 3
%define %%name  %1
%define %%size  %2
%define %%align %3

%assign _FIELD_OFFSET (_FIELD_OFFSET + (%%align) - 1) & (~ ((%%align)-1))
%%name	equ	_FIELD_OFFSET
%assign _FIELD_OFFSET _FIELD_OFFSET + (%%size)
%if (%%align > _STRUCT_ALIGN)
%assign _STRUCT_ALIGN %%align
%endif
%endm

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

START_FIELDS	;; BitBuf2

;;      name		size    align
FIELD	_m_bits,	8,	8
FIELD	_m_bit_count,	4,	4
FIELD	_m_out_buf,	8,	8
FIELD	_m_out_end,	8,	8
FIELD	_m_out_start,	8,	8

%assign _BitBuf2_size	_FIELD_OFFSET
%assign _BitBuf2_align	_STRUCT_ALIGN

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

START_FIELDS	;; LZ_State2

;;      name		size    align
FIELD	_b_bytes_valid,	4,	4
FIELD	_b_bytes_processed,	4,	4
FIELD	_file_start,	8,	8
%if ((MAJOR_VERSION == IGZIP0) || (MAJOR_VERSION == IGZIP1))
 FIELD	_crc,		4,	4
%elif ((MAJOR_VERSION == IGZIP0C) || (MAJOR_VERSION == IGZIP1C))
 FIELD	_crc,		64,	16
%else
 %error NO MAJOR VERSION SELECTED
 % error
%endif
FIELD	_bitbuf,	_BitBuf2_size,	_BitBuf2_align
FIELD	_state,		4,	4
FIELD	_count,		4,	4
FIELD   _tmp_out_buff,	16,	1
FIELD   _tmp_out_start,	4,	4
FIELD	_tmp_out_end,	4,	4

FIELD	_buffer,	BSIZE+16,	16
%if ((MAJOR_VERSION == IGZIP0) || (MAJOR_VERSION == IGZIP0C))
 FIELD	_head,		HASH_SIZE*2,	16
%elif ((MAJOR_VERSION == IGZIP1) || (MAJOR_VERSION == IGZIP1C))
 FIELD	_head,		HASH_SIZE*8,	16
%else
 %error NO MAJOR VERSION SELECTED
 % error
%endif

%assign _LZ_State2_size		_FIELD_OFFSET
%assign _LZ_State2_align	_STRUCT_ALIGN

_bitbuf_m_bits		equ	_bitbuf+_m_bits
_bitbuf_m_bit_count	equ	_bitbuf+_m_bit_count
_bitbuf_m_out_buf	equ	_bitbuf+_m_out_buf
_bitbuf_m_out_end	equ	_bitbuf+_m_out_end
_bitbuf_m_out_start	equ	_bitbuf+_m_out_start
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

START_FIELDS	;; LZ_Stream2

;;      name		size    align
FIELD	_next_in,	8,	8
FIELD	_avail_in,	4,	4
FIELD	_total_in,	4,	4
FIELD	_next_out,	8,	8
FIELD	_avail_out,	4,	4
FIELD	_total_out,	4,	4
FIELD	_end_of_stream,	4,	4
FIELD	_internal_state,	_LZ_State2_size,	_LZ_State2_align

%assign _LZ_Stream2_size	_FIELD_OFFSET
%assign _LZ_Stream2_align	_STRUCT_ALIGN

_internal_state_b_bytes_valid	      equ   _internal_state+_b_bytes_valid
_internal_state_b_bytes_processed     equ   _internal_state+_b_bytes_processed
_internal_state_file_start	      equ   _internal_state+_file_start
_internal_state_crc		      equ   _internal_state+_crc
_internal_state_bitbuf		      equ   _internal_state+_bitbuf
_internal_state_state		      equ   _internal_state+_state
_internal_state_count		      equ   _internal_state+_count
_internal_state_tmp_out_buff	      equ   _internal_state+_tmp_out_buff
_internal_state_tmp_out_start	      equ   _internal_state+_tmp_out_start
_internal_state_tmp_out_end	      equ   _internal_state+_tmp_out_end
_internal_state_buffer		      equ   _internal_state+_buffer
_internal_state_head		      equ   _internal_state+_head
_internal_state_bitbuf_m_bits	      equ   _internal_state+_bitbuf_m_bits
_internal_state_bitbuf_m_bit_count    equ   _internal_state+_bitbuf_m_bit_count
_internal_state_bitbuf_m_out_buf      equ   _internal_state+_bitbuf_m_out_buf
_internal_state_bitbuf_m_out_end      equ   _internal_state+_bitbuf_m_out_end
_internal_state_bitbuf_m_out_start    equ   _internal_state+_bitbuf_m_out_start

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

LZS2_HDR	equ	0
LZS2_TRL	equ	2

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;%define PRINT_OFFSETS
%ifdef PRINT_OFFSETS
	mov	rax, _m_bits
	mov	rax, _m_bit_count
	mov	rax, _m_out_buf
	mov	rax, _m_out_end
	mov	rax, _m_out_start
	mov	rax, _BitBuf2_size
	mov	rax, _BitBuf2_align
	mov	rax, _b_bytes_valid
	mov	rax, _b_bytes_processed
	mov	rax, _file_start
	mov	rax, _crc
	mov	rax, _bitbuf
	mov	rax, _state
	mov	rax, _count
	mov	rax, _buffer
	mov	rax, _head
	mov	rax, _LZ_State2_size
	mov	rax, _LZ_State2_align
	mov	rax, _bitbuf_m_bits
	mov	rax, _bitbuf_m_bit_count
	mov	rax, _bitbuf_m_out_buf
	mov	rax, _bitbuf_m_out_end
	mov	rax, _bitbuf_m_out_start
	mov	rax, _next_in
	mov	rax, _avail_in
	mov	rax, _total_in
	mov	rax, _next_out
	mov	rax, _avail_out
	mov	rax, _total_out
	mov	rax, _end_of_stream
	mov	rax, _internal_state
	mov	rax, _LZ_Stream2_size
	mov	rax, _LZ_Stream2_align
	mov	rax, _internal_state_b_bytes_valid
	mov	rax, _internal_state_b_bytes_processed
	mov	rax, _internal_state_file_start
	mov	rax, _internal_state_bitbuf
	mov	rax, _internal_state_state
	mov	rax, _internal_state_count
	mov	rax, _internal_state_buffer
	mov	rax, _internal_state_head
	mov	rax, _internal_state_bitbuf_m_bits
	mov	rax, _internal_state_bitbuf_m_bit_count
	mov	rax, _internal_state_bitbuf_m_out_buf
	mov	rax, _internal_state_bitbuf_m_out_end
	mov	rax, _internal_state_bitbuf_m_out_start
%endif

%endif
