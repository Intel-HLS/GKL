package com.intel.gkl.testingutils;

import java.security.SecureRandom;
import java.util.Arrays;

public class TestingUtils {
    public static final int MAX_SW_SEQUENCE_LENGTH = 32*1024-1; //2^15 - 1
    public static final int MAX_SW_MATCH_VALUE = 64*1024; // 2^16

    public static final int SIZE_4MB = 4 *1024 * 1024;
    public static final int MAX_ARRAY_SIZE = Integer.MAX_VALUE -2;

    static SecureRandom rng = new SecureRandom();

    public static void randomDNA(byte[] array) {
        final byte[] DNA_CHARS = {'A', 'C', 'G', 'T'};

        for (int i = 0; i < array.length; i++) {
            array[i] = DNA_CHARS[rng.nextInt(DNA_CHARS.length)];
        }
    }

    public static byte[] generateRandomDNAArray(int size){
        byte[] dna = new byte[size];
        randomDNA(dna);
        return dna;
    }

    public static byte[] generateSimpleDNAArray(int size) {
        byte[] dna = new byte[size];
        Arrays.fill(dna, (byte) 'A');
        return dna;
    }

    public static int getMaxInputSizeForGivenOutputSize(int outputBufferSize, int level, boolean nowrap){
        if(level != 0){
            return outputBufferSize;
        }
        return (int) (0.99 * outputBufferSize);
    }
}
