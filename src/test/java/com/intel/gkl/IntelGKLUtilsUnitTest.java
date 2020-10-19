package com.intel.gkl;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.testng.Assert;
import org.testng.annotations.Test;
import com.intel.gkl.pairhmm.IntelPairHmmOMP;

public class IntelGKLUtilsUnitTest {
    private final static Logger log = LogManager.getLogger(IntelGKLUtilsUnitTest.class);

    // disable this test because it fails with jvmArg '-XX:+RestoreMXCSROnJNICalls'
    @Test(enabled = false)
    public void simpleTest() {

        IntelGKLUtils utils = new IntelGKLUtils();

        boolean isLoaded = utils.load(null);
        assert(isLoaded);

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
            assert(isLoaded);

            ftzValue = utils.getFlushToZero();
            log.info("Child thread FTZ = " + ftzValue);
        }
    }

    // disable this test because it fails with jvmArg '-XX:+RestoreMXCSROnJNICalls'
    @Test(enabled = false)
    public void childThreadTest() {

        log.info("Parent setting FTZ = true");

        IntelGKLUtils utils = new IntelGKLUtils();

        boolean isLoaded = utils.load(null);
        assert(isLoaded);

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
    public void testInvalidInputsForPathToTestResource() {

        IntelGKLUtils utils = new IntelGKLUtils();

        try {
            IntelGKLUtils.pathToTestResource(null);
            Assert.fail("NullPointerException expected.");
        }
        catch(NullPointerException e) {}

        try {
            IntelGKLUtils.pathToTestResource("");
            Assert.fail("IllegalArgumentException expected.");
        }
        catch(IllegalArgumentException e) {}

        try {
            IntelGKLUtils.pathToTestResource("fi|e$.in");
            Assert.fail("IllegalArgumentException expected.");
        }
        catch(IllegalArgumentException e) {}

    }

    @Test(enabled = true)
    public void platformFeaturesTest() {

        log.info("Check Platform Features");

        IntelGKLUtils utils = new IntelGKLUtils();

        boolean isLoaded = utils.load(null);
        assert(isLoaded);

        boolean isAVX = utils.isAvxSupported();
        if(isAVX) log.info("AVX is supported");
        else log.info("AVX is not supported");

    }

    @Test(enabled = true)
    public void availableOmpThreadsTest() {

        IntelGKLUtils utils = new IntelGKLUtils();

        boolean isLoaded = utils.load(null);
        assert(isLoaded);

        IntelPairHmmOMP phmm = new IntelPairHmmOMP();
        if(phmm.load(null)) {
            log.info("Test getting OpenMP threads when OMP available");
            int value = utils.getAvailableOmpThreads();
            assert(value > 0);
        }
        else {
            log.info("Test getting OpenMP threads when OMP unavailable");
            int value = utils.getAvailableOmpThreads();
            assert(value == 0);
        }
    }
}
