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

%if ((MAJOR_VERSION == IGZIP0C) || (MAJOR_VERSION == IGZIP1C))

; Functional versions of CRC macros

%define NO_CRC_DATA_EXTERNS
%include "../crc_pcl.asm"


%define crc_0		xmm0	; in/out: crc state
%define crc_1		xmm1	; in/out: crc state
%define crc_2		xmm2	; in/out: crc state
%define crc_3		xmm3	; in/out: crc state
%define crc_fold	xmm4	; in:	(loaded from fold_4)
%define crc_tmp0	xmm5	; tmp
%define crc_tmp1	xmm6	; tmp
%define crc_tmp2	xmm7	; tmp
%define crc_tmp3	xmm8	; tmp
%define crc_tmp4	xmm9	; tmp
%define tmp4		rax

;; copy x bytes (rounded up to 16 bytes) from src to dst with crc
;; src & dst are unaligned
;; void copy_in_crc(UINT8 *dst, UINT8 *src, UINT32 size, UINT32 *crc)
;; arg 1: rcx: pointer to dst
;; arg 2: rdx: pointer to src
;; arg 3: r8:  size (in bytes)
;; arg 4: r9:  pointer to CRC
;global copy_in_crc
;copy_in_crc:
;%ifdef LINUX
;	mov	r9, rcx
;	mov	r8, rdx
;	mov	rdx, rsi
;	mov	rcx, rdi
;%endif
;
;	; Save xmm registers that need to be preserved.
;	sub	rsp, 8 + 4*16
;	movdqa	[rsp+0*16], xmm6
;	movdqa	[rsp+1*16], xmm7
;	movdqa	[rsp+2*16], xmm8
;	movdqa	[rsp+3*16], xmm9
;
;	movdqa	crc_0, [r9 + 0*16]
;	movdqa	crc_1, [r9 + 1*16]
;	movdqa	crc_2, [r9 + 2*16]
;	movdqa	crc_3, [r9 + 3*16]
;
;	movdqa	crc_fold, [fold_4 wrt rip]	
;	COPY_IN_CRC	rcx, rdx, r8, tmp4, crc_0, crc_1, crc_2, crc_3, \
;			crc_fold, \
;			crc_tmp0, crc_tmp1, crc_tmp2, crc_tmp3, crc_tmp4
;
;	movdqa	[r9 + 0*16], crc_0
;	movdqa	[r9 + 1*16], crc_1
;	movdqa	[r9 + 2*16], crc_2
;	movdqa	[r9 + 3*16], crc_3
;
;	movdqa	xmm9, [rsp+3*16]
;	movdqa	xmm8, [rsp+2*16]
;	movdqa	xmm7, [rsp+1*16]
;	movdqa	xmm6, [rsp+0*16]
;	add	rsp, 8 + 4*16	
;ret
	
;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; ; void init_crc(UINT32 *crc_ptr)
;; ; arg 1: rcx: pointer to (4 x 16bytes) of CRC
;; global init_crc
;; init_crc:
;; 	movdqa	crc_0, [rcx+0*16]
;; 	movdqa	crc_1, [rcx+1*16]
;; 	movdqa	crc_2, [rcx+2*16]
;; 	movdqa	crc_3, [rcx+3*16]	
;; ret	


; Convert 512-bit CRC data to real 32-bit value
; UINT32 crc_512to32(UINT32 *crc)
; arg 1:   rcx: pointer to CRC
; returns: eax: 32 bit crc
global crc_512to32 :function hidden
crc_512to32:	
%ifdef LINUX
	mov	rcx, rdi
