package com.intel.gkl.smithwaterman;


import com.intel.gkl.smithwaterman.IntelSmithWaterman;
import com.intel.gkl.IntelGKLUtils;
import org.broadinstitute.gatk.nativebindings.smithwaterman.SWParameters;
import org.broadinstitute.gatk.nativebindings.smithwaterman.SWOverhangStrategy;
import org.broadinstitute.gatk.nativebindings.smithwaterman.SWNativeAlignerResult;
import org.testng.Assert;
import org.testng.annotations.Test;

import java.io.*;


public class SmithWatermanUnitTest {

    static final String smithwatermanData = IntelGKLUtils.pathToTestResource("smith-waterman.SOFTCLIP.in");
    static final String smithwatermanOutput = IntelGKLUtils.pathToTestResource("smith-waterman.SOFTCLIP.out");
    int MAX_SEQ_LEN = 1024;

    @Test(enabled = true)
    public void simpleTest() {

        final boolean isloaded = new IntelSmithWaterman().load(null);

        final IntelSmithWaterman SmithWaterman = new IntelSmithWaterman();
        Assert.assertTrue(isloaded);

        try {

            final File inputFile = new File(smithwatermanData);
            final File outputFile = new File(smithwatermanOutput);
            long inputBytes = inputFile.length();
            final FileReader input = new FileReader(inputFile);
            final FileWriter output = new FileWriter(outputFile);
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
              SWNativeAlignerResult result = SmithWaterman.align(refString.getBytes(), altString.getBytes(), SWparameters, SWstrategy);

              //  output.write(result.cigar);
              //  output.write(" ");
              //  output.write(result.alignment_offset);
              //  output.write("\n");

               }

            } catch (java.io.IOException e) {
                e.printStackTrace();

            }
        }

}


