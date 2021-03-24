package com.intel.gkl.compression;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.zip.DataFormatException;
import java.util.zip.Deflater;
import java.util.zip.Inflater;

import com.intel.gkl.testingutils.TestingUtils;

import org.apache.commons.io.FileUtils;
import org.testng.Assert;
import org.testng.annotations.DataProvider;
import org.testng.annotations.Test;

public class LongReadsDeflateInflateTest {
    // final int BUFFER_SIZE = TestingUtils.MAX_ARRAY_SIZE;
    final int BUFFER_SIZE = 1024*1024;
    @Test(enabled = true, dataProvider = "inflateDeflateData")
    public void testInflateDeflate(String path, Integer level) throws IOException, DataFormatException {
        FileInputStream inputStream = new FileInputStream(path);
        byte[] inputBuff = new byte[BUFFER_SIZE];
        byte[] outputBuff = new byte[BUFFER_SIZE];
        // File outputFile = File.createTempFile("longreads-deflated", null);
        File outputFile = new File("/home/bduser/michalpi/mock-data/deflated.data");
        FileOutputStream outputStream = new FileOutputStream(outputFile);
        // outputFile.deleteOnExit();

        // Deflate
        // Deflater deflater = new IntelDeflaterFactory(null).makeDeflater(level, true);
        Deflater deflater = new Deflater(level, true);

        System.out.println("Starting deflation");
        int inputLength = 0;
        while (inputLength != -1) {
            inputLength = inputStream.read(inputBuff);
            System.out.println(inputLength);
            if (inputLength > 0) {
                deflater.setInput(inputBuff, 0, inputLength);            
            } else {
                deflater.finish();
            }
            int resultLength = deflater.deflate(outputBuff, 0, outputBuff.length);
            System.out.println(resultLength);
            outputStream.write(outputBuff, 0, resultLength);
        }
        deflater.end();

        inputStream.close();
        outputStream.close();
        System.out.println("Deflation finished");
        // return;

        // /*
        // Inflate
        // Inflater inflater = new IntelInflaterFactory(null).makeInflater(true);
        Inflater inflater = new Inflater(true);
        FileInputStream deflatedInputStream = new FileInputStream(outputFile);
        // File inflatedFile = File.createTempFile("longreads-inflated", null);
        File inflatedFile = new File("/home/bduser/michalpi/mock-data/inflated.data");
        FileOutputStream inflatedOutputStream = new FileOutputStream(inflatedFile);
        // inflatedFile.deleteOnExit();

        System.out.println("\nStarting inflation");
        inputLength = 0;
        while (true) {
            inputLength = deflatedInputStream.read(inputBuff);
            if (inputLength == -1) {
                break;
            }
            System.out.println("Loaded: ");
            System.out.println(inputLength);
            inflater.setInput(inputBuff, 0, inputLength);            
            int resultLength;
            do {
                resultLength = inflater.inflate(outputBuff, 0, outputBuff.length);
                System.out.println(inflater.needsInput());
                System.out.println("decompressed: ");
                System.out.println(resultLength);
                inflatedOutputStream.write(outputBuff, 0, resultLength);
            } while (resultLength > 0);
        }
        inflater.end();

        deflatedInputStream.close();
        inflatedOutputStream.close();

        Assert.assertTrue(FileUtils.contentEquals(new File(path), inflatedFile));
        // */
    }

    @DataProvider(name = "inflateDeflateData")
    public static Object[][] inflateDeflateData() {
        return new Object[][] {
            {"/home/bduser/michalpi/mock-data/test.data", 1}
        };
    }
}
