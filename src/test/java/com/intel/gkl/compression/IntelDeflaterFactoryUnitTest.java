package com.intel.gkl.compression;

import com.intel.gkl.testingutils.CompressionDataProviders;
import org.testng.Assert;
import org.testng.annotations.Test;

import java.util.zip.Deflater;

public class IntelDeflaterFactoryUnitTest {
    @Test(enabled = true, dataProviderClass = CompressionDataProviders.class, dataProvider ="compressionLevels")
    public void IntelDeflaterFactoryReturnsIntelDeflaterIfGzipIsSetTest(int level){
        IntelDeflaterFactory intelDeflaterFactory = new IntelDeflaterFactory();

        Deflater intelDeflater = intelDeflaterFactory.makeDeflater(level, true);

        Assert.assertTrue(intelDeflater instanceof IntelDeflater);
    }

    @Test(enabled = true, dataProviderClass = CompressionDataProviders.class, dataProvider ="allCompressionCases")
    public void IntelDeflaterFactoryReturnsIntelDeflaterIfCompressionLevelIsNotOneTest(int level, boolean nowrap){
        if(level==1) return;

        IntelDeflaterFactory intelDeflaterFactory = new IntelDeflaterFactory();

        Deflater intelDeflater = intelDeflaterFactory.makeDeflater(level, nowrap);

        Assert.assertTrue(intelDeflater instanceof IntelDeflater);
    }

    @Test(enabled = true)
    public void IntelDeflaterFactoryReturnsJavaDeflaterIfCompressionLevelIsOneAndGzipIsNotSetTest(){
        IntelDeflaterFactory intelDeflaterFactory = new IntelDeflaterFactory();

        Deflater intelDeflater = intelDeflaterFactory.makeDeflater(1, false);

        Assert.assertFalse(intelDeflater instanceof IntelDeflater);
    }

}
