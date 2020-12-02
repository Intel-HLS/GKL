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
import com.intel.gkl.NativeLibraryLoader;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.broadinstitute.gatk.nativebindings.NativeLibrary;

import java.io.File;
import java.util.zip.Deflater;

/**
 * Provides a native Deflater implementation accelerated for the Intel Architecture.
 */
public final class IntelDeflater extends Deflater implements NativeLibrary {
    private final static Logger logger = LogManager.getLogger(IntelInflater.class);
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

    private native static void initNative();
    private native void resetNative(boolean nowrap);
    private native int deflateNative(byte[] b, int len);
    private native void endNative();

    private long lz_stream;
    private byte[] inputBuffer;
    private int inputBufferLength;
    private boolean endOfStream;
    private boolean finished;
    private int level;
    private int strategy;
    private boolean nowrap;


     /**
     * Creates a new compressor using the specified compression level.
     * If 'nowrap' is true then the ZLIB header and checksum fields will
     * not be used in order to support the compression format used in
     * both GZIP and PKZIP.
     * @param level the compression level (0-9)
     * @param nowrap if true then use GZIP compatible compression
     */
    public IntelDeflater(int level, boolean nowrap) {
        if ((level < 0 || level > 9) && level != DEFAULT_COMPRESSION) {
            throw new IllegalArgumentException("Illegal compression level");
        }
        this.level = level;
        this.nowrap = nowrap;
        strategy = DEFAULT_STRATEGY;

    }

     /**
     * Creates a new compressor using the specified compression level.
     * Compressed data will be generated in ZLIB format.
     * @param level the compression level (0-9)
     */
    public IntelDeflater(int level) {
        this(level, false);
    }

    /**
     * Creates a new compressor with the default compression level.
     * Compressed data will be generated in ZLIB format.
     */
    public IntelDeflater() {
        this(DEFAULT_COMPRESSION, false);
    }

    @Override
    public void reset() throws OutOfMemoryError, RuntimeException {
        try {
            resetNative(nowrap);
        } catch (OutOfMemoryError e) {
            logger.warn("Exception thrown from native Deflater resetNative function call %s", e.getMessage());
            throw new OutOfMemoryError("Memory allocation failed");
        } catch (RuntimeException e) {
            logger.warn("Exception thrown from native Deflater resetNative function call %s", e.getMessage());
            throw new RuntimeException("Deflate Initialization issue");
        }

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
     * @see IntelDeflater
     */
    @Override
    public void setInput(byte[] b, int off, int len) throws NullPointerException, IllegalArgumentException {
        if(lz_stream == 0) reset();
        if(b == null) {
            throw new NullPointerException("Input is null");
        }
        if(len <= 0) {
            throw new IllegalArgumentException("Input buffer length is less or equal zero.");
        }
        if(off < 0 || off > b.length - len )
        {
            throw new IllegalArgumentException("Offset cannot be less then 0 and greater then length.");
        }
        inputBuffer = b;
        inputBufferLength = len;
    }

    /**
     * When called, indicates that compression should end with the current
     * contents of the input buffer.
     */
    @Override
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
    @Override
    public int deflate(byte[] b, int off, int len )  throws NullPointerException, IllegalArgumentException {
        if(b == null) {
            throw new NullPointerException("Input is null");
        }
        if(len <= 0) {
            throw new NullPointerException("Input buffer length is less or equal zero.");
        }
        if(off != 0) {
            throw new IllegalArgumentException("Offset must be equal zero.");
        }

        return deflateNative(b, len);
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
