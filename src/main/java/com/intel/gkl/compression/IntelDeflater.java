package com.intel.gkl.compression;

import java.io.File;
import java.util.zip.Deflater;

public class IntelDeflater extends Deflater {
    private final static String GKL_LIB_NAME = "IntelGKL";

    private native static void init();
    private native void resetNative();
    private native int deflate(byte[] b, int len);

    private long lz_stream;
    private byte[] inputBuffer;
    private int inputBufferLength;
    private boolean endOfStream;
    private boolean finished;

    static {
        System.loadLibrary("IntelGKL");
        init();
    }

    public boolean load(File tmpDir) {
        try {
            System.loadLibrary(GKL_LIB_NAME);
            System.out.printf("Loaded from library path: ", System.mapLibraryName(GKL_LIB_NAME));
        } catch (UnsatisfiedLinkError e) {
            String pathInJar = "/lib" + "/linux" + "/" + System.mapLibraryName(GKL_LIB_NAME);
            System.out.printf("Extract from jar: ", pathInJar);
        } catch (final Exception e) {
            return false;
        }

        init();

        return true;
    }

    public IntelDeflater(int level, boolean nowrap) throws Exception {
        // currently only supports level 1 and nowrap == true
        if (level != 1 && !nowrap) {
            throw new Exception("Not supported");
        }
        reset();
    }

    public IntelDeflater() {
        reset();
    }

    public void reset() {
        resetNative();
        inputBuffer = null;
        inputBufferLength = 0;
        endOfStream = false;
        finished = false;
    }

    public void setInput(byte[] b, int off, int len) {
        inputBuffer = b;
        inputBufferLength = len;
    }

    public void finish() {
        endOfStream = true;
    }

    public int deflate(byte[] b, int off, int len) {
        return deflate(b, len);
    }

    public boolean finished() {
        return finished;
    }
}
