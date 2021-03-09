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
#include "smithwaterman_common.h"

int32_t fast_itoa(char * ptr, int32_t number){
    bool is_neg = false;

    if(number < 0){
        number = -number;
        is_neg = true;
    }

    int32_t cp_number = number;
    int32_t digits = 0;
    
    while (cp_number > 0){
        cp_number /= 10;
        digits++;
    }
    
    if (ptr == NULL){
        // if the number is negative add 1 for the minus sign, 0 otherwise
        return digits + (int) is_neg;
    }

    if(is_neg){
        *(ptr++) = '-';
    }

    for(int i = digits-1; i >= 0; i--){
        *(ptr + i) = '0' +  (number % 10);
        number /= 10;
    }

    // if the number is negative add 1 for the minus sign, 0 otherwise
    return digits + (int) is_neg;
}
