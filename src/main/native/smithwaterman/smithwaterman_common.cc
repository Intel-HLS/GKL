#include "smithwaterman_common.h"

int32_t fast_itoa(char * ptr, int32_t number){
    bool is_neg = false;

    if(number < 0){
        number = -number;
        is_neg = true;
    }

    int32_t cp_number = number;
    int32_t digits = 0;
    
    do {
        cp_number /= 10;
        digits++;
    }
    while (cp_number != 0);
    
    if (ptr == nullptr){
        return digits + is_neg;
    }

    if(is_neg){
        *(ptr++) = '-';
    }

    for(int i = digits-1; i >= 0; i--){
        *(ptr + i) = '0' +  (number % 10);
        number /= 10;
    }

    return digits + is_neg;
}
