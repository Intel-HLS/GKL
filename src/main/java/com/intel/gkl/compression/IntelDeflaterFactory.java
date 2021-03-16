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
