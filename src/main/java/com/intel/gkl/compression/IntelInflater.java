/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2021 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
package com.intel.gkl.compression;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import com.intel.gkl.NativeLibraryLoader;
import org.broadinstitute.gatk.nativebindings.NativeLibrary;

import java.io.File;
import java.util.zip.DataFormatException;
import java.util.zip.Inflater;

/**
 * Provides a native Inflater implementation accelerated for the Intel Architecture.
 */
public final class IntelInflater extends Inflater implements NativeLibrary {
    private static final Logger logger = LogManager.getLogger(IntelInflater.class);
    private static final String NATIVE_LIBRARY_NAME = "gkl_compression";
    private static boolean initialized = false;

    /**
     * Loads the native library, if it is supported on this platform. <p>
     * Returns false if AVX is not supported. <br>
     * Returns false if the native library cannot be loaded for any reason. <br>
     * Initializes the native library after the first load. <br>
     *
     * @param tempDir  directory where the native library is extracted or null to use the system temp directory
     * @return  true if the native library is supported and loaded, false otherwise
     */
    @Override
    public synchronized boolean load(File tempDir) {
        if (!NativeLibraryLoader.load(tempDir, NATIVE_LIBRARY_NAME)) {
            return false;
        }
        if (!initialized) {
            initNative();
            initialized = true;
        }
        return true;
    }

    private long lz_stream;
    private byte[] inputBuffer;
    private int inputBufferLength;
    private int inputBufferOffset;
    private boolean finished;
    private boolean nowrap;

    private static native void initNative();
    private native void resetNative(boolean nowrap);
    private native int inflateNative( byte[] b, int off, int len);
    private native void endNative();

    /**
     * Creates a new compressor using the specified compression level.
     * If 'nowrap' is true then the ZLIB header and checksum fields will
     * not be used in order to support the compression format used in
     * both GZIP and PKZIP.
     * @param nowrap if true then use GZIP compatible compression
     */
    public IntelInflater(boolean nowrap) {
	if(nowrap == false) {
		 throw new IllegalArgumentException("ZLIB format is not supported at this time with Intel GKL");
	}
	    
	this.nowrap = nowrap;

    }

    /**
     * Creates a new compressor using the specified compression level.
     * Compressed data will be generated in ZLIB format.
     */
    public IntelInflater() {
        this(true);
    }

    @Override
    public void reset() throws OutOfMemoryError {
        try {
            resetNative(nowrap);
        } catch (OutOfMemoryError e) {
            logger.warn("Exception thrown from native Inflater resetNative function call %s", e.getMessage());
            throw new OutOfMemoryError("Memory allocation failed");
        }

        inputBuffer = null;
        inputBufferLength = 0;
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
    @Override
    public void setInput(byte[] b, int off, int len) {
        if(lz_stream == 0) reset();
        if(b == null) {
            throw new NullPointerException("Input buffer is null");
        }
        if (off < 0) {
            throw new ArrayIndexOutOfBoundsException("Offset value is less than zero");
        }
        if (len < 0) {
            throw new ArrayIndexOutOfBoundsException("Length value is less than zero");
        }
        if (off > b.length - len) {
            throw new ArrayIndexOutOfBoundsException("Length value exceeds permissible range");
        }
        inputBuffer = b;
        inputBufferOffset = off;
        inputBufferLength = len;

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
    @Override
    public int inflate (byte[] b, int off, int len ) {
        if(b == null) {
            throw new NullPointerException("Output buffer is null");
        }
        if (off < 0){
            throw new ArrayIndexOutOfBoundsException("Offset value is less than zero");
        }
        if(len < 0){
            throw new ArrayIndexOutOfBoundsException("Length value is less than zero");
        }
        if(off > b.length - len) {
            throw new ArrayIndexOutOfBoundsException("Length value exceeds permissible range");
        }
        if(inputBuffer == null || inputBufferLength == 0) {
            throw new NullPointerException("Input buffer is null");
        }

        int bytesWritten = inflateNative(b, off, len);
        if(bytesWritten == 0)
            logger.warn(String.format("Zero Bytes Written : %d", bytesWritten));
        return bytesWritten;
    }

    /**
     * Returns true if the end of the compressed data output stream has
     * been reached.
     * @return true if the end of the compressed data output stream has
     * been reached
     */
    @Override
    public int inflate (byte[] b) {
        if(b == null) {
            throw new NullPointerException("Output buffer is null");
        }

        return inflateNative( b, 0, b.length);
    }

    /**
     * Returns true if the end of the compressed data output stream has
     * been reached.
     * @return true if the end of the compressed data output stream has
     * been reached
     */
    @Override
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
        if(lz_stream != 0) {
            endNative();
            lz_stream = 0;
        }
    }
}
