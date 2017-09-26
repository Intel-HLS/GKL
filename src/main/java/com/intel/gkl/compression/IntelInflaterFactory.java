package com.intel.gkl.compression;

import htsjdk.samtools.util.zip.InflaterFactory;

import org.apache.log4j.Logger;

import java.io.File;
import java.util.zip.Inflater;

/**
 * Created by pnvaidya on 2/1/17.
 */
public class IntelInflaterFactory extends InflaterFactory {
    private final static Logger logger = Logger.getLogger(IntelDeflaterFactory.class);
    private boolean intelInflaterSupported;

    public IntelInflaterFactory(File tmpDir) {
        intelInflaterSupported = new IntelInflater().load(tmpDir);
    }

    public IntelInflaterFactory() {
        this(null);
    }

    public Inflater makeInflater(final boolean nowrap) {
        if (intelInflaterSupported && nowrap) {
            return new IntelInflater(nowrap);
        }
        logger.warn("IntelInflater is not supported, using Java.util.zip.Inflater");
        return new Inflater(nowrap);
    }

    public boolean usingIntelInflater() {
        return intelInflaterSupported;
    }
}
