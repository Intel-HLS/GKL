package com.intel.gkl.smithwaterman;

import com.intel.gkl.IntelGKLUtils;
import com.intel.gkl.testingutils.TestingUtils;
import org.broadinstitute.gatk.nativebindings.smithwaterman.SWParameters;
import org.broadinstitute.gatk.nativebindings.smithwaterman.SWOverhangStrategy;
import org.broadinstitute.gatk.nativebindings.smithwaterman.SWNativeAlignerResult;
import org.testng.Assert;
import org.testng.SkipException;
import org.testng.annotations.Test;

import java.io.*;
import java.util.Arrays;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

public class SmithWatermanUnitTest {

    private final static Logger logger = LogManager.getLogger(SmithWatermanUnitTest.class);

    static final String smithwatermanData = IntelGKLUtils.pathToTestResource("smith-waterman.SOFTCLIP.in");

    @Test(enabled = true)
    public void inputDataTest() {
        final IntelSmithWaterman smithWaterman = new IntelSmithWaterman();

        final boolean isLoaded = smithWaterman.load(null);

        if(!isLoaded) {
            String err = "Could not load IntelSmithWaterman; skipping test...";
            logger.warn(err);
            throw new SkipException(err);
        }

        try {

            final File inputFile = new File(smithwatermanData);
            final FileReader input = new FileReader(inputFile);
            final BufferedReader in = new BufferedReader(input);

            String refString, altString;
            SWParameters SWparameters = new SWParameters(200, -150, -260, -11);
            //SWParameters SWparameters = new SWParameters(3, -1, -4, -3);
            SWOverhangStrategy SWstrategy = SWOverhangStrategy.SOFTCLIP;

            refString = in.readLine();
            altString = in.readLine();

            try {
                smithWaterman.align(null, altString.getBytes(), SWparameters, SWstrategy);
                Assert.fail("NullPointerException expected.");
            } catch(NullPointerException e) {}

            try {
                smithWaterman.align(refString.getBytes(), null, SWparameters, SWstrategy);
                Assert.fail("NullPointerException expected.");
            } catch(NullPointerException e) {}

            try {
                smithWaterman.align(refString.getBytes(), altString.getBytes(), null, SWstrategy);
                Assert.fail("NullPointerException expected.");
            } catch(NullPointerException e) {}

            try {
                smithWaterman.align(refString.getBytes(), altString.getBytes(), SWparameters, null);
                Assert.fail("NullPointerException expected.");
            } catch(NullPointerException e) {}
        } catch (java.io.IOException e) {
            e.printStackTrace();
        }
    }

    @Test(expectedExceptions = IllegalArgumentException.class)
    public void smithWatermanThrowsIllegalArgumentExceptionIfRefSequenceLengthTooLong(){
        final IntelSmithWaterman sw = new IntelSmithWaterman();
        int sequenceLength = TestingUtils.MAX_SW_SEQUENCE_LENGTH+1;

        byte[] ref = TestingUtils.generateRandomDNAArray(sequenceLength);
        byte[] align = new byte[]{'T', 'C', 'C', 'G'};

        SWParameters SWparameters = new SWParameters(10, -5, -10, -10);
        sw.align(ref, align, SWparameters, SWOverhangStrategy.IGNORE);

        Assert.fail();
    }

    @Test(expectedExceptions = IllegalArgumentException.class)
    public void smithWatermanThrowsIllegalArgumentExceptionIfAlignSequenceLengthTooLong(){
        final IntelSmithWaterman sw = new IntelSmithWaterman();
        int sequenceLength = TestingUtils.MAX_SW_SEQUENCE_LENGTH+1;

        byte[] ref = new byte[]{'T', 'C', 'C', 'G'};
        byte[] align = TestingUtils.generateRandomDNAArray(sequenceLength);

        SWParameters SWparameters = new SWParameters(10, -5, -10, -10);
        sw.align(ref, align, SWparameters, SWOverhangStrategy.IGNORE);

        Assert.fail();
    }

    @Test(expectedExceptions = IllegalArgumentException.class)
    public void smithWatermanThrowsIllegalArgumentExceptionIfMatchValueGreaterThanMaxAllowed(){
        final IntelSmithWaterman sw = new IntelSmithWaterman();
        int matchValue = TestingUtils.MAX_SW_MATCH_VALUE + 1;
        byte[] ref = new byte[]{'A', 'C', 'C', 'G'};
        byte[] align = new byte[]{'T', 'C', 'C', 'G'};

        SWParameters SWparameters = new SWParameters(matchValue, -5, -10, -10);
        sw.align(ref, align, SWparameters, SWOverhangStrategy.IGNORE);

        Assert.fail();
    }

