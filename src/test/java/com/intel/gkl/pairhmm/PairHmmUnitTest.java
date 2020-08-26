package com.intel.gkl.pairhmm;

import com.intel.gkl.IntelGKLUtils;
import com.intel.gkl.NativeLibraryLoader;
import com.intel.gkl.pairhmm.IntelPairHmm;
import org.broadinstitute.gatk.nativebindings.pairhmm.HaplotypeDataHolder;
import org.broadinstitute.gatk.nativebindings.pairhmm.PairHMMNativeArguments;
import org.broadinstitute.gatk.nativebindings.pairhmm.PairHMMNativeBinding;
import org.broadinstitute.gatk.nativebindings.pairhmm.ReadDataHolder;
import org.testng.Assert;
import org.testng.annotations.Test;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.util.Scanner;

public class PairHmmUnitTest {
    static final String pairHMMTestData = IntelGKLUtils.pathToTestResource("pairhmm-testdata.txt");

    @Test(enabled = true)
    public void simpleTest() {

        final boolean isloaded = new IntelPairHmm().load(null);

        final IntelPairHmm pairHmm = new IntelPairHmm();
        Assert.assertTrue(isloaded);

        final PairHMMNativeArguments args = new PairHMMNativeArguments();
        args.maxNumberOfThreads = 1;
        args.useDoublePrecision = false;

        pairHmm.initialize(args);

        ReadDataHolder[] readDataArray = new ReadDataHolder[1];
        HaplotypeDataHolder[] haplotypeDataArray = new HaplotypeDataHolder[1];
        double[] likelihoodArray = new double[1];

        // read data from file
        haplotypeDataArray[0] = new HaplotypeDataHolder();
        haplotypeDataArray[0].haplotypeBases = "ACGT".getBytes();
        readDataArray[0] = new ReadDataHolder();
        readDataArray[0].readBases = "ACGT".getBytes();
        readDataArray[0].readQuals = "++++".getBytes();
        readDataArray[0].insertionGOP = "++++".getBytes();
        readDataArray[0].deletionGOP = "++++".getBytes();
        readDataArray[0].overallGCP = "++++".getBytes();
        double expectedResult = -6.022797e-01;

        // call pairHMM
        pairHmm.computeLikelihoods(readDataArray, haplotypeDataArray, likelihoodArray);

        // check result
        Assert.assertEquals(likelihoodArray[0], expectedResult, 1e-5, "Likelihood not in expected range.");
    }

    @Test(enabled = true)
    public void fpgaTest() {
        final boolean shaccIsLoaded = NativeLibraryLoader.load(null, "gkl_pairhmm_shacc");
        Assert.assertFalse(shaccIsLoaded);

        final boolean isloaded = new IntelPairHmmFpga().load(null);
        Assert.assertTrue(isloaded);
    }

    @Test(enabled = true)
    public void omp_Test() {
        final boolean isSupported = new IntelPairHmmOMP().load(null);

        if(!isSupported) {
            IntelPairHmm pairHmm = new IntelPairHmm();
            final boolean isloaded = new IntelPairHmm().load(null);


            Assert.assertTrue(isloaded);

            final PairHMMNativeArguments args = new PairHMMNativeArguments();
            args.maxNumberOfThreads = 1;
            args.useDoublePrecision = false;

            pairHmm.initialize(args);

            ReadDataHolder[] readDataArray = new ReadDataHolder[1];
            HaplotypeDataHolder[] haplotypeDataArray = new HaplotypeDataHolder[1];
            double[] likelihoodArray = new double[1];

            // read data from file
            haplotypeDataArray[0] = new HaplotypeDataHolder();
            haplotypeDataArray[0].haplotypeBases = "ACGT".getBytes();
            readDataArray[0] = new ReadDataHolder();
            readDataArray[0].readBases = "ACGT".getBytes();
            readDataArray[0].readQuals = "++++".getBytes();
            readDataArray[0].insertionGOP = "++++".getBytes();
            readDataArray[0].deletionGOP = "++++".getBytes();
            readDataArray[0].overallGCP = "++++".getBytes();
            double expectedResult = -6.022797e-01;

            // call pairHMM
            pairHmm.computeLikelihoods(readDataArray, haplotypeDataArray, likelihoodArray);

            // check result
            Assert.assertEquals(likelihoodArray[0], expectedResult, 1e-5, "Likelihood not in expected range.");
        }
        else {
            final IntelPairHmmOMP pairHmm = new IntelPairHmmOMP();
            Assert.assertTrue(isSupported);

            final PairHMMNativeArguments args = new PairHMMNativeArguments();
            args.maxNumberOfThreads = 10;
            args.useDoublePrecision = false;

            pairHmm.initialize(args);

            ReadDataHolder[] readDataArray = new ReadDataHolder[1];
            HaplotypeDataHolder[] haplotypeDataArray = new HaplotypeDataHolder[1];
            double[] likelihoodArray = new double[1];

            // read data from file
            haplotypeDataArray[0] = new HaplotypeDataHolder();
            haplotypeDataArray[0].haplotypeBases = "ACGT".getBytes();
            readDataArray[0] = new ReadDataHolder();
            readDataArray[0].readBases = "ACGT".getBytes();
            readDataArray[0].readQuals = "++++".getBytes();
            readDataArray[0].insertionGOP = "++++".getBytes();
            readDataArray[0].deletionGOP = "++++".getBytes();
            readDataArray[0].overallGCP = "++++".getBytes();
            double expectedResult = -6.022797e-01;

            // call pairHMM
            pairHmm.computeLikelihoods(readDataArray, haplotypeDataArray, likelihoodArray);

            // check result
            Assert.assertEquals(likelihoodArray[0], expectedResult, 1e-5, "Likelihood not in expected range.");
        }
    }

