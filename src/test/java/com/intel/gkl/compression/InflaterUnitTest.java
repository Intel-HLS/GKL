package com.intel.gkl.compression;

import com.intel.gkl.IntelGKLUtils;
import com.intel.gkl.testingutils.CompressionDataProviders;
import com.intel.gkl.testingutils.TestingUtils;
import htsjdk.samtools.util.BlockCompressedInputStream;
import org.apache.commons.logging.LogFactory;
import org.apache.commons.logging.Log;
import org.testng.Assert;
import org.testng.annotations.Test;

import java.io.File;
import java.io.IOException;

import java.util.Arrays;
import java.util.zip.DataFormatException;
import java.util.zip.Deflater;
import java.util.zip.Inflater;

/**
 * Created by pnvaidya on 2/1/17.
 */
public class InflaterUnitTest extends CompressionUnitTestBase {

    private final static Log log = LogFactory.getLog(InflaterUnitTest.class);
    private final static String INPUT_FILE = IntelGKLUtils.pathToTestResource("HiSeq.1mb.1RG.2k_lines.bam");

    final File inputFile = new File(INPUT_FILE);
    long inputBytes = inputFile.length();

    @Test(enabled = true, expectedExceptions = NullPointerException.class)
    public void setInputOneArgThrowsNullPointerExceptionWhenNullBufferTest(){
        final Inflater inflater = new IntelInflaterFactory().makeInflater(true);

        inflater.setInput((byte[]) null);

        Assert.fail();
    }

    @Test(enabled = true, expectedExceptions = NullPointerException.class)
    public void setInputThreeArgsThrowsNullPointerExceptionWhenNullBufferTest(){
        final Inflater inflater = new IntelInflaterFactory().makeInflater(true);

        inflater.setInput(null, 0, 10);

        Assert.fail();
    }

    @Test(enabled = true, expectedExceptions = ArrayIndexOutOfBoundsException.class)
    public void setInputThrowsArrayIndexOutOfBoundsExceptionWhenOffsetLessThanZeroTest(){
        final Inflater inflater = new IntelInflaterFactory().makeInflater(true);
        byte[] input = new byte[10];

        inflater.setInput(input, -1, input.length);

        Assert.fail();
    }
    @Test(enabled = true, expectedExceptions = ArrayIndexOutOfBoundsException.class)
    public void setInputThrowsArrayIndexOutOfBoundsExceptionWhenLengthLessThanZeroTest(){
        final Inflater inflater = new IntelInflaterFactory().makeInflater(true);
        byte[] input = new byte[10];

        inflater.setInput(input, -1, -1);

        Assert.fail();
    }
    @Test(enabled = true, expectedExceptions = ArrayIndexOutOfBoundsException.class)
    public void setInputThrowsArrayIndexOutOfBoundsExceptionWhenOffsetGreaterThanLengthTest(){
        final Inflater inflater = new IntelInflaterFactory().makeInflater(true);
        byte[] input = new byte[10];

        inflater.setInput(input, input.length+1, input.length);

        Assert.fail();
    }
    @Test(enabled = true, expectedExceptions = NullPointerException.class)
    public void inflateOneArgThrowsNullPointerExceptionWhenNullBufferTest() throws DataFormatException {
        final Inflater inflater = new IntelInflaterFactory().makeInflater(true);

        inflater.inflate((byte []) null);

        Assert.fail();
    }
    @Test(enabled = true, expectedExceptions = NullPointerException.class)
    public void inflateThreeArgsThrowsNullPointerExceptionWhenNullBufferTest() throws DataFormatException {
       final Inflater inflater = new IntelInflaterFactory().makeInflater(true);

        inflater.inflate(null, 0, 1);

        Assert.fail();
    }
    @Test(enabled = true, expectedExceptions = NullPointerException.class)
    public void inflateInputBufferNotSetTest() throws DataFormatException {
        final Inflater inflater = new IntelInflaterFactory().makeInflater(true);
        byte[] output = new byte[10];
        try
        {
            inflater.inflate(output, 0, 10);
        }
        catch(Exception e){
            log.error(e.getMessage());
            throw e;
        }
        Assert.fail();
    }
    @Test(enabled = true, expectedExceptions = RuntimeException.class)
    public void inflateOutputBufferOverflowShortTest() throws DataFormatException, java.io.UnsupportedEncodingException
    {
        boolean nowrap = true;
        int level = 1;
        final Inflater inflater = new IntelInflaterFactory().makeInflater(nowrap);
        final IntelDeflaterFactory intelDeflaterFactory = new IntelDeflaterFactory();
        final Deflater deflater = intelDeflaterFactory.makeDeflater(level, nowrap);

        try
        {
            String sequence = "ACTGTC";
            byte[] input = sequence.getBytes("UTF-8");
            byte[] compressed = new byte[sequence.length()];
            byte[] result = new byte[sequence.length() - 2];

            deflater.setInput(input);
            deflater.finish();
            int compressedDataLength = deflater.deflate(compressed,0,compressed.length);
            deflater.end();
            log.info(String.format("Compressed length : %d Seq : %s" , compressedDataLength ,
                    new String(compressed, "UTF8")));

            inflater.setInput(compressed, 0, compressedDataLength);
            int resultLength = inflater.inflate(result, 0 , result.length);
            inflater.end();

            String seq2 = new String(result, 0, resultLength, "UTF8");
            log.info(String.format("UnCompressed length : %d Seq : %s" , seq2.length() ,  seq2));
            Assert.assertEquals(sequence, seq2);
        }
        catch(RuntimeException dfe){
            log.error(dfe.getMessage());
            throw dfe;
        }
    }
    @Test(enabled = true, expectedExceptions = NullPointerException.class)
    public void inflateEmptyInputBufferSetOverflowTest() throws DataFormatException, java.io.UnsupportedEncodingException
    {

        final Inflater inflater = new IntelInflaterFactory().makeInflater(true);
        try
        {
            byte[] input = new byte[0];
            byte[] result = new byte[1024];

            inflater.setInput(input, 0, input.length);
            int resultLength = inflater.inflate(result, 0 , 1024);
            if (resultLength != 0)
		 Assert.fail();
	    inflater.end();
        }
        catch(RuntimeException dfe) {
            log.error(dfe.getMessage());
            throw dfe;
        }
        Assert.fail();
    }
    @Test(enabled = true)
    public void inflateNowrapFalseJavaTest() throws DataFormatException, java.io.UnsupportedEncodingException
    {
        boolean nowrap = false; // not supported for level 1 and 2
        int level = 2;
        final Inflater inflater = new IntelInflaterFactory().makeInflater(nowrap);
        final IntelDeflaterFactory intelDeflaterFactory = new IntelDeflaterFactory();
        final Deflater deflater = intelDeflaterFactory.makeDeflater(level, nowrap);
        try
        {
            String sequence = "ACTGTC";
            byte[] input = sequence.getBytes("UTF-8");
            byte[] output = new byte[1024];
            byte[] result = new byte[1024];

            deflater.setInput(input);
            deflater.finish();
            int compressedDataLength = deflater.deflate(output,0,1024);
            deflater.end();
            log.info(String.format("Compressed length : %d Seq : %s" , compressedDataLength ,
                    new String(output, "UTF8")));

            inflater.setInput(output, 0, compressedDataLength);
            int resultLength = inflater.inflate(result, 0 , 1024);
            inflater.end();

            String seq2 = new String(result, 0, resultLength, "UTF8");
            log.info(String.format("UnCompressed length : %d Seq : %s" , seq2.length() ,
                    seq2));
            Assert.assertEquals(sequence, seq2);
        }
        catch(Exception dfe){
            log.error(dfe.getMessage());
        }

    }

