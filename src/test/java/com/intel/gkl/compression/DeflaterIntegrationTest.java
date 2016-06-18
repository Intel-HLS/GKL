package com.intel.gkl.compression;

import htsjdk.samtools.*;
import htsjdk.samtools.util.zip.DeflaterFactory;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.testng.annotations.Test;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.zip.Deflater;

/**
 * Integration and performance/compression profiling test for IntelDeflater
 */
public class DeflaterIntegrationTest {

    private final static Logger log = LogManager.getLogger(DeflaterIntegrationTest.class);
    private final static String INPUT_FILE =
            "/home/gspowley/work/gatk4/release/gatk/src/test/resources/large/CEUTrio.HiSeq.WGS.b37.NA12878.20.21.bam";

    @Test(enabled = true)
    public void integrationTest() throws IOException {
        final File inputFile = new File(INPUT_FILE);
        final File outputFile = File.createTempFile("output", ".bam");
        outputFile.deleteOnExit();

        SamReaderFactory readerFactory =
                SamReaderFactory.makeDefault().validationStringency(ValidationStringency.SILENT);
        readerFactory = readerFactory.enable(SamReaderFactory.Option.EAGERLY_DECODE);

        // deflater factory for Intel GKL compression
        final DeflaterFactory intelDeflaterFactory = new DeflaterFactory() {
            public Deflater makeDeflater(final int compressionLevel, final boolean nowrap) {
                boolean intelDeflaterSupported = new IntelDeflater().load();
                if (intelDeflaterSupported) {
                    if ((compressionLevel == 1 && nowrap) || compressionLevel != 1) {
                        return new IntelDeflater(compressionLevel, nowrap);
                    }
                }

                return new Deflater(compressionLevel, nowrap);
            }
        };

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

        log.info("input filesize = " + inputFile.length());
        log.info("deflater level, time (sec), filesize");

        int loopCount = 1;
        for (DeflaterFactory deflaterFactory : deflaterFactories) {
            for (int compressionLevel = 0; compressionLevel < 10; compressionLevel++) {
                for (int loop = 0; loop < loopCount; loop++) {

                    long totalRecords = 0;
                    try (final SamReader reader = readerFactory.open(inputFile)) {
                        final SAMFileHeader header = reader.getFileHeader();
                        final SAMFileWriterFactory writerFactory = new SAMFileWriterFactory();
                        writerFactory.setCompressionLevel(compressionLevel);
                        writerFactory.setDeflaterFactory(deflaterFactory);
                        final SAMFileWriter writer = writerFactory.makeBAMWriter(header, true, outputFile);

                        long totalTime = 0;
                        for (final SAMRecord record : reader) {
                            final long start = System.currentTimeMillis();
                            writer.addAlignment(record);
                            totalTime += System.currentTimeMillis() - start;
                            totalRecords++;
                        }

                        writer.close();

                        log.info(String.format("PROFILE %d %.3f %d",
                                compressionLevel, totalTime / 1000.0, outputFile.length()));
                    }

                    final SamReader expectedFile = readerFactory.open(inputFile);
                    final SamReader generatedFile = readerFactory.open(outputFile);

                    log.info("Checking generated output. Total records = " + totalRecords);

                    Iterator<SAMRecord> generatedIterator = generatedFile.iterator();
                    for (final SAMRecord expected : expectedFile) {
                        SAMRecord generated = generatedIterator.next();
                        assert(expected.toString().equals(generated.toString()));
                    }
                }
            }
        }
    }
}
