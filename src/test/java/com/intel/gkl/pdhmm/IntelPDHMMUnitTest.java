package com.intel.gkl.pdhmm;

import com.intel.gkl.IntelGKLUtils;
import org.testng.Assert;
import org.testng.annotations.Test;

import java.io.FileInputStream;

import htsjdk.samtools.util.BufferedLineReader;

import java.nio.charset.StandardCharsets;
import java.util.Arrays;

import org.apache.commons.lang3.ArrayUtils;

import htsjdk.samtools.SAMUtils;

public class IntelPDHMMUnitTest {
    static final String pdhmmData = IntelGKLUtils.pathToTestResource("expected.PDHMM.hmmresults.txt");
    static final double DOUBLE_ASSERTION_DELTA = 0.0001;
    public static final int READ_MAX_LENGTH = 200;
    public static final int HAPLOTYPE_MAX_LENGTH = 500;

    @Test(enabled = true)
    public void pdhmmPerformanceTest() {

        final boolean isloaded = new IntelPDHMM().load(null);

        final IntelPDHMM intelPDHMM = new IntelPDHMM();
        Assert.assertTrue(isloaded);

        try {
            FileInputStream fis = new FileInputStream(pdhmmData);
            BufferedLineReader br = new BufferedLineReader(fis);
            br.readLine(); // skip first line
            int testcase = 0;

            while ((br.readLine()) != null) {
                testcase++;
            }
            br.close();

            int hapArraySize = testcase * HAPLOTYPE_MAX_LENGTH;
            int readArraySize = testcase * READ_MAX_LENGTH;

            byte[] alleleBasesFull = new byte[hapArraySize];
            byte[] allelePDBasesFull = new byte[hapArraySize];
            byte[] readBasesFull = new byte[readArraySize];
            byte[] readQualsFull = new byte[readArraySize];
            byte[] readInsQualsFull = new byte[readArraySize];
            byte[] readDelQualsFull = new byte[readArraySize];
            byte[] overallGCPFull = new byte[readArraySize];
            double[] expectedFull = new double[testcase];
            long[] hapLength = new long[testcase];
            long[] readLength = new long[testcase];

            fis.close();
            fis = new FileInputStream(pdhmmData);
            br = new BufferedLineReader(fis);
            br.readLine(); // skip first line

            int currentTestcase = 0;
            String line;
            while ((line = br.readLine()) != null) {
                String[] split = line.split("\t"); // Assuming the integers are space-separated
                byte[] alleleBases = split[0].getBytes(StandardCharsets.UTF_8);
                byte[] allelePDBases = ArrayUtils.toPrimitive(
                        Arrays.stream(split[1].substring(1, split[1].length() - 1).split(","))
                                .map(num -> Byte.parseByte(num.trim())).toArray(Byte[]::new));
                byte[] readBases = split[2].getBytes(StandardCharsets.UTF_8);
                byte[] readQuals = SAMUtils.fastqToPhred(split[3]);
                byte[] readInsQuals = SAMUtils.fastqToPhred(split[4]);
                byte[] readDelQuals = SAMUtils.fastqToPhred(split[5]);
                byte[] overallGCP = SAMUtils.fastqToPhred(split[6]);
                double expected = Double.parseDouble(split[7]);

                // append testcase to full arrays
                System.arraycopy(alleleBases, 0, alleleBasesFull, currentTestcase * HAPLOTYPE_MAX_LENGTH,
                        alleleBases.length);
                System.arraycopy(allelePDBases, 0, allelePDBasesFull, currentTestcase * HAPLOTYPE_MAX_LENGTH,
                        allelePDBases.length);
                System.arraycopy(readBases, 0, readBasesFull, currentTestcase * READ_MAX_LENGTH, readBases.length);
                System.arraycopy(readQuals, 0, readQualsFull, currentTestcase * READ_MAX_LENGTH, readQuals.length);
                System.arraycopy(readInsQuals, 0, readInsQualsFull, currentTestcase * READ_MAX_LENGTH,
                        readInsQuals.length);
                System.arraycopy(readDelQuals, 0, readDelQualsFull, currentTestcase * READ_MAX_LENGTH,
                        readDelQuals.length);
                System.arraycopy(overallGCP, 0, overallGCPFull, currentTestcase * READ_MAX_LENGTH, overallGCP.length);

                expectedFull[currentTestcase] = expected;
                hapLength[currentTestcase] = alleleBases.length;
                readLength[currentTestcase] = readBases.length;
                currentTestcase++;
            }
            br.close();

            // Call Function
            long start = System.nanoTime();
            double[] actual = intelPDHMM.computePDHMM(alleleBasesFull, allelePDBasesFull,
                    readBasesFull, readQualsFull, readInsQualsFull, readDelQualsFull,
                    overallGCPFull, hapLength,
                    readLength,
                    testcase, HAPLOTYPE_MAX_LENGTH, READ_MAX_LENGTH);
            long end = System.nanoTime();
            System.out.println("Total Elapsed Time = " + (end - start) / 1e9);
            // Check Values
            for (int i = 0; i < testcase; i++) {
                Assert.assertEquals(actual[i], expectedFull[i], DOUBLE_ASSERTION_DELTA,
                        String.format(
                                "Mismatching score actual: %e expected: %e computed on testcase number %d",
                                actual[i],
                                expectedFull[i], i));

            }

        } catch (Exception e) {
            e.printStackTrace();
        }

    }

