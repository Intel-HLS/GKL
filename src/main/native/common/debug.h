/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2021 Intel Corporation
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
#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>

#ifdef DEBUG
#  define DBG(M, ...) fprintf(stderr, "[DEBUG] (%s:%d) : " M "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#  define INFO(M, ...) fprintf(stderr, "[INFO] (%s:%d) : " M "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#  define WARN(M, ...) fprintf(stderr, "[WARNING] (%s:%d) : " M "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#  define ERROR(M, ...) fprintf(stderr, "[ERROR] (%s:%d) :" M "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#  define DBG(M, ...)
#  define INFO(M, ...) fprintf(stderr, "[INFO] " M "\n", ##__VA_ARGS__)
#  define WARN(M, ...) fprintf(stderr, "[WARNING] " M "\n", ##__VA_ARGS__)
#  define ERROR(M, ...) fprintf(stderr, "[ERROR] " M "\n", ##__VA_ARGS__)
#endif

#endif
