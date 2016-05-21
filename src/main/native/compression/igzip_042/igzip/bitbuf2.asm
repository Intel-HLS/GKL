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
%ifndef __BITBUF2_ASM__
%define __BITBUF2_ASM__

%include "options.asm"

; Assumes m_out_buf is a register
; Clobbers RCX
; code is clobbered
; write_bits_always	m_bits, m_bit_count, code, count, m_out_buf, tmp1
%macro write_bits_always 6
%define %%m_bits	%1
%define %%m_bit_count	%2
%define %%code		%3
%define %%count		%4
%define %%m_out_buf	%5
%define %%tmp1		%6

%ifdef USE_HSWNI
	shlx	%%code, %%code, %%m_bit_count
%else
	mov	rcx, %%m_bit_count
	shl	%%code, cl
%endif
	or	%%m_bits, %%code
	add	%%m_bit_count, %%count

	movnti	[%%m_out_buf], %%m_bits
	mov	rcx, %%m_bit_count
	shr	rcx, 3			; rcx = bytes
	add	%%m_out_buf, rcx
	shl	rcx, 3			; rcx = bits
	sub	%%m_bit_count, rcx
%ifdef USE_HSWNI
	shrx	%%m_bits, %%m_bits, rcx
%else
	shr	%%m_bits, cl
%endif
%endm

; Assumes m_out_buf is a register
; Clobbers RCX
; code is clobbered
; write_bits_safe	m_bits, m_bit_count, code, count, m_out_buf, tmp1
%macro write_bits_safe 6
%define %%m_bits	%1
%define %%m_bit_count	%2
%define %%code		%3
%define %%count		%4
%define %%m_out_buf	%5
%define %%tmp1		%6

	mov	%%tmp1, %%code
%ifdef USE_HSWNI
	shlx	%%tmp1, %%tmp1, %%m_bit_count
%else
	mov	rcx, %%m_bit_count
	shl	%%tmp1, cl
%endif
	or	%%m_bits, %%tmp1
	add	%%m_bit_count, %%count
	cmp	%%m_bit_count, 64
	jb	%%not_full
;	mov	%%tmp1, %%m_out_buf
	sub	%%m_bit_count, 64
;	movnti	[%%tmp1], %%m_bits
	movnti	[%%m_out_buf], %%m_bits
	add	%%m_out_buf, 8
;	add	%%tmp1, 8
;	mov	%%m_out_buf, %%tmp1
	mov	rcx, %%count
	sub	rcx, %%m_bit_count
	mov	%%m_bits, %%code
%ifdef USE_HSWNI
	shrx	%%m_bits, %%m_bits, rcx
%else
	shr	%%m_bits, cl
%endif
%%not_full:
%endm

; Assumes m_out_buf is a register
; Clobbers RCX
;; check_space	num_bits, m_bits, m_bit_count, m_out_buf, tmp1
%macro check_space 5
%define %%num_bits	%1
%define %%m_bits	%2
%define %%m_bit_count	%3
%define %%m_out_buf	%4
%define %%tmp1		%5

	mov	%%tmp1, 63
	sub	%%tmp1, %%m_bit_count
	cmp	%%tmp1, %%num_bits
	jae	%%space_ok
	
	; if (63 - m_bit_count < num_bits)
;	mov	%%tmp1, %%m_out_buf
;	movnti	[%%tmp1], %%m_bits
	movnti	[%%m_out_buf], %%m_bits
	mov	rcx, %%m_bit_count
	shr	rcx, 3			; rcx = bytes
;	add	%%tmp1, rcx
	add	%%m_out_buf, rcx
	shl	rcx, 3			; rcx = bits
;	mov	%%m_out_buf, %%tmp1
	sub	%%m_bit_count, rcx
%ifdef USE_HSWNI
	shrx	%%m_bits, %%m_bits, rcx
%else
	shr	%%m_bits, cl
%endif
%%space_ok:
%endm

; rcx is clobbered
; code is clobbered
; write_bits	m_bits, m_bit_count, code, count
%macro write_bits 4
%define %%m_bits		%1
%define %%m_bit_count	%2
%define %%code		%3
%define %%count		%4
%ifdef USE_HSWNI
	shlx	%%code, %%code, %%m_bit_count
%else
	mov	rcx, %%m_bit_count
	shl	%%code, cl
%endif
	or	%%m_bits, %%code
	add	%%m_bit_count, %%count
%endm	

; pad_to_byte m_bit_count, extra_bits
%macro pad_to_byte 2
%define %%m_bit_count	%1
%define %%extra_bits	%2

	mov	%%extra_bits, %%m_bit_count
	neg	%%extra_bits
	and	%%extra_bits, 7
	add	%%m_bit_count, %%extra_bits
%endm

; Assumes m_out_buf is a memory reference
; flush	m_bits, m_bit_count, m_out_buf, tmp1
%macro flush 4
%define %%m_bits	%1
%define %%m_bit_count	%2
%define %%m_out_buf	%3
%define %%tmp1		%4

	test	%%m_bit_count, %%m_bit_count
	jz	%%bit_count_is_zero

	mov	%%tmp1, %%m_out_buf
	movnti	[%%tmp1], %%m_bits

	add	%%m_bit_count, 7
	shr	%%m_bit_count, 3	; bytes
	add	%%tmp1, %%m_bit_count
	mov	%%m_out_buf, %%tmp1

%%bit_count_is_zero:
	xor	%%m_bits, %%m_bits
	xor	%%m_bit_count, %%m_bit_count
%endm

%endif
