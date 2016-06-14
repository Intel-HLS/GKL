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
    private static boolean isLoaded = false;

    public static synchronized boolean load(File tempDir) {
        if (isLoaded) { return true; }

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

        try {
            // try to extract from classpath
            String resourcePath = "native/" +  System.mapLibraryName(GKL_LIB_NAME);
            URL inputUrl = IntelGKLUtils.class.getResource(resourcePath);
            if (inputUrl == null) {
                logger.warn("Unable to find Intel GKL library: " + resourcePath);
                return false;
            }

            logger.info(String.format("Trying to load Intel GKL library from:\n\t%s", inputUrl.toString()));

            File temp = File.createTempFile(FilenameUtils.getBaseName(resourcePath),
                    "." + FilenameUtils.getExtension(resourcePath), tempDir);
            FileUtils.copyURLToFile(inputUrl, temp);
            temp.deleteOnExit();
            logger.debug(String.format("Extracted Intel GKL to %s\n", temp.getAbsolutePath()));

            System.load(temp.getAbsolutePath());
            logger.info("Intel GKL library loaded from classpath.");
        } catch (IOException ioe) {
            // not supported
            logger.warn("Unable to load Intel GKL library.");
            return false;
        }

        isLoaded = true;
        return true;
    }

    static final String TEST_RESOURCES_PATH = System.getProperty("user.dir") + "/src/test/resources/";
    static final String TEST_RESOURCES_ABSPATH = new File(TEST_RESOURCES_PATH).getAbsolutePath() + "/";

    public static String pathToTestResource(String filename) {
        return TEST_RESOURCES_ABSPATH + filename;
    }
}
