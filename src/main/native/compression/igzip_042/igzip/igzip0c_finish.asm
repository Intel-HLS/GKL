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
%if (MAJOR_VERSION == IGZIP0C)

%include "lz0a_const.asm"
%include "data_struct2.asm"
%include "bitbuf2.asm"
%include "huffman.asm"
%include "utils.asm"
%include "hash.asm"

%include "reg_sizes.asm"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

%if 1

%define tmp1		rax

%define f_index		rbx
%define code		rbx
%define tmp4		rbx
%define tmp5		rbx
%define tmp6		rbx

%define tmp2		rcx
%define hash		rcx

%define tmp3		rdx

%define stream		rsi

%define f_i		rdi

%define code_len2	rbp

%define m_out_buf	r8

%define m_bits		r9

%define dist		r10

%define m_bit_count	r11

%define code2		r12

%define f_end_i		r13

%define file_start	r14

%define len		r15

%endif

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; void fast_lz2_finish ( LZ_Stream2 *stream )
; arg 1: rcx: addr of stream
global fast_lz2_finish
fast_lz2_finish:
	push	rbx
	push	rsi
	push	rdi
	push	rbp
	push	r12
	push	r13
	push	r14
	push	r15


%ifdef LINUX
	mov	rcx, rdi
%endif

	mov	stream, rcx

	; state->bitbuf.set_buf(stream->next_out, stream->avail_out);
	mov	m_out_buf, [stream + _next_out]
	mov	[stream + _internal_state_bitbuf_m_out_start], m_out_buf
	mov	tmp1 %+ d, [stream + _avail_out]
	add	tmp1, m_out_buf
	sub	tmp1, SLOP
	mov	[stream + _internal_state_bitbuf_m_out_end], tmp1

	mov	m_bits,           [stream + _internal_state_bitbuf_m_bits]
	mov	m_bit_count %+ d, [stream + _internal_state_bitbuf_m_bit_count]

	; f_i = state->b_bytes_processed;
	; f_end_i   = state->b_bytes_valid;
	mov	f_i %+ d,     [stream + _internal_state_b_bytes_processed]
	mov	f_end_i %+ d, [stream + _internal_state_b_bytes_valid]

	; f_i     += (UINT32)(state->buffer - state->file_start);
	; f_end_i += (UINT32)(state->buffer - state->file_start);
	mov	file_start, [stream + _internal_state_file_start]
	lea	tmp1, [stream + _internal_state_buffer]
	sub	tmp1, file_start
	add	f_i, tmp1
	add	f_end_i, tmp1

	; for (f_i = f_start_i; f_i < f_end_i; f_i++) {
	cmp	f_i, f_end_i
	jge	.end_loop_2

	mov	tmp1 %+ d, [file_start + f_i]

.loop2:
	; if (state->bitbuf.is_full()) {
	cmp	m_out_buf, [stream + _internal_state_bitbuf_m_out_end]
	ja	.end_loop_2

	; hash = compute_hash(state->file_start + f_i) & HASH_MASK;
	compute_hash	hash, tmp1, tmp2
	and	hash %+ d, HASH_MASK

	; f_index = state->head[hash];
	movzx	f_index %+ d, word [stream + _internal_state_head + 2 * hash]

	; state->head[hash] = (UINT16) f_i;
	mov	[stream + _internal_state_head + 2 * hash], f_i %+ w

	; dist = f_i - f_index; // mod 64k
	mov	dist %+ d, f_i %+ d
	sub	dist %+ d, f_index %+ d
	and	dist %+ d, 0xFFFF

	; if ((dist-1) < (D-1)) { // should this <=??? @@@
	mov	tmp1 %+ d, dist %+ d
	sub	tmp1 %+ d, 1
	cmp	tmp1 %+ d, (D-1)
	jae	.encode_literal

	; len = f_end_i - f_i;
	mov	tmp4, f_end_i
	sub	tmp4, f_i

	; if (len > 258) len = 258;
	cmp	tmp4, 258
	cmovg	tmp4, [c258 wrt rip]

	; len = compare(state->file_start + f_i,
        ;               state->file_start + f_i - dist, len);
	lea	tmp1, [file_start + f_i]
	mov	tmp2, tmp1
	sub	tmp2, dist
	compare	tmp4, tmp1, tmp2, len, tmp3

	; if (len >= SHORTEST_MATCH) {
	cmp	len, SHORTEST_MATCH
	jb	.encode_literal

	;; encode as dist/len

	; get_dist_code(dist, &code2, &code_len2);
	get_dist_code	dist, code2, code_len2 ;; clobbers dist, rcx

	; get_len_code(len, &code, &code_len);
	get_len_code	len, code, rcx		;; rcx is code_len

	; code2 <<= code_len
	; code2 |= code
	; code_len2 += code_len
%ifdef USE_HSWNI
	shlx	code2, code2, rcx
%else
	shl	code2, cl
%endif
	or	code2, code
	add	code_len2, rcx

	; for (k = f_i+1, f_i += len-1; k <= f_i; k++) {
	lea	tmp3, [f_i + 1]	; tmp3 <= k
	add	f_i, len
%ifdef LIMIT_HASH_UPDATE
	; only update hash twice

	; hash = compute_hash(state->file_start + k) & HASH_MASK;
	mov	tmp6 %+ d, [file_start + tmp3]
	compute_hash	hash, tmp6, tmp2
	and	hash %+ d, HASH_MASK
	; state->head[hash] = k;
	mov	[stream + _internal_state_head + 2 * hash], tmp3 %+ w

	add	tmp3, 1

	; hash = compute_hash(state->file_start + k) & HASH_MASK;
	mov	tmp6 %+ d, [file_start + tmp3]
	compute_hash	hash, tmp6, tmp2
	and	hash %+ d, HASH_MASK
	; state->head[hash] = k;
	mov	[stream + _internal_state_head + 2 * hash], tmp3 %+ w

%else
.loop3:
	; hash = compute_hash(state->file_start + k) & HASH_MASK;
	mov	tmp6 %+ d, [file_start + tmp3]
	compute_hash	hash, tmp6, tmp2
	and	hash %+ d, HASH_MASK
	; state->head[hash] = k;
	mov	[stream + _internal_state_head + 2 * hash], tmp3 %+ w
	inc	tmp3
	cmp	tmp3, f_i
	jl	.loop3
%endif

	mov	tmp1 %+ d, [file_start + f_i]

%ifdef USE_BITBUF8
	write_bits_safe	m_bits, m_bit_count, code2, code_len2, m_out_buf, tmp5
%elifdef USE_BITBUFB
	write_bits_always m_bits, m_bit_count, code2, code_len2, m_out_buf, tmp5
%else
	; state->bitbuf.check_space(code_len2);
	check_space	code_len2, m_bits, m_bit_count, m_out_buf, tmp5
	; state->bitbuf.write_bits(code2, code_len2);
	write_bits	m_bits, m_bit_count, code2, code_len2
	; code2 is clobbered, rcx is clobbered
%endif

	; continue
	cmp	f_i, f_end_i
	jl	.loop2
	jmp	.end_loop_2

.encode_literal:
	mov	tmp1 %+ d, [file_start + f_i + 1]

	; get_lit_code(state->file_start[f_i], &code2, &code_len2);
	movzx	tmp5, byte [file_start + f_i]
	get_lit_code	tmp5, code2, code_len2

%ifdef USE_BITBUF8
	write_bits_safe	m_bits, m_bit_count, code2, code_len2, m_out_buf, tmp5
%elifdef USE_BITBUFB
	write_bits_always m_bits, m_bit_count, code2, code_len2, m_out_buf, tmp5
%else
	; state->bitbuf.check_space(code_len2);
	check_space	code_len2, m_bits, m_bit_count, m_out_buf, tmp5
	; state->bitbuf.write_bits(code2, code_len2);
	write_bits	m_bits, m_bit_count, code2, code_len2
	; code2 is clobbered, rcx is clobbered
%endif

	; continue
	add	f_i, 1
	cmp	f_i, f_end_i
	jl	.loop2

.end_loop_2:

	; if ((f_i >= f_end_i) && ! state->bitbuf.is_full()) {
	cmp	f_i, f_end_i
	jl	.not_end
	cmp	m_out_buf, [stream + _internal_state_bitbuf_m_out_end]
	ja	.not_end

        ;	get_lit_code(256, &code2, &code_len2);
	get_lit_code_const	256, code2, code_len2

%ifdef USE_BITBUF8
	write_bits_safe	m_bits, m_bit_count, code2, code_len2, m_out_buf, tmp1
%elifdef USE_BITBUFB
	write_bits_always m_bits, m_bit_count, code2, code_len2, m_out_buf, tmp1
%else
        ;	state->bitbuf.check_space(code_len2);
	check_space	code_len2, m_bits, m_bit_count, m_out_buf, tmp1
	;       state->bitbuf.write_bits(code2, code_len2);
	write_bits	m_bits, m_bit_count, code2, code_len2
	; code2 is clobbered, rcx is clobbered
%endif

	;       state->state = LZS2_TRL;
	mov	dword [stream + _internal_state_state], LZS2_TRL

	;    }
.not_end:

	; state->b_bytes_processed = f_i - (state->buffer - state->file_start);
	add	f_i, [stream + _internal_state_file_start]
	sub	f_i, stream
	sub	f_i, _internal_state_buffer
	mov	[stream + _internal_state_b_bytes_processed], f_i %+ d

	;    // update output buffer
	;    stream->next_out = state->bitbuf.buffer_ptr();
	mov	[stream + _next_out], m_out_buf
	;    len = state->bitbuf.buffer_used();
	sub	m_out_buf, [stream + _internal_state_bitbuf_m_out_start]
	
	;    stream->avail_out -= len;
	sub	[stream + _avail_out], m_out_buf %+ d
	;    stream->total_out += len;
	add	[stream + _total_out], m_out_buf %+ d

	mov	[stream + _internal_state_bitbuf_m_bits], m_bits
	mov	[stream + _internal_state_bitbuf_m_bit_count], m_bit_count %+ d

	pop	r15
	pop	r14
	pop	r13
	pop	r12
	pop	rbp
	pop	rdi
	pop	rsi
	pop	rbx

	ret

section .data
	align 8
c258:	dq	258

%endif ;; if (MAJOR_VERSION == IGZIP0C)
