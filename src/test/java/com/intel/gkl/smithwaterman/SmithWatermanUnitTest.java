package com.intel.gkl.smithwaterman;

import com.intel.gkl.IntelGKLUtils;
import com.intel.gkl.smithwaterman.IntelSmithWaterman;
import htsjdk.samtools.util.BlockCompressedInputStream;
import org.broadinstitute.gatk.nativebindings.smithwaterman.SWAlignerArguments;
import org.broadinstitute.gatk.nativebindings.smithwaterman.SWAlignmentResult;
import org.testng.Assert;
import org.testng.annotations.Test;

import java.io.File;
import java.io.FileReader;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.InputStream;

/**
 * Created by pnvaidya on 4/17/17.
 */
public class SmithWatermanUnitTest {

    static final String smithwatermanData = IntelGKLUtils.pathToTestResource("smith-waterman.SOFTCLIP.in");
    int MAX_SEQ_LEN = 1024;

    @Test(enabled = true)
    public void simpleTest() {

        final boolean isloaded = new IntelSmithWaterman().load(null);

        final IntelSmithWaterman smithWaterman = new IntelSmithWaterman();
        Assert.assertTrue(isloaded);

            try {

                final File inputFile = new File(smithwatermanData);
                long inputBytes = inputFile.length();
                final FileReader input = new FileReader(inputFile);
                final BufferedReader in = new BufferedReader(input);

                byte[] ref = new byte[1024*4];
                byte[] alt = new byte[1024*4];
                byte[] res = new byte[1024*4];
                String refString, altString;

                for(int i=0;i<MAX_SEQ_LEN;i++) {

                    SWAlignerArguments args = new SWAlignerArguments(SWAlignerArguments.OverhangStrategy.SOFTCLIP,-11,200,-150,-260);

                    smithWaterman.initialize(args);
                    refString = in.readLine();
                    ref = refString.getBytes();

                    altString = in.readLine();
                    alt = altString.getBytes();

                    if(refString == null) break;
                    if(altString == null) break;

                    //Get the results for one pair
                    SWAlignmentResult result = smithWaterman.align(ref, alt);

                    res = result.cigar.getBytes();

                }

            } catch (java.io.IOException e) {
                e.printStackTrace();

            }
    }
}


