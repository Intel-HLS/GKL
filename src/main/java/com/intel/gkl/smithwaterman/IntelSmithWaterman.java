package com.intel.gkl.smithwaterman;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import com.intel.gkl.IntelGKLUtils;
import com.intel.gkl.NativeLibraryLoader;

import org.broadinstitute.gatk.nativebindings.smithwaterman.SWParameters;
import org.broadinstitute.gatk.nativebindings.smithwaterman.SWOverhangStrategy;
import org.broadinstitute.gatk.nativebindings.smithwaterman.SWAlignerNativeBinding;

import htsjdk.samtools.Cigar;
import htsjdk.samtools.CigarElement;
import htsjdk.samtools.CigarOperator;
import org.broadinstitute.gatk.nativebindings.smithwaterman.SWNativeAlignerResult;

import java.io.File;
import java.lang.Object;
import java.nio.charset.Charset;
import java.util.Arrays;

/**
 * Provides a native SmithWaterman implementation accelerated for the Intel Architecture.
 */

public class IntelSmithWaterman implements SWAlignerNativeBinding {

    private final static Logger logger = LogManager.getLogger(IntelSmithWaterman.class);
    private static final String NATIVE_LIBRARY_NAME = "gkl_smithwaterman";
    private String nativeLibraryName = "gkl_smithwaterman";
    private static boolean initialized = false;
    private IntelGKLUtils gklUtils = new IntelGKLUtils();

    void setNativeLibraryName(String nativeLibraryName) {
        this.nativeLibraryName = nativeLibraryName;
    }

    public IntelSmithWaterman() {

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

        if(!isLoaded)
        {
            logger.warn("Intel GKL Utils not loaded");
            return false;
        }

        if (!gklUtils.isAvxSupported() || !gklUtils.isAvx2Supported()) {
            return false;
        }

        if (!NativeLibraryLoader.load(tempDir, NATIVE_LIBRARY_NAME)) {
            return false;
        }
        if (!initialized) {

            initialized = true;
        }

        if(gklUtils.isAvx512Supported()) {
            logger.info("Using CPU-supported AVX-512 instructions");
        }

        /*
         Initializes the function pointers to use machine specific optimized code
         */
        initNative();
        return true;
    }


    /**
     *Implements the native implementation of SmithWaterman, and returns the Cigar String and alignment_offset
     *
     * @param refArray array of reference data
     * @param altArray array of alternate data
     *
     */

    @Override
    public SWNativeAlignerResult align(byte[] refArray, byte[] altArray, SWParameters parameters, SWOverhangStrategy overhangStrategy) throws NullPointerException, OutOfMemoryError, IllegalArgumentException
    {
        int offset = 0;

        if(refArray == null)
            throw new NullPointerException("Reference data array is null.");
        if(altArray == null)
            throw new NullPointerException("Alternate data array is null.");
        if(parameters == null)
            throw new NullPointerException("Parameter structure is null.");
        if(overhangStrategy == null)
            throw new NullPointerException("OverhangStrategy is null.");

        int intStrategy =  getStrategy(overhangStrategy);
        byte[] cigar = new byte[2*Integer.max(refArray.length, altArray.length)];

        if(cigar.length <= 0 || intStrategy < 9 || intStrategy > 12)
            throw new IllegalArgumentException("Strategy is invalid.");

        try {
            offset = alignNative(refArray, altArray, cigar, parameters.getMatchValue(), parameters.getMismatchPenalty(), parameters.getGapOpenPenalty(), parameters.getGapExtendPenalty(), intStrategy);
        }  catch (OutOfMemoryError e) {
            logger.warn("Exception thrown from native SW alignNative function call %s", e.getMessage());
            throw new OutOfMemoryError("Memory allocation failed");
        } catch (IllegalArgumentException e) {
            logger.warn("Exception thrown from native SW alignNative function call %s", e.getMessage());
            throw new IllegalArgumentException("Ran into invalid argument issue");
        }

        return new SWNativeAlignerResult(new String(cigar).trim(), offset);
    }

    public int getStrategy(SWOverhangStrategy strategy)
    {
        int intStrategy = 0;

        switch(strategy){
            case SOFTCLIP: intStrategy = 9;
                break;
            case INDEL: intStrategy = 10;
                break;
            case LEADING_INDEL: intStrategy = 11;
                break;
            case IGNORE: intStrategy = 12;
                break;
        }

        return intStrategy;

    }

    public void close()
    {
        doneNative();
    }

    private native static void initNative();
    private native static int alignNative(byte[] refArray, byte[] altArray, byte[] cigar, int match, int mismatch, int open, int extend, int strategy);
    private native static void doneNative();
}

