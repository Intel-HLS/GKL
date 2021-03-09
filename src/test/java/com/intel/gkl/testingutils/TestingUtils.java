/* The MIT License
 *
 * Copyright (c) 2021 Intel Corporation
 *
 * 	Permission is hereby granted, free of charge, to any person
 * 	obtaining a copy of this software and associated documentation
 * 	files (the "Software"), to deal in the Software without
 * 	restriction, including without limitation the rights to use,
 * 	copy, modify, merge, publish, distribute, sublicense, and/or
 * 	sell copies of the Software, and to permit persons to whom the
 * 	Software is furnished to do so, subject to the following
 * 	conditions:
 *
 * 	The above copyright notice and this permission notice shall be
 * 	included in all copies or substantial portions of the
 * 	Software.
 *
 * 	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
 * 	KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * 	WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * 	PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * 	COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * 	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * 	OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * 	SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

package com.intel.gkl.testingutils;

import java.security.SecureRandom;
import java.util.Arrays;

public class TestingUtils {
    public static final int MAX_SW_SEQUENCE_LENGTH = 32*1024-1; //2^15 - 1
    public static final int MAX_SW_MATCH_VALUE = 64*1024; // 2^16

    public static final int SIZE_4MB = 4 *1024 * 1024;
    //maximum allocatable array size, validated for openjdk-1.8
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

    public static int getMaxInputSizeForGivenOutputSize(int outputBufferSize, int level){
        if(level == 0){
            // compression level 0 introduces an overhead
            // that won't fit into the buffer of the same size
            return (int) (0.99 * outputBufferSize);
        }

        return outputBufferSize;
    }
}
