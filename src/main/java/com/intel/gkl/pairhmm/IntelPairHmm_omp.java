package com.intel.gkl.pairhmm;

import com.intel.gkl.IntelGKLUtils;
import org.broadinstitute.gatk.nativebindings.pairhmm.HaplotypeDataHolder;
import org.broadinstitute.gatk.nativebindings.pairhmm.PairHMMNativeArguments;
import org.broadinstitute.gatk.nativebindings.pairhmm.PairHMMNativeBinding;
import org.broadinstitute.gatk.nativebindings.pairhmm.ReadDataHolder;

import java.io.File;

/**
 * Created by pnvaidya on 10/12/16.
 */
public class IntelPairHmm_omp implements PairHMMNativeBinding {

    private final String libFileName = "GKL_pairHMM_omp";
    private static boolean isLoaded = false;
    /**
     * Load native library using system temp directory to store the shared object.
     *
     * @return true if IntelPairHmm is supported on the platform
     */
    public boolean load() {
        return load(null, libFileName);
    }

    /**
     * Load native library using tmpDir to store the shared object.
     *
     * @param tempDir the directory used to store a copy of the shared object
     * @return true if IntelPairHmm is supported on the platform
     */

    public boolean load(File tempDir) {
        return load(tempDir, libFileName);
    }

    public boolean load(File tmpDir, String libFileName) {
        if(!isLoaded) {
            isLoaded = true;
            return IntelGKLUtils.load(tmpDir, libFileName);
        }
        else return true;
    }

    /**
     * Initialize native PairHMM with the supplied args.
     *
     * @param args the args used to configure native PairHMM
     */

    @Override
    public void initialize(PairHMMNativeArguments args) {
        initNative(ReadDataHolder.class, HaplotypeDataHolder.class, false, 100);
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

