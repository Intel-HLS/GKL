package com.intel.gkl.compression;

import com.intel.gkl.IntelGKLUtils;
import htsjdk.samtools.*;
import htsjdk.samtools.util.zip.DeflaterFactory;
import org.apache.commons.logging.LogFactory;
import org.apache.commons.logging.Log;
import org.testng.annotations.Test;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.zip.Deflater;

/**
 * Integration and performance/compression profiling test for IntelDeflater
 */
public class DeflaterProfile {

    private final static Log log = LogFactory.getLog(DeflaterIntegrationTest.class);
    private final static String INPUT_FILE = IntelGKLUtils.pathToTestResource("HiSeq.1mb.1RG.2k_lines.bam");

    @Test(enabled = true)
    public void profileTest() throws IOException {
        final String filename = System.getProperty("input", INPUT_FILE);
        final File inputFile = new File(filename);
        final File outputFile = File.createTempFile("output", ".bam");
        outputFile.deleteOnExit();
        final File profileFile = File.createTempFile("profile", ".csv");
        profileFile.deleteOnExit();

        SamReaderFactory readerFactory =
                SamReaderFactory.makeDefault().validationStringency(ValidationStringency.SILENT);
        readerFactory = readerFactory.enable(SamReaderFactory.Option.EAGERLY_DECODE);

        // deflater factory for Intel GKL compression
        final DeflaterFactory intelDeflaterFactory = new IntelDeflaterFactory();

        // deflater factory for Java zip deflater
        final DeflaterFactory javaDeflaterFactory= new DeflaterFactory() {
            public Deflater makeDeflater(final int compressionLevel, final boolean nowrap) {
                return new Deflater(compressionLevel, nowrap);
            }
        };

        // create list of deflater factories
        List<DeflaterFactory> deflaterFactories = new ArrayList<DeflaterFactory>();
        deflaterFactories.add(intelDeflaterFactory);
        deflaterFactories.add(javaDeflaterFactory);

        // create profile log file
        final FileWriter fileWriter = new FileWriter(profileFile);

        fileWriter.write("level, time(sec), filesize\n");

        //int loopCount = Integer.parseInt(System.getProperty("loop", "10"));
        int loopCount = 1;
        for (DeflaterFactory deflaterFactory : deflaterFactories) {
            for (int compressionLevel = 1; compressionLevel < 2; compressionLevel++) {
                log.info(String.format("Level %d", compressionLevel));
                long totalTime = 0;
                for (int loop = 0; loop < loopCount; loop++) {

                    long totalRecords = 0;
                    try (final SamReader reader = readerFactory.open(inputFile)) {
                        final SAMFileHeader header = reader.getFileHeader();
                        final SAMFileWriterFactory writerFactory = new SAMFileWriterFactory();
                        writerFactory.setCompressionLevel(compressionLevel);
                        writerFactory.setDeflaterFactory(deflaterFactory);
                        final SAMFileWriter writer = writerFactory.makeBAMWriter(header, true, outputFile);

                        for (final SAMRecord record : reader) {
                            final long start = System.currentTimeMillis();
                            writer.addAlignment(record);
                            totalTime += System.currentTimeMillis() - start;
                            totalRecords++;
                        }

                        writer.close();
                    }
                }
                fileWriter.write(String.format("%d, %.3f, %d\n",
                        compressionLevel, totalTime / 1000.0 / loopCount, outputFile.length()));
                fileWriter.flush();
            }
        }
        fileWriter.close();
    }
}
