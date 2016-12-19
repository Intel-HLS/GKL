package com.intel.gkl;

import org.apache.commons.io.FileUtils;
import org.apache.commons.io.FilenameUtils;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import java.io.File;
import java.net.URL;
import java.util.HashSet;
import java.util.Set;

/**
 *
 */
public final class NativeLibraryLoader {
    private static final Logger logger = LogManager.getLogger(NativeLibraryLoader.class);
    private static final String USE_LIB_PATH = "USE_LIB_PATH";
    private static final Set<String> loadedLibraries = new HashSet<String>();

    private final String libraryName;

    public NativeLibraryLoader(String libraryName) {
        this.libraryName = libraryName;
    }

    public boolean isLoaded() {
        return loadedLibraries.contains(libraryName);
    }

    /**
     * Tries to load the GKL shared library. If AVX is not supported, it returns {@code false} without
     * trying to load the library.
     *
     * If GKL is loaded from a jar file, the shared library file is extracted to the
     * {@code tempDir} directory first.
     *
     * @param tempDir directory where the shared library file is extracted
     * @param libFileName name of the library file to be loaded
     * @return {@code true} if GKL loaded successfully, {@code false} otherwise
     */
    /**
     *
     * @param tempDir
     * @return {@code true} if GKL loaded successfully, {@code false} otherwise
     */
    public synchronized boolean load(File tempDir) {
        if (isLoaded()) {
            return true;
        }

        // try to load from Java library path if USE_LIB_PATH env var is defined
        if (System.getenv(USE_LIB_PATH) != null) {
            try {
                String javaLibraryPath = System.getProperty("java.library.path");
                logger.info(String.format("Trying to load native library from: \n\t%s",
                        javaLibraryPath.replaceAll(":", "\n\t")));
                System.loadLibrary(libraryName);
                logger.info("Native library loaded from Java library path.");

                return true;
            } catch (UnsatisfiedLinkError ule) {
                logger.warn("Unable to load library.");
                return false;
            }
        }

        // try to extract from classpath
        try {
            String resourcePath = "native/" +  System.mapLibraryName(libraryName);
            URL inputUrl = NativeLibraryLoader.class.getResource(resourcePath);
            if (inputUrl == null) {
                logger.warn("Unable to find native library: " + resourcePath);
                return false;
            }

            logger.info(String.format("Trying to load native library from:\n\t%s", inputUrl.toString()));

            File temp = File.createTempFile(FilenameUtils.getBaseName(resourcePath),
                    "." + FilenameUtils.getExtension(resourcePath), tempDir);
            FileUtils.copyURLToFile(inputUrl, temp);
            temp.deleteOnExit();
            logger.debug(String.format("Extracted native to %s\n", temp.getAbsolutePath()));

            System.load(temp.getAbsolutePath());
            logger.info("Native library loaded from classpath.");
        } catch (Exception e) {
            logger.warn("Unable to load native library.");
            return false;
        }

        loadedLibraries.add(libraryName);
        return true;
    }
}
