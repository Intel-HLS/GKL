package com.intel.gkl.compression;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.zip.DataFormatException;

import org.apache.commons.io.FileUtils;
import org.testng.Assert;
import org.testng.annotations.DataProvider;
import org.testng.annotations.Test;

import htsjdk.samtools.util.BlockCompressedInputStream;
import htsjdk.samtools.util.BlockCompressedOutputStream;

public class LongReadsDeflateInflateTest {
    /**
     * The choice of buffer size is arbitrary but *may* influence performance.
     * Most sources recommend using an 8k buffer when working with streams but
     * the block compressed streams use 64k buffers internally so it's not as
     * straight forward.
     *
     * IMPORTANT: I realized this too late into running the tests but 64M is
     * quite extreme in that it guarantees the buffer will never fit into CPU
     * cache. Before running this test again I would recommend testing on
     * smaller data with a buffer size of 64k or a multiple of that value.
     */
    final int BUFFER_SIZE = 64 * 1024 * 1024;

    /**
     * Disabled by default as this test is very time consuming.
     *
     * This test verifies correct integration of compression classes with samtools streams used by the GATK project.
     * It will compress the file specified by {@code path} using {@code BlockCompressedOutputStream} storing the result
     * in a temporary file in the same location. It will then decompress the file using {@code BlockCompressedInputStream}
     * and once more write the result to another temporary file. The resulting file is then compared against the original
     * input file to verify no data corruption occurred.
     *
     * This test will also report basic statistics - compressed and uncompressed sizes and the duration of compression
     * and decompression.
     *
     * To run this test set "enabled" to true execute:
     * <repository root>/gradlew test --tests LongReadsDeflateInflateTest -D longreads-directory=<data directory>
     *
     * For example:
     * ./gradlew test --tests LongReadsDeflateInflateTest -D longreads-directory=/tmp/genomics_temp/longreads
     */
    @Test(enabled = true, dataProvider = "inflateDeflateData")
    public void testInflateDeflate(String path, Integer level) throws IOException, DataFormatException {
        File rawFile = new File(path);
        FileInputStream rawInputStream = new FileInputStream(path);
        File directory = rawFile.getParentFile();

        // Files are created ahead of time so that we can delete them in the finally clause if anything goes wrong.
        // This is done consciously instead of calling deleteOnExit() as that only causes files to be deleted during JVM shutdown.
        // Given that we're working with large files it's more desirable to clean up as soon as we don't need them.
        File compressedFile = File.createTempFile("longreads-compressed", ".tmp", directory);
        File decompressedFile = File.createTempFile("longreads-decompressed", ".tmp", directory);

        try {
            // Compress
            BlockCompressedOutputStream compressedOutputStream = new BlockCompressedOutputStream(compressedFile, level, new IntelDeflaterFactory());

            System.out.println("Starting compression");
            long compressionStart = System.currentTimeMillis();
            copyStream(rawInputStream, compressedOutputStream);
            long compressionDuration = System.currentTimeMillis() - compressionStart;
            System.out.println(String.format("Compression finished in: %.3fs", (double) compressionDuration / 1000));
            System.out.println(String.format("Raw file size: %d", rawFile.length()));
            System.out.println(String.format("Compressed file size: %d bytes", compressedFile.length()));
            System.out.println(String.format("Compressed/raw ratio: %.2f", (double) compressedFile.length() / rawFile.length()));

            rawInputStream.close();
            compressedOutputStream.close();

            // Decompress
            FileOutputStream decompressedOutputStream = new FileOutputStream(decompressedFile);
            BlockCompressedInputStream compressedInputStream = new BlockCompressedInputStream(compressedFile, new IntelInflaterFactory());

            System.out.println("Starting decompression");
            long decompressionStart = System.currentTimeMillis();
            copyStream(compressedInputStream, decompressedOutputStream);
            long decompressionDuration = System.currentTimeMillis() - decompressionStart;
            System.out.println(String.format("Decompression finished in: %.3fs", (double) decompressionDuration / 1000));

            compressedInputStream.close();
            decompressedOutputStream.close();

            // Compare
            Assert.assertTrue(FileUtils.contentEquals(rawFile, decompressedFile));
        } finally {
            compressedFile.delete();
            decompressedFile.delete();
        }
    }

    /**
     * This test was designed to be ran with big input files which are not stored in this repository. The data provider
     * will generate parameters for each possible compression level (0-9, inclusive) for each file which name ends with
     * ".raw" found in the directory pointed to by the "longreads-directory" system property.
     */
    @DataProvider(name = "inflateDeflateData")
    public static Iterator<Object[]> inflateDeflateData() {
        List<Object[]> parameters = new ArrayList<Object[]>();


        String directoryPath = System.getProperty("longreads-directory");
        if (directoryPath == null) {
            throw new RuntimeException("longreads-directory isn't set");
        }

        File testFilesDirectory = new File(directoryPath);
        if (!testFilesDirectory.isDirectory()) {
            throw new RuntimeException(String.format("%s doesn't exist or isn't a directory", testFilesDirectory.getAbsolutePath()));
        }

        File[] testFiles = testFilesDirectory.listFiles(file -> file.isFile() && file.getName().endsWith(".raw"));

        for (int level = 0; level <= 9; level++) {
            for (File file : testFiles) {
                parameters.add(new Object[] { file.getAbsolutePath(), level });
            }
        }

        return parameters.iterator();
    }

    /**
     * Copies all data available in the {@code input} stream into the {@code output} stream.
     * Copying will stop once no more data is available in the {@code input}.
     * May throw an IOException if any of the underlying stream methods fails.
     *
     * @param input  the stream to consume data from
     * @param output the stream into which data will be written
     * @throws IOException
     */
    private void copyStream(InputStream input, OutputStream output) throws IOException {
        byte[] buffer = new byte[BUFFER_SIZE];
        while (true) {
            int readLength = input.read(buffer);
            if (readLength == -1) {
                break;
            }
            output.write(buffer, 0, readLength);
        }
    }
}
