package com.intel.gkl.pairhmm;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import com.intel.gkl.IntelGKLUtils;
import com.intel.gkl.NativeLibraryLoader;
import org.broadinstitute.gatk.nativebindings.pairhmm.HaplotypeDataHolder;
import org.broadinstitute.gatk.nativebindings.pairhmm.PairHMMNativeArguments;
import org.broadinstitute.gatk.nativebindings.pairhmm.PairHMMNativeBinding;
import org.broadinstitute.gatk.nativebindings.pairhmm.ReadDataHolder;

import java.io.File;

/**
 * Provides a native PairHMM implementation accelerated for the Intel Architecture.
 */
public class IntelPairHmm implements PairHMMNativeBinding {
    private final static Logger logger = LogManager.getLogger(IntelPairHmm.class);
    private static final String NATIVE_LIBRARY_NAME = "gkl_pairhmm";
    private String nativeLibraryName = "gkl_pairhmm";
    private IntelGKLUtils gklUtils = new IntelGKLUtils();
    boolean useOmp = false;

    void setNativeLibraryName(String nativeLibraryName) {
        this.nativeLibraryName = nativeLibraryName;
    }

    public IntelPairHmm() {
        setNativeLibraryName(NATIVE_LIBRARY_NAME);
    }

    /**
     * Loads the native library, if it is supported on this platform. <p>
     * Returns false if AVX is not supported. <br>
     * Returns false if the native library cannot be loaded for any reason. <br>
     *
     * @param tempDir  directory where the native library is extracted or null to use the system temp directory
     * @return  true if the native library is supported and loaded, false otherwise
     */
    @Override
    public synchronized boolean load(File tempDir) {
        boolean isLoaded = gklUtils.load(null);

        if(!isLoaded) {
            logger.warn("Intel GKL Utils not loaded");
            return false;
        }

        if(!gklUtils.isAvxSupported()) {
            return false;
        }

        return NativeLibraryLoader.load(tempDir, nativeLibraryName);
    }

    /**
     * Initialize native PairHMM with the supplied args.
     *
     * @param args the args used to configure native PairHMM
     */
    public void initialize(PairHMMNativeArguments args) {
        if (args == null) {
            args = new PairHMMNativeArguments();
            args.useDoublePrecision = false;
            args.maxNumberOfThreads = 1;
        }

        if(gklUtils.isAvx512Supported()) {
            logger.info("Using CPU-supported AVX-512 instructions");
        }

        if(!gklUtils.getFlushToZero()) {
            logger.info("Flush-to-zero (FTZ) is enabled when running PairHMM");
        }

        initNative(ReadDataHolder.class, HaplotypeDataHolder.class, args.useDoublePrecision, args.maxNumberOfThreads);

        // log information about threads
        int reqThreads = args.maxNumberOfThreads;
        if (useOmp) {
            int availThreads = gklUtils.getAvailableOmpThreads();
            int maxThreads = Math.min(reqThreads, availThreads);

            logger.info(String.format("Available threads: %d", availThreads));
            logger.info(String.format("Requested threads: %d", reqThreads));
            if (reqThreads > availThreads) {
                logger.warn(String.format("Using %d available threads, but %d were requested", maxThreads, reqThreads));
            }
        }
        else {
            if (reqThreads != 1) {
                logger.warn(String.format("Ignoring request for %d threads; not using OpenMP implementation", reqThreads));
            }
        }
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
                                   double[] likelihoodArray) throws NullPointerException
    {
        if(readDataArray == null || haplotypeDataArray == null || likelihoodArray == null) {
            throw new NullPointerException("Input is null");
        }
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
