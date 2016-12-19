package com.intel.gkl.ftz;

import com.intel.gkl.NativeLibraryLoader;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.broadinstitute.gatk.nativebindings.NativeLibrary;

import java.io.File;

public final class FTZ implements NativeLibrary {
    private static final Logger logger = LogManager.getLogger(FTZ.class);
    private static final NativeLibraryLoader libraryLoader = new NativeLibraryLoader("gkl_ftz");

    @Override
    public synchronized boolean load(File tempDir) {
        return libraryLoader.load(tempDir);
    }

    public boolean isLoaded() {
        return libraryLoader.isLoaded();
    }

    public boolean getFlushToZero() {
        return getFlushToZeroNative();
    }

    public void setFlushToZero(boolean value) {
        setFlushToZeroNative(value);
    }

    private native boolean getFlushToZeroNative();
    private native void setFlushToZeroNative(boolean value);
}
