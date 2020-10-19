package com.intel.gkl.compression;

import com.intel.gkl.IntelGKLUtils;
import htsjdk.samtools.util.BlockCompressedInputStream;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.testng.Assert;
import org.testng.annotations.Test;

import java.io.File;
import java.io.IOException;
import java.util.zip.Deflater;
import java.util.zip.Inflater;

/**
 * Created by pnvaidya on 2/1/17.
 */
public class InflaterUnitTest {

    private final static Logger log = LogManager.getLogger(InflaterUnitTest.class);
    private final static String INPUT_FILE = IntelGKLUtils.pathToTestResource("HiSeq.1mb.1RG.2k_lines.bam");

    final File inputFile = new File(INPUT_FILE);
    long inputBytes = inputFile.length();

    final IntelInflaterFactory intelInflaterFactoryInputTest = new IntelInflaterFactory();
    final Inflater inflaterInputTest = intelInflaterFactoryInputTest.makeInflater(true);

    @Test (enabled = true)
    public void testInvalidInputsForSetInput() {
        inflaterInputTest.reset();
        final byte[] inputbuffer = new byte[(int)inputBytes];

        try {
            inflaterInputTest.setInput(null, 0, inputbuffer.length);
            Assert.fail("NullPointerException expected.");
        }catch (NullPointerException ne){}

        try {
            inflaterInputTest.setInput(inputbuffer, -1, inputbuffer.length);
            Assert.fail("IllegalArgumentException expected.");
        }catch (IllegalArgumentException ie){}

        try {
            inflaterInputTest.setInput(inputbuffer, 0, -1);
            Assert.fail("IllegalArgumentException expected.");
        }catch (IllegalArgumentException ie){}


        inflaterInputTest.end();
    }

    @Test (enabled = true)
    public void testInvalidInputsForInflate() {
        final byte[] inputbuffer = new byte[(int)inputBytes];
        final byte[] outputbuffer = new byte[(int)inputBytes];
        
        try{
            
            inflaterInputTest.reset();
            inflaterInputTest.setInput(inputbuffer, 0, inputbuffer.length);
    
            try {
                inflaterInputTest.inflate(null);
                Assert.fail("NullPointerException expected.");
            } catch (NullPointerException ne) {}
    
            try {
                inflaterInputTest.inflate(null, 0, inputbuffer.length);
                Assert.fail("NullPointerException expected.");
            } catch (NullPointerException ne) {}
    
            try {
                inflaterInputTest.inflate(outputbuffer, -1, inputbuffer.length);
                Assert.fail("IllegalArgumentException expected.");
            } catch (IllegalArgumentException ie) {}
    
            try {
                inflaterInputTest.inflate(outputbuffer, 0, -1);
                Assert.fail("IllegalArgumentException expected.");
            } catch (IllegalArgumentException ie) {}
    
            inflaterInputTest.end();

        } catch (java.util.zip.DataFormatException e) {
            e.printStackTrace();
        }

    }

    @Test(enabled = true)
    public void inflaterUnitTest() throws IOException {
        int compressedBytes = 0;
        final byte[] inputbuffer = new byte[(int)inputBytes];
        final byte[] outputbuffer = new byte[(int)inputBytes];
        final byte[] finalbuffer = new byte[(int)inputBytes];

        long totalTime = 0;

        for (int i = 1; i < 10; i++) {

            final IntelInflaterFactory intelInflaterFactory = new IntelInflaterFactory();
            final Inflater inflater = intelInflaterFactory.makeInflater(true);

            final IntelDeflaterFactory intelDeflaterFactory = new IntelDeflaterFactory();
            final Deflater deflater = intelDeflaterFactory.makeDeflater(i, true);


            final BlockCompressedInputStream inputStream = new BlockCompressedInputStream(inputFile);

            inputBytes = inputStream.read(inputbuffer, 0, inputbuffer.length);
            deflater.reset();
            deflater.setInput(inputbuffer, 0, inputbuffer.length);
            deflater.finish();
            compressedBytes = deflater.deflate(outputbuffer, 0, outputbuffer.length);


            try {
                inflater.reset();
                inflater.setInput(outputbuffer, 0, compressedBytes);
                final long start = System.currentTimeMillis();
                inflater.inflate(finalbuffer, 0, inputbuffer.length);
                totalTime = System.currentTimeMillis() - start;
                log.info(String.format("Level: %d, time: %d", i, totalTime));


            } catch (java.util.zip.DataFormatException e) {
                
            }
            Assert.assertEquals(inputbuffer, finalbuffer);
            inflater.end();
            deflater.end();
        }


    }
}
