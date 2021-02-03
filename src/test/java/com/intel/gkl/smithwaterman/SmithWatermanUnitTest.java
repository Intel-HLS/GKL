package com.intel.gkl.smithwaterman;

import com.intel.gkl.IntelGKLUtils;
import org.broadinstitute.gatk.nativebindings.smithwaterman.SWParameters;
import org.broadinstitute.gatk.nativebindings.smithwaterman.SWOverhangStrategy;
import org.broadinstitute.gatk.nativebindings.smithwaterman.SWNativeAlignerResult;
import org.testng.Assert;
import org.testng.SkipException;
import org.testng.annotations.Test;

import java.io.*;
import java.util.Arrays;
import java.util.Random;

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
    public byte[] randomDNA(int len) {
        Random rng = new Random();
        final byte[] DNA_CHARS = {'A', 'C', 'G', 'T'};
        byte[] array = new byte[len];
        for (int i = 0; i < array.length; i++) {
            array[i] = DNA_CHARS[rng.nextInt(DNA_CHARS.length)];
        }
        return array;
    }

    // MAX SEQUENCE LENGTH = 2^15 -1
    // MAX MATCH VALUE = 2^16 + 2
    @Test
    public void maxSequenceFullAlignmentTest(){
        final IntelSmithWaterman smithWaterman = new IntelSmithWaterman();
        final boolean isLoaded = smithWaterman.load(null);
        
        if(!isLoaded) throw new SkipException("Could not load IntelSmithWaterman; skipping test...");

        int matchValue =64*1024;
        int sequenceLength = 32*1024-1;

        String expectedCigar = String.format("%dM", sequenceLength);

        byte[] ref = randomDNA(sequenceLength);
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

