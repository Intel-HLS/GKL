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
%if (MAJOR_VERSION == IGZIP1C)

%include "lz0a_const.asm"
%include "data_struct2.asm"
%include "bitbuf2.asm"
%include "huffman.asm"
%include "utils.asm"
%include "crc_pcl.asm"
%include "hash.asm"
%include "reg_sizes.asm"

%macro MARK 1
;global %1
%1:
%endm

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%if (COMPARE_TYPE == 1) || (COMPARE_TYPE == 4) || (COMPARE_TYPE == 5)
%if 1
%define tmp1		rax
%define code		rax

%define b_bytes_valid	rbx
%define tmp_dist	rbx
%define tmp6		rbx
%define code2		rbx

%define tmp2		rcx
%define tmp5		rcx
%define tmp7		rcx

%define tmp4		rdx
%define f_end_i		rdx
%define hash		rdx
%define len		rdx

%define tmp_len		rsi
%define code_len2	rsi

%define x		rdi
%define file_start	rdi

%define tmp3		rbp
%define f_indices	rbp

%define in_buf		r8

%define blen		r9

%define dist		r10

%define	stream		r11

%define f_i		r12

%define m_bit_count	r13

%define m_bits		r14

%define	m_out_buf	r15


%define crc_0		xmm0	; in/out: crc state
%define crc_1		xmm1	; in/out: crc state
%define crc_2		xmm2	; in/out: crc state
%define crc_3		xmm3	; in/out: crc state
%define crc_fold	xmm4	; in:	(loaded from fold_4)
%define xtmp0		xmm5	; tmp
%define xtmp1		xmm6	; tmp
%define xtmp2		xmm7	; tmp
%define xtmp3		xmm8	; tmp
%define xtmp4		xmm9	; tmp

%define ytmp0		ymm5	; tmp
%define ytmp1		ymm6	; tmp

%endif

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%elif (COMPARE_TYPE == 2)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%if 1
%define tmp1		rax
%define code		rax

%define tmp2		rbx
%define f_end_i		rbx
%define tmp5		rbx
%define code2		rbx

%define tmp3		rcx
%define tmp_dist	rcx
%define tmp_len		rcx

%define b_bytes_valid	rdx
%define tmp7		rdx
%define tmp_c2		rdx

%define x		rsi
%define hash		rsi
%define tmp6		rsi

%define in_buf		rbp
%define f_indices	rbp

%define blen		rdi
%define len		rdi

%define file_start	r8

%define dist		r9

%define tmp4		r10
%define tmp_c1		r10
%define code_len2	r10

%define	stream		r11

%define f_i		r12

%define m_bit_count	r13

%define m_bits		r14

%define	m_out_buf	r15


%define crc_0		xmm0	; in/out: crc state
%define crc_1		xmm1	; in/out: crc state
%define crc_2		xmm2	; in/out: crc state
%define crc_3		xmm3	; in/out: crc state
%define crc_fold	xmm4	; in:	(loaded from fold_4)
%define xtmp0	xmm5	; tmp
%define xtmp1	xmm6	; tmp
%define xtmp2	xmm7	; tmp
%define xtmp3	xmm8	; tmp
%define xtmp4	xmm9	; tmp

%endif

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%elif (COMPARE_TYPE == 3)
	%error not yet implemented
	 % error

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%else
	%error Unknown Compare type COMPARE_TYPE
	 % error
%endif

%define b_bytes_processed	f_i

%if (COMPARE_TYPE != 1) && (COMPARE_TYPE != 4) && (COMPARE_TYPE != 5)
blen_mem_offset     equ  0	  ; local variable (8 bytes)
in_buf_mem_offset   equ  8        ; local variable (8 bytes)
%endif
f_end_i_mem_offset  equ 16	  ; local variable (8 bytes)
gpr_save_mem_offset equ 3*8       ; gpr save area (8*8 bytes)
xmm_save_mem_offset equ 3*8 + 8*8 ; xmm save area (4*16 bytes) (16 byte aligned)
stack_size          equ 3*8 + 8*8 + 4*16 + 8


; void fast_lz2_body ( LZ_Stream2 *stream )
; arg 1: rcx: addr of stream
global fast_lz2_body
fast_lz2_body:
%ifdef LINUX
	mov	rcx, rdi
