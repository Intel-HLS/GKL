#ifndef JAVADATA_H
#define JAVADATA_H

#include <vector>
#include "pairhmm_common.h"

class JavaData {
 public:
  static void init(JNIEnv *env, jclass readDataHolder, jclass haplotypeDataHolder) {
    m_readBasesFid = getFieldId(env, readDataHolder, "readBases", "[B");
    m_readQualsFid = getFieldId(env, readDataHolder, "readQuals", "[B");
    m_insertionGopFid = getFieldId(env, readDataHolder, "insertionGOP", "[B");
    m_deletionGopFid = getFieldId(env, readDataHolder, "deletionGOP", "[B");
    m_overallGcpFid = getFieldId(env, readDataHolder, "overallGCP", "[B");
    m_haplotypeBasesFid = getFieldId(env, haplotypeDataHolder, "haplotypeBases", "[B");
  }

  std::vector<testcase> getData(JNIEnv *env, jobjectArray& readDataArray, jobjectArray& haplotypeDataArray) {
    int numReads = env->GetArrayLength(readDataArray);
    int numHaplotypes = env->GetArrayLength(haplotypeDataArray);

    testcases.resize(numReads * numHaplotypes);

    std::vector<char*> haplotypes(numHaplotypes);
    std::vector<int> haplotypeLengths(numHaplotypes);
    
    // get haplotypes
    for (int i = 0; i < numHaplotypes; i++) {
      int length = 0;
      haplotypes[i] = getCharArray(env, haplotypeDataArray, i, m_haplotypeBasesFid, length);
      haplotypeLengths[i] = length;
    }
    
    // get reads and create testcases 
    int i = 0;
    for (int r = 0; r < numReads; r++) {
      int length = 0;
      char* reads = getCharArray(env, readDataArray, r, m_readBasesFid, length);
      int readLength = length;
      char* insGops = getCharArray(env, readDataArray, r, m_insertionGopFid, length);
      char* delGops = getCharArray(env, readDataArray, r, m_deletionGopFid, length);
      char* gapConts = getCharArray(env, readDataArray, r, m_overallGcpFid, length);
      char* readQuals = getCharArray(env, readDataArray, r, m_readQualsFid, length);
      
      for (int h = 0; h < numHaplotypes; h++) {
        testcases[i].hap = haplotypes[h];
        testcases[i].haplen = haplotypeLengths[h];
        testcases[i].rs = reads;
        testcases[i].rslen = readLength;
        testcases[i].i = insGops;
        testcases[i].d = delGops;
        testcases[i].c = gapConts;
        testcases[i].q = readQuals;
        i++;
      }
    }

    return testcases;
  }

  double* getOutputArray(JNIEnv *env, jdoubleArray array) {
    return getDoubleArray(env, array);
  }

  void releaseData(JNIEnv *env) {
    for (int i = 0; i < byteArrays.size(); i++) {
      env->ReleaseByteArrayElements(byteArrays[i].first, byteArrays[i].second, 0);
    }
    for (int i = 0; i < doubleArrays.size(); i++) {
      env->ReleaseDoubleArrayElements(doubleArrays[i].first, doubleArrays[i].second, 0);
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
    byteArrays.push_back(std::make_pair(byteArray, primArray));
    return (char*)primArray;
  }

  double* getDoubleArray(JNIEnv *env, jdoubleArray array) {
    jdouble* primArray = (jdouble*)env->GetDoubleArrayElements(array, NULL);
    if (primArray == NULL) {
      env->ThrowNew(env->FindClass("java/lang/OutOfMemoryError"), "Unable to access jdoubleArray");
    }
    doubleArrays.push_back(std::make_pair(array, primArray));
    return (double*)primArray;
  }

  std::vector<testcase> testcases;
  std::vector<std::pair<jbyteArray, jbyte*> > byteArrays;
  std::vector<std::pair<jdoubleArray, jdouble*> > doubleArrays;
  
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
