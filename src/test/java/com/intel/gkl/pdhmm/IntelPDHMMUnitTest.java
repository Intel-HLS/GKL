package com.intel.gkl.pdhmm;

import com.intel.gkl.IntelGKLUtils;
import org.testng.Assert;
import org.testng.annotations.AfterMethod;
import org.testng.annotations.BeforeMethod;
import org.testng.annotations.DataProvider;
import org.testng.annotations.Test;

import java.io.FileInputStream;

import htsjdk.samtools.util.BufferedLineReader;

import java.lang.reflect.Array;
import java.nio.charset.StandardCharsets;
import java.util.Arrays;

import org.apache.commons.lang3.ArrayUtils;

import htsjdk.samtools.SAMUtils;

public class IntelPDHMMUnitTest {
    private static class PDHMMTestData {
        private final int hapArraySize;
        private final int readArraySize;

        public byte[] hapBases;
        public byte[] hapPdbases;
        public byte[] readBases;
        public byte[] readQual;
        public byte[] readInsQual;
        public byte[] readDelQual;
        public byte[] gcp;
        public long[] hapLengths;
        public long[] readLengths;
        public int batchSize;
        public int maxHapLength;
        public int maxReadLength;

        PDHMMTestData(int batchSize, int maxHapLength, int maxReadLength) {
            this.batchSize = batchSize;
            this.maxHapLength = maxHapLength;
            this.maxReadLength = maxReadLength;

            hapArraySize = this.batchSize * this.maxHapLength;
            hapBases = new byte[hapArraySize];
            hapPdbases = new byte[hapArraySize];

            readArraySize = this.batchSize * this.maxReadLength;
            readBases = new byte[readArraySize];
            readQual = new byte[readArraySize];
            readInsQual = new byte[readArraySize];
            readDelQual = new byte[readArraySize];
            gcp = new byte[readArraySize];

            hapLengths = new long[this.batchSize];
            readLengths = new long[this.batchSize];
        }

        PDHMMTestData copyWithHapBases(byte[] hapBases) {
            PDHMMTestData td = new PDHMMTestData(this.batchSize, this.maxHapLength, this.maxReadLength);
            td.hapBases = hapBases;
            return td;
        }

        PDHMMTestData copyWithHapPdbases(byte[] hapPdbases) {
            PDHMMTestData td = new PDHMMTestData(this.batchSize, this.maxHapLength, this.maxReadLength);
            td.hapPdbases = hapPdbases;
            return td;
        }

        PDHMMTestData copyWithReadBases(byte[] readBases) {
            PDHMMTestData td = new PDHMMTestData(this.batchSize, this.maxHapLength, this.maxReadLength);
            td.readBases = readBases;
            return td;
        }

        PDHMMTestData copyWithReadQual(byte[] readQual) {
            PDHMMTestData td = new PDHMMTestData(this.batchSize, this.maxHapLength, this.maxReadLength);
            td.readQual = readQual;
            return td;
        }

        PDHMMTestData copyWithReadInsQual(byte[] readInsQual) {
            PDHMMTestData td = new PDHMMTestData(this.batchSize, this.maxHapLength, this.maxReadLength);
            td.readInsQual = readInsQual;
            return td;
        }

        PDHMMTestData copyWithReadDelQual(byte[] readDelQual) {
            PDHMMTestData td = new PDHMMTestData(this.batchSize, this.maxHapLength, this.maxReadLength);
            td.readDelQual = readDelQual;
            return td;
        }

        PDHMMTestData copyWithGcp(byte[] gcp) {
            PDHMMTestData td = new PDHMMTestData(this.batchSize, this.maxHapLength, this.maxReadLength);
            td.gcp = gcp;
            return td;
        }

        PDHMMTestData copyWithHapLengths(long[] hapLengths) {
            PDHMMTestData td = new PDHMMTestData(this.batchSize, this.maxHapLength, this.maxReadLength);
            td.hapLengths = hapLengths;
            return td;
        }

        PDHMMTestData copyWithReadLengths(long[] readLengths) {
            PDHMMTestData td = new PDHMMTestData(this.batchSize, this.maxHapLength, this.maxReadLength);
            td.readLengths = readLengths;
            return td;
        }

        PDHMMTestData copyWithBatchSize(int batchSize) {
            PDHMMTestData td = new PDHMMTestData(this.batchSize, this.maxHapLength, this.maxReadLength);
            td.batchSize = batchSize;
            return td;
        }

        PDHMMTestData copyWithMaxHapLength(int maxHapLength) {
            PDHMMTestData td = new PDHMMTestData(this.batchSize, this.maxHapLength, this.maxReadLength);
            td.maxHapLength = maxHapLength;
            return td;
        }

        PDHMMTestData copyWithMaxReadLength(int maxReadLength) {
            PDHMMTestData td = new PDHMMTestData(this.batchSize, this.maxHapLength, this.maxReadLength);
            td.maxReadLength = maxReadLength;
            return td;
        }

        @Override
        public String toString() {
            return arrayNameWithSize(hapBases, "hap_bases") +
                    arrayNameWithSize(hapPdbases, "hap_pdbases") +
                    arrayNameWithSize(readBases, "read_bases") +
                    arrayNameWithSize(readQual, "read_qual") +
                    arrayNameWithSize(readInsQual, "read_ins_qual") +
                    arrayNameWithSize(readDelQual, "read_del_qual") +
                    arrayNameWithSize(gcp, "gcp") +
                    arrayNameWithSize(hapLengths, "hap_lengths") +
                    arrayNameWithSize(readLengths, "read_lengths") +
                    "batchSize=" + batchSize +
                    ", maxHapLength=" + maxHapLength +
                    ", maxReadLength=" + maxReadLength
                    ;
        }