    @Test(enabled = true)
    public void testInvalidInputsForComputePDHMM() {
        final boolean isloaded = new IntelPDHMM().load(null);

        final IntelPDHMM intelPDHMM = new IntelPDHMM();
        Assert.assertTrue(isloaded);
        int testcase = 1;
        int hapArraySize = testcase * HAPLOTYPE_MAX_LENGTH;
        int readArraySize = testcase * READ_MAX_LENGTH;

        byte[] alleleBasesFull = new byte[hapArraySize];
        byte[] allelePDBasesFull = new byte[hapArraySize];
        byte[] readBasesFull = new byte[readArraySize];
        byte[] readQualsFull = new byte[readArraySize];
        byte[] readInsQualsFull = new byte[readArraySize];
        byte[] readDelQualsFull = new byte[readArraySize];
        byte[] overallGCPFull = new byte[readArraySize];
        long[] hapLength = new long[testcase];
        long[] readLength = new long[testcase];

        try {
            intelPDHMM.computePDHMM(null, allelePDBasesFull, readBasesFull, readQualsFull, readInsQualsFull,
                    readDelQualsFull, overallGCPFull, hapLength, readLength, testcase, HAPLOTYPE_MAX_LENGTH,
                    READ_MAX_LENGTH);
            Assert.fail("NullPointerException expected.");
        } catch (NullPointerException e) {
        }
        try {
            intelPDHMM.computePDHMM(alleleBasesFull, null, readBasesFull, readQualsFull, readInsQualsFull,
                    readDelQualsFull, overallGCPFull, hapLength, readLength, testcase, HAPLOTYPE_MAX_LENGTH,
                    READ_MAX_LENGTH);
            Assert.fail("NullPointerException expected.");
        } catch (NullPointerException e) {
        }
        try {
            intelPDHMM.computePDHMM(alleleBasesFull, allelePDBasesFull, null, readQualsFull, readInsQualsFull,
                    readDelQualsFull, overallGCPFull, hapLength, readLength, testcase, HAPLOTYPE_MAX_LENGTH,
                    READ_MAX_LENGTH);
            Assert.fail("NullPointerException expected.");
        } catch (NullPointerException e) {
        }

        try {
            intelPDHMM.computePDHMM(alleleBasesFull, allelePDBasesFull, readBasesFull, null, readInsQualsFull,
                    readDelQualsFull, overallGCPFull, hapLength, readLength, testcase, HAPLOTYPE_MAX_LENGTH,
                    READ_MAX_LENGTH);
            Assert.fail("NullPointerException expected.");
        } catch (NullPointerException e) {
        }
        try {
            intelPDHMM.computePDHMM(alleleBasesFull, allelePDBasesFull, readBasesFull, readQualsFull, null,
                    readDelQualsFull, overallGCPFull, hapLength, readLength, testcase, HAPLOTYPE_MAX_LENGTH,
                    READ_MAX_LENGTH);
            Assert.fail("NullPointerException expected.");
        } catch (NullPointerException e) {
        }
        try {
            intelPDHMM.computePDHMM(alleleBasesFull, allelePDBasesFull, readBasesFull, readQualsFull, readInsQualsFull,
                    null, overallGCPFull, hapLength, readLength, testcase, HAPLOTYPE_MAX_LENGTH,
                    READ_MAX_LENGTH);
            Assert.fail("NullPointerException expected.");
        } catch (NullPointerException e) {
        }
        try {
            intelPDHMM.computePDHMM(alleleBasesFull, allelePDBasesFull, readBasesFull, readQualsFull, readInsQualsFull,
                    readDelQualsFull, null, hapLength, readLength, testcase, HAPLOTYPE_MAX_LENGTH,
                    READ_MAX_LENGTH);
            Assert.fail("NullPointerException expected.");
        } catch (NullPointerException e) {
        }
        try {
            intelPDHMM.computePDHMM(alleleBasesFull, allelePDBasesFull, readBasesFull, readQualsFull, readInsQualsFull,
                    readDelQualsFull, overallGCPFull, null, readLength, testcase, HAPLOTYPE_MAX_LENGTH,
                    READ_MAX_LENGTH);
            Assert.fail("NullPointerException expected.");
        } catch (NullPointerException e) {
        }
        try {
            intelPDHMM.computePDHMM(alleleBasesFull, allelePDBasesFull, readBasesFull, readQualsFull, readInsQualsFull,
                    readDelQualsFull, overallGCPFull, hapLength, null, testcase, HAPLOTYPE_MAX_LENGTH,
                    READ_MAX_LENGTH);
            Assert.fail("NullPointerException expected.");
        } catch (NullPointerException e) {
        }
        try {
            intelPDHMM.computePDHMM(alleleBasesFull, allelePDBasesFull, readBasesFull, readQualsFull, readInsQualsFull,
                    readDelQualsFull, overallGCPFull, hapLength, readLength, 0, HAPLOTYPE_MAX_LENGTH,
                    READ_MAX_LENGTH);
            Assert.fail("IllegalArgumentException expected.");
        } catch (IllegalArgumentException e) {
        }
        try {
            intelPDHMM.computePDHMM(alleleBasesFull, allelePDBasesFull, readBasesFull, readQualsFull, readInsQualsFull,
                    readDelQualsFull, overallGCPFull, hapLength, readLength, testcase, -1,
                    READ_MAX_LENGTH);
            Assert.fail("IllegalArgumentException expected.");
        } catch (IllegalArgumentException e) {
        }
        try {
            intelPDHMM.computePDHMM(alleleBasesFull, allelePDBasesFull, readBasesFull, readQualsFull, readInsQualsFull,
                    readDelQualsFull, overallGCPFull, hapLength, readLength, testcase, HAPLOTYPE_MAX_LENGTH,
                    -20);
            Assert.fail("IllegalArgumentException expected.");
        } catch (IllegalArgumentException e) {
        }

    }

}
