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
%ifndef __UTILS_ASM__
%define __UTILS_ASM__

%include "options.asm"

%ifdef FIX_CACHE_READ
%define movntdqa movdqa
%else
%macro prefetchnta 1
%endm
%endif

;%assign K   1024;
;%assign D   8 * K;       ; Amount of history
;%assign LA  17 * 16;     ; Max look-ahead, rounded up to 32 byte boundary

; copy D + LA bytes from src to dst
; dst is aligned
;void copy_D_LA(UINT8 *dst, UINT8 *src);
; arg 1: rcx : dst
; arg 2: rdx : src
; copy_D_LA dst, src, tmp, xtmp0, xtmp1, xtmp2, xtmp3
%macro	copy_D_LA 7
%define %%dst	%1	; reg, clobbered
%define %%src	%2	; reg, clobbered
%define %%tmp	%3
%define %%xtmp0 %4
%define %%xtmp1 %5
%define %%xtmp2 %6
%define %%xtmp3 %7

%assign %%SIZE (D + LA) / 16   ; number of DQ words to be copied
%assign %%SIZE4 %%SIZE/4

	lea	%%tmp, [%%dst + 4 * 16 * %%SIZE4]
	jmp	%%copy_D_LA_1
align 16
%%copy_D_LA_1:
	movdqu	%%xtmp0, [%%src]
	movdqu	%%xtmp1, [%%src+16]
	movdqu	%%xtmp2, [%%src+32]
	movdqu	%%xtmp3, [%%src+48]
	movdqa	[%%dst],    %%xtmp0
	movdqa	[%%dst+16], %%xtmp1
	movdqa	[%%dst+32], %%xtmp2
	movdqa	[%%dst+48], %%xtmp3
	add	%%src, 4*16
	add	%%dst, 4*16
	cmp	%%dst, %%tmp
	jne	%%copy_D_LA_1
%assign %%i 0
%rep (%%SIZE - 4 * %%SIZE4)

%if (%%i == 0)
	movdqu	%%xtmp0, [%%src + %%i*16]
%elif (%%i == 1)
	movdqu	%%xtmp1, [%%src + %%i*16]
%elif (%%i == 2)
	movdqu	%%xtmp2, [%%src + %%i*16]
%elif (%%i == 3)
	movdqu	%%xtmp3, [%%src + %%i*16]
%else
	%error too many i
	 % error
%endif

%assign %%i %%i+1
%endrep
%assign %%i 0
%rep (%%SIZE - 4 * %%SIZE4)

%if (%%i == 0)
	movdqa	[%%dst + %%i*16], %%xtmp0
%elif (%%i == 1)
	movdqa	[%%dst + %%i*16], %%xtmp1
%elif (%%i == 2)
	movdqa	[%%dst + %%i*16], %%xtmp2
%elif (%%i == 3)
	movdqa	[%%dst + %%i*16], %%xtmp3
%else
	%error too many i
	 % error
%endif

%assign %%i %%i+1
%endrep
%endm

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; copy x bytes (rounded up to 16 bytes) from src to dst
; src & dst are unaligned
;void copy_in(UINT8 *dst, UINT8 *src, UINT32 x);
; arg 1: rcx : dst
; arg 2: rdx : src
; arg 3: r8  : size in bytes
; clobbers xmm0...xmm3
; clobbers all arguments
; copy_in dst, src, size_in_bytes, tmp
%macro copy_in 4
%define %%dst	%1	; reg, clobbered
%define %%src	%2	; reg, clobbered
%define %%size	%3	; reg, clobbered
%define %%tmp	%4

	; align source
	xor	%%tmp, %%tmp
	sub	%%tmp, %%src
	and	%%tmp, 15
	jz	%%copy_in_1
	; need to align
	movdqu	xmm0, [%%src]
	movdqu	[%%dst], xmm0
	add	%%dst, %%tmp
	add	%%src, %%tmp
	sub	%%size, %%tmp
%%copy_in_1:
	sub	%%size, 49
	jl	%%copy_in_3
	jmp	%%copy_in_2
align 16
%%copy_in_2:
;;	prefetchnta	[%%src+0*16]
	movntdqa	xmm0, [%%src]
	movntdqa	xmm1, [%%src+16]
	movntdqa	xmm2, [%%src+32]
	movntdqa	xmm3, [%%src+48]
	movdqu	[%%dst], xmm0
	movdqu	[%%dst+16], xmm1
	movdqu	[%%dst+32], xmm2
	movdqu	[%%dst+48], xmm3
	add	%%src, 4*16
	add	%%dst, 4*16
	sub	%%size, 4*16
	jge	%%copy_in_2
