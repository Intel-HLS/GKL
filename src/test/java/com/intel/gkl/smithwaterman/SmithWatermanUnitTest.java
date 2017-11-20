package com.intel.gkl.smithwaterman;

import com.intel.gkl.smithwaterman.IntelSmithWaterman;
import com.intel.gkl.IntelGKLUtils;
import org.broadinstitute.gatk.nativebindings.smithwaterman.SWParameters;
import org.broadinstitute.gatk.nativebindings.smithwaterman.SWOverhangStrategy;
import org.broadinstitute.gatk.nativebindings.smithwaterman.SWNativeAlignerResult;
import org.testng.Assert;
import org.testng.SkipException;
import org.testng.annotations.Test;

import java.io.*;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

public class SmithWatermanUnitTest {

    private final static Logger logger = LogManager.getLogger(SmithWatermanUnitTest.class);

    static final String smithwatermanData = IntelGKLUtils.pathToTestResource("smith-waterman.SOFTCLIP.in");
    int MAX_SEQ_LEN = 1024;

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

