#ifndef JAVADATA_H
#define JAVADATA_H

#include <vector>
#include "pairhmm_common.h"
#include "shacc_pairhmm.h"
#include "debug.h"

class JavaData {
 public:

  // cache field ids
  static void init(JNIEnv *env, jclass readDataHolder, jclass haplotypeDataHolder) {
    m_readBasesFid = getFieldId(env, readDataHolder, "readBases", "[B");
    m_readQualsFid = getFieldId(env, readDataHolder, "readQuals", "[B");
    m_insertionGopFid = getFieldId(env, readDataHolder, "insertionGOP", "[B");
    m_deletionGopFid = getFieldId(env, readDataHolder, "deletionGOP", "[B");
    m_overallGcpFid = getFieldId(env, readDataHolder, "overallGCP", "[B");
    m_haplotypeBasesFid = getFieldId(env, haplotypeDataHolder, "haplotypeBases", "[B");
  }

  // create array of testcases
  std::vector<testcase> getData(JNIEnv *env, jobjectArray& readDataArray, jobjectArray& haplotypeDataArray) {
    int numReads = env->GetArrayLength(readDataArray);
    int numHaplotypes = env->GetArrayLength(haplotypeDataArray);

    m_batch.num_reads = numReads;
    m_batch.num_haps = numHaplotypes;

    std::vector<char*> haplotypes;
    std::vector<int> haplotypeLengths;

    long total_hap_length = 0;
    long total_read_length = 0;
    
    // get haplotypes
    for (int i = 0; i < numHaplotypes; i++) {
      int length = 0;
      haplotypes.push_back(getCharArray(env, haplotypeDataArray, i, m_haplotypeBasesFid, length));
      haplotypeLengths.push_back(length);
      total_hap_length += length;
    }

    // get reads and create testcases 
    for (int r = 0; r < numReads; r++) {
      int length = 0;
      char* reads = getCharArray(env, readDataArray, r, m_readBasesFid, length);
      int readLength = length;
      char* insGops = getCharArray(env, readDataArray, r, m_insertionGopFid, length);
      char* delGops = getCharArray(env, readDataArray, r, m_deletionGopFid, length);
      char* gapConts = getCharArray(env, readDataArray, r, m_overallGcpFid, length);
      char* readQuals = getCharArray(env, readDataArray, r, m_readQualsFid, length);
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

  double* getOutputArray(JNIEnv *env, jdoubleArray array) {
    return getDoubleArray(env, array);
  }

  // create shacc_pairhmm::batch from array of testcases
  shacc_pairhmm::Batch getBatch() {
    int num_testcases = m_batch.num_reads * m_batch.num_haps;

    // get reads
    for (int i = 0; i < num_testcases; i += m_batch.num_haps) {
      shacc_pairhmm::Read read;
      read.bases = m_testcases[i].rs;
      read.length = m_testcases[i].rslen;
      read.i = m_testcases[i].i;
      read.d = m_testcases[i].d;
      read.c = m_testcases[i].c;
      read.q = m_testcases[i].q;
      m_reads.push_back(read);
    }
    m_batch.reads = m_reads.data();

    // get haplotypes
    for (int i = 0; i < m_batch.num_haps; i++) {
      shacc_pairhmm::Haplotype hap;
      DBG("hap #%d len = %d", i, m_testcases[i].haplen);
      hap.bases = m_testcases[i].hap;
      hap.length = m_testcases[i].haplen;
      m_haps.push_back(hap);
    }
    m_batch.haps = m_haps.data();

    m_batch.num_cells = m_total_cells;
    
    return m_batch;
  }

  void releaseData(JNIEnv *env) {
    for (int i = 0; i < m_byteArrays.size(); i++) {
      env->ReleaseByteArrayElements(m_byteArrays[i].first, m_byteArrays[i].second, 0);
    }
    for (int i = 0; i < m_doubleArrays.size(); i++) {
      env->ReleaseDoubleArrayElements(m_doubleArrays[i].first, m_doubleArrays[i].second, 0);
    }
  }

 private:
  static jfieldID getFieldId(JNIEnv *env, jclass clazz, const char *name, const char *sig) {
    jfieldID id = env->GetFieldID(clazz, name, sig);
    if (id == NULL) {
      env->ThrowNew(env->FindClass("java/lang/Exception"), "Unable to get field ID"); 
    }
    return id;
  }

  char* getCharArray(JNIEnv* env, jobjectArray& array, int index, jfieldID fieldId, int& length) {
    jobject object = env->GetObjectArrayElement(array, index);
    jbyteArray byteArray = (jbyteArray)env->GetObjectField(object, fieldId);
    jbyte* primArray = (jbyte*)env->GetByteArrayElements(byteArray, NULL);
    if (primArray == NULL) {
      env->ThrowNew(env->FindClass("java/lang/OutOfMemoryError"), "Unable to access jbyteArray");
    }
    length = env->GetArrayLength(byteArray);
    m_byteArrays.push_back(std::make_pair(byteArray, primArray));
    return (char*)primArray;
  }

  double* getDoubleArray(JNIEnv *env, jdoubleArray array) {
    jdouble* primArray = (jdouble*)env->GetDoubleArrayElements(array, NULL);
    if (primArray == NULL) {
      env->ThrowNew(env->FindClass("java/lang/OutOfMemoryError"), "Unable to access jdoubleArray");
    }
    m_doubleArrays.push_back(std::make_pair(array, primArray));
    return (double*)primArray;
  }

  shacc_pairhmm::Batch m_batch;
  std::vector<shacc_pairhmm::Read> m_reads;
  std::vector<shacc_pairhmm::Haplotype> m_haps;
  std::vector<testcase> m_testcases;
  std::vector<std::pair<jbyteArray, jbyte*> > m_byteArrays;
  std::vector<std::pair<jdoubleArray, jdouble*> > m_doubleArrays;
  long m_total_cells;
  
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