%%copy_in_3:
	; %%size contains (num bytes left - 49)
	; range: -64 ... -49 :  0 bytes left
	;        -48 ... -33 : 16 bytes left
	;        -32 ... -17 : 32 bytes left
	;        -17 ...  -1 : 48 bytes left
	add	%%size, 48
	jl	%%copy_in_4
	movntdqa	xmm0, [%%src]
	movdqu	[%%dst], xmm0
	cmp	%%size, 16
	jl	%%copy_in_4
	movntdqa	xmm0, [%%src+16]
	movdqu	[%%dst+16], xmm0
	cmp	%%size, 32
	jl	%%copy_in_4
	movntdqa	xmm0, [%%src+32]
	movdqu	[%%dst+32], xmm0
%%copy_in_4:
%endm

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; compare macro

;; sttni2 is faster, but it can't be debugged
;; so following code is based on "mine5"

;; compare 258 bytes = 8 * 32 + 2
;; tmp16 is a 16-bit version of tmp
;; compare258 src1, src2, result, tmp
%macro compare258 4
%define %%src1		%1
%define %%src2		%2
%define %%result	%3
%define %%tmp		%4
%define %%tmp16		%4w	; tmp as a 16-bit register

	xor	%%result, %%result
%%loop1:
	mov	%%tmp, [%%src1 + %%result]
	xor	%%tmp, [%%src2 + %%result]
	jnz	%%miscompare
	add	%%result, 8

	mov	%%tmp, [%%src1 + %%result]
	xor	%%tmp, [%%src2 + %%result]
	jnz	%%miscompare
	add	%%result, 8

	cmp	%%result, 256
	jb	%%loop1	

	; compare last two bytes
	mov	%%tmp16, [%%src1 + %%result]
	xor	%%tmp16, [%%src2 + %%result]
	jnz	%%miscompare16

	; no miscompares, return 258
	add	%%result, 2
	jmp	%%end

%%miscompare16:
	and	%%tmp, 0xFFFF
%%miscompare:
	bsf	%%tmp, %%tmp
	shr	%%tmp, 3
	add	%%result, %%tmp
%%end:
%endm


;; compare 258 bytes = 8 * 32 + 2
;; compare258_2 src1, src2, result, tmp, tmp2, tmp3
%macro compare258_2 6
%define %%src1		%1
%define %%src2		%2
%define %%result	%3
%define %%tmp		%4
%define %%tmp2		%5
%define %%tmp3		%6

	xor	%%tmp3, %%tmp3
%%loop1:
	mov	%%tmp, [%%src1 + %%tmp3 + 8]
	mov	%%tmp2, [%%src1 + %%tmp3]
	lea	%%result, [%%tmp3 + 8]
	xor	%%tmp, [%%src2 + %%tmp3 + 8]
	xor	%%tmp2, [%%src2 + %%tmp3]

	cmovnz	%%tmp, %%tmp2
	cmovnz	%%result, %%tmp3

	test	%%tmp, %%tmp
	jnz	%%miscompare

	add	%%tmp3, 16

	cmp	%%tmp3, 256
	jb	%%loop1	

	; compare last two bytes
	mov	%%tmp %+ w, [%%src1 + %%tmp3]
	xor	%%tmp %+ w, [%%src2 + %%tmp3]
	jnz	%%miscompare16

	; no miscompares, return 258
	mov	%%result, 258
	jmp	%%end

%%miscompare16:
	and	%%tmp, 0xFFFF
	mov	%%result, %%tmp3
%%miscompare:
	bsf	%%tmp, %%tmp
	shr	%%tmp, 3
	add	%%result, %%tmp
%%end:
%endm


;; compare with STTNI (pcmpestri)
;; compare 258 bytes = 8 * 32 + 2
;; compare258_s src1, src2, result, xtmp
;; EAX, ECX, EDX are clobbered
%macro compare258_s 4
%define %%src1		%1
%define %%src2		%2
%define %%result	%3
%define %%xtmp		%4

	xor	%%result, %%result
	mov	eax, 16		; set up length register
	mov	edx, 16		; set up length register
%%loop1:
	movdqu		%%xtmp, [%%src1 + %%result]
	pcmpestri	%%xtmp, [%%src2 + %%result], 0x18
	; CF is cleared if all bytes equal
	jc		%%miscompare
	add		%%result, 16
	
	movdqu		%%xtmp, [%%src1 + %%result]
	pcmpestri	%%xtmp, [%%src2 + %%result], 0x18
	; CF is cleared if all bytes equal
	jc		%%miscompare
	add		%%result, 16
	
	cmp		%%result, 256
	jb		%%loop1

	; compare last two bytes
	mov	ax, [%%src1 + %%result]
	xor	ax, [%%src2 + %%result]
	jnz	%%miscompare16

	; no miscompares, return 258
	add	%%result, 2
	jmp	%%end

%%miscompare16:
	; miscompare in last two bytes
	bsf	ecx, eax
	shr	ecx, 3

%%miscompare:
	add	%%result, rcx
%%end:
%endm

