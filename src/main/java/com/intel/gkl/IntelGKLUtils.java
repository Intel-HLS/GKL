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

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import java.io.File;
import java.io.IOException;

/**
 * Provides utilities used by the GKL library.
 */
public final class IntelGKLUtils {
    private final static Logger logger = LogManager.getLogger(IntelGKLUtils.class);

    /**
     * Check if AVX is supported on the platform.
     *
     * @return  true if AVX is supported and enabled on the CPU, false otherwise
     */
    public static boolean isAvxSupported() {
        final boolean runningOnMac = System.getProperty("os.name", "unknown").toLowerCase().startsWith("mac");
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

    private static final String TEST_RESOURCES_PATH = System.getProperty("user.dir") + "/src/test/resources/";
    private static final String TEST_RESOURCES_ABSPATH = new File(TEST_RESOURCES_PATH).getAbsolutePath() + "/";

    public static String pathToTestResource(String filename) {
        return TEST_RESOURCES_ABSPATH + filename;
    }
}
