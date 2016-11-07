package com.intel.gkl.pairhmm;


import com.intel.gkl.IntelGKLUtils;
import org.broadinstitute.gatk.nativebindings.pairhmm.HaplotypeDataHolder;
import org.broadinstitute.gatk.nativebindings.pairhmm.PairHMMNativeArguments;
import org.broadinstitute.gatk.nativebindings.pairhmm.PairHMMNativeBinding;
import org.broadinstitute.gatk.nativebindings.pairhmm.ReadDataHolder;

import java.io.File;

public class IntelPairHmm implements PairHMMNativeBinding {

    public IntelPairHmm () {
        setLibFileName("gkl_pairhmm");
        setIsLoaded(false);
    }


    public static String libFileName = "gkl_pairhmm";
    public static boolean isLoaded = false;


    /**
     * Load native library using system temp directory to store the shared object.
     *
     * @return true if IntelPairHmm is supported on the platform
     */
    public boolean load() {
        return load(null, libFileName);
    }

    public boolean load(File tempDir) {
        return load(tempDir, libFileName);
    }
    /**
     * Load native library using tmpDir to store the shared object.
     *
     * @param tmpDir the directory used to store a copy of the shared object
     * @param libFileName library name to be loaded
     * @return true if IntelPairHmm is supported on the platform
     */

    public boolean load(File tmpDir, String libFileName) {
        if(!isLoaded) {
            isLoaded = IntelGKLUtils.load(tmpDir, libFileName);
            if(isLoaded == false) libFileName = null;
            return isLoaded;
        }
        return true;
    }

    public static void setLibFileName(String newlibFileName)
    {
        libFileName = newlibFileName;
    }

    public static void setIsLoaded(boolean flag)
    {
        isLoaded = flag;
    }

    /**
     * Initialize native PairHMM with the supplied args.
     *
     * @param args the args used to configure native PairHMM
     */


    public void initialize(PairHMMNativeArguments args) {
        if(args == null)
        {
            args = new PairHMMNativeArguments();
            args.useDoublePrecision = false;
            args.maxNumberOfThreads = 1;
        }
        initNative(ReadDataHolder.class, HaplotypeDataHolder.class, args.useDoublePrecision, args.maxNumberOfThreads);
    }

    /**
     *
     *
     * @param readDataArray array of read data
     * @param haplotypeDataArray array of haplotype data
     * @param likelihoodArray array of double results
     */

    @Override
    public void computeLikelihoods(ReadDataHolder[] readDataArray,
                                   HaplotypeDataHolder[] haplotypeDataArray,
                                   double[] likelihoodArray)
    {
        computeLikelihoodsNative(readDataArray, haplotypeDataArray, likelihoodArray);
    }

    /**
     *
     */
    @Override
    public void done() {
        doneNative();
    }

    private native static void initNative(Class<?> readDataHolderClass,
                                          Class<?> haplotypeDataHolderClass,
                                          boolean doublePrecision,
                                          int maxThreads);

    private native void computeLikelihoodsNative(Object[] readDataArray,
                                                 Object[] haplotypeDataArray,
                                                 double[] likelihoodArray);

    private native void doneNative();
}
