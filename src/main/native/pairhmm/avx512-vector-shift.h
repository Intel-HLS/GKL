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
