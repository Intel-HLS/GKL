/*
 * The MIT License
 *
 * Copyright (c) 2016 Intel Corporation
 *
 * 	Permission is hereby granted, free of charge, to any person
 * 	obtaining a copy of this software and associated documentation
 * 	files (the "Software"), to deal in the Software without
 * 	restriction, including without limitation the rights to use,
 * 	copy, modify, merge, publish, distribute, sublicense, and/or
 * 	sell copies of the Software, and to permit persons to whom the
 * 	Software is furnished to do so, subject to the following
 * 	conditions:
 *
 * 	The above copyright notice and this permission notice shall be
 * 	included in all copies or substantial portions of the
 * 	Software.
 *
 * 	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
 * 	KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * 	WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * 	PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * 	COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * 	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * 	OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * 	SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

package com.intel.gkl.compression;

import com.intel.gkl.IntelGKLUtils;

import java.io.File;
import java.util.zip.Deflater;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.broadinstitute.gatk.nativebindings.NativeLibrary;


public class IntelDeflater extends Deflater implements NativeLibrary {

    private final static Logger logger = LogManager.getLogger(IntelDeflater.class);

    private boolean isSupported = false;

    public boolean load() {
        return load(null);
    }

    public boolean load(File tmpDir) {
        isSupported = IntelGKLUtils.load(tmpDir);
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
