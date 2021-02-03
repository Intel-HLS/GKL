package com.intel.gkl.compression;

import com.intel.gkl.testing_utils.TestingUtils;
import org.testng.annotations.BeforeClass;

public class CompressionUnitTestBase {
    protected int compatibilityTestsBufferSize;
    protected byte[] compatibilityInputBuffer;
    protected static final String compatibilityTestsDataProviderName = "gzipOnlyCompressionCases";
    private final boolean simplifyDNAForCompatibilityTests = false;  // if set dna array will be fully filled with 'A'
                                                                    // otherwise random array of set {A,C,G,T} will be generated

    @BeforeClass()
    public void generateTempFile() {
        compatibilityTestsBufferSize = System.getProperty("long_tests") != null
                ? TestingUtils.MAX_ARRAY_SIZE
                : TestingUtils.SIZE_4MB;

        compatibilityInputBuffer = simplifyDNAForCompatibilityTests
                ? TestingUtils.generateSimpleDNAArray(compatibilityTestsBufferSize)
                : TestingUtils.generateRandomDNAArray(compatibilityTestsBufferSize);
    }
}
