package com.intel.gkl.compression;

import org.testng.Assert;
import org.testng.annotations.Test;

import java.util.zip.Inflater;

public class IntelInflaterFactoryUnitTest {
    @Test
    public void IntelInflaterFactoryReturnsIntelInflaterIfGzipIsSetTest(){
        IntelInflaterFactory intelInflaterFactory = new IntelInflaterFactory();

        Inflater intelInflater = intelInflaterFactory.makeInflater( true);

        Assert.assertTrue(intelInflater instanceof IntelInflater);
    }

    @Test
    public void IntelInflaterFactoryReturnsJavaInflaterIfGzipIsNotSetTest(){
        IntelInflaterFactory intelInflaterFactory = new IntelInflaterFactory();

        Inflater intelInflater = intelInflaterFactory.makeInflater( false);

        Assert.assertFalse(intelInflater instanceof IntelInflater);
    }
}
