package com.intel.gkl.compression;

import htsjdk.samtools.util.zip.DeflaterFactory;
import org.apache.log4j.Logger;

import java.io.File;
import java.util.zip.Deflater;

/**
 * Provides an IntelDeflater object using the DeflaterFactory API defined in HTSJDK
 */
public class IntelDeflaterFactory extends DeflaterFactory {
    private final static Logger logger = Logger.getLogger(IntelDeflaterFactory.class);
    private boolean intelDeflaterSupported;

    public IntelDeflaterFactory(File tmpDir) {
        intelDeflaterSupported = new IntelDeflater().load(tmpDir);
    }

    public IntelDeflaterFactory() {
        this(null);
    }

    /**
     * Returns an IntelDeflater if supported on the platform, otherwise returns a Java Deflater
     *
     * @param compressionLevel  the compression level (0-9)
     * @param gzipCompatible  if true the use GZIP compatible compression
     * @return a Deflater object
     */
    public Deflater makeDeflater(final int compressionLevel, final boolean gzipCompatible) {
        if (intelDeflaterSupported) {
            if ((compressionLevel == 1 && gzipCompatible) || compressionLevel != 1) {
                return new IntelDeflater(compressionLevel, gzipCompatible);
            }
        }
        logger.warn("IntelDeflater is not supported, using Java.util.zip.Deflater");
        return new Deflater(compressionLevel, gzipCompatible);
    }

    public boolean usingIntelDeflater() {
        return intelDeflaterSupported;
    }
}
