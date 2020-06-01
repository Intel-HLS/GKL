package com.intel.gkl.pairhmm;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

/**
 * Provides a native PairHMM implementation accelerated using Intel FPGAs
 */
public final class IntelPairHmmFpga extends IntelPairHmm {
    private static final String NATIVE_LIBRARY_NAME = "gkl_pairhmm";
    private final static Logger logger = LogManager.getLogger(IntelPairHmmFpga.class);

    public IntelPairHmmFpga() {
   	    	logger.warn("PairHMM does not support FPGA Implementation. Please use AVX PairHMM");

    }
}
