// Copyright(c) 2016-2017, Intel Corporation
//
// Redistribution  and  use  in source  and  binary  forms,  with  or  without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of  source code  must retain the  above copyright notice,
//   this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the name  of Intel Corporation  nor the names of its contributors
//   may be used to  endorse or promote  products derived  from this  software
//   without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,  BUT NOT LIMITED TO,  THE
// IMPLIED WARRANTIES OF  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED.  IN NO EVENT  SHALL THE COPYRIGHT OWNER  OR CONTRIBUTORS BE
// LIABLE  FOR  ANY  DIRECT,  INDIRECT,  INCIDENTAL,  SPECIAL,  EXEMPLARY,  OR
// CONSEQUENTIAL  DAMAGES  (INCLUDING,  BUT  NOT LIMITED  TO,  PROCUREMENT  OF
// SUBSTITUTE GOODS OR SERVICES;  LOSS OF USE,  DATA, OR PROFITS;  OR BUSINESS
// INTERRUPTION)  HOWEVER CAUSED  AND ON ANY THEORY  OF LIABILITY,  WHETHER IN
// CONTRACT,  STRICT LIABILITY,  OR TORT  (INCLUDING NEGLIGENCE  OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,  EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#include <omp.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>
#include <xmmintrin.h>
#include "pairhmm_common.h"

#include "PairHMMUnitTest.h"
#include "shacc_pairhmm.h"

using namespace std;
using namespace shacc_pairhmm;

int g_total_pass = 0;
int g_total_fail = 0;
float g_cpu_max_perf = 0;
float g_fpga_max_perf = 0;
long g_cpu_total_time = 0;
long g_fpga_total_time = 0;
long g_total_cells = 0;


void initPairHMM();
void computelikelihoodsfloat(testcase *testcases, float *expected_result);


void normalize(string& str, int min_value=0) {
  for (int i = 0; i < str.length(); i++) {
    str[i] = max(min_value, str[i] - 33);
  }
}

Batch read_batch(istream &is) {
  Batch batch;

  is >> batch.num_reads >> batch.num_haps >> ws;

  batch.reads = (Read*)malloc(batch.num_reads * sizeof(Read));
  batch.haps = (Haplotype*)malloc(batch.num_haps * sizeof(Haplotype));
  batch.results = (float*)malloc(sizeof(float) * batch.num_reads * batch.num_haps);

  long total_read_length = 0;
  long total_hap_length = 0;

  for (int r = 0; r < batch.num_reads; r++) {
    string bases, q, i, d, c;
    is >> bases >> q >> i >> d >> c >> ws;
    normalize(q, 6);
    normalize(i);
    normalize(d);
    normalize(c);
    int length = bases.size();
    total_read_length += length;

    Read* read = &batch.reads[r];
    read->length = length;
    read->bases = strndup(bases.c_str(), length);
    read->q = strndup(q.c_str(), length);
    read->i = strndup(i.c_str(), length);
    read->d = strndup(d.c_str(), length);
    read->c = strndup(c.c_str(), length);
  }

  for (int h = 0; h < batch.num_haps; h++) {
    string bases;
    is >> bases >> ws;
    int length = bases.size();
    total_hap_length += length;

    Haplotype* hap = &batch.haps[h];
    hap->length = length;
    hap->bases = strndup(bases.c_str(), length);
  }

  batch.num_cells = total_read_length * total_hap_length;

  return batch;
}

vector<Batch> read_testfile(string filename) {
  istream *is;
  ifstream ifs;
  if (filename == "") {
    printf("Reading test data from stdin");
    is = &std::cin;
  }
  else {
    printf("Reading test data from file: %s\n", filename.c_str());
    ifs.open(filename.c_str());
    if (!ifs.is_open()) {
      printf("Cannot open file : %s", filename.c_str());
      exit(0);
    }
    is = &ifs;
  }

  vector<Batch> batches;
  while (!is->eof()) {
    Batch batch = read_batch(*is);
    batches.push_back(batch);
  }

  return batches;
}


void check_results(Batch& batch) {
  int batch_size = batch.num_reads * batch.num_haps;
  vector<float> expected_results(batch_size);
  testcase testcases[batch_size];

  testcase *tc = testcases;
  for (int r = 0; r < batch.num_reads; r++) {
    for (int h = 0; h < batch.num_haps; h++) {
      tc->rslen = batch.reads[r].length;
      tc->haplen = batch.haps[h].length;
      tc->hap = batch.haps[h].bases;
      tc->rs = batch.reads[r].bases;
      tc->q = batch.reads[r].q;
      tc->i = batch.reads[r].i;
      tc->d = batch.reads[r].d;
      tc->c = batch.reads[r].c;
      tc++;
    }
  }

  for (int i = 0; i < batch_size; i++) {
    computelikelihoodsfloat(&testcases[i], &expected_results[i]);
  }

  return;
}

void time_pairhmm(Batch& batch, int num_pe=0, bool check=false) {

  long batch_cells = 0;
  for (int r = 0; r < batch.num_reads; r++) {
    for (int h = 0; h < batch.num_haps; h++) {
      batch_cells += batch.reads[r].length * batch.haps[h].length;
    }
  }

  g_total_cells += batch_cells;

  if (check) {
    check_results(batch);
  }
  return;
}

int main(int argc, char** argv) {
  // disable stdout buffering
  setbuf(stdout, NULL);

  initPairHMM();

  ConvertChar::init();

  vector<Batch> batches;

  // simple arg passing through env with no checking for now
  const char* env_batch = getenv("BATCH");
  int batch = env_batch ? atoi(env_batch) : -1;

  const char* env_check = getenv("CHECK");
  bool check = env_check ? (atoi(env_check) ? true : false) : false;

  #ifdef CPU_ONLY
  check = true;
  #endif

  const char* env_loop = getenv("LOOP");
  int loop = env_loop ? atoi(env_loop) : 1;

  const char* env_num_pe = getenv("NUM_PE");
  int num_pe = env_num_pe ? atoi(env_num_pe) : 0;

  const char* env_testfile = argv[1];


  // display config
  printf("Environment variable values:\n");
  printf("  TESTFILE = %s\n", env_testfile);
  printf("  BATCH    = %s\n", batch > 0 ? env_batch : "undefined (all batches)");
  printf("  NUM_PE   = %s\n", num_pe > 0 ? env_num_pe : "undefined (all PEs)");
  printf("  CHECK    = %d\n", check);
  printf("  LOOP     = %d\n", loop);

  batches = read_testfile(env_testfile);

  // run single batch or all batches
  if (batch >= 0) {
    while (loop) {
    //  printf("Batch %d", batch);
      time_pairhmm(batches[batch], num_pe, check);
      loop--;
    }
  }
  else {
    while (loop) {
      for (int i = 0; i < batches.size(); i++) {
       // printf("Batch %d", i);
        time_pairhmm(batches[i], num_pe, check);
      }
      loop--;
    }
  }

  printf("\nPairHMM completed.\n");

}
