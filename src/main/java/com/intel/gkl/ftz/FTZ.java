package com.intel.gkl.ftz;

import com.intel.gkl.IntelGKLUtils;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import java.io.File;

public class FTZ {
    private final static Logger logger = LogManager.getLogger(FTZ.class);

    private boolean isGklLoaded;

    public FTZ() {
        this(null);
    }

    public FTZ(File tmpDir) {
        isGklLoaded = IntelGKLUtils.load(tmpDir);

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
