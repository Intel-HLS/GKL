package com.intel.gkl.ftz;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.testng.annotations.Test;

public class FtzUnitTest {
    private final static Logger log = LogManager.getLogger(FtzUnitTest.class);
/*
    @Test(enabled = false)
    public void simpleTest() {
        boolean ftzIsLoaded = new FTZ().load(null);
        assert(ftzIsLoaded);

        FTZ ftz = new FTZ();

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
            ftz.load(null);
            assert(ftz.isLoaded());

            ftzValue = ftz.getFlushToZero();
            log.info("Child thread FTZ = " + ftzValue);
        }
    }

    @Test(enabled = true)
    public void childThreadTest() {
        FTZ ftz = new FTZ();
        ftz.load(null);
        assert (ftz.isLoaded());

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
*/
}