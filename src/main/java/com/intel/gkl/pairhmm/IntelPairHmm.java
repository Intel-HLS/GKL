package com.intel.gkl.pairhmm;

import com.intel.gkl.IntelGKLUtils;
import com.intel.gkl.NativeLibraryLoader;
import org.broadinstitute.gatk.nativebindings.pairhmm.HaplotypeDataHolder;
import org.broadinstitute.gatk.nativebindings.pairhmm.PairHMMNativeArguments;
import org.broadinstitute.gatk.nativebindings.pairhmm.PairHMMNativeBinding;
import org.broadinstitute.gatk.nativebindings.pairhmm.ReadDataHolder;

import java.io.File;

public class IntelPairHmm implements PairHMMNativeBinding {
    private static NativeLibraryLoader libraryLoader;

    protected static void setLibraryName(String libraryName) {
        libraryLoader = new NativeLibraryLoader(libraryName);
    }

    public IntelPairHmm() {
        setLibraryName("gkl_pairhmm");
    }

    @Override
    public synchronized boolean load(File tempDir) {
        if (!IntelGKLUtils.isAvxSupported()) {
            return false;
        }
        return libraryLoader.load(tempDir);
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
