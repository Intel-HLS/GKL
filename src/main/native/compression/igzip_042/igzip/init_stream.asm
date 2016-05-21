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
%include "options.asm"

%include "lz0a_const.asm"
%include "data_struct2.asm"

; void init_stream(LZ_Stream2 *stream)
; arg 1: rcx: addr of stream
global init_stream : function
init_stream:
%ifdef LINUX
	mov	rcx, rdi
%endif

	xor	rax, rax
	mov	[rcx + _total_in], eax
	mov	[rcx + _total_out], eax
	mov	[rcx + _internal_state_b_bytes_valid], eax
	mov	[rcx + _internal_state_b_bytes_processed], eax
	mov	dword [rcx + _internal_state_state], LZS2_HDR
	mov	[rcx + _internal_state_count], eax

	; tmp_out_start = tmp_out_end = 0
	mov	[rcx + _internal_state_tmp_out_start], eax
	mov	[rcx + _internal_state_tmp_out_end], eax

	; file_start = &buffer[0];
	lea	rdx, [rcx + _internal_state_buffer]
	mov	[rcx + _internal_state_file_start], rdx

	; state->bitbuf.init();
	mov	[rcx + _internal_state_bitbuf_m_bits], rax
	mov	[rcx + _internal_state_bitbuf_m_bit_count], eax

%if ((MAJOR_VERSION == IGZIP0) || (MAJOR_VERSION == IGZIP1))
	; init crc
	not	rax
	mov	[rcx + _internal_state_crc], eax
%elif ((MAJOR_VERSION == IGZIP0C) || (MAJOR_VERSION == IGZIP1C))
	;;; MAGIC 512-bit number that will become 0xFFFFFFFF after folding
	pxor xmm0, xmm0
	movdqa	[rcx + _internal_state_crc + 48], xmm0
	movdqa	[rcx + _internal_state_crc + 32], xmm0
	movdqa	[rcx + _internal_state_crc + 16], xmm0
	mov eax, 0x9db42487
	movd xmm0, eax
	movdqa	[rcx + _internal_state_crc], xmm0
%else
 %error NO MAJOR VERSION SELECTED
 % error
%endif

	; for (i=0; i<HASH_SIZE; i++) state->head[i] = (UINT16) -(int)(D + 1);
	movdqa	xmm0, [init_val wrt rip]
	add	rcx, _internal_state_head

%if ((MAJOR_VERSION == IGZIP0) || (MAJOR_VERSION == IGZIP0C))
	mov	rax, HASH_SIZE / (4 * 8)
%elif ((MAJOR_VERSION == IGZIP1) || (MAJOR_VERSION == IGZIP1C))
	mov	rax, HASH_SIZE / (4 * 2)
%else
 %error NO MAJOR VERSION SELECTED
 % error
%endif
init_loop:
	movdqa	[rcx + 16 * 0], xmm0
	movdqa	[rcx + 16 * 1], xmm0
	movdqa	[rcx + 16 * 2], xmm0
	movdqa	[rcx + 16 * 3], xmm0
	add	rcx, 16*4
	sub	rax, 1
	jne	init_loop

	ret

section .data
%assign VAL (-(D + 1)) & 0xFFFF
align 16
init_val:
	dw	VAL, VAL, VAL, VAL, VAL, VAL, VAL, VAL
