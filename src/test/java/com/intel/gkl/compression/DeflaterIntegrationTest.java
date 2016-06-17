package com.intel.gkl.compression;

import htsjdk.samtools.*;
import htsjdk.samtools.util.zip.DeflaterFactory;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.testng.annotations.Test;

import java.io.File;
import java.io.IOException;
import java.net.InetAddress;
import java.util.List;
import java.util.stream.Collectors;
import java.util.zip.Deflater;

/**
 * Created by gspowley on 6/15/16.
 */
public class DeflaterIntegrationTest {

    private final static Logger log = LogManager.getLogger(DeflaterIntegrationTest.class);
    private final static String INPUT_FILE =
            "/home/gspowley/work/gatk4/release/gatk/src/test/resources/large/CEUTrio.HiSeq.WGS.b37.NA12878.20.21.bam";
    private final static String OUTPUT_FILE = "out.bam";

    @Test(enabled = true)
    public void integrationTest() throws IOException {
        final File inputFile = new File(INPUT_FILE);
        final boolean eagerDecode = true;
        final File outputFile = new File(OUTPUT_FILE);
        outputFile.deleteOnExit();

        SamReaderFactory readerFactory = SamReaderFactory.makeDefault().validationStringency(ValidationStringency.SILENT);
        if (eagerDecode) {
            readerFactory = readerFactory.enable(SamReaderFactory.Option.EAGERLY_DECODE);
        }

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

        final DeflaterFactory javaDeflaterFactory= new DeflaterFactory(){
            public Deflater makeDeflater(final int compressionLevel, final boolean nowrap) {
                return new Deflater(compressionLevel, nowrap);
            }
        };

        log.info("input filesize = " + inputFile.length());
        log.info("deflater level, time (sec), filesize");

        for (int compressionLevel = 11; compressionLevel < 10; compressionLevel++) {
            try (final SamReader reader = readerFactory.open(inputFile)) {
                final SAMFileHeader header = reader.getFileHeader();
                final SAMFileWriterFactory writerFactory = new SAMFileWriterFactory();
                writerFactory.setCompressionLevel(compressionLevel);
                writerFactory.setDeflaterFactory(javaDeflaterFactory);
                final SAMFileWriter writer = writerFactory.makeBAMWriter(header, true, outputFile);

                final long start = System.currentTimeMillis();
                for (final SAMRecord record : reader) {
                    writer.addAlignment(record);
                }

                final long end = System.currentTimeMillis();
                final long elapsed = end - start;
                log.info(String.format("PROFILE java %d %.3f %d", compressionLevel, elapsed / 1000.0, outputFile.length()));
            }
        }

        for (int compressionLevel = 0; compressionLevel < 10; compressionLevel++) {
            try (final SamReader reader = readerFactory.open(inputFile)) {
                final SAMFileHeader header = reader.getFileHeader();
                final SAMFileWriterFactory writerFactory = new SAMFileWriterFactory();
                writerFactory.setCompressionLevel(compressionLevel);
                writerFactory.setDeflaterFactory(intelDeflaterFactory);
                final SAMFileWriter writer = writerFactory.makeBAMWriter(header, true, outputFile);

                final long start = System.currentTimeMillis();
                for (final SAMRecord record : reader) {
                    writer.addAlignment(record);
                }

                final long end = System.currentTimeMillis();
                final long elapsed = end - start;
                log.info(String.format("PROFILE intel %d %.3f %d", compressionLevel, elapsed / 1000.0, outputFile.length()));
            }
        }
    }
}
