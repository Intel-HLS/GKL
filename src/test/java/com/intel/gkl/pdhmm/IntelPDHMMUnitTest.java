package com.intel.gkl.pdhmm;

import com.intel.gkl.IntelGKLUtils;
import org.testng.Assert;
import org.testng.annotations.Test;

import java.io.FileInputStream;

import htsjdk.samtools.util.BufferedLineReader;

import java.nio.charset.StandardCharsets;
import java.util.Arrays;
import java.util.concurrent.TimeUnit;

import org.apache.commons.lang3.ArrayUtils;

import htsjdk.samtools.SAMUtils;

public class IntelPDHMMUnitTest {

    private static final String[] pdhmmDataFiles = { IntelGKLUtils.pathToTestResource("expected.PDHMM.hmmresults.txt"),
            IntelGKLUtils.pathToTestResource("pdhmm_syn_199_68_51.txt"),
            IntelGKLUtils.pathToTestResource("pdhmm_syn_1412_129_223.txt"),
            IntelGKLUtils.pathToTestResource("pdhmm_syn_990_1_2.txt") };

    static final double DOUBLE_ASSERTION_DELTA = 0.0001;
    public static final int READ_MAX_LENGTH = 200;
    public static final int HAPLOTYPE_MAX_LENGTH = 500;

