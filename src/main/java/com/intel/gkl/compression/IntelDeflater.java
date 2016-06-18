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


/**
 * This is a copy of java.util.zip.Deflater from OpenJDK 7, with the following changes:
 * - package and class name changed
 * - static block to load libIntelGKL library
 * - extends java.util.zip.Deflater so that IntelDeflater object can be used as regular Deflater object.
 *   Note however that all methods of Deflater are overridden.
 *
 * The shared library is packaged is a jar file and is loaded when GKL_USE_LIB_PATH is set.
 *
 *
 * The rest of this document is copied verbatim from the original OpenJDK file.
 *
 * This class provides support for general purpose compression using the
 * popular ZLIB compression library. The ZLIB compression library was
 * initially developed as part of the PNG graphics standard and is not
 * protected by patents. It is fully described in the specifications at
 * the <a href="package-summary.html#package_description">java.util.zip
 * package description</a>.
 *
 * <p>The following code fragment demonstrates a trivial compression
 * and decompression of a string using <tt>IntelDeflater</tt> and
 * <tt>Inflater</tt>.
 *
 * <blockquote><pre>
 * try {
 *     // Encode a String into bytes
 *     String inputString = "blahblahblah";
 *     byte[] input = inputString.getBytes("UTF-8");
 *
 *     // Compress the bytes
 *     byte[] output = new byte[100];
 *     IntelDeflater compresser = new IntelDeflater();
 *     compresser.setInput(input);
 *     compresser.finish();
 *     int compressedDataLength = compresser.deflate(output);
 *     compresser.end();
 *
 *     // Decompress the bytes
 *     Inflater decompresser = new Inflater();
 *     decompresser.setInput(output, 0, compressedDataLength);
 *     byte[] result = new byte[100];
 *     int resultLength = decompresser.inflate(result);
 *     decompresser.end();
 *
 *     // Decode the bytes into a String
 *     String outputString = new String(result, 0, resultLength, "UTF-8");
 * } catch(java.io.UnsupportedEncodingException ex) {
 *     // handle
 * } catch (java.util.zip.DataFormatException ex) {
 *     // handle
 * }
 * </pre></blockquote>
 *
 * @see         java.util.zip.Inflater
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
    private native void resetNative(boolean nowrap);
    private native int deflate(byte[] b, int len);
   
    

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
            throw new IllegalArgumentException("Invalid compression level");
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

    public void reset() {
        logger.debug("Reset deflater");
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

    public int deflate(byte[] b, int off, int len ) {
        return deflate(b, len);
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
        
    }

    /**
     * Closes the compressor when garbage is collected.
     */
    protected void finalize() {
        end();
    }
}
