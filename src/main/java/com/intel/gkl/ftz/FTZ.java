package com.intel.gkl.ftz;

import com.intel.gkl.IntelGKLUtils;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.broadinstitute.gatk.nativebindings.NativeLibrary;

import java.io.File;

public class FTZ implements NativeLibrary {

    private final static Logger logger = LogManager.getLogger(FTZ.class);

    private boolean isGklLoaded;
    private static final String libFileName = "GKL_FTZ";


    public FTZ() {
        this(null);
    }

    public boolean load() {
        return load(null, libFileName);
    }

    public boolean load(File tempDir) {
        return load(tempDir, libFileName);
    }

    public boolean load(File tmpDir, String libFileName) {
        isGklLoaded = IntelGKLUtils.load(tmpDir, libFileName);

        if(isGklLoaded){
            logger.warn("GKL Loaded.");
            return true;
        }

        else {
            logger.warn("FTZ control is not supported on this platform.");
            return false;
        }

    }


    public FTZ(File tmpDir) {
        isGklLoaded = IntelGKLUtils.load(tmpDir, libFileName);

        if(isGklLoaded){
            logger.warn("GKL Loaded.");
        }

        if (!isGklLoaded) {
            logger.warn("FTZ control is not supported on this platform.");
        }

    }

    public boolean isSupported() {
        return isGklLoaded;
    }

    public boolean getFlushToZero() {
        return isGklLoaded && getFlushToZeroNative();
    }

    public void setFlushToZero(boolean value) {
        if (isGklLoaded) {
            setFlushToZeroNative(value);
        }
    }

    private native boolean getFlushToZeroNative();
    private native void setFlushToZeroNative(boolean value);
}
