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
%ifndef __OPTIONS_ASM__
%define __OPTIONS_ASM__

; Options:dir
; c - Use PCLMULQDQ for CRC calc
; m - reschedule mem reads
; e b - bitbuff style
; t s x - compare style
; h - limit hash updates
; l - use longer huffman table
; f - fix cache read

; Invalid Combinations
; --------+----+-------+-------+-------+-------+------------------
;         |          COMPARE TYPE              | LIMIT_HASH_UPDATE
;         | 1  | 2 (t) | 3 (s) | 4 (x) | 5 (y) |
;         +----+-------+-------+-------+-------+------------------
; IGZIP0  | no |       |  no   |  no   |       |
; IGZIP0c |    |  no   |  no   |       |       |
; IGZIP1  | no |       |  no   |  no   |       |      no
; IGZIP1c |    |       |  no   |       |       |
; --------+----+-------+-------+-------+-------+------------------

; All versions are "m" variants
%define IGZIP0  1
%define IGZIP0C 2
%define IGZIP1  3
%define IGZIP1C 4

%ifndef MAJOR_VERSION
%define MAJOR_VERSION IGZIP0C
%endif

%define HIST_SIZE 8
;%define HIST_SIZE 32

; bit buffer types
; BITBUF8: (e) Always write 8 bytes of data
; BITBUFB: (b) Always write data
;%define USE_BITBUF8
%define USE_BITBUFB

; compare types
; 1: ( ) original
; 2: (t) with CMOV
; 3: (s) with sttni
; 4: (x) with xmm / pmovbmsk
; 5: (y) with ymm / pmovbmsk (32-bytes at a time)
%ifndef COMPARE_TYPE
%define COMPARE_TYPE 2
%endif

%if ((MAJOR_VERSION == IGZIP0C) && (COMPARE_TYPE == 2))
%undef COMPARE_TYPE
%define COMPARE_TYPE 1
%endif

; (h) limit hash update
;    not supported in version IGZIP1
%if (MAJOR_VERSION != IGZIP1)
%define LIMIT_HASH_UPDATE
%endif

; (l) longer huffman table
%define LONGER_HUFFTABLE

; (f) fix cache read problem
%define FIX_CACHE_READ

; (a) Use HSW-specific instructions, e.g. SHLX
;%define USE_HSWNI

%if (HIST_SIZE > 8)
%undef LONGER_HUFFTABLE
%endif

;%define TEST
;%define DO_DBGPRINT

;%define GENOME_SAM
;%define GENOME_BAM
;%define ONLY_DEFLATE

%ifdef GENOME_SAM
%define WIDER_LIT_TABLES
%endif
%ifdef GENOME_BAM
%define WIDER_LIT_TABLES
%endif

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

%endif ; ifndef __OPTIONS_ASM__
