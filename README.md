[![Build Status](https://travis-ci.org/Intel-HLS/GKL.svg?branch=master)](https://travis-ci.org/Intel-HLS/GKL)
[![Maven Central](https://maven-badges.herokuapp.com/maven-central/com.intel.gkl/gkl/badge.svg)](https://maven-badges.herokuapp.com/maven-central/com.intel.gkl/gkl)

# Genomics Kernel Library (GKL)
This repository contains optimized versions of compute kernels used in genomics applications like
[GATK](https://github.com/broadinstitute/gatk) and [HTSJDK](https://github.com/samtools/htsjdk). These kernels are
optimized to run on Intel Architecture (AVX, AVX2, AVX-512, and multicore) under 64-bit Linux and Mac OSX.

Kernels included:
* **PairHMM**
  * AVX and AVX-512 optimized versions of PairHMM used in GATK HaplotypeCaller and MuTect2. 
  * OpenMP support for multicore processors.
* **Smith-Waterman**
  * AVX2 and AVX-512 optimized versions of Smith-Waterman used in GATK HaplotypeCaller and MuTect2.
* **DEFLATE Compression/Decompression**:
  * Performance optimized Level 1 and 2 compression and decompression from Intel's [ISA-L](https://github.com/01org/isa-l) library.
  * Performance optimized Level 3 through 9 compression from Intel's [Open Source Technology Center](https://01.org/) [zlib](https://github.com/jtkukunas/zlib) library.

# Building GKL
GKL release binaries are built on CentOS 6 to enable running on most Linux distributions (see [holy-build-box](https://github.com/phusion/holy-build-box#problem-introduction) for a good discription of portability issues).

## Requirements
* Java JDK 8
* Git >= 2.5
* CMake >= 2.8.12.2
* GCC g++ >= 5.3.1
* GNU patch >= 2.6
* GNU libtool >= 2.2.6
* GNU automake >= 1.11.1
* Yasm >= 1.2.0
* zlib-devel >= 1.2.7

## Setup
Run these commands to setup the build environment on CentOS:
```
sudo yum install -y java-1.8.0-openjdk-devel git cmake patch libtool automake yasm zlib-devel centos-release-scl
sudo yum install -y devtoolset-4-gcc-c++
source scl_source enable devtoolset-4
```

## Build and Test
After build requirements are met, clone, build, and test:
```
git clone https://github.com/Intel-HLS/GKL.git
cd GKL
./gradlew test
```

# License
All code is licensed under the [MIT License](https://opensource.org/licenses/MIT), except:
* PairHMM code from GATK is licensed under the [BSD 3-Clause License](https://opensource.org/licenses/BSD-3-Clause).
* ISA-L code is licensed under the [BSD 3-Clause License](https://opensource.org/licenses/BSD-3-Clause).
* Intel Open Source Technology Center zlib (otc_zlib) code is licensed under the [Zlib License](https://opensource.org/licenses/Zlib).
* zlib code is licensed under the [Zlib License](https://opensource.org/licenses/Zlib).