%endif

	;; do nothing if (avail_in == 0)
	cmp	dword [rcx + _avail_in], 0
	jne	skip1
	ret
skip1:

	sub	rsp, stack_size
	
	mov [rsp + gpr_save_mem_offset + 0*8], rbx
	mov [rsp + gpr_save_mem_offset + 1*8], rsi
	mov [rsp + gpr_save_mem_offset + 2*8], rdi
	mov [rsp + gpr_save_mem_offset + 3*8], rbp
	mov [rsp + gpr_save_mem_offset + 4*8], r12
	mov [rsp + gpr_save_mem_offset + 5*8], r13
	mov [rsp + gpr_save_mem_offset + 6*8], r14
	mov [rsp + gpr_save_mem_offset + 7*8], r15
	movdqa	[rsp + xmm_save_mem_offset + 0*16], xmm6
	movdqa	[rsp + xmm_save_mem_offset + 1*16], xmm7
	movdqa	[rsp + xmm_save_mem_offset + 2*16], xmm8
	movdqa	[rsp + xmm_save_mem_offset + 3*16], xmm9

	mov	stream, rcx

	movdqa	crc_0, [stream + _internal_state_crc + 0*16]
	movdqa	crc_1, [stream + _internal_state_crc + 1*16]
	movdqa	crc_2, [stream + _internal_state_crc + 2*16]
	movdqa	crc_3, [stream + _internal_state_crc + 3*16]
	movdqa	crc_fold, [fold_4 wrt rip]	

	; state->bitbuf.set_buf(stream->next_out, stream->avail_out);
	mov	m_out_buf, [stream + _next_out]
	mov	[stream + _internal_state_bitbuf_m_out_start], m_out_buf
	mov	tmp1 %+ d, [stream + _avail_out]
	add	tmp1, m_out_buf
	sub	tmp1, SLOP
	mov	[stream + _internal_state_bitbuf_m_out_end], tmp1

	mov	m_bits,           [stream + _internal_state_bitbuf_m_bits]
	mov	m_bit_count %+ d, [stream + _internal_state_bitbuf_m_bit_count]

	; in_buf = stream->next_in
	mov	in_buf,    [stream + _next_in]
	mov	blen %+ d, [stream + _avail_in]

	; while (blen != 0)
