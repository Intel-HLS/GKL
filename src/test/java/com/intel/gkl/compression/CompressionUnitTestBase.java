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

import com.intel.gkl.testingutils.TestingUtils;
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
