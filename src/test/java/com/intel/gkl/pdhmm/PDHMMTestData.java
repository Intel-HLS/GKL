package com.intel.gkl.pdhmm;

import java.lang.reflect.Array;

class PDHMMTestData {
    final int hapArraySize;
    final int readArraySize;

    byte[] hapBases;
    byte[] hapPdbases;
    byte[] readBases;
    byte[] readQual;
    byte[] readInsQual;
    byte[] readDelQual;
    byte[] gcp;
    long[] hapLengths;
    long[] readLengths;
    int batchSize;
    int maxHapLength;
    int maxReadLength;

    PDHMMTestData(int batchSize, int maxHapLength, int maxReadLength) {
        this.batchSize = batchSize;
        this.maxHapLength = maxHapLength;
        this.maxReadLength = maxReadLength;

        hapArraySize = this.batchSize * this.maxHapLength;
        hapBases = new byte[hapArraySize];
        hapPdbases = new byte[hapArraySize];

        readArraySize = this.batchSize * this.maxReadLength;
        readBases = new byte[readArraySize];
        readQual = new byte[readArraySize];
        readInsQual = new byte[readArraySize];
        readDelQual = new byte[readArraySize];
        gcp = new byte[readArraySize];

        hapLengths = new long[this.batchSize];
        readLengths = new long[this.batchSize];
    }

    PDHMMTestData copyWithHapBases(byte[] hapBases) {
        PDHMMTestData td = new PDHMMTestData(this.batchSize, this.maxHapLength, this.maxReadLength);
        td.hapBases = hapBases;
        return td;
    }

    PDHMMTestData copyWithHapPdbases(byte[] hapPdbases) {
        PDHMMTestData td = new PDHMMTestData(this.batchSize, this.maxHapLength, this.maxReadLength);
        td.hapPdbases = hapPdbases;
        return td;
    }

    PDHMMTestData copyWithReadBases(byte[] readBases) {
        PDHMMTestData td = new PDHMMTestData(this.batchSize, this.maxHapLength, this.maxReadLength);
        td.readBases = readBases;
        return td;
    }

    PDHMMTestData copyWithReadQual(byte[] readQual) {
        PDHMMTestData td = new PDHMMTestData(this.batchSize, this.maxHapLength, this.maxReadLength);
        td.readQual = readQual;
        return td;
    }

    PDHMMTestData copyWithReadInsQual(byte[] readInsQual) {
        PDHMMTestData td = new PDHMMTestData(this.batchSize, this.maxHapLength, this.maxReadLength);
        td.readInsQual = readInsQual;
        return td;
    }

    PDHMMTestData copyWithReadDelQual(byte[] readDelQual) {
        PDHMMTestData td = new PDHMMTestData(this.batchSize, this.maxHapLength, this.maxReadLength);
        td.readDelQual = readDelQual;
        return td;
    }

    PDHMMTestData copyWithGcp(byte[] gcp) {
        PDHMMTestData td = new PDHMMTestData(this.batchSize, this.maxHapLength, this.maxReadLength);
        td.gcp = gcp;
        return td;
    }

    PDHMMTestData copyWithHapLengths(long[] hapLengths) {
        PDHMMTestData td = new PDHMMTestData(this.batchSize, this.maxHapLength, this.maxReadLength);
        td.hapLengths = hapLengths;
        return td;
    }

    PDHMMTestData copyWithReadLengths(long[] readLengths) {
        PDHMMTestData td = new PDHMMTestData(this.batchSize, this.maxHapLength, this.maxReadLength);
        td.readLengths = readLengths;
        return td;
    }

    PDHMMTestData copyWithBatchSize(int batchSize) {
        PDHMMTestData td = new PDHMMTestData(this.batchSize, this.maxHapLength, this.maxReadLength);
        td.batchSize = batchSize;
        return td;
    }

    PDHMMTestData copyWithMaxHapLength(int maxHapLength) {
        PDHMMTestData td = new PDHMMTestData(this.batchSize, this.maxHapLength, this.maxReadLength);
        td.maxHapLength = maxHapLength;
        return td;
    }

    PDHMMTestData copyWithMaxReadLength(int maxReadLength) {
        PDHMMTestData td = new PDHMMTestData(this.batchSize, this.maxHapLength, this.maxReadLength);
        td.maxReadLength = maxReadLength;
        return td;
    }

    @Override
    public String toString() {
        return arrayNameWithSize(hapBases, "hap_bases") +
                arrayNameWithSize(hapPdbases, "hap_pdbases") +
                arrayNameWithSize(readBases, "read_bases") +
                arrayNameWithSize(readQual, "read_qual") +
                arrayNameWithSize(readInsQual, "read_ins_qual") +
                arrayNameWithSize(readDelQual, "read_del_qual") +
                arrayNameWithSize(gcp, "gcp") +
                arrayNameWithSize(hapLengths, "hap_lengths") +
                arrayNameWithSize(readLengths, "read_lengths") +
                "batchSize=" + batchSize +
                ", maxHapLength=" + maxHapLength +
                ", maxReadLength=" + maxReadLength
                ;
    }

    private String arrayNameWithSize(Object o, String name) {
        return (o == null ? "null" : String.format("%s[%d]", name, Array.getLength(o))) + ", ";
    }
}
