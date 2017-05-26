package com.intel.gkl;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.testng.Assert;
import org.testng.annotations.Test;

public class IntelGKLUtilsUnitTest {
    private final static Logger log = LogManager.getLogger(IntelGKLUtilsUnitTest.class);

    @Test(enabled = true)
    public void simpleTest() {

        IntelGKLUtils utils = new IntelGKLUtils();

        boolean isLoaded = utils.load(null);

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
            boolean isLoaded = utils.load(null);

            ftzValue = utils.getFlushToZero();
            log.info("Child thread FTZ = " + ftzValue);
        }
    }

    @Test(enabled = true)
    public void childThreadTest() {

        log.info("Parent setting FTZ = true");

        IntelGKLUtils utils = new IntelGKLUtils();

        boolean isLoaded = utils.load(null);

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

        log.info("Check Platform Features");

        IntelGKLUtils utils = new IntelGKLUtils();

        boolean isLoaded = utils.load(null);


        boolean isAVX = utils.isAvxSupported();
        if(isAVX) log.info("AVX is supported");
        else log.info("AVX is not supported");

    }


}