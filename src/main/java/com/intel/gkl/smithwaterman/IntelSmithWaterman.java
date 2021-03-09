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
package com.intel.gkl.smithwaterman;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import com.intel.gkl.IntelGKLUtils;
import com.intel.gkl.NativeLibraryLoader;

import org.broadinstitute.gatk.nativebindings.smithwaterman.SWParameters;
import org.broadinstitute.gatk.nativebindings.smithwaterman.SWOverhangStrategy;
import org.broadinstitute.gatk.nativebindings.smithwaterman.SWAlignerNativeBinding;

import org.broadinstitute.gatk.nativebindings.smithwaterman.SWNativeAlignerResult;

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

    // limited due to the internal implementation of the native code in C
    private final int MAX_SW_SEQUENCE_LENGTH = 32*1024-1; // 2^15 - 1
    // prevents integer overflow on the diagonal of the scoring matrix
    private final int MAXIMUM_SW_MATCH_VALUE = 64*1024; // 2^16

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
        if(refArray.length <=0 || altArray.length <=0)
            throw new IllegalArgumentException("Cannot align empty sequences");


        int intStrategy =  getStrategy(overhangStrategy);
        byte[] cigar = new byte[2*Integer.max(refArray.length, altArray.length)];

        if(refArray.length > MAX_SW_SEQUENCE_LENGTH || altArray.length > MAX_SW_SEQUENCE_LENGTH){
            throw new IllegalArgumentException(String.format("Sequences exceed maximum length of %d bytes", MAX_SW_SEQUENCE_LENGTH));
        }
        if(parameters.getMatchValue() > MAXIMUM_SW_MATCH_VALUE){
            throw new IllegalArgumentException(String.format("Match value parameter exceed maximum value of %d", MAXIMUM_SW_MATCH_VALUE));
        }

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

