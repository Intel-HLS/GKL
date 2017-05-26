package com.intel.gkl;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.testng.Assert;
import org.testng.annotations.Test;

public class IntelGKLUtilsUnitTest {
    private final static Logger log = LogManager.getLogger(IntelGKLUtilsUnitTest.class);

    @Test(enabled = false)
    public void simpleTest() {
        boolean ftzIsLoaded = new IntelGKLUtils().load(null);
        assert(ftzIsLoaded);

        IntelGKLUtils utils = new IntelGKLUtils();

        log.info("Test setting FTZ");
        boolean value = false;
        utils.setFlushToZero(true);
        value = utils.getFlushToZero();
        assert(value == true);

        log.info("Test clearing FTZ");
        value = true;
        utils.setFlushToZero(false);
        value = utils.getFlushToZero();
        assert(value == false);
    }

    class ChildThread extends Thread {
        boolean ftzValue;

        public void run() {
            IntelGKLUtils utils = new IntelGKLUtils();
            boolean ftzIsLoaded = utils.load(null);
            assert(ftzIsLoaded);

            ftzValue = utils.getFlushToZero();
            log.info("Child thread FTZ = " + ftzValue);
        }
    }

    @Test(enabled = true)
    public void childThreadTest() {
        IntelGKLUtils utils = new IntelGKLUtils();
        boolean ftzIsLoaded = utils.load(null);
        assert (ftzIsLoaded);

        log.info("Parent setting FTZ = true");

        boolean value = false;
        utils.setFlushToZero(true);
        value = utils.getFlushToZero();
        assert(value == true);

        ChildThread childThread = new ChildThread();

        childThread.start();
        try {
            childThread.join();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        assert(childThread.ftzValue == false);
    }

    @Test(enabled = true)
    public void platformFeaturesTest() {
        IntelGKLUtils utils = new IntelGKLUtils();
        boolean ftzIsLoaded = utils.load(null);
        assert (ftzIsLoaded);

        log.info("Check Platform Features");


        boolean isAVX = IntelGKLUtils.isAvxSupported();
        if(isAVX) log.info("AVX is supported");
        else log.info("AVX is not supported");

    }


}