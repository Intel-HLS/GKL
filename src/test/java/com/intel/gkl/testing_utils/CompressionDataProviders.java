package com.intel.gkl.testing_utils;

public class CompressionDataProviders {
    @org.testng.annotations.DataProvider(name = "allCompressionCases")
    public static Object[][] generateAllCompressionCases(){
        int levels = 10;
        Object[][] params = new Object[levels*2][];
        for (int i = 0; i < levels; i++) {
            params[i] = new Object[]{i, true};
            params[i+levels] = new Object[]{i, false};
        }
        return params;
    }

    @org.testng.annotations.DataProvider(name = "gzipOnlyCompressionCases")
    public static Object[][] generateGzipOnlyCompressionCases(){
        int levels = 10;
        Object[][] params = new Object[levels][];
        for (int i = 0; i < levels; i++) {
            params[i] = new Object[]{i, true};
        }
        return params;
    }
    @org.testng.annotations.DataProvider(name = "compressionLevels")
    public static Object[][] generateCompressionLevels(){
        int levels = 10;
        Object[][] params = new Object[levels][];
        for (int i = 0; i < levels; i++) {
            params[i] = new Object[]{i};
        }
        return params;
    }
}
