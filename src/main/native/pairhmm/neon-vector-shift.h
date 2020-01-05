#ifdef PRECISION

inline void CONCAT(CONCAT(_vector_shift,SIMD_ENGINE), PRECISION) (UNION_TYPE &x, MAIN_TYPE shiftIn, MAIN_TYPE &shiftOut)
{
    
    IF_MAIN_TYPE tempIn, tempOut;
    tempIn.f = shiftIn;

    /* extract H */
    tempOut.i = VEC_EXTRACT_UNIT(x.i, SHIFT_CONST1);
    shiftOut = tempOut.f;

    /* shift */
    x.i = VEC_128_SHIFT_LEFT(x.i, SHIFT_CONST2);

    /* insert L */
    x.i = VEC_INSERT_UNIT(x.i, tempIn.i, SHIFT_CONST3);

}

inline void CONCAT(CONCAT(_vector_shift_last,SIMD_ENGINE), PRECISION) (UNION_TYPE &x, MAIN_TYPE shiftIn)
{

    IF_MAIN_TYPE temp;
    temp.f = shiftIn;

    /* shift */
    x.i = VEC_128_SHIFT_LEFT(x.i, SHIFT_CONST2);

    /* insert L */
    x.i = VEC_INSERT_UNIT(x.i, temp.i, SHIFT_CONST3);

}

#endif