    @Test(enabled = true)
    public void pdhmmPerformanceTest() {

        final boolean isLoaded = new IntelPDHMM().load(null);

        final IntelPDHMM intelPDHMM = new IntelPDHMM();
        Assert.assertTrue(isLoaded);

        for (String pdhmmData : pdhmmDataFiles) {
            try {
                FileInputStream fis = new FileInputStream(pdhmmData);
                BufferedLineReader br = new BufferedLineReader(fis);
                br.readLine(); // skip first line
                int batchSize = 0;
                int max_read_length = 0, max_hap_length = 0;
                String line;
                while ((line = br.readLine()) != null) {
                    String[] split = line.split("\t"); // Assuming the integers are space-separated
                    byte[] alleleBases = split[0].getBytes(StandardCharsets.UTF_8);
                    byte[] readBases = split[2].getBytes(StandardCharsets.UTF_8);
                    max_hap_length = Math.max(max_hap_length, alleleBases.length);
                    max_read_length = Math.max(max_read_length, readBases.length);
                    batchSize++;
                }
                br.close();

                int hapArraySize = batchSize * max_hap_length;
                int readArraySize = batchSize * max_read_length;

                byte[] alleleBasesFull = new byte[hapArraySize];
                byte[] allelePDBasesFull = new byte[hapArraySize];
                byte[] readBasesFull = new byte[readArraySize];
                byte[] readQualsFull = new byte[readArraySize];
                byte[] readInsQualsFull = new byte[readArraySize];
                byte[] readDelQualsFull = new byte[readArraySize];
                byte[] overallGCPFull = new byte[readArraySize];
                double[] expectedFull = new double[batchSize];
                long[] hapLength = new long[batchSize];
                long[] readLength = new long[batchSize];

                fis.close();
                fis = new FileInputStream(pdhmmData);
                br = new BufferedLineReader(fis);
                br.readLine(); // skip first line

                int currentTestcase = 0;
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
                    System.arraycopy(alleleBases, 0, alleleBasesFull, currentTestcase * max_hap_length,
                            alleleBases.length);
                    System.arraycopy(allelePDBases, 0, allelePDBasesFull, currentTestcase * max_hap_length,
                            allelePDBases.length);
                    System.arraycopy(readBases, 0, readBasesFull, currentTestcase * max_read_length, readBases.length);
                    System.arraycopy(readQuals, 0, readQualsFull, currentTestcase * max_read_length, readQuals.length);
                    System.arraycopy(readInsQuals, 0, readInsQualsFull, currentTestcase * max_read_length,
                            readInsQuals.length);
                    System.arraycopy(readDelQuals, 0, readDelQualsFull, currentTestcase * max_read_length,
                            readDelQuals.length);
                    System.arraycopy(overallGCP, 0, overallGCPFull, currentTestcase * max_read_length,
                            overallGCP.length);

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
                        batchSize, max_hap_length, max_read_length);
                long end = System.nanoTime();
                System.out.println(
                        String.format("Total Elapsed Time = %d ms.", TimeUnit.NANOSECONDS.toMillis(end - start)));
                // Check Values
                for (int i = 0; i < batchSize; i++) {
                    Assert.assertEquals(actual[i], expectedFull[i], DOUBLE_ASSERTION_DELTA,
                            String.format(
                                    "Mismatching score. Actual: %e, Expected: %e. Computed on testcase number %d",
                                    actual[i],
                                    expectedFull[i], i));
                }

            } catch (Exception e) {
                e.printStackTrace();
            }
        }
        intelPDHMM.done();
    }

    @Test(enabled = true)
    public void testInvalidInputsForComputePDHMM() {
        final boolean isLoaded = new IntelPDHMM().load(null);

        final IntelPDHMM intelPDHMM = new IntelPDHMM();
        Assert.assertTrue(isLoaded);
        int batchSize = 1;
        int hapArraySize = batchSize * HAPLOTYPE_MAX_LENGTH;
        int readArraySize = batchSize * READ_MAX_LENGTH;

        byte[] alleleBasesFull = new byte[hapArraySize];
        byte[] allelePDBasesFull = new byte[hapArraySize];
        byte[] readBasesFull = new byte[readArraySize];
        byte[] readQualsFull = new byte[readArraySize];
        byte[] readInsQualsFull = new byte[readArraySize];
        byte[] readDelQualsFull = new byte[readArraySize];
        byte[] overallGCPFull = new byte[readArraySize];
        long[] hapLength = new long[batchSize];
        long[] readLength = new long[batchSize];

        final String NULL_ERROR = "NullPointerException expected.";
        final String ILLEGAL_ARG_ERROR = "IllegalArgumentException expected.";

        try {
            intelPDHMM.computePDHMM(null, allelePDBasesFull, readBasesFull, readQualsFull, readInsQualsFull,
                    readDelQualsFull, overallGCPFull, hapLength, readLength, batchSize, HAPLOTYPE_MAX_LENGTH,
                    READ_MAX_LENGTH);
            Assert.fail(NULL_ERROR);
        } catch (NullPointerException e) {
        }
        try {
            intelPDHMM.computePDHMM(alleleBasesFull, null, readBasesFull, readQualsFull, readInsQualsFull,
                    readDelQualsFull, overallGCPFull, hapLength, readLength, batchSize, HAPLOTYPE_MAX_LENGTH,
                    READ_MAX_LENGTH);
            Assert.fail(NULL_ERROR);
        } catch (NullPointerException e) {
        }
        try {
            intelPDHMM.computePDHMM(alleleBasesFull, allelePDBasesFull, null, readQualsFull, readInsQualsFull,
                    readDelQualsFull, overallGCPFull, hapLength, readLength, batchSize, HAPLOTYPE_MAX_LENGTH,
                    READ_MAX_LENGTH);
            Assert.fail(NULL_ERROR);
        } catch (NullPointerException e) {
        }

        try {
            intelPDHMM.computePDHMM(alleleBasesFull, allelePDBasesFull, readBasesFull, null, readInsQualsFull,
                    readDelQualsFull, overallGCPFull, hapLength, readLength, batchSize, HAPLOTYPE_MAX_LENGTH,
                    READ_MAX_LENGTH);
            Assert.fail(NULL_ERROR);
        } catch (NullPointerException e) {
        }
        try {
            intelPDHMM.computePDHMM(alleleBasesFull, allelePDBasesFull, readBasesFull, readQualsFull, null,
                    readDelQualsFull, overallGCPFull, hapLength, readLength, batchSize, HAPLOTYPE_MAX_LENGTH,
                    READ_MAX_LENGTH);
            Assert.fail(NULL_ERROR);
        } catch (NullPointerException e) {
        }
        try {
            intelPDHMM.computePDHMM(alleleBasesFull, allelePDBasesFull, readBasesFull, readQualsFull, readInsQualsFull,
                    null, overallGCPFull, hapLength, readLength, batchSize, HAPLOTYPE_MAX_LENGTH,
                    READ_MAX_LENGTH);
            Assert.fail(NULL_ERROR);
        } catch (NullPointerException e) {
        }
        try {
            intelPDHMM.computePDHMM(alleleBasesFull, allelePDBasesFull, readBasesFull, readQualsFull, readInsQualsFull,
                    readDelQualsFull, null, hapLength, readLength, batchSize, HAPLOTYPE_MAX_LENGTH,
                    READ_MAX_LENGTH);
            Assert.fail(NULL_ERROR);
        } catch (NullPointerException e) {
        }
        try {
            intelPDHMM.computePDHMM(alleleBasesFull, allelePDBasesFull, readBasesFull, readQualsFull, readInsQualsFull,
                    readDelQualsFull, overallGCPFull, null, readLength, batchSize, HAPLOTYPE_MAX_LENGTH,
                    READ_MAX_LENGTH);
            Assert.fail(NULL_ERROR);
        } catch (NullPointerException e) {
        }
        try {
            intelPDHMM.computePDHMM(alleleBasesFull, allelePDBasesFull, readBasesFull, readQualsFull, readInsQualsFull,
                    readDelQualsFull, overallGCPFull, hapLength, null, batchSize, HAPLOTYPE_MAX_LENGTH,
                    READ_MAX_LENGTH);
            Assert.fail(NULL_ERROR);
        } catch (NullPointerException e) {
        }
        try {
            intelPDHMM.computePDHMM(alleleBasesFull, allelePDBasesFull, readBasesFull, readQualsFull, readInsQualsFull,
                    readDelQualsFull, overallGCPFull, hapLength, readLength, 0, HAPLOTYPE_MAX_LENGTH,
                    READ_MAX_LENGTH);
            Assert.fail(ILLEGAL_ARG_ERROR);
        } catch (IllegalArgumentException e) {
        }
        try {
            intelPDHMM.computePDHMM(alleleBasesFull, allelePDBasesFull, readBasesFull, readQualsFull, readInsQualsFull,
                    readDelQualsFull, overallGCPFull, hapLength, readLength, -1, HAPLOTYPE_MAX_LENGTH,
                    READ_MAX_LENGTH);
            Assert.fail(ILLEGAL_ARG_ERROR);
        } catch (IllegalArgumentException e) {
        }
        try {
            intelPDHMM.computePDHMM(alleleBasesFull, allelePDBasesFull, readBasesFull, readQualsFull, readInsQualsFull,
                    readDelQualsFull, overallGCPFull, hapLength, readLength, batchSize, 0,
                    READ_MAX_LENGTH);
            Assert.fail(ILLEGAL_ARG_ERROR);
        } catch (IllegalArgumentException e) {
        }
        try {
            intelPDHMM.computePDHMM(alleleBasesFull, allelePDBasesFull, readBasesFull, readQualsFull, readInsQualsFull,
                    readDelQualsFull, overallGCPFull, hapLength, readLength, batchSize, -1,
                    READ_MAX_LENGTH);
            Assert.fail(ILLEGAL_ARG_ERROR);
        } catch (IllegalArgumentException e) {
        }
        try {
            intelPDHMM.computePDHMM(alleleBasesFull, allelePDBasesFull, readBasesFull, readQualsFull, readInsQualsFull,
                    readDelQualsFull, overallGCPFull, hapLength, readLength, batchSize, HAPLOTYPE_MAX_LENGTH,
                    -20);
            Assert.fail(ILLEGAL_ARG_ERROR);
        } catch (IllegalArgumentException e) {
        }
        try {
            intelPDHMM.computePDHMM(alleleBasesFull, allelePDBasesFull, readBasesFull, readQualsFull, readInsQualsFull,
                    readDelQualsFull, overallGCPFull, hapLength, readLength, batchSize, 0,
                    -20);
            Assert.fail(ILLEGAL_ARG_ERROR);
        } catch (IllegalArgumentException e) {
        }

        alleleBasesFull = new byte[hapArraySize - 1]; // incorrect sized array

        try {
            intelPDHMM.computePDHMM(
                    alleleBasesFull, allelePDBasesFull, readBasesFull, readQualsFull, readInsQualsFull,
                    readDelQualsFull, overallGCPFull, hapLength, readLength, batchSize, HAPLOTYPE_MAX_LENGTH,
                    READ_MAX_LENGTH);
            Assert.fail(ILLEGAL_ARG_ERROR);
        } catch (IllegalArgumentException e) {
        }

        intelPDHMM.done();
    }

    /**
     * This test repeatedly calls computePDHMM.
     * *
     * <p>
     * To specify the number of iterations (repeatCount), use the following
     * command-line syntax with Gradle:
     * </p>
     *
     * <pre>{@code
     * ./gradlew test -DrepeatCount=<number_of_iterations>
     * }</pre>
     *
     * <p>
     * Example:
     * </p>
     *
     * <pre>{@code
     * ./gradlew test -DrepeatCount=5
     * }</pre>
     *
     * <p>
     * If the repeatCount is not provided or is invalid, it defaults to 1.
     * </p>
     */
    @Test(enabled = true)
    public void repeatedTest() {
        String repeatCountProperty = System.getProperty("repeatCount", "1");
        int repeatCount;
        try {
            repeatCount = Integer.parseInt(repeatCountProperty);
        } catch (NumberFormatException e) {
            System.err.println("Invalid format for repeatCount: " + repeatCountProperty + ". Using default value 1.");
            repeatCount = 1;
        }
        final boolean isLoaded = new IntelPDHMM().load(null);

        Assert.assertTrue(isLoaded);
        final IntelPDHMM intelPDHMM = new IntelPDHMM();
        for (int repeat = 0; repeat < repeatCount; repeat++) {
            for (String pdhmmData : pdhmmDataFiles) {
                try {
                    FileInputStream fis = new FileInputStream(pdhmmData);
                    BufferedLineReader br = new BufferedLineReader(fis);
                    br.readLine(); // skip first line
                    int testcase = 0;
                    int max_read_length = 0, max_hap_length = 0;
                    String line;
                    while ((line = br.readLine()) != null) {
                        String[] split = line.split("\t"); // Assuming the integers are space-separated
                        byte[] alleleBases = split[0].getBytes(StandardCharsets.UTF_8);
                        byte[] readBases = split[2].getBytes(StandardCharsets.UTF_8);
                        max_hap_length = Math.max(max_hap_length, alleleBases.length);
                        max_read_length = Math.max(max_read_length, readBases.length);
                        testcase++;
                    }
                    br.close();

                    int hapArraySize = testcase * max_hap_length;
                    int readArraySize = testcase * max_read_length;

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
                        System.arraycopy(alleleBases, 0, alleleBasesFull, currentTestcase * max_hap_length,
                                alleleBases.length);
                        System.arraycopy(allelePDBases, 0, allelePDBasesFull, currentTestcase * max_hap_length,
                                allelePDBases.length);
                        System.arraycopy(readBases, 0, readBasesFull, currentTestcase * max_read_length,
                                readBases.length);
                        System.arraycopy(readQuals, 0, readQualsFull, currentTestcase * max_read_length,
                                readQuals.length);
                        System.arraycopy(readInsQuals, 0, readInsQualsFull, currentTestcase * max_read_length,
                                readInsQuals.length);
                        System.arraycopy(readDelQuals, 0, readDelQualsFull, currentTestcase * max_read_length,
                                readDelQuals.length);
                        System.arraycopy(overallGCP, 0, overallGCPFull, currentTestcase * max_read_length,
                                overallGCP.length);

                        expectedFull[currentTestcase] = expected;
                        hapLength[currentTestcase] = alleleBases.length;
                        readLength[currentTestcase] = readBases.length;
                        currentTestcase++;
                    }
                    br.close();

                    // Call Function
                    double[] actual = intelPDHMM.computePDHMM(alleleBasesFull, allelePDBasesFull,
                            readBasesFull, readQualsFull, readInsQualsFull, readDelQualsFull,
                            overallGCPFull, hapLength,
                            readLength,
                            testcase, max_hap_length, max_read_length);
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
        }
        intelPDHMM.done();

    }

}