        private String arrayNameWithSize(Object o, String name) {
            return (o == null ? "null" : String.format("%s[%d]", name, Array.getLength(o))) + ", ";
        }
    }

    static final String pdhmmData = IntelGKLUtils.pathToTestResource("expected.PDHMM.hmmresults.txt");
    static final double DOUBLE_ASSERTION_DELTA = 0.0001;
    public static final int READ_MAX_LENGTH = 200;
    public static final int HAPLOTYPE_MAX_LENGTH = 500;
    private IntelPDHMM intelPDHMM;

    @DataProvider
    public Object[][] dataWithNullArguments() {
        PDHMMTestData td = new PDHMMTestData(1, HAPLOTYPE_MAX_LENGTH, READ_MAX_LENGTH);

        return new Object[][]{
                {td.copyWithHapBases(null)},
                {td.copyWithHapPdbases(null)},
                {td.copyWithReadBases(null)},
                {td.copyWithReadQual(null)},
                {td.copyWithReadInsQual(null)},
                {td.copyWithReadDelQual(null)},
                {td.copyWithGcp(null)},
                {td.copyWithReadLengths(null)},
                {td.copyWithHapLengths(null)}
        };
    }

    @DataProvider
    public Object[][] dataWithWrongSizeArrays() {
        PDHMMTestData td = new PDHMMTestData(1, HAPLOTYPE_MAX_LENGTH, READ_MAX_LENGTH);

        byte[] wrongSizeHapArray = new byte[td.hapArraySize + 1];
        byte[] wrongSizeReadArray = new byte[td.readArraySize + 1];
        long[] wrongSizeLengthsArray = new long[td.batchSize + 1];

        return new Object[][]{
                {td.copyWithHapBases(wrongSizeHapArray)},
                {td.copyWithHapPdbases(wrongSizeHapArray)},
                {td.copyWithReadBases(wrongSizeReadArray)},
                {td.copyWithReadQual(wrongSizeReadArray)},
                {td.copyWithReadInsQual(wrongSizeReadArray)},
                {td.copyWithReadDelQual(wrongSizeReadArray)},
                {td.copyWithGcp(wrongSizeReadArray)},
                {td.copyWithReadLengths(wrongSizeLengthsArray)},
                {td.copyWithHapLengths(wrongSizeLengthsArray)}
        };
    }

    @DataProvider
    public Object[][] dataWithWrongBatchSizeAndLengths() {
        PDHMMTestData td = new PDHMMTestData(1, HAPLOTYPE_MAX_LENGTH, READ_MAX_LENGTH);

        return new Object[][]{
                {td.copyWithBatchSize(0)},
                {td.copyWithBatchSize(-1)},
                {td.copyWithMaxHapLength(0)},
                {td.copyWithMaxHapLength(-1)},
                {td.copyWithMaxReadLength(0)},
                {td.copyWithMaxReadLength(-1)}
        };
    }

    @BeforeMethod
    public void initializePDHMM() {
        final boolean isLoaded = new IntelPDHMM().load(null);
        Assert.assertTrue(isLoaded);
        intelPDHMM = new IntelPDHMM();
    }

//    @AfterMethod
//    public void closePDHMM() {
//        intelPDHMM.done();
//    }

    @Test(dataProvider = "dataWithNullArguments", expectedExceptions = {NullPointerException.class})
    public void testComputePDHMM_ThrowsNullPointerException_WhenArgumentIsNull(PDHMMTestData td) {
        intelPDHMM.computePDHMM(
                td.hapBases,
                td.hapPdbases,
                td.readBases,
                td.readQual,
                td.readInsQual,
                td.readDelQual,
                td.gcp,
                td.hapLengths,
                td.readLengths,
                td.batchSize,
                td.maxHapLength,
                td.maxReadLength
        );
    }

    @Test(dataProvider = "dataWithWrongSizeArrays", expectedExceptions = {IllegalArgumentException.class})
    public void testComputePDHMM_ThrowsIllegalArgumentException_WhenArrayHasWrongSize(PDHMMTestData td) {
        intelPDHMM.computePDHMM(
                td.hapBases,
                td.hapPdbases,
                td.readBases,
                td.readQual,
                td.readInsQual,
                td.readDelQual,
                td.gcp,
                td.hapLengths,
                td.readLengths,
                td.batchSize,
                td.maxHapLength,
                td.maxReadLength
        );
    }

    @Test(dataProvider = "dataWithWrongBatchSizeAndLengths", expectedExceptions = {IllegalArgumentException.class})
    public void testComputePDHMM_ThrowsIllegalArgumentException_WhenBatchSizeOrLengthsHaveZeroOrNegativeValue(PDHMMTestData td) {
        intelPDHMM.computePDHMM(
                td.hapBases,
                td.hapPdbases,
                td.readBases,
                td.readQual,
                td.readInsQual,
                td.readDelQual,
                td.gcp,
                td.hapLengths,
                td.readLengths,
                td.batchSize,
                td.maxHapLength,
                td.maxReadLength
        );
    }

    @Test(enabled = true)
    public void pdhmmPerformanceTest() {
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
}
