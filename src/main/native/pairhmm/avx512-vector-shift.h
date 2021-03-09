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
#ifdef PRECISION

inline void CONCAT(CONCAT(_vector_shift,SIMD_ENGINE), PRECISION) (UNION_TYPE &x, MAIN_TYPE shiftIn, MAIN_TYPE &shiftOut)
{

    IF_256 xhigh;  
    IF_MAIN_TYPE shiftOutH,shiftInH;
    /* extract high value to return back */
    xhigh.f = VEC_EXTRACT_256(x.d, 1); 
    shiftOutH.i = VEC_EXTRACT_UNIT(xhigh.i, SHIFT_CONST2); 
    shiftOut = shiftOutH.f;  
    /* control regiuster idx for permute shift by one float value to left */
    _512_INT_TYPE idx = VEC_SET1();
    x.i = VEC_PERMUTE(idx, x.i);
    idx = VEC_SET2(); 
    x.i = VEC_AND(x.i, idx);
    /*shift in the ShiftIn value */
    shiftInH.f = shiftIn; 
    idx = VEC_SET3(shiftInH.i);
    x.i = VEC_OR(x.i, idx);    
}

inline void CONCAT(CONCAT(_vector_shift_last,SIMD_ENGINE), PRECISION) (UNION_TYPE &x, MAIN_TYPE shiftIn)
{
    IF_MAIN_TYPE shiftInH;
    _512_INT_TYPE idx = VEC_SET1();
    x.i = VEC_PERMUTE(idx, x.i);
    idx = VEC_SET2();  
    x.i = VEC_AND(x.i, idx);
    shiftInH.f = shiftIn; 
    idx = VEC_SET3(shiftInH.i);
    x.i = VEC_OR(x.i, idx);
}

#endif