MARK __Compute_X
loop1:
	; x = D + LA - (state->b_bytes_valid - state->b_bytes_processed);
	mov	b_bytes_valid %+ d,     [stream + _internal_state_b_bytes_valid]
	mov	b_bytes_processed %+ d, [stream + _internal_state_b_bytes_processed]
	lea	x, [b_bytes_processed + D + LA]
	sub	x, b_bytes_valid

	; if (x > D) x = D;
	cmp	x, D
	cmova	x, [const_D wrt rip]

	; if (blen < D) x = blen;
	cmp	blen, D
	cmovb	x, blen

	;; process x bytes starting at in_buf

        ;; If there isn't enough room, shift buffer down
        ; if (x > BSIZE - state->b_bytes_valid) {
	mov	tmp1, BSIZE
	sub	tmp1, b_bytes_valid
	cmp	x, tmp1
	jbe	skip_move

	; if (state->b_bytes_processed < state->b_bytes_valid - LA) {
	mov	tmp1, b_bytes_valid
	sub	tmp1, LA
	cmp	b_bytes_processed, tmp1
	jae	do_move

	;; We need to move an odd amount, skip move for this copy of loop
	xor	x,x
%if (COMPARE_TYPE != 1) && (COMPARE_TYPE != 4) && (COMPARE_TYPE != 5)
	mov	[rsp + blen_mem_offset], blen
%endif
	jmp	skip_move_zero

MARK __shift_data_down
do_move:
	; offset = state->b_bytes_valid - (D + LA);
	mov	tmp4, b_bytes_valid
	sub	tmp4, D + LA
	; copy_D_LA(state->buffer, state->buffer + offset);
	lea	tmp1, [stream + _internal_state_buffer]
	lea	tmp2, [tmp1 + tmp4]
	copy_D_LA	tmp1, tmp2, tmp3, xtmp0, xtmp1, xtmp2, xtmp3
	; tmp1 clobbered
	
	; state->file_start        -= offset;
	sub	[stream + _internal_state_file_start], tmp4
	; state->b_bytes_processed -= offset;
	sub	b_bytes_processed, tmp4
	mov	b_bytes_valid, D + LA

MARK __copy_in
skip_move:
	sub	blen, x

%if (COMPARE_TYPE != 1) && (COMPARE_TYPE != 4) && (COMPARE_TYPE != 5)
	mov	[rsp + blen_mem_offset], blen
%endif
	
	; copy_in(state->buffer + state->b_bytes_valid, in_buf, x);
	lea	tmp1, [stream + _internal_state_buffer + b_bytes_valid]
	mov	tmp2, in_buf
	mov	tmp3, x
	COPY_IN_CRC	tmp1, tmp2, tmp3, tmp4, crc_0, crc_1, crc_2, crc_3, \
			crc_fold, \
			xtmp0, xtmp1, xtmp2, xtmp3, xtmp4

	; in_buf += x;
	add	in_buf, x
MARK __prepare_loop
skip_move_zero:
%if (COMPARE_TYPE != 1) && (COMPARE_TYPE != 4) && (COMPARE_TYPE != 5)
	mov	[rsp + in_buf_mem_offset], in_buf
%endif
	; state->b_bytes_valid += x;
	add	b_bytes_valid, x
	mov	[stream + _internal_state_b_bytes_valid], b_bytes_valid %+ d

	; f_end_i   = state->b_bytes_valid - LA;
%ifnidn f_end_i, b_bytes_valid
	mov	f_end_i, b_bytes_valid
%endif
	sub	f_end_i, LA
	; if (f_end_i <= 0) continue;
	cmp	f_end_i, 0
	jle	continue_while

	; f_start_i = state->b_bytes_processed;
	;; f_i and b_bytes_processed are same register, just store b_bytes_proc
	mov	[stream + _internal_state_b_bytes_processed], b_bytes_processed %+ d

	; f_start_i += (UINT32)(state->buffer - state->file_start);
	mov	file_start, [stream + _internal_state_file_start]
	lea	tmp1, [stream + _internal_state_buffer]
	sub	tmp1, file_start
	add	f_i, tmp1
	add	f_end_i, tmp1


	; for (f_i = f_start_i; f_i < f_end_i; f_i++) {
	cmp	f_i, f_end_i
	jge	end_loop_2

	mov	[rsp + f_end_i_mem_offset], f_end_i

MARK __compute_hash
	mov	tmp1 %+ d, [file_start + f_i]
loop2:
	mov tmp7, f_i
	and tmp7, 0x000000000000FFFF
	; if (state->bitbuf.is_full()) {
	cmp	m_out_buf, [stream + _internal_state_bitbuf_m_out_end]
	ja	bitbuf_full

	; hash = compute_hash(state->file_start + f_i) & HASH_MASK;
;;	mov	tmp1 %+ d, [file_start + f_i]
	compute_hash	hash, tmp1, tmp2
MARK __misc_compute_hash_lookup
	and	hash %+ d, HASH_MASK

	; f_indices = state->head[hash];
	mov	f_indices, [stream + _internal_state_head + 8 * hash]

	; state->head[hash] = (f_indices << 16) | (f_i & 0xFFFF);
	mov	tmp1, f_indices
	shl	tmp1, 16
	or	tmp1, tmp7
	mov	[stream + _internal_state_head + 8 * hash], tmp1

	xor	len, len

	; tmp5 = (f_i + 0x8000) << (64-16)
	mov	tmp5, f_i
	add	tmp5, 0x7FFF
	shl	tmp5, (64-16)

; logical start of index_loop
	; tmp_dist = (UINT16)(f_i - f_indices); // mod 64k
	mov	tmp_dist %+ d, f_i %+ d
	sub	tmp_dist %+ d, f_indices %+ d
	and	tmp_dist %+ d, 0xFFFF

	; if ((tmp_dist-1) < (D-1)) { // should this <=??? @@@
	mov	tmp1 %+ d, tmp_dist %+ d
	sub	tmp1 %+ d, 1
	cmp	tmp1 %+ d, (D-1)
	ja	end_index_loop

	; f_indices >>= 16;
	; f_indices |= ((UINT64)(f_i + 0x8000)) << (64-16);
	shr	f_indices, 16
	or	f_indices, tmp5

index_loop:

MARK __compare
	; tmp_len = compare258(state->file_start + f_i,
        ;                      state->file_start + f_i - tmp_dist);
	lea	tmp1, [file_start + f_i]
	mov	tmp2, tmp1
	sub	tmp2, tmp_dist	;; tmp2 := tmp1 - tmp_dist
	
%if (COMPARE_TYPE == 1)
	compare258	tmp1, tmp2, tmp_len, tmp6
%elif (COMPARE_TYPE == 2)
	compare258_2	tmp1, tmp2, tmp_len, tmp6, tmp_c1, tmp_c2
%elif (COMPARE_TYPE == 3)
	compare258_s	tmp1, tmp2, tmp_len, xtmp0
%elif (COMPARE_TYPE == 4)
	compare258_x	tmp1, tmp2, tmp_len, tmp6, xtmp0, xtmp1
%elif (COMPARE_TYPE == 5)
	compare258_y	tmp1, tmp2, tmp_len, tmp6, ytmp0, ytmp1
%else
	%error Unknown Compare type COMPARE_TYPE
	 % error
%endif

	sub tmp1, tmp2		;; tmp1 := tmp1 - (tmp1 - tmp_dist) = tmp_dist
	; if (tmp_len > len)
	;   len = tmp_len;
	;   dist = tmp_dist;
	cmp	tmp_len, len
;	cmova	len, tmp_len
;	cmova	dist, tmp1	;; tmp1 = tmp_dist
	cmovg	len, tmp_len
	cmovg	dist, tmp1	;; tmp1 = tmp_dist

	;; duplicate start of loop above
	; tmp_dist = (UINT16)(f_i - f_indices); // mod 64k
	mov	tmp_dist %+ d, f_i %+ d
	sub	tmp_dist %+ d, f_indices %+ d
	and	tmp_dist %+ d, 0xFFFF
	
	; f_indices >>= 16;
	shr	f_indices, 16

	; if ((tmp_dist-1) < (D-1)) { // should this <=??? @@@
	mov	tmp1 %+ d, tmp_dist %+ d
	sub	tmp1 %+ d, 1
	cmp	tmp1 %+ d, (D-1)
	jbe	index_loop

end_index_loop:

	; if (len >= SHORTEST_MATCH) {
	cmp	len, SHORTEST_MATCH
	jb	encode_literal

	;; encode as dist/len

MARK __len_dist_huffman
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

MARK __update_hash_for_symbol
	; for (k = f_i+1, f_i += len-1; k <= f_i; k++) {
	lea	tmp3, [f_i + 1]	; tmp3 <= k
	add	f_i, len
%ifdef LIMIT_HASH_UPDATE
	; only update hash twice

	; hash = compute_hash(state->file_start + k) & HASH_MASK;
	mov	tmp1 %+ d, [file_start + tmp3]
	compute_hash	hash, tmp1, tmp2
	and	hash %+ d, HASH_MASK
	
	mov tmp7, tmp3
	and tmp7, 0x000000000000FFFF

	; state->head[hash] = (state->head[hash] << 16) | (k & 0xFFFF);
	mov	tmp1, [stream + _internal_state_head + 8 * hash]
	shl	tmp1, 16
	or	tmp1 , tmp7
	mov	[stream + _internal_state_head + 8 * hash], tmp1

	add	tmp3,1

	; hash = compute_hash(state->file_start + k) & HASH_MASK;
	mov	tmp1 %+ d, [file_start + tmp3]
	compute_hash	hash, tmp1, tmp2
	and	hash %+ d, HASH_MASK
	
	mov tmp7, tmp3
	and tmp7, 0x000000000000FFFF

	; state->head[hash] = (state->head[hash] << 16) | (k & 0xFFFF);
	mov	tmp1, [stream + _internal_state_head + 8 * hash]
	shl	tmp1, 16
	or	tmp1 , tmp7
	mov	[stream + _internal_state_head + 8 * hash], tmp1

%else

loop3:
	mov tmp7, tmp3
	and tmp7, 0x000000000000FFFF
	; hash = compute_hash(state->file_start + k) & HASH_MASK;
	mov	tmp1 %+ d, [file_start + tmp3]
	compute_hash	hash, tmp1, tmp2
	and	hash %+ d, HASH_MASK
	; state->head[hash] = (state->head[hash] << 16) | (k & 0xFFFF);
	mov	tmp1, [stream + _internal_state_head + 8 * hash]
	shl	tmp1, 16
	or	tmp1 , tmp7
	mov	[stream + _internal_state_head + 8 * hash], tmp1

	add	tmp3,1
	cmp	tmp3, f_i
	jl	loop3
%endif

skip_loop3:

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
	cmp	f_i, [rsp + f_end_i_mem_offset]
	jl	loop2
	jmp	end_loop_2

MARK __literal_huffman
encode_literal:
	mov	tmp1 %+ d, [file_start + f_i + 1]
	
	; get_lit_code(state->file_start[f_i], &code2, &code_len2);
	movzx	tmp5, byte [file_start + f_i]
	get_lit_code	tmp5, code2, code_len2
	add	f_i,1

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
	cmp	f_i, [rsp + f_end_i_mem_offset]
	jl	loop2

MARK __end_loops
end_loop_2:

	; state->b_bytes_processed = f_i - (state->buffer - state->file_start);
	add	f_i, [stream + _internal_state_file_start]
	sub	f_i, stream
	sub	f_i, _internal_state_buffer
	mov	[stream + _internal_state_b_bytes_processed], f_i %+ d

	; continue
continue_while:
%if (COMPARE_TYPE != 1) && (COMPARE_TYPE != 4) && (COMPARE_TYPE != 5)
	mov	blen, [rsp + blen_mem_offset]
	mov	in_buf, [rsp + in_buf_mem_offset]
%endif
	cmp	blen, 0
	jnz	loop1

end:
	;; update input buffer
	; stream->total_in += (UINT32)(in_buf - stream->next_in); // bytes copied
	mov	tmp1 %+ d, [stream + _total_in]
	add	tmp1, in_buf
	sub	tmp1, [stream + _next_in]
	mov	[stream + _total_in], tmp1

	mov	[stream + _next_in], in_buf
	mov	[stream + _avail_in], blen %+ d

	mov	[stream + _next_out], m_out_buf
	; offset = state->bitbuf.buffer_used();
	sub	m_out_buf, [stream + _internal_state_bitbuf_m_out_start]
	sub	[stream + _avail_out], m_out_buf %+ d
	add	[stream + _total_out], m_out_buf %+ d

	mov	[stream + _internal_state_bitbuf_m_bits], m_bits
	mov	[stream + _internal_state_bitbuf_m_bit_count], m_bit_count %+ d

	movdqa	[stream + _internal_state_crc + 0*16], crc_0
	movdqa	[stream + _internal_state_crc + 1*16], crc_1
	movdqa	[stream + _internal_state_crc + 2*16], crc_2
	movdqa	[stream + _internal_state_crc + 3*16], crc_3

	mov rbx, [rsp + gpr_save_mem_offset + 0*8]
	mov rsi, [rsp + gpr_save_mem_offset + 1*8]
	mov rdi, [rsp + gpr_save_mem_offset + 2*8]
	mov rbp, [rsp + gpr_save_mem_offset + 3*8]
	mov r12, [rsp + gpr_save_mem_offset + 4*8]
	mov r13, [rsp + gpr_save_mem_offset + 5*8]
	mov r14, [rsp + gpr_save_mem_offset + 6*8]
	mov r15, [rsp + gpr_save_mem_offset + 7*8]
	movdqa	xmm6, [rsp + xmm_save_mem_offset + 0*16]
	movdqa	xmm7, [rsp + xmm_save_mem_offset + 1*16]
	movdqa	xmm8, [rsp + xmm_save_mem_offset + 2*16]
	movdqa	xmm9, [rsp + xmm_save_mem_offset + 3*16]

	add	rsp, stack_size
	ret

MARK __bitbuf_full
bitbuf_full:
%if (COMPARE_TYPE != 1) && (COMPARE_TYPE != 4) && (COMPARE_TYPE != 5)
	mov	blen, [rsp + blen_mem_offset]
	mov	in_buf, [rsp + in_buf_mem_offset]
%endif
	; state->b_bytes_processed = f_i - (state->buffer - state->file_start);
	add	f_i, [stream + _internal_state_file_start]
	sub	f_i, stream
	sub	f_i, _internal_state_buffer
	mov	[stream + _internal_state_b_bytes_processed], f_i %+ d
	jmp	end

section .data
	align 8
const_D: dq	D

%endif ;; if (MAJOR_VERSION == IGZIP1C)
