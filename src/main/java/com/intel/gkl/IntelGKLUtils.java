/*
 * The MIT License
 *
 * Copyright (c) 2016 Intel Corporation
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

package com.intel.gkl;

import org.apache.commons.io.FileUtils;
import org.apache.commons.io.FilenameUtils;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import java.io.File;
import java.io.IOException;
import java.net.URL;

/**
 *This class provides support for Utilities for compression libraries support.
 *The shared library libIntelGKL is packaged as a jar files and its path is retireved
 *if the  GKL_USE_LIB_PATH variable is set. This utility class implements the load function
 *to load the library path for the packaged jar file.
 */


public class IntelGKLUtils {
    private final static Logger logger = LogManager.getLogger(IntelGKLUtils.class);
    private final static String GKL_USE_LIB_PATH = "GKL_USE_LIB_PATH";
    private final static String GKL_LIB_NAME = "IntelGKL";
    private final static String GKL_OMP_LIB_NAME = "IntelGKL_omp";
    private final static Boolean runningOnMac =
            System.getProperty("os.name", "unknown").toLowerCase().startsWith("mac");

    private static boolean isLoaded = false;


    /**
     * Check if AVX is supported and enabled on the CPU
     *
     * @return {@code true} if AVX is supported and enabled on the CPU, {@code false} otherwise
     */
    private static Boolean isAvxSupported() {
        // use a grep command to check for AVX support
        // grep exit code = 0 if a match was found
        final String command = runningOnMac ? "sysctl -a | grep machdep.cpu.features | grep -i avx" :
                "grep -i avx /proc/cpuinfo";
        try {
            Process process = new ProcessBuilder("/bin/sh", "-c", command).start();
            if (process.waitFor() != 0) {
                logger.warn("Error starting process to check for AVX support : " + command);
                return false;
            }
            if (process.exitValue() != 0) {
                logger.info("AVX is not supported on this system : " + command);
                return false;
            }
        }
        catch (InterruptedException | IOException e) {
            logger.warn("Error running command to check for AVX support : " + command);
            return false;
        }
        return true;
    }

    private static void tryLoad(String resourcePath, File tempDir) throws UnsatisfiedLinkError, IOException {
        URL inputUrl = IntelGKLUtils.class.getResource(resourcePath);
        if (inputUrl == null) {
            logger.warn("Unable to find Intel GKL library: " + resourcePath);
            throw new IOException();
        }

        logger.info(String.format("Trying to load Intel GKL library from:\n\t%s", inputUrl.toString()));

        File temp = File.createTempFile(FilenameUtils.getBaseName(resourcePath),
                "." + FilenameUtils.getExtension(resourcePath), tempDir);
        FileUtils.copyURLToFile(inputUrl, temp);
        temp.deleteOnExit();
        logger.debug(String.format("Extracted Intel GKL to %s\n", temp.getAbsolutePath()));

        System.load(temp.getAbsolutePath());
        logger.info("Intel GKL library loaded from classpath.");
    }

    /**
     * Tries to load the GKL shared library. If AVX is not supported, it returns {@code false} without
     * trying to load the library.
     *
     * If GKL is loaded from a jar file, the shared library file is extracted to the
     * {@code tempDir} directory first.
     *
     * @param tempDir directory where the shared library file is extracted
     * @return {@code true} if GKL loaded successfully, {@code false} otherwise
     */
    public static synchronized boolean load(File tempDir) {
        if (isLoaded) { return true; }

        if (!isAvxSupported()) { return false; }

        // try to load from Java library path if GKL_USE_LIB_PATH env var is defined
        if (System.getenv(GKL_USE_LIB_PATH) != null) {
            try {
                String javaLibraryPath = System.getProperty("java.library.path");
                logger.info(String.format("Trying to load Intel GKL library from: \n\t%s",
                        javaLibraryPath.replaceAll(":", "\n\t")));
                System.loadLibrary(GKL_LIB_NAME);
                logger.info("Intel GKL library loaded from Java library path.");
                isLoaded = true;
                return true;
            } catch(UnsatisfiedLinkError ule) {
                // this is not fatal, continue and try to load from classpath
            }
        }

        // try to load OpenMP version of GKL
        try {
            tryLoad("native/" + System.mapLibraryName(GKL_OMP_LIB_NAME), tempDir);
            isLoaded = true;
            return true;
        } catch (UnsatisfiedLinkError ule) {
            logger.warn("Unable to load OpenMP Intel GKL library. Ensure OpenMP is installed on this platform.");
        } catch (IOException ioe) {
            logger.warn("Unable to load OpenMP Intel GKL library. OpenMP is not supported on this platform.");
        }

        // try to load non-OpenMP version of GKL
        try {
            // try to extract from classpath
            tryLoad("native/" + System.mapLibraryName(GKL_LIB_NAME), tempDir);
            isLoaded = true;
            return true;
        } catch (UnsatisfiedLinkError ule) {
            logger.warn("Unable to load Intel GKL library.");
            return false;
        } catch (IOException ioe) {
            logger.warn("Unable to load Intel GKL library.");
            return false;
        }
    }

    static final String TEST_RESOURCES_PATH = System.getProperty("user.dir") + "/src/test/resources/";
    static final String TEST_RESOURCES_ABSPATH = new File(TEST_RESOURCES_PATH).getAbsolutePath() + "/";

    public static String pathToTestResource(String filename) {
        return TEST_RESOURCES_ABSPATH + filename;
    }
}
