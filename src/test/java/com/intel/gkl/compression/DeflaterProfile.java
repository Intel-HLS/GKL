/* The MIT License
 *
 * Copyright (c) 2021 Intel Corporation
 *
 * 	Permission is hereby granted, free of charge, to any person
 * 	obtaining a copy of this software and associated documentation
 * 	files (the "Software"), to deal in the Software without
 * 	restriction, including without limitation the rights to use,
 * 	copy, modify, merge, publish, distribute, sublicense, and/or
 * 	sell copies of the Software, and to permit persons to whom the
 * 	Software is furnished to do so, subject to the following
 * 	conditions:
 *
 * 	The above copyright notice and this permission notice shall be
 * 	included in all copies or substantial portions of the
 * 	Software.
 *
 * 	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
 * 	KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * 	WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * 	PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * 	COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * 	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * 	OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * 	SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

package com.intel.gkl.compression;

import com.intel.gkl.IntelGKLUtils;
import htsjdk.samtools.*;
import htsjdk.samtools.util.zip.DeflaterFactory;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
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

    private final static Logger log = LogManager.getLogger(DeflaterIntegrationTest.class);
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
