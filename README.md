[![Build Status](https://travis-ci.org/Intel-HLS/GKL.svg?branch=master)](https://travis-ci.org/Intel-HLS/GKL)

# Intel Genomics Kernel Library (GKL)
This repository contains optimized versions of compute kernels used in genomics applications like
[GATK](https://github.com/broadinstitute/gatk) and [HTSJDK](https://github.com/samtools/htsjdk). These kernels are
optimized to run on Intel Architecture under 64-bit Linux and Mac OSX.

Kernels included:
* **PairHMM**: AVX optimized version of the PairHMM algorithm, which is used to perform pairwise alignment of reads vs. haplotypes
in GATK HaplotypeCaller and MuTect2
* **igzip compression**: High performance level 1 compression optimized for genomic data
* **otc_zlib compression**: Optimized zlib compression for level 2 through 9

## Requirements
* Java 8

## Quickstart
Clone, build, and test:
```
git clone https://github.com/Intel-HLS/GKL.git
cd GKL
./gradlew test
```

## License
All code is licensed under the [MIT License](https://opensource.org/licenses/MIT), except:
* PairHMM code from GATK is licensed under the [BSD 3-Clause License](https://opensource.org/licenses/BSD-3-Clause).
* Intel Open Source Technology Center zlib (otc_zlib) code is licensed under the [Zlib License](https://opensource.org/licenses/Zlib).
* zlib code is licensed under the [Zlib License](https://opensource.org/licenses/Zlib).
