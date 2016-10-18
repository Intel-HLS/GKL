package com.intel.gkl.ftz;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.testng.annotations.Test;

public class FtzUnitTest {
    private final static Logger log = LogManager.getLogger(FtzUnitTest.class);

    @Test(enabled = true)
    public void simpleTest() {
        FTZ ftz = new FTZ();
        assert(ftz.isSupported());


        log.info("Test setting FTZ");
        boolean value = false;
        ftz.setFlushToZero(true);
        value = ftz.getFlushToZero();
        assert(value == true);

        log.info("Test clearing FTZ");
        value = true;
        ftz.setFlushToZero(false);
        value = ftz.getFlushToZero();
        assert(value == false);
    }

    class ChildThread extends Thread {
        boolean ftzValue;

        public void run() {
            FTZ ftz = new FTZ();
            assert(ftz.isSupported());

            ftzValue = ftz.getFlushToZero();
            log.info("Child thread FTZ = " + ftzValue);
        }
    }

    @Test(enabled = true)
    public void childThreadTest() {
        FTZ ftz = new FTZ();
        assert (ftz.isSupported());

        log.info("Parent setting FTZ = true");

        boolean value = false;
        ftz.setFlushToZero(true);
        value = ftz.getFlushToZero();
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
}