;; compare 258 bytes = 8 * 32 + 2
;; compares 16 bytes at a time, using pcmpeqb/pmovmskb
;; compare258_x src1, src2, result, tmp, xtmp1, xtmp2
%macro compare258_x 6
%define %%src1		%1
%define %%src2		%2
%define %%result	%3
%define %%tmp		%4
%define %%tmp16		%4w	; tmp as a 16-bit register
%define %%xtmp		%5
%define %%xtmp2		%6

	xor	%%result, %%result
%%loop1:
	movdqu		%%xtmp, [%%src1 + %%result]
	movdqu		%%xtmp2, [%%src2 + %%result]
	pcmpeqb		%%xtmp, %%xtmp2
	pmovmskb	%%tmp, %%xtmp
	xor		%%tmp, 0xFFFF
	jnz		%%miscompare
	add		%%result, 16

	movdqu		%%xtmp, [%%src1 + %%result]
	movdqu		%%xtmp2, [%%src2 + %%result]
	pcmpeqb		%%xtmp, %%xtmp2
	pmovmskb	%%tmp, %%xtmp
	xor		%%tmp, 0xFFFF
	jnz		%%miscompare
	add		%%result, 16

	cmp	%%result, 256
	jb	%%loop1	

	; compare last two bytes
	mov	%%tmp16, [%%src1 + %%result]
	xor	%%tmp16, [%%src2 + %%result]
	jnz	%%miscompare16

	; no miscompares, return 258
	add	%%result, 2
	jmp	%%end

%%miscompare16:
	and	%%tmp, 0xFFFF
	bsf	%%tmp, %%tmp
	shr	%%tmp, 3
	add	%%result, %%tmp
	jmp	%%end
%%miscompare:
	bsf	%%tmp, %%tmp
	add	%%result, %%tmp
%%end:
%endm

%if (COMPARE_TYPE == 5)
;; compare 258 bytes = 8 * 32 + 2
;; compares 32 bytes at a time, using pcmpeqb/pmovmskb
;; compare258_y src1, src2, result, tmp, xtmp1, xtmp2
%macro compare258_y 6
%define %%src1		%1
%define %%src2		%2
%define %%result	%3
%define %%tmp		%4
%define %%tmp16		%4w	; tmp as a 16-bit register
%define %%tmp32		%4d	; tmp as a 32-bit register
%define %%ytmp		%5
%define %%ytmp2		%6

	xor	%%result, %%result
%%loop1:
	vmovdqu		%%ytmp, [%%src1 + %%result]
	vmovdqu		%%ytmp2, [%%src2 + %%result]
	vpcmpeqb	%%ytmp, %%ytmp, %%ytmp2
	vpmovmskb	%%tmp, %%ytmp
	xor		%%tmp32, 0xFFFFFFFF
	jnz		%%miscompare
	add		%%result, 32

	vmovdqu		%%ytmp, [%%src1 + %%result]
	vmovdqu		%%ytmp2, [%%src2 + %%result]
	vpcmpeqb	%%ytmp, %%ytmp, %%ytmp2
	vpmovmskb	%%tmp, %%ytmp
	xor		%%tmp32, 0xFFFFFFFF
	jnz		%%miscompare
	add		%%result, 32

	cmp	%%result, 256
	jb	%%loop1	

	; compare last two bytes
	mov	%%tmp16, [%%src1 + %%result]
	xor	%%tmp16, [%%src2 + %%result]
	jnz	%%miscompare16

	; no miscompares, return 258
	add	%%result, 2
	jmp	%%end

%%miscompare16:
	and	%%tmp, 0xFFFF
	bsf	%%tmp, %%tmp
	shr	%%tmp, 3
	add	%%result, %%tmp
	jmp	%%end
%%miscompare:
	bsf	%%tmp, %%tmp
	add	%%result, %%tmp
%%end:
%endm
%endif ;; if COMPARE_TYPE == 5

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;; compare size, src1, src2, result, tmp
%macro compare 5
%define %%size		%1
%define %%src1		%2
%define %%src2		%3
%define %%result	%4
%define %%tmp		%5
%define %%tmp8		%5b	; tmp as a 8-bit register

	xor	%%result, %%result
	sub	%%size, 7
	jle	%%lab2
%%loop1:
	mov	%%tmp, [%%src1 + %%result]
	xor	%%tmp, [%%src2 + %%result]
	jnz	%%miscompare
	add	%%result, 8
	sub	%%size, 8
	jg	%%loop1
%%lab2:
	;; if we fall through from above, we have found no mismatches,
	;; %%size+7 is the number of bytes left to look at, and %%result is the
	;; number of bytes that have matched
	add	%%size, 7
	jle	%%end
%%loop3:
	mov	%%tmp8, [%%src1 + %%result]
	cmp	%%tmp8, [%%src2 + %%result]
	jne	%%end
	inc	%%result
	dec	%%size
	jg	%%loop3
	jmp	%%end
%%miscompare:
	bsf	%%tmp, %%tmp
	shr	%%tmp, 3
	add	%%result, %%tmp
%%end:
%endm

%endif
