package com.intel.gkl.compression;

import com.intel.gkl.Utils;

import java.io.File;
import java.util.zip.Deflater;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;


public class IntelDeflater extends Deflater {

    private final static Logger logger = LogManager.getLogger(IntelDeflater.class);

    private static boolean isSupported = false;

    public static boolean load() {
        return load(null);
    }

    public static boolean load(File tmpDir) {
        isSupported = Utils.load(tmpDir);
        if (isSupported) {
            init();
        }
        return isSupported;
    }

    private native static void init();
    private native void resetNative();
    private native int deflate(byte[] b, int len);

    private long lz_stream;
    private byte[] inputBuffer;
    private int inputBufferLength;
    private boolean endOfStream;
    private boolean finished;

    public IntelDeflater(int level, boolean nowrap) {}

    public IntelDeflater() {}

    public void reset() {
        logger.debug("Reset deflater");
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
