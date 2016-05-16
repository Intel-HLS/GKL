package com.intel.gkl.deflater;

import org.testng.Assert;
import org.testng.annotations.*;

/**
 * Created by gspowley on 5/14/16.
 */
public class DeflaterUnitTest {

    @Test()
    public void failingTest() {
        IntelDeflater deflater = new IntelDeflater();
        deflater.finished();
        Assert.assertEquals(2, 2);
    }

    @Test()
    public void successfulTest() {
        Assert.assertTrue(2 == 2);
    }
}
