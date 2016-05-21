package com.intel.gkl.compression;

import org.testng.Assert;
import org.testng.annotations.*;

import java.util.Random;
import java.util.zip.Inflater;

public class DeflaterUnitTest {

    public void randomDNA(byte[] array) {
        final byte[] DNA_CHARS = {'A', 'C', 'G', 'T'};
        final Random rng = new Random();

        for (int i = 0; i < array.length; i++) {
            array[i] = DNA_CHARS[rng.nextInt(4)];
        }
    }

    @Test()
    public void randomDNATest() {
        final int LEN = 64*1024;
        final IntelDeflater deflater = new IntelDeflater();

        final byte[] input = new byte[LEN];
        final byte[] compressed = new byte[2*LEN];
        final byte[] result = new byte[LEN];

        for (int i = 0; i < 10; i++) {
            randomDNA(input);
            deflater.reset();
            deflater.setInput(input, 0, input.length);
            deflater.finish();

            int compressedBytes = 0;
            while (!deflater.finished()) {
                compressedBytes = deflater.deflate(compressed, 0, compressed.length);
            }

            System.out.printf("%d bytes compressed to %d bytes : %2.2f%% compression\n",
                    LEN, compressedBytes, 100.0 - 100.0 * compressedBytes / LEN);

            Inflater inflater = new Inflater(true);
            try {
                inflater.setInput(compressed, 0, compressedBytes);
                inflater.inflate(result);
                inflater.end();
            } catch (java.util.zip.DataFormatException e) {
                e.printStackTrace();
            }

            Assert.assertEquals(input, result);
        }
    }

}
