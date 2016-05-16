package com.intel.gkl.deflater;

public class IntelDeflater {

    private native void initLibrary();
    private native void resetLibrary();
    private native void setInput(byte[] b, int len);
    private native void setEndOfStream();
    private native int deflate(byte[] b, int len);
    private native boolean deflateFinished();

    public IntelDeflater() {
        System.loadLibrary("IntelGKL");
        initLibrary();
    }

    public void reset() {
        resetLibrary();
    }

    public void setInput(byte[] b, int off, int len) {
        setInput(b, len);
    }

    public void finish() {
        setEndOfStream();
    }

    public int deflate(byte[] b, int off, int len) {
        return deflate(b, len);
    }

    public boolean finished() {
        return deflateFinished();
    }
}