    @Test(enabled = true)
    public void dataFileTest() {
        // load native library
        final boolean isSupported = new IntelPairHmm().load(null);
        Assert.assertTrue(isSupported);

        boolean[] udvals = {false, true};
        for(boolean useDbl : udvals) {
            // instantiate and initialize IntelPairHmm
            final IntelPairHmm pairHmm = new IntelPairHmm();

            final PairHMMNativeArguments args = new PairHMMNativeArguments();
            args.maxNumberOfThreads = 1;
            args.useDoublePrecision = useDbl;
            pairHmm.initialize(args);

            // data structures
            ReadDataHolder[] readDataArray = new ReadDataHolder[1];
            readDataArray[0] = new ReadDataHolder();
            HaplotypeDataHolder[] haplotypeDataArray = new HaplotypeDataHolder[1];
            haplotypeDataArray[0] = new HaplotypeDataHolder();
            double[] likelihoodArray = new double[1];

            // read test data from file
            Scanner s = null;
            try {
                s = new Scanner(new BufferedReader(new FileReader(pairHMMTestData)));

                while (s.hasNext()) {
                    // skip comment lines
                    if(s.hasNext("#.*")) {
                        s.nextLine();
                        continue;
                    }

                    haplotypeDataArray[0].haplotypeBases = s.next().getBytes();
                    readDataArray[0].readBases = s.next().getBytes();
                    readDataArray[0].readQuals = normalize(s.next().getBytes(), 6);
                    readDataArray[0].insertionGOP = normalize(s.next().getBytes());
                    readDataArray[0].deletionGOP = normalize(s.next().getBytes());
                    readDataArray[0].overallGCP = normalize(s.next().getBytes());
                    double expectedResult = s.nextDouble();

                    // call pairHMM
                    pairHmm.computeLikelihoods(readDataArray, haplotypeDataArray, likelihoodArray);

                    // check result
                    Assert.assertEquals(likelihoodArray[0], expectedResult, 1e-5, "Likelihood not in expected range.");
                }
            } catch (FileNotFoundException e) {
                Assert.fail("File not found : " + pairHMMTestData);
            } catch (Exception e) {
                e.printStackTrace();
                Assert.fail("Unexpected exception");
            }

            s.close();
            pairHmm.done();
        }
    }

    @Test(enabled = false)
    public void testDataFileBatchTest() {
        // load native library
        final boolean isSupported = new IntelPairHmm().load(null);
        Assert.assertTrue(isSupported);

        // instantiate and initialize IntelPairHmm
        final IntelPairHmm pairHmm = new IntelPairHmm();
        pairHmm.initialize(null);

        int maxBatchSize = 10;

        // data structures
        ReadDataHolder[] readDataArray = new ReadDataHolder[maxBatchSize];
        HaplotypeDataHolder[] haplotypeDataArray = new HaplotypeDataHolder[maxBatchSize];
        for (int i = 0; i < maxBatchSize; i++) {
            readDataArray[i] = new ReadDataHolder();
            haplotypeDataArray[i] = new HaplotypeDataHolder();
        }
        double[] likelihoodArray = new double[maxBatchSize];
        double[] expectedResult = new double[maxBatchSize];

        // read test data from file
        Scanner s = null;
        try {
            s = new Scanner(new BufferedReader(new FileReader(pairHMMTestData)));

            int batchSize = 0;
            while (s.hasNext()) {
                // skip comment lines
                if(s.hasNext("#.*")) {
                    s.nextLine();
                    continue;
                }

                haplotypeDataArray[batchSize].haplotypeBases = s.next().getBytes();
                readDataArray[batchSize].readBases = s.next().getBytes();
                readDataArray[batchSize].readQuals = normalize(s.next().getBytes(), 6);
                readDataArray[batchSize].insertionGOP = normalize(s.next().getBytes());
                readDataArray[batchSize].deletionGOP = normalize(s.next().getBytes());
                readDataArray[batchSize].overallGCP = normalize(s.next().getBytes());
                expectedResult[batchSize] = s.nextDouble();
                System.out.printf("expected[%d] = %e\n", batchSize, expectedResult[batchSize]);
                batchSize++;

                if (batchSize == maxBatchSize) {
                    // call pairHMM
                    pairHmm.computeLikelihoods(readDataArray, haplotypeDataArray, likelihoodArray);

                    // check result
                    for (int i = 0; i < batchSize; i++) {
                        System.out.printf("result[%d] = %e, expected = %e", i, likelihoodArray[i], expectedResult[i]);
                        Assert.assertEquals(likelihoodArray[i], expectedResult[i], 1e-5, "Likelihood not in expected range.");
                    }
                    batchSize = 0;
                }
            }
        } catch (FileNotFoundException e) {
            Assert.fail("File not found : " + pairHMMTestData);
        } catch (Exception e) {
            e.printStackTrace();
            Assert.fail("Unexpected exception");
        }

        s.close();
        pairHmm.done();
    }


    static byte[] normalize(byte[] scores) {
        return normalize(scores, 0);
    }

    static byte[] normalize(byte[] scores, int min) {
        for (int i = 0; i < scores.length; i++) {
            scores[i] -= 33;
            scores[i] = scores[i] < min ? (byte)min : scores[i];
        }
        return scores;
    }
}
