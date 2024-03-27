/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2023 Intel Corporation
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

    public double[] computePDHMM(byte[] hap_bases, byte[] hap_pdbases, byte[] read_bases, byte[] read_qual,
            byte[] read_ins_qual, byte[] read_del_qual, byte[] gcp, long[] hap_lengths, long[] read_lengths,
            int testcase, int maxHapLength, int maxReadLength)
            throws RuntimeException, OutOfMemoryError {
        if (hap_bases == null)
            throw new NullPointerException("hap_bases array is null.");
        if (hap_pdbases == null)
            throw new NullPointerException("hap_pdbases array is null.");
        if (read_bases == null)
            throw new NullPointerException("read_bases array is null.");
        if (read_qual == null)
            throw new NullPointerException("read_qual array is null.");
        if (read_ins_qual == null)
            throw new NullPointerException("read_ins_qual array is null.");
        if (read_del_qual == null)
            throw new NullPointerException("read_del_qual array is null.");
        if (gcp == null)
            throw new NullPointerException("gcp array is null.");
        if (hap_lengths == null)
            throw new NullPointerException("hap_lengths array is null.");
        if (read_lengths == null)
            throw new NullPointerException("read_lengths array is null.");
        if (testcase <= 0)
            throw new IllegalArgumentException("Invalid number of testcase.");
        if (maxHapLength <= 0 || maxReadLength <= 0)
            throw new IllegalArgumentException("Cannot perform pdhmm on empty sequences");

        try {
            return computePDHMMNative(hap_bases, hap_pdbases, read_bases, read_qual,
                    read_ins_qual, read_del_qual, gcp, hap_lengths, read_lengths,
                    testcase, maxHapLength, maxReadLength);
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

    private native static void initNative();

    private native double[] computePDHMMNative(byte[] hap_bases, byte[] hap_pdbases, byte[] read_bases,
            byte[] read_qual,
            byte[] read_ins_qual, byte[] read_del_qual, byte[] gcp, long[] hap_lengths, long[] read_lengths,
            int testcase, int maxHapLength, int maxReadLength);
}
