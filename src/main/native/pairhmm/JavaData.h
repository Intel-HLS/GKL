/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2021 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef JAVADATA_H
#define JAVADATA_H

#include <vector>
#include <debug.h>
#include <exception>
#include <string>
#include "pairhmm_common.h"

class JavaException : std::exception {
  public:
  const char* classPath;
  const char* message;

  JavaException(const char* classPath, const char* message)
    : classPath(classPath),
      message(message)
  { }
};

class JavaData {
 public:
  JavaData(JNIEnv *env) : m_env(env) {
  
  }

  virtual ~JavaData() {
    releaseData();
  }

  // cache field ids
  void init(jclass readDataHolder, jclass haplotypeDataHolder) {
    m_readBasesFid = getFieldId(readDataHolder, "readBases", "[B");
    m_readQualsFid = getFieldId(readDataHolder, "readQuals", "[B");
    m_insertionGopFid = getFieldId(readDataHolder, "insertionGOP", "[B");
    m_deletionGopFid = getFieldId(readDataHolder, "deletionGOP", "[B");
    m_overallGcpFid = getFieldId(readDataHolder, "overallGCP", "[B");
    m_haplotypeBasesFid = getFieldId(haplotypeDataHolder, "haplotypeBases", "[B");
  }

  // create array of testcases
  std::vector<testcase> getData(jobjectArray& readDataArray, jobjectArray& haplotypeDataArray) {
    int numReads = m_env->GetArrayLength(readDataArray);
    int numHaplotypes = m_env->GetArrayLength(haplotypeDataArray);

    std::vector<char*> haplotypes;
    std::vector<int> haplotypeLengths;

    long total_hap_length = 0;
    long total_read_length = 0;
    
    // get haplotypes
    for (int i = 0; i < numHaplotypes; i++) {
      int length = 0;
      haplotypes.push_back(getCharArray(haplotypeDataArray, i, m_haplotypeBasesFid, length));
      haplotypeLengths.push_back(length);
      total_hap_length += length;
    }

    // get reads and create testcases 
    for (int r = 0; r < numReads; r++) {
      int length = 0;
      char* reads = getCharArray(readDataArray, r, m_readBasesFid, length);
      int readLength = length;
      char* insGops = getCharArray(readDataArray, r, m_insertionGopFid, length);
      char* delGops = getCharArray(readDataArray, r, m_deletionGopFid, length);
      char* gapConts = getCharArray(readDataArray, r, m_overallGcpFid, length);
      char* readQuals = getCharArray(readDataArray, r, m_readQualsFid, length);
      total_read_length += length;
      
      for (int h = 0; h < numHaplotypes; h++) {
        testcase tc;
        tc.hap = haplotypes[h];
        tc.haplen = haplotypeLengths[h];
        tc.rs = reads;
        tc.rslen = readLength;
        tc.i = insGops;
        tc.d = delGops;
        tc.c = gapConts;
        tc.q = readQuals;
        m_testcases.push_back(tc);
      }
    }

    m_total_cells = total_read_length * total_hap_length;

    return m_testcases;
  }

  double* getOutputArray(jdoubleArray array) {
    return getDoubleArray(array);
  }

 private:
  void releaseData() {
    for (int i = 0; i < m_byteArrays.size(); i++) {
      m_env->ReleaseByteArrayElements(m_byteArrays[i].first, m_byteArrays[i].second, 0);
    }
    for (int i = 0; i < m_doubleArrays.size(); i++) {
      m_env->ReleaseDoubleArrayElements(m_doubleArrays[i].first, m_doubleArrays[i].second, 0);
    }
  }

  jfieldID getFieldId(jclass clazz, const char *name, const char *sig) {
    jfieldID id = m_env->GetFieldID(clazz, name, sig);
    if (id == NULL) {
      throw JavaException("java/lang/IllegalArgumentException", "Unable to get field ID");
    }
    return id;
  }

  char* getCharArray(jobjectArray& array, int index, jfieldID fieldId, int& length) {
    jobject object = m_env->GetObjectArrayElement(array, index);
    jbyteArray byteArray = (jbyteArray)m_env->GetObjectField(object, fieldId);
    jbyte* primArray = (jbyte*)m_env->GetByteArrayElements(byteArray, NULL);
    if (primArray == NULL) {
      throw JavaException("java/lang/OutOfMemoryError", "Unable to access jbyteArray");
    }
    length = m_env->GetArrayLength(byteArray);
    m_byteArrays.push_back(std::make_pair(byteArray, primArray));
    return (char*)primArray;
  }

  double* getDoubleArray(jdoubleArray array) {
    jdouble* primArray = (jdouble*)m_env->GetDoubleArrayElements(array, NULL);
    if (primArray == NULL) {
      throw JavaException("java/lang/OutOfMemoryError", "Unable to access jdoubleArray");
    }
    m_doubleArrays.push_back(std::make_pair(array, primArray));
    return (double*)primArray;
  }

  std::vector<testcase> m_testcases;
  std::vector<std::pair<jbyteArray, jbyte*> > m_byteArrays;
  std::vector<std::pair<jdoubleArray, jdouble*> > m_doubleArrays;
  long m_total_cells;

  JNIEnv* m_env;
  
  static jfieldID m_readBasesFid;
  static jfieldID m_readQualsFid;
  static jfieldID m_insertionGopFid;
  static jfieldID m_deletionGopFid;
  static jfieldID m_overallGcpFid;
  static jfieldID m_haplotypeBasesFid;  
};

jfieldID JavaData::m_readBasesFid;
jfieldID JavaData::m_readQualsFid;
jfieldID JavaData::m_insertionGopFid;
jfieldID JavaData::m_deletionGopFid;
jfieldID JavaData::m_overallGcpFid;
jfieldID JavaData::m_haplotypeBasesFid;  

#endif
