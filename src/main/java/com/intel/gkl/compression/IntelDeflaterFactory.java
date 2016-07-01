package com.intel.gkl.compression;

import htsjdk.samtools.util.zip.DeflaterFactory;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import java.io.File;
import java.util.zip.Deflater;

public class IntelDeflaterFactory extends DeflaterFactory {

    private final static Logger logger = LogManager.getLogger(IntelDeflaterFactory.class);
    private boolean intelDeflaterSupported;

    public IntelDeflaterFactory(File tmpDir) {
        intelDeflaterSupported = new IntelDeflater().load(tmpDir);
    }

    public IntelDeflaterFactory() {
        this(null);
    }

    public Deflater makeDeflater(final int compressionLevel, final boolean nowrap) {
        if (intelDeflaterSupported) {
            if ((compressionLevel == 1 && nowrap) || compressionLevel != 1) {
                return new IntelDeflater(compressionLevel, nowrap);
            }
        }
        logger.warn("IntelDeflater is not supported, using Java.util.zip.Deflater");
        return new Deflater(compressionLevel, nowrap);
    }

    public boolean usingIntelDeflater() {
        return intelDeflaterSupported;
    }
}
