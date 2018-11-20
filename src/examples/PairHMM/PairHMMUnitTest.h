// ***************************************************************************
// Copyright (c) 2016-2017, Intel Corporation
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
// * Neither the name of Intel Corporation nor the names of its contributors
// may be used to endorse or promote products derived from this software
// without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
// ***************************************************************************

#ifndef PAIRHMM_H
#define PAIRHMM_H

#define QUOTE(x) #x
#define STR(x) QUOTE(x)

#define MAX_READ_LENGTH 32*1024
#define MAX_HAP_LENGTH 32*1024

#define MAX_NUM_RESULTS 64*1024*1024

#ifndef ROWS
#define ROWS 26
#endif

#ifndef COLS
#define COLS 8
#endif


#ifndef DBG
#ifdef DEBUG
#define DBG(M, ...)  printf("[AOC-DEBUG] (%s:%d) " M "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define DBG(M, ...)
#endif
#endif


typedef struct {
  char base;
  char position;
  short hap_num;
  float y_init;
} HapData;

typedef struct {
  char base;
  char position;
  short read_num;
  float mx; // Pr(match to insert gap)
  float my; // Pr(match to delete gap)
  float gg; // Pr(gap to gap)
  float mm_1m_qual; // Pr(match to match) * (1 - read_qual)
  float mm_qual_div3; // Pr(match to match) * read_qual / 3
  float gm_1m_qual; // Pr(gap to match) * (1 - read_qual)
  float gm_qual_div3; // Pr(gap to match) * read_qual / 3
} ReadData;

typedef struct {
  float m, x, y;
} PeData;

typedef struct {
  float m, x;
  bool final_result;
  bool first_hap;
  int read_num;
  int hap_num;
} RowResults;

typedef struct {
  HapData hap_row[COLS+1];
  HapData hap_col[ROWS+2];
  ReadData reads[COLS];
  PeData pe_data[2][COLS];
  float result;
  bool first_col;
  bool last_row;
} PairHmmInputData;

typedef struct {
  unsigned int read_length;
  unsigned int hap_length;
  float y_init;
  unsigned int num_rows;
} PairHmmGlobalControlData;

typedef struct {
  PeData pe_data[2][COLS];
  HapData hap_row[COLS+1];
  float result;
} PairHmmOutputData;

typedef struct {
  int result_read_num;
  int result_hap_num;
  float result;
} PairHmmResultData;

typedef struct {
  char id[64];
  char version[64];
  int rows;
  int cols;
} PairHmmAttributes;


void print_pe_data(int r, int c, ReadData read, HapData hap, PeData out) {
  read.base = read.base > 32 ? read.base : '.';
  hap.base = hap.base > 32 ? hap.base : '.';
  DBG("PE %d %d %c %c %.6e %.6e %.6e %d %d %d %d %d",
      r, c,
      read.base, hap.base,    // 0, 1
      out.m, out.x, out.y,    // 2, 3, 4
      read.position & FIRST, // 5
      read.position & LAST,  // 6
      hap.position & FIRST,  // 7
      hap.position & LAST,   // 8
      read.position & LAST && hap.position & LAST // 9
      );
}


#endif
