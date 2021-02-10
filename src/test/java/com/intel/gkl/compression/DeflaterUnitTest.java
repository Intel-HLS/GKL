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


import com.intel.gkl.testing_utils.CompressionDataProviders;
import com.intel.gkl.testing_utils.TestingUtils;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.testng.Assert;
import org.testng.annotations.Test;

import java.util.Arrays;
import java.util.zip.DataFormatException;
import java.util.zip.Deflater;
import java.util.zip.Inflater;


public class DeflaterUnitTest extends CompressionUnitTestBase {
    protected final static Logger log = LogManager.getLogger(DeflaterUnitTest.class);

    @Test(enabled = true)
    public void loadLibrary() {
        final boolean isSupported = new IntelDeflater().load(null);
        Assert.assertTrue(isSupported);
        final boolean Supported = new IntelInflater().load(null);
        Assert.assertTrue(Supported);
    }

    @Test(enabled = true, expectedExceptions = NullPointerException.class)
    public void setInputShortThrowsNullPointerExceptionWhenNullBufferTest(){
        final Deflater deflater = new IntelDeflaterFactory().makeDeflater(0, true);

        deflater.setInput(null);

        Assert.fail();
    }
    @Test(enabled = true, expectedExceptions = NullPointerException.class)
    public void setInputLongThrowsNullPointerExceptionWhenNullBufferTest(){
        final Deflater deflater = new IntelDeflaterFactory().makeDeflater(0, true);

        deflater.setInput(null, 0, 0);

        Assert.fail();
    }

    @Test(enabled = true, expectedExceptions = ArrayIndexOutOfBoundsException.class)
    public void setInputThrowsArrayIndexOutOfBoundsExceptionWhenOffsetLessThanZeroTest(){
        final Deflater deflater = new IntelDeflaterFactory().makeDeflater(0, true);
        byte[] buffer = new byte[] {'A', 'C', 'G'};

        deflater.setInput(buffer,-1, buffer.length);

        Assert.fail();
    }

    @Test(enabled = true, expectedExceptions = ArrayIndexOutOfBoundsException.class)
    public void setInputThrowsArrayIndexOutOfBoundsExceptionWhenLengthLessThanZeroTest(){
        final Deflater deflater = new IntelDeflaterFactory().makeDeflater(0, true);
        byte[] buffer = new byte[] {'A', 'C', 'G'};

        deflater.setInput(buffer,0, -1);

        Assert.fail();
    }
    @Test(enabled = true, expectedExceptions = ArrayIndexOutOfBoundsException.class)
    public void setInputThrowsArrayIndexOutOfBoundsExceptionWhenOffsetGreaterThanLengthTest(){
        final Deflater deflater = new IntelDeflaterFactory().makeDeflater(0, true);
        byte[] buffer = new byte[] {'A', 'C', 'G'};

        deflater.setInput(buffer,4, buffer.length);

        Assert.fail();
    }

    @Test(enabled = true,expectedExceptions = NullPointerException.class)
    public void deflateThrowsNullPointerExceptionWhenNullBufferTest(){
        final Deflater deflater = new IntelDeflaterFactory().makeDeflater(0, true);

        deflater.deflate(null, 0, 1);

        Assert.fail();
    }

    @Test(enabled = true,expectedExceptions = IllegalArgumentException.class)
    public void deflateThrowsIllegalArgumentExceptionWhenOffsetGreaterThanZeroTest(){
        final Deflater deflater = new IntelDeflaterFactory().makeDeflater(0, true);
        byte[] outputBuffer = new byte[10];

        deflater.deflate(outputBuffer, 1, outputBuffer.length);

        Assert.fail();
    }

    @Test(enabled = true,expectedExceptions = IllegalArgumentException.class)
    public void deflateThrowsIllegalArgumentExceptionWhenOffsetLessThanZeroTest(){
        final Deflater deflater = new IntelDeflaterFactory().makeDeflater(0, true);
        byte[] outputBuffer = new byte[10];

        deflater.deflate(outputBuffer, -1, outputBuffer.length);

        Assert.fail();
    }

    @Test(enabled = true,expectedExceptions = ArrayIndexOutOfBoundsException.class)
    public void deflateThrowsArrayIndexOutOfBoundsExceptionWhenLengthLessThanZeroTest(){
        final Deflater deflater = new IntelDeflaterFactory().makeDeflater(0, true);
        byte[] outputBuffer = new byte[10];

        deflater.deflate(outputBuffer, 0, -1);

        Assert.fail();
    }

    @Test(enabled = true,expectedExceptions = ArrayIndexOutOfBoundsException.class)
    public void deflateThrowsArrayIndexOutOfBoundsExceptionWhenLengthEqualZeroTest(){
        final Deflater deflater = new IntelDeflaterFactory().makeDeflater(0, true);
        byte[] outputBuffer = new byte[10];

        deflater.deflate(outputBuffer, 0, 0);

        Assert.fail();
    }

    @Test(enabled = true, dataProviderClass = CompressionDataProviders.class,
            dataProvider = compatibilityTestsDataProviderName, groups = {"compatibilityTests"})
    public void intelDeflationJavaInflationCompatibilityTest(int level, boolean nowrap) throws DataFormatException {
        //arrange
        int inputLen = TestingUtils.getMaxInputSizeForGivenOutputSize(compatibilityTestsBufferSize, level, nowrap);
        int compressedLen = compatibilityTestsBufferSize;

        final byte[] input = Arrays.copyOf(compatibilityInputBuffer, inputLen);
        final byte[] compressed = new byte[compressedLen];
        final byte[] decompressed = new byte[inputLen];

        Deflater intelDeflater = new IntelDeflaterFactory().makeDeflater(level, nowrap);
        Inflater javaInflater = new Inflater(nowrap);

        //act
        intelDeflater.setInput(input,0,input.length);
        intelDeflater.finish();
        intelDeflater.deflate(compressed,0,compressed.length);
        intelDeflater.end();

        javaInflater.setInput(compressed,0,compressed.length);
        javaInflater.inflate(decompressed,0,decompressed.length);
        javaInflater.end();

        //assert
        Assert.assertEquals(decompressed,input);
    }


    @Test(enabled = true, dataProviderClass = CompressionDataProviders.class,
            dataProvider = compatibilityTestsDataProviderName, groups = {"compatibilityTests"})
    public void intelDeflationIntelInflationCompatibilityTest(int level, boolean nowrap) throws DataFormatException {
        //arrange
        int inputLen = TestingUtils.getMaxInputSizeForGivenOutputSize(compatibilityTestsBufferSize, level, nowrap);
        int compressedLen = compatibilityTestsBufferSize;

        final byte[] input = Arrays.copyOf(compatibilityInputBuffer, inputLen);
        final byte[] compressed = new byte[compressedLen];
        final byte[] decompressed = new byte[inputLen];

        Deflater intelDeflater = new IntelDeflaterFactory().makeDeflater(level, nowrap);
        Inflater intelInflater = new IntelInflaterFactory().makeInflater(nowrap);

        //act
        intelDeflater.setInput(input,0,input.length);
        intelDeflater.finish();
        intelDeflater.deflate(compressed,0, compressed.length);
        intelDeflater.end();

        intelInflater.setInput(compressed,0,compressed.length);
        intelInflater.inflate(decompressed,0,decompressed.length);
        intelInflater.end();

        //assert
        Assert.assertEquals(decompressed, input);
    }

    @Test(enabled = true)
    public void randomDNATest() {
        final int LEN = TestingUtils.SIZE_4MB;
        final byte[] input = new byte[LEN];
        final byte[] compressed = new byte[2*LEN];
        final byte[] result = new byte[LEN];


        for(int level=0; level <10; level++) {
            for (int i = 0; i < 1; i++) {
                final IntelDeflaterFactory intelDeflaterFactory = new IntelDeflaterFactory();
                final Deflater deflater = intelDeflaterFactory.makeDeflater(level, true);

                Assert.assertTrue(intelDeflaterFactory.usingIntelDeflater());

                final boolean isSupported = new IntelInflater().load(null);
                Assert.assertTrue(isSupported);
                final IntelInflater inflater = new IntelInflater(true);

                TestingUtils.randomDNA(input);

                deflater.setInput(input, 0, input.length);
                deflater.finish();

                int compressedBytes = 0;
                while (!deflater.finished()) {
                    compressedBytes = deflater.deflate(compressed, 0, compressed.length);
                }

                log.info(String.format("%d bytes compressed to %d bytes : %2.2f%% compression",
                        LEN, compressedBytes, (100.0 - 100.0 * compressedBytes / LEN)));

                long totalTime = 0;


                try {

                    inflater.setInput(compressed, 0, compressedBytes);
                    final long start = System.currentTimeMillis();
                    inflater.inflate(result, 0, LEN);
                    totalTime = System.currentTimeMillis() - start;
                    log.info(String.format("%d ", totalTime));


                } catch (Exception e) {
                    e.printStackTrace();
                }

                Assert.assertEquals(input, result);

                inflater.end();
                deflater.end();
            }
        }
    }
}