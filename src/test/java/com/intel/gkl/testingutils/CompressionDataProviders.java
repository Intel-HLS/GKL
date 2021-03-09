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

package com.intel.gkl.testingutils;

public class CompressionDataProviders {
    @org.testng.annotations.DataProvider(name = "allCompressionCases")
    public static Object[][] generateAllCompressionCases(){
        int levels = 10;
        Object[][] params = new Object[levels*2][];
        for (int i = 0; i < levels; i++) {
            params[i] = new Object[]{i, true};
            params[i+levels] = new Object[]{i, false};
        }
        return params;
    }

    @org.testng.annotations.DataProvider(name = "gzipOnlyCompressionCases")
    public static Object[][] generateGzipOnlyCompressionCases(){
        int levels = 10;
        Object[][] params = new Object[levels][];
        for (int i = 0; i < levels; i++) {
            params[i] = new Object[]{i, true};
        }
        return params;
    }
    @org.testng.annotations.DataProvider(name = "compressionLevels")
    public static Object[][] generateCompressionLevels(){
        int levels = 10;
        Object[][] params = new Object[levels][];
        for (int i = 0; i < levels; i++) {
            params[i] = new Object[]{i};
        }
        return params;
    }

    @org.testng.annotations.DataProvider(name = "compressionLevelsOneAndTwo")
    public static Object[][] compressionLevelsOnAndTwo(){
        return new Object[][]{{1}, {2}};
    }

    @org.testng.annotations.DataProvider(name = "compressionGzipOptions")
    public static Object[][] compressionGzipOptions(){
        return new Object[][]{{true}, {false}};
    }
}
