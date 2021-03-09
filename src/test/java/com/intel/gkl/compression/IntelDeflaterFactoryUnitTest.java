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

import com.intel.gkl.testingutils.CompressionDataProviders;
import org.testng.Assert;
import org.testng.SkipException;
import org.testng.annotations.Parameters;
import org.testng.annotations.Test;

import java.util.zip.Deflater;

public class IntelDeflaterFactoryUnitTest {
    @Test(enabled = true, dataProviderClass = CompressionDataProviders.class, dataProvider ="compressionLevels")
    public void IntelDeflaterFactoryReturnsIntelDeflaterIfGzipIsSetTest(int level){
        IntelDeflaterFactory intelDeflaterFactory = new IntelDeflaterFactory();

        Deflater intelDeflater = intelDeflaterFactory.makeDeflater(level, true);

        Assert.assertTrue(intelDeflater instanceof IntelDeflater);
    }

    @Test(enabled = true, dataProviderClass = CompressionDataProviders.class, dataProvider ="compressionLevels")
    public void IntelDeflaterFactoryReturnsIntelDeflaterIfCompressionLevelIsNotOneOrTwoTest(int level){
        if(level==1 || level==2) throw new SkipException("Not applicable");

        IntelDeflaterFactory intelDeflaterFactory = new IntelDeflaterFactory();

        Deflater intelDeflater = intelDeflaterFactory.makeDeflater(level, false);

        Assert.assertTrue(intelDeflater instanceof IntelDeflater);
    }

    @Test(enabled = true, dataProviderClass = CompressionDataProviders.class, dataProvider = "compressionLevelsOneAndTwo")
    public void IntelDeflaterFactoryReturnsJavaDeflaterIfCompressionLevelIsOneOrTwoAndGzipIsNotSetTest(int level){
        IntelDeflaterFactory intelDeflaterFactory = new IntelDeflaterFactory();

        Deflater javaDeflater = intelDeflaterFactory.makeDeflater(level, false);

        Assert.assertFalse(javaDeflater instanceof IntelDeflater);
    }

    @Test(enabled = true, dataProviderClass = CompressionDataProviders.class, dataProvider = "compressionGzipOptions")
    public void IntelDeflaterFactoryDoesNotRaiseExceptionIfCompressionLevelMinusOneTest(boolean nowrap){
        IntelDeflaterFactory intelDeflaterFactory = new IntelDeflaterFactory();

        intelDeflaterFactory.makeDeflater(-1, nowrap);

        Assert.assertTrue(true);
    }

    @Test(enabled = true, dataProviderClass = CompressionDataProviders.class, dataProvider = "compressionGzipOptions",
            expectedExceptions = IllegalArgumentException.class)
    public void IntelDeflaterFactoryRaisesIllegalArgumentExceptionIfCompressionLevelIsLessThanMinusOneTest(boolean nowrap){
        IntelDeflaterFactory intelDeflaterFactory = new IntelDeflaterFactory();

        intelDeflaterFactory.makeDeflater(-2, nowrap);

        Assert.fail();
    }

    @Test(enabled = true, dataProviderClass = CompressionDataProviders.class, dataProvider = "compressionGzipOptions",
            expectedExceptions = IllegalArgumentException.class)
    public void IntelDeflaterFactoryRaisesIllegalArgumentExceptionIfCompressionLevelIsGreaterThanNineTest(boolean nowrap){
        IntelDeflaterFactory intelDeflaterFactory = new IntelDeflaterFactory();

        intelDeflaterFactory.makeDeflater(10, nowrap);

        Assert.fail();
    }

}