%endif

	movdqa	crc_0, [rcx + 0*16]
	movdqa	crc_1, [rcx + 1*16]
	movdqa	crc_2, [rcx + 2*16]
	movdqa	crc_3, [rcx + 3*16]
	
	movdqa crc_fold, [rk1 wrt rip]		;k1
	
	; fold the 4 xmm registers to 1 xmm register with different constants
	movdqa crc_tmp0, crc_0
	pclmulqdq crc_0, crc_fold, 0x1
	pclmulqdq crc_tmp0, crc_fold, 0x10
	pxor crc_1, crc_tmp0
	pxor crc_1, crc_0
	
	movdqa crc_tmp0, crc_1
	pclmulqdq crc_1, crc_fold, 0x1
	pclmulqdq crc_tmp0, crc_fold, 0x10
	pxor crc_2, crc_tmp0
	pxor crc_2, crc_1
    
	movdqa crc_tmp0, crc_2
	pclmulqdq crc_2, crc_fold, 0x1
	pclmulqdq crc_tmp0, crc_fold, 0x10
	pxor crc_3, crc_tmp0
	pxor crc_3, crc_2		


	movdqa crc_fold, [rk5 wrt rip]
	movdqa crc_0, crc_3
	
	pclmulqdq crc_3, crc_fold, 0

	psrldq crc_0, 8

	pxor crc_3, crc_0
	
	movdqa crc_0, crc_3
	
	
	pslldq crc_3, 4	

	pclmulqdq crc_3, crc_fold, 0x10
	
	
	pxor crc_3, crc_0
	
	pand crc_3, [mask2 wrt rip]
	
	movdqa crc_1, crc_3
	
	movdqa crc_2, crc_3

	movdqa crc_fold, [rk7 wrt rip]


	pclmulqdq crc_3, crc_fold, 0
	pxor crc_3, crc_2
	
	pand crc_3, [mask wrt rip]
	
	movdqa crc_2, crc_3
	
	pclmulqdq crc_3, crc_fold, 0x10
	
	pxor crc_3, crc_2
	
	pxor crc_3, crc_1

	pextrd eax, crc_3, 2    

	not eax
	
	ret	

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


; precomputed constants	
section .data

align 32

global pshufb_shf_table :data hidden
global fold_4 :data hidden
global rk1 :data hidden
global rk5 :data hidden
global rk7 :data hidden
global mask :data hidden
global mask2 :data hidden
global mask3 :data hidden

pshufb_shf_table:
ddq 0x008f8e8d8c8b8a898887868584838281 ; shl 15 (16-1) / shr1
ddq 0x01008f8e8d8c8b8a8988878685848382 ; shl 14 (16-3) / shr2
ddq 0x0201008f8e8d8c8b8a89888786858483 ; shl 13 (16-4) / shr3
ddq 0x030201008f8e8d8c8b8a898887868584 ; shl 12 (16-4) / shr4
ddq 0x04030201008f8e8d8c8b8a8988878685 ; shl 11 (16-5) / shr5
ddq 0x0504030201008f8e8d8c8b8a89888786 ; shl 10 (16-6) / shr6
ddq 0x060504030201008f8e8d8c8b8a898887 ; shl 9  (16-7) / shr7
ddq 0x07060504030201008f8e8d8c8b8a8988 ; shl 8  (16-8) / shr8
ddq 0x0807060504030201008f8e8d8c8b8a89 ; shl 7  (16-9) / shr9
ddq 0x090807060504030201008f8e8d8c8b8a ; shl 6  (16-10) / shr10
ddq 0x0a090807060504030201008f8e8d8c8b ; shl 5  (16-11) / shr11
ddq 0x0b0a090807060504030201008f8e8d8c ; shl 4  (16-12) / shr12
ddq 0x0c0b0a090807060504030201008f8e8d ; shl 3  (16-13) / shr13
ddq 0x0d0c0b0a090807060504030201008f8e ; shl 2  (16-14) / shr14
ddq 0x0e0d0c0b0a090807060504030201008f ; shl 1  (16-15) / shr15

;; ; MAGIC value, which when folded 4 times gives FFFFFF00000...0000
;; global crc_init_4
;; crc_init_4:
;; ddq 0x9db42487
;; ddq 0x0
;; ddq 0x0
;; ddq 0x0
	
; constant used to shift/fold one XMM reg down by 4 XMM widths
fold_4:
dq 0x00000001c6e41596
dq 0x0000000154442bd4
	
; constants used in crc_512to32
align 16
rk1:	dq 0x00000000ccaa009e
rk2:	dq 0x00000001751997d0
rk5:	dq 0x00000000ccaa009e
rk6:	dq 0x0000000163cd6124
rk7:	dq 0x00000001f7011640
rk8:	dq 0x00000001db710640
mask:	dq 0xFFFFFFFFFFFFFFFF, 0x0000000000000000
mask2:	dq 0xFFFFFFFF00000000, 0xFFFFFFFFFFFFFFFF	

;value, which when xored with pshufb_shf_table entry gives shr value
mask3:	ddq 0x80808080808080808080808080808080	

%endif ;; if ((MAJOR_VERSION == IGZIP0C) || (MAJOR_VERSION == IGZIP1C))
