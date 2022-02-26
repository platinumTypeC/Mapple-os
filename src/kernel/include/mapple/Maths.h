#pragma once

#include <stdint.h>

uint64_t findGCD(uint64_t a, uint64_t b) {
    if (b == 0){
        return a;
    }
    return findGCD(b, a % b);
}
void lowestFraction(
    uint64_t num1, 
    uint64_t num2,
    uint64_t* numirator,
    uint64_t* denominator
){
    uint64_t denom;
    denom = findGCD(num1,num2);
    num1/=denom;
    num2/=denom;
    *numirator = num1;
    *denominator = num2;
}