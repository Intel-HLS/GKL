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


import org.testng.Assert;
import org.testng.annotations.Test;
import com.intel.gkl.compression.IntelInflater;
import htsjdk.samtools.util.zip.DeflaterFactory;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import java.security.SecureRandom;
import java.util.zip.Inflater;
import java.util.zip.Deflater;

public class DeflaterUnitTest {

    final SecureRandom rng = new SecureRandom();
    private final static Logger log = LogManager.getLogger(DeflaterUnitTest.class);

    public void randomDNA(byte[] array) {
        final byte[] DNA_CHARS = {'A', 'C', 'G', 'T'};

        for (int i = 0; i < array.length; i++) {
            array[i] = DNA_CHARS[this.rng.nextInt(DNA_CHARS.length)];
        }
    }

    @Test(enabled = true)
    public void loadLibrary() {
        final boolean isSupported = new IntelDeflater().load(null);
        Assert.assertTrue(isSupported);
        final boolean Supported = new IntelInflater().load(null);
        Assert.assertTrue(Supported);
    }

    @Test(enabled = true)
    public void randomDNATest() {
        final int LEN = 4*1024*1024;
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

                randomDNA(input);
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
