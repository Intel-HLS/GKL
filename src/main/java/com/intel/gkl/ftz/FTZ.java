package com.intel.gkl.ftz;

import com.intel.gkl.NativeLibraryLoader;
import org.broadinstitute.gatk.nativebindings.NativeLibrary;

import java.io.File;

/**
 * Provides methods to set and get the Flush-To-Zero flag on Intel Architecture.
 */
public final class FTZ implements NativeLibrary {
    private static final String NATIVE_LIBRARY_NAME = "gkl_ftz";
    private static boolean loaded = false;

    /**
     * Loads the native library, if it is supported on this platform. <p>
     * Returns false if the native library cannot be loaded for any reason. <br>
     *
     * @param tempDir  directory where the native library is extracted or null to use the system temp directory
     * @return  true if the native library is supported and loaded, false otherwise
     */
    @Override
    public synchronized boolean load(File tempDir) {
        loaded = NativeLibraryLoader.load(tempDir, NATIVE_LIBRARY_NAME);
        return loaded;
    }

    public boolean isLoaded() {
        return loaded;
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
