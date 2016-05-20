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
%ifndef __HUFFMAN_ASM__
%define __HUFFMAN_ASM__

%include "options.asm"
%include "lz0a_const.asm"

; Macros for doing Huffman Encoding

%ifdef LONGER_HUFFTABLE
	%if (D > 8192)	
		%error History D is larger than 8K, cannot use %LONGER_HUFFTABLE
		 % error
	%else
		%define DIST_TABLE_SIZE 8192
		%define DECODE_OFFSET     26	
	%endif
%else
	%define DIST_TABLE_SIZE 1024
	%define DECODE_OFFSET     20
%endif

;extern const struct HuffTables {
;
;    // bits 7:0 are the length
;    // bits 31:8 are the code
;    UINT32 dist_table[DIST_TABLE_SIZE];
;
;    // bits 7:0 are the length
;    // bits 31:8 are the code
;    UINT32 len_table[256];
;
;    // bits 3:0 are the length
;    // bits 15:4 are the code
;    UINT16 lit_table[257];
;
;    // bits 3:0 are the length
;    // bits 15:4 are the code
;    UINT16 dcodes[30 - DECODE_OFFSET];
;
;} huff_tables;

extern huff_tables
extern dist_table
extern len_table
extern lit_table
%ifndef LONGER_HUFFTABLE
extern dcodes
%endif

%ifdef LONGER_HUFFTABLE
; Uses RCX, clobbers dist
; get_dist_code	dist, code, len
%macro get_dist_code 3
%define %%dist %1d	; 32-bit IN, clobbered
%define %%distq %1	; 64-bit IN
%define %%code %2d	; 32-bit OUT
%define %%len  %3d	; 32-bit OUT
%define %%lenq %3	; 64-bit TMP

;	mov	%%len, [dist_table - 4 + 4*%%dist]
	lea	%%lenq, [dist_table - 4 wrt rip]
	mov	%%len, [%%lenq + 4*%%distq]
	mov	%%code, %%len
	and	%%len, 0xFF;
	shr	%%code, 8
%endm

%else
; Assumes (dist != 0)
; Uses RCX, clobbers dist
; void compute_dist_code	dist, code, len
%macro compute_dist_code 3
%define %%dist %1	; IN, clobbered
%define %%code %2	; OUT
%define %%len  %3	; OUT

	dec	%%dist
	bsr	ecx, %%dist	; ecx = msb = bsr(dist)
	dec	ecx		; ecx = num_extra_bits = msb - N
	mov	%%code, 1
	shl	%%code, CL
	dec	%%code		; code = ((1 << num_extra_bits) - 1)
	and	%%code, %%dist	; code = extra_bits
	shr	%%dist, CL	; dist >>= num_extra_bits
	lea	%%dist, [%%dist + 2*ecx] ; dist = sym = dist + num_extra_bits*2
	movzx	%%dist, word [dcodes + 2 * (%%dist - DECODE_OFFSET)]
	mov	%%len, ecx	; len = num_extra_bits
	mov	ecx, %%dist	; ecx = sym
	and	ecx, 0xF	; ecx = sym & 0xF
	shl	%%code, CL	; code = extra_bits << (sym & 0xF)
	shr	%%dist, 4	; dist = (sym >> 4)
	or	%%code, %%dist	; code = (sym >> 4) | (extra_bits << (sym & 0xF))
	add	%%len, ecx	; len = num_extra_bits + (sym & 0xF)
%endm

; Uses RCX, clobbers dist
; get_dist_code	dist, code, len
%macro get_dist_code 3
%define %%dist %1d	; 32-bit IN, clobbered
%define %%code %2d	; 32-bit OUT
%define %%len  %3d	; 32-bit OUT

	cmp	%%dist, DIST_TABLE_SIZE
	jg	%%do_compute
	mov	%%len, [dist_table - 4 + 4*%%dist]
	mov	%%code, %%len
	and	%%len, 0xFF;
	shr	%%code, 8
	jmp	%%done
%%do_compute:
	compute_dist_code	%%dist, %%code, %%len
%%done:
%endm

%endif


; "len" can be same register as "length"
; get_len_code	length, code, len 
%macro get_len_code 3
%define %%length %1d	; 32-bit IN
%define %%lengthq %1	; 64-bit IN
%define %%code %2d	; 32-bit OUT
%define %%len  %3d	; 32-bit OUT
%define %%lenq %3	; 64-bit TMP

;	mov	%%len, [len_table - 12 + 4*%%length]
	lea	%%lenq, [len_table - 12 wrt rip]
	mov	%%len, [%%lenq + 4*%%lengthq]
	mov	%%code, %%len
	and	%%len, 0xFF;
	shr	%%code, 8
%endm

%ifdef WIDER_LIT_TABLES
; Use 4-byte lit tables

; "len" can be same register as "lit"
; get_lit_code	lit, code, len 
%macro get_lit_code 3
%define %%lit %1d	; 32-bit IN
%define %%litq %1	; 64-bit IN
%define %%code %2d	; 32-bit OUT
%define %%len  %3d	; 32-bit OUT
%define %%lenq %3	; 64-bit TMP
	
;	movzx	%%len, word [lit_table + 4*%%lit]
	lea	%%lenq, [lit_table wrt rip]
	mov	%%len, dword [%%lenq + 4*%%litq]
	mov	%%code, %%len
	and	%%len, 0xF;
	shr	%%code, 4
%endm
%macro get_lit_code_const 3
%define %%lit %1	; 32-bit IN  (constant)
%define %%litq %1	; 64-bit IN
%define %%code %2d	; 32-bit OUT
%define %%len  %3d	; 32-bit OUT
%define %%lenq %3	; 64-bit TMP
	
	mov	%%len, dword [lit_table + 4*%%lit wrt rip]
	mov	%%code, %%len
	and	%%len, 0xF;
	shr	%%code, 4
%endm

%else
; Use 2-byte lit tables

; "len" can be same register as "lit"
; get_lit_code	lit, code, len 
%macro get_lit_code 3
%define %%lit %1d	; 32-bit IN
%define %%litq %1	; 64-bit IN
%define %%code %2d	; 32-bit OUT
%define %%len  %3d	; 32-bit OUT
%define %%lenq %3	; 64-bit TMP
	
;	movzx	%%len, word [lit_table + 2*%%lit]
	lea	%%lenq, [lit_table wrt rip]
	movzx	%%len, word [%%lenq + 2*%%litq]
	mov	%%code, %%len
	and	%%len, 0xF;
	shr	%%code, 4
%endm
%macro get_lit_code_const 3
%define %%lit %1	; 32-bit IN  (constant)
%define %%litq %1	; 64-bit IN
%define %%code %2d	; 32-bit OUT
%define %%len  %3d	; 32-bit OUT
%define %%lenq %3	; 64-bit TMP
	
;	movzx	%%len, word [lit_table + 2*%%lit]
	lea	%%lenq, [lit_table wrt rip]
	movzx	%%len, word [%%lenq + 2*%%litq]
	mov	%%code, %%len
	and	%%len, 0xF;
	shr	%%code, 4
%endm

%endif ;; end WIDER_LIT_TABLES / else

%endif