    @Test(enabled = true, expectedExceptions = ArrayIndexOutOfBoundsException.class)
    public void inflateThrowsNArrayIndexOutOfBoundsExceptionLengthLessThanZeroTest() throws DataFormatException {
        final Inflater inflater = new IntelInflaterFactory().makeInflater(true);
        byte[] output = new byte[10];

        inflater.inflate(output,0, -1);

        Assert.fail();
    }
    @Test(enabled = true, expectedExceptions = ArrayIndexOutOfBoundsException.class)
    public void inflateThrowsNArrayIndexOutOfBoundsExceptionLOffsetGreaterThanLengthTest() throws DataFormatException {
        final Inflater inflater = new IntelInflaterFactory().makeInflater(true);
        byte[] output = new byte[10];

        inflater.inflate(output,output.length+1, output.length);

        Assert.fail();
    }

    @Test(enabled = true, dataProviderClass = CompressionDataProviders.class,
            dataProvider = compatibilityTestsDataProviderName, groups = {"compatibilityTests"})
    public void javaDeflationIntelInflationCompatibilityTest(int level, boolean nowrap) throws DataFormatException {
        //arrange
        int inputLen = TestingUtils.getMaxInputSizeForGivenOutputSize(compatibilityTestsBufferSize, level);
        int outputLen = compatibilityTestsBufferSize;

        final byte[] input = Arrays.copyOf(compatibilityInputBuffer, inputLen);
        final byte[] compressed = new byte[outputLen];
        final byte[] decompressed = new byte[inputLen];

        Deflater javaDeflater = new Deflater(level, nowrap);
        Inflater intelInflater = new IntelInflaterFactory().makeInflater(nowrap);

        //act
        javaDeflater.setInput(input,0,input.length);
        javaDeflater.finish();
        javaDeflater.deflate(compressed,0,compressed.length);
        javaDeflater.end();

        intelInflater.setInput(compressed,0,compressed.length);
        intelInflater.inflate(decompressed,0,decompressed.length);
        intelInflater.end();

        //assert
        Assert.assertEquals(decompressed,input);
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
	    try {
		inputBytes = inputStream.read(inputbuffer, 0, inputbuffer.length);
	    } catch (IOException e) {System.err.println("Caught IOException: " +  e.getMessage());}
	    inputStream.close();

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
                e.printStackTrace();
            }
            Assert.assertEquals(inputbuffer, finalbuffer);
            inflater.end();
            deflater.end();
        }


    }
}
