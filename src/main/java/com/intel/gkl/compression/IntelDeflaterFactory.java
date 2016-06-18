package com.intel.gkl.compression;

import htsjdk.samtools.util.zip.DeflaterFactory;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import java.util.zip.Deflater;

public class IntelDeflaterFactory extends DeflaterFactory {

    private final static Logger logger = LogManager.getLogger(IntelDeflaterFactory.class);
    private boolean usingIntelDeflater;

    public Deflater makeDeflater(final int compressionLevel, final boolean nowrap) {
        boolean intelDeflaterSupported = new IntelDeflater().load();
        if (intelDeflaterSupported) {
            if ((compressionLevel == 1 && nowrap) || compressionLevel != 1) {
                usingIntelDeflater = true;
                return new IntelDeflater(compressionLevel, nowrap);
            }
        }
        logger.warn("IntelDeflater is not supported, using Java.util.zip.Deflater");
        usingIntelDeflater = false;
        return new Deflater(compressionLevel, nowrap);
    }

    public boolean usingIntelDeflater() {
        return usingIntelDeflater;
    }
}
