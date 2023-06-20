The Intel Genomics Kernel Library version 0.8.10 does not include the latest functional and security updates. Intel Genomics Kernel Library Version 0.8.11 will be released on June 29, 2023, and includes important security updates and bug fixes. Customers should update to the latest version when available.

Intel has additional testing under way on the v0.8.11 release we expect to finish in June 2023.  


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
GKL release binaries are built on CentOS 7, to enable running on most Linux distributions (see [holy-build-box](https://github.com/phusion/holy-build-box#problem-introduction) for a good description of portability issues).

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
Run these commands to set up the build environment on CentOS:
```
sudo yum install -y java-1.8.0-openjdk-devel git cmake patch libtool automake yasm zlib-devel centos-release-scl help2man
sudo yum install -y devtoolset-7-gcc-c++
source scl_source enable devtoolset-7
```

## Build and Test
After build requirements are met, clone, and build :
```
git clone https://github.com/Intel-HLS/GKL.git
cd GKL
./gradlew build
```
For more details check `build.sh`

# Known issues
* When compressing using ISA-L library (compression levels 1, 2) outputted compressed data size can differ by small amount of bytes (up to 100) for the same input.
  This does not affect original uncompressed contents. Investigation of this issue is ongoing.

# License
All code is licensed under the [MIT License](https://opensource.org/licenses/MIT), except:
* PairHMM code from GATK is licensed under the [BSD 3-Clause License](https://opensource.org/licenses/BSD-3-Clause).
* ISA-L code is licensed under the [BSD 3-Clause License](https://opensource.org/licenses/BSD-3-Clause).
* Intel Open Source Technology Center zlib (otc_zlib) code is licensed under the [Zlib License](https://opensource.org/licenses/Zlib).
* zlib code is licensed under the [Zlib License](https://opensource.org/licenses/Zlib).

