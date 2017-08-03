package com.intel.gkl.smithwaterman;


import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import com.intel.gkl.IntelGKLUtils;
import com.intel.gkl.NativeLibraryLoader;
import org.broadinstitute.gatk.nativebindings.NativeLibrary;
import org.broadinstitute.gatk.nativebindings.smithwaterman.SWAlignerArguments;
import org.broadinstitute.gatk.nativebindings.smithwaterman.SWAlignerNativeBinding;

import htsjdk.samtools.Cigar;
import htsjdk.samtools.CigarElement;
import htsjdk.samtools.CigarOperator;
import org.broadinstitute.gatk.nativebindings.smithwaterman.SWAlignmentResult;

import java.io.File;

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

        if (!gklUtils.isAvxSupported()) {
            return false;
        }

        if (!NativeLibraryLoader.load(tempDir, NATIVE_LIBRARY_NAME)) {
            return false;
        }
        if (!initialized) {

            initialized = true;
        }
        return true;
    }

    /**
     * Initialize native SmithWaterman with the supplied args.
     *
     * @param args the args used to configure native SmithWaterman
     */

    public void initialize(SWAlignerArguments args) {
        if(args == null)
        {
                args = new SWAlignerArguments(SWAlignerArguments.OverhangStrategy.SOFTCLIP,1,-1,1,1);
        }
        else {
            w_extend = args.w_extend;
            w_match = args.w_match;
            w_mismatch = args.w_mismatch;
            w_open = args.w_open;
            strategy = args.strategy;
        }

    }

    public int w_extend;
    public int w_match;
    public int w_mismatch;
    public int w_open;
    public SWAlignerArguments.OverhangStrategy strategy;
    private native static int alignNative(byte[] refArray, byte[] altArray, byte[] cigar, int match, int mismatch, int open, int extend, int strategy);

    /**
     *Implements the native implementation of SmithWaterman, and returns the Cigar String and alignment_offset
     *
     * @param refArray array of reference data
     * @param altArray array of alternate data
     *
     */

    public SWAlignmentResult align(byte[] refArray, byte[] altArray)
    {
        int intStrategy =  getStrategy(strategy);
        byte[] cigar = new byte[200];
        int offset = alignNative(refArray, altArray, cigar, w_match, w_mismatch, w_open, w_extend, intStrategy);
        return new SWAlignmentResult(new String(cigar),offset);
    }

    public static int getStrategy(SWAlignerArguments.OverhangStrategy strategy)
    {
        int intStrategy=0;

        switch(strategy){
            case SOFTCLIP: intStrategy = 9;
                return intStrategy;
            case INDEL: intStrategy = 10;
                return intStrategy;
            case LEADING_INDEL: intStrategy = 11;
                return intStrategy;
            case IGNORE: intStrategy = 12;
                return intStrategy;
        }

        return intStrategy;

    }


}
