package com.intel.gkl.compression;

import htsjdk.samtools.util.zip.DeflaterFactory;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import java.io.File;
import java.util.zip.Deflater;

/**
 * Provides an IntelDeflater object using the DeflaterFactory API defined in HTSJDK
 */
public class IntelDeflaterFactory extends DeflaterFactory {
    private final static Logger logger = LogManager.getLogger(IntelDeflaterFactory.class);
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
	      try {
                    return new IntelDeflater(compressionLevel, gzipCompatible); 
              } catch  (IllegalArgumentException e) {
                    logger.warn("Invalid configuration requsted, using Java.util.zip.Deflater");
	      }
    	} else {
                    logger.warn("Intel Deflater not supported, using Java.util.zip.Deflater");
	       }

        return new Deflater(compressionLevel, gzipCompatible);
   }
    public boolean usingIntelDeflater() {
        return intelDeflaterSupported;
    }
}
