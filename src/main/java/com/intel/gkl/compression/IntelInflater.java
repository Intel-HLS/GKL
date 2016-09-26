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
import java.util.zip.Inflater;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.broadinstitute.gatk.nativebindings.NativeLibrary;



/**
 * Created by pnvaidya on 8/30/16.
 */


public class IntelInflater extends Inflater implements NativeLibrary {



    private final static Logger logger = LogManager.getLogger(IntelInflater.class);

    private boolean isSupported = false;

    public boolean load() {
        return load(null);
    }

    public boolean load(File tmpDir) {
        isSupported = IntelGKLUtils.load(tmpDir);
        if (isSupported) {
            initNative();
        }
        return isSupported;
    }


    private long lz_stream;
    private byte[] inputBuffer;
    private int inputBufferLength;
    private boolean endOfStream;
    private boolean finished;
    private boolean nowrap;

    private static native void initNative();
    private native void resetNative(boolean nowrap);
    private native int inflateNative( byte[] b, int len);
    private native void endNative();



    /**
     * Creates a new compressor using the specified compression level.
     * If 'nowrap' is true then the ZLIB header and checksum fields will
     * not be used in order to support the compression format used in
     * both GZIP and PKZIP.
     * @param nowrap if true then use GZIP compatible compression
     */

    public IntelInflater(boolean nowrap) {
      //  initFieldsNative();
        this.nowrap = nowrap;
    }


    /**
     * Creates a new compressor using the specified compression level.
     * Compressed data will be generated in ZLIB format.
     */
    public IntelInflater() {
        this(false);
    }


    public void reset() {
        //logger.debug("Reset inflater");

        resetNative(nowrap);
        inputBuffer = null;
        inputBufferLength = 0;
        endOfStream = false;
        finished = false;
    }

    /**
     * Sets input data for compression. This should be called whenever
     * needsInput() returns true indicating that more input data is required.
     * @param b the input data bytes
     * @param off the start offset of the data
     * @param len the length of the data
     * @see IntelDeflater#needsInput
     */

    public void setInput(byte[] b, int off, int len) throws NullPointerException {
        if(b == null) {
            throw new NullPointerException("Input is null");
        }
        if(len <= 0) {
            throw new NullPointerException("Input buffer length is zero.");
        }
        inputBuffer = b;
        inputBufferLength = len;
    }




    /**
     * When called, indicates that compression should end with the current
     * contents of the input buffer.
     */

    public void finish() {
        endOfStream = true;
    }

    /**
     * Compresses the input data and fills specified buffer with compressed
     * data. Returns actual number of bytes of compressed data. A return value
     * of 0 indicates that {@link #needsInput() needsInput} should be called
     * in order to determine if more input data is required.
     *
     * @param b the buffer for the compressed data
     * @param off the start offset of the data
     * @param len the maximum number of bytes of compressed data
     * @return the actual number of bytes of compressed data written to the
     *         output buffer
     */

    public int inflate (byte[] b, int off, int len ) {
        return inflateNative(b, len);
    }


    /**
     * Returns true if the end of the compressed data output stream has
     * been reached.
     * @return true if the end of the compressed data output stream has
     * been reached
     */

    public int inflate (byte[] b ) {
        return inflateNative( b, 0);
    }


    /**
     * Returns true if the end of the compressed data output stream has
     * been reached.
     * @return true if the end of the compressed data output stream has
     * been reached
     */

    public boolean finished() {
        return finished;
    }

    /**
     * Closes the compressor and discards any unprocessed input.
     * This method should be called when the compressor is no longer
     * being used, but will also be called automatically by the
     * finalize() method. Once this method is called, the behavior
     * of the IntelDeflater object is undefined.
     */
    @Override
    public void end() {
        endNative();
    }

    /**
     * Closes the compressor when garbage is collected.
     */
    protected void finalize() {
        end();
    }
}
