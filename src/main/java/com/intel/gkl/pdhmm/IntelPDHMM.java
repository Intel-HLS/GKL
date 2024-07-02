/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2023-2024 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
package com.intel.gkl.pdhmm;

import com.intel.gkl.NativeLibraryLoader;
import org.broadinstitute.gatk.nativebindings.NativeLibrary;

import java.io.File;
import java.lang.reflect.Array;
import java.util.Objects;

public class IntelPDHMM implements NativeLibrary {
    private static final Object lock_class = new Object();
    private static final String NATIVE_LIBRARY_NAME;
    private static boolean initialized;

    static {
        NATIVE_LIBRARY_NAME = "gkl_pdhmm";
        initialized = false;
    }

    @Override
    public boolean load(File tempDir) {
        synchronized (lock_class) {
            if (!NativeLibraryLoader.load(tempDir, NATIVE_LIBRARY_NAME)) {
                return false;
            }
            if (!initialized) {
                initialized = true;
            }
        }
        initNative();
        return true;
    }

    private static void checkArraySize(Object array, int expectedSize, String arrayName) {
        Objects.requireNonNull(array, arrayName + " must not be null.");
        if (!array.getClass().isArray()) {
            throw new IllegalArgumentException(arrayName + " is not an array.");
        } else if (Array.getLength(array) != expectedSize) {
            String errorMessage = String.format("Array %s has size %d, but expected size is %d.", arrayName,
                    Array.getLength(
                            array),
                    expectedSize);
            throw new IllegalArgumentException(errorMessage);
        }
    }

    public double[] computePDHMM(byte[] hap_bases, byte[] hap_pdbases, byte[] read_bases, byte[] read_qual,
            byte[] read_ins_qual, byte[] read_del_qual, byte[] gcp, long[] hap_lengths, long[] read_lengths,
            int batchSize, int maxHapLength, int maxReadLength) {
        int hapArrayLength = maxHapLength * batchSize;
        int readArrayLength = maxReadLength * batchSize;

        checkArraySize(hap_bases, hapArrayLength, "hap_bases");
        checkArraySize(hap_pdbases, hapArrayLength, "hap_pdbases");
        checkArraySize(read_bases, readArrayLength, "read_bases");
        checkArraySize(read_qual, readArrayLength, "read_qual");
        checkArraySize(read_ins_qual, readArrayLength, "read_ins_qual");
        checkArraySize(read_del_qual, readArrayLength, "read_del_qual");
        checkArraySize(gcp, readArrayLength, "gcp");
        checkArraySize(hap_lengths, batchSize, "hap_lengths");
        checkArraySize(read_lengths, batchSize, "read_lengths");

        if (batchSize <= 0)
            throw new IllegalArgumentException("batchSize must be greater than 0.");

        if (maxHapLength <= 0)
            throw new IllegalArgumentException(
                    "maxHapLength must be greater than 0. Cannot perform PDHMM on empty sequence");

        if (maxReadLength <= 0)
            throw new IllegalArgumentException(
                    "maxReadLength must be greater than 0. Cannot perform PDHMM on empty sequence");

        try {
            return computePDHMMNative(hap_bases, hap_pdbases, read_bases, read_qual,
                    read_ins_qual, read_del_qual, gcp, hap_lengths, read_lengths,
                    batchSize, maxHapLength, maxReadLength);
        } catch (OutOfMemoryError e) {
            throw new OutOfMemoryError(
                    "OutOfMemory exception thrown from native pdhmm function call " + e.getMessage());
        } catch (IllegalArgumentException e) {
            throw new IllegalArgumentException(
                    "IllegalArgument exception thrown from native pdhmm function call " + e.getMessage());
        } catch (RuntimeException e) {
            throw new RuntimeException(
                    "Runtime exception thrown from native pdhmm function call " + e.getMessage());
        }
    }

    public void done() {
        doneNative();
    }

    private native static void initNative();

    private native double[] computePDHMMNative(byte[] hap_bases, byte[] hap_pdbases, byte[] read_bases,
            byte[] read_qual,
            byte[] read_ins_qual, byte[] read_del_qual, byte[] gcp, long[] hap_lengths, long[] read_lengths,
            int testcase, int maxHapLength, int maxReadLength);

    private native static void doneNative();
}