    @Test(expectedExceptions = IllegalArgumentException.class)
    public void emptyReferenceSequenceThrowsIllegalArgumentExceptionTest(){
        final IntelSmithWaterman sw = new IntelSmithWaterman();

        byte[] ref = new byte [0];
        byte[] align =  new byte [] {'A', 'C'};

        SWParameters SWparameters = new SWParameters( 3,-2,-2,-1);
        sw.align(ref, align, SWparameters, SWOverhangStrategy.IGNORE);

        Assert.fail();
    }

    @Test(expectedExceptions = IllegalArgumentException.class)
    public void emptyAlignedSequenceThrowsIllegalArgumentExceptionTest(){
        final IntelSmithWaterman sw = new IntelSmithWaterman();

        byte[] ref =  new byte [] {'A', 'C'};
        byte[] align = new byte [0];

        SWParameters SWparameters = new SWParameters( 3,-2,-2,-1);
        sw.align(ref, align, SWparameters, SWOverhangStrategy.IGNORE);

        Assert.fail();
    }

    @Test(enabled = true)
    public void singleElementSequencesAlignmentTest(){
        final IntelSmithWaterman sw = new IntelSmithWaterman();
        boolean isLoaded = sw.load(null);
        if(!isLoaded) throw new SkipException("Could not load IntelSmithWaterman; skipping test...");

        byte[] ref =    new byte [] {'C'};
        byte[] align =  new byte [] {'C'};

        SWParameters SWparameters = new SWParameters( 3,-2,-2,-1);
        SWNativeAlignerResult result = sw.align(ref, align, SWparameters, SWOverhangStrategy.IGNORE);

        Assert.assertEquals(result.cigar, "1M");
    }
    @Test(enabled = true)
    public void twoElementSequencesAlignmentTest(){
        final IntelSmithWaterman sw = new IntelSmithWaterman();
        boolean isLoaded = sw.load(null);
        if(!isLoaded) throw new SkipException("Could not load IntelSmithWaterman; skipping test...");

        byte[] ref =    new byte [] {'A', 'D'};
        byte[] align =  new byte [] {'A', 'T'};

        SWParameters SWparameters = new SWParameters( 3,-5,-2,-1);
        SWNativeAlignerResult result = sw.align(ref, align, SWparameters, SWOverhangStrategy.IGNORE);

        Assert.assertEquals(result.cigar, "1M1I");
    }

    @Test(enabled = false)
    public void maxSequenceFullAlignmentTest(){
        final IntelSmithWaterman smithWaterman = new IntelSmithWaterman();
        final boolean isLoaded = smithWaterman.load(null);
        
        if(!isLoaded) throw new SkipException("Could not load IntelSmithWaterman; skipping test...");

        int matchValue = TestingUtils.MAX_SW_MATCH_VALUE;
        int sequenceLength = TestingUtils.MAX_SW_SEQUENCE_LENGTH;

        String expectedCigar = String.format("%dM", sequenceLength);

        byte[] ref = TestingUtils.generateRandomDNAArray(sequenceLength);
        byte[] align = Arrays.copyOf(ref, ref.length);

        SWParameters SWparameters = new SWParameters(matchValue, -5, -10, -10);
        SWNativeAlignerResult result = smithWaterman.align(ref, align, SWparameters, SWOverhangStrategy.IGNORE);


        Assert.assertEquals(result.cigar, expectedCigar);
    }

    @Test(enabled = true)
    public void simpleTest() {

        final IntelSmithWaterman smithWaterman = new IntelSmithWaterman();

        // skip test if SW could not be loaded (most likely cause: AVX2
        // not supported)
        final boolean isLoaded = smithWaterman.load(null);
        if(!isLoaded) {
            String err = "Could not load IntelSmithWaterman; skipping test...";
            logger.warn(err);
            throw new SkipException(err);
        }

        try {

            final File inputFile = new File(smithwatermanData);
            long inputBytes = inputFile.length();
            final FileReader input = new FileReader(inputFile);
            final BufferedReader in = new BufferedReader(input);

            byte[] ref;
            byte[] alt;

            String refString, altString;
            SWParameters SWparameters = new SWParameters(200, -150, -260, -11);
            //SWParameters SWparameters = new SWParameters(3, -1, -4, -3);
            SWOverhangStrategy SWstrategy = SWOverhangStrategy.SOFTCLIP;


            while(in.readLine() !=null) {

               refString = in.readLine();
                ref = refString.getBytes();

                altString = in.readLine();
               alt = altString.getBytes();


                //Get the results for one pair
                SWNativeAlignerResult result = smithWaterman.align(refString.getBytes(), altString.getBytes(), SWparameters, SWstrategy);

            }

        } catch (java.io.IOException e) {
            e.printStackTrace();
        }
    }
}

