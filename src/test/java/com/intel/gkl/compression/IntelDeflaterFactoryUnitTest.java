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
