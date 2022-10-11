# Motors3D-Follow
 Week by week followup

~~~c++

#include <stdint.h>
#include <bit>
#include <stdlib.h>
#include <assert.h>
#include <algorithm>
#include <stdio.h>

uint32_t ToBits(float f) { return std::bit_cast<uint32_t>(f); }
float FromBits(uint32_t bits) {return std::bit_cast<float>(bits);}

void printbits(uint32_t bits) {
    printf("Sign: %i\n", bits >> 31 & 1);
    printf("Exponent: ");
    for(int i = 30; i > 30-8; --i)
        printf("%i", bits >> i & 1);
    printf("\n");

    printf("Significand: ");
    for(int i = 30 - 8; i > -1; --i)
        printf("%i", bits >> i & 1);
    printf("\n");
    printf("\n");
}

uint32_t PrintFloatBits(float a) {
    printf("\n");
    printf("\n");
    printf("Printing float: %f", a);
    printf("\n");
    uint32_t bits = ToBits(a);
    printbits(bits);

    return bits;
}

#define TGL_FLAG(n, f) ((n) ^= (f))

int main() {
    float a = 0.5;
    uint32_t bits = PrintFloatBits(a);
    
    for(int i = 30; i > 30-8; -i)
        TGL_FLAG(bits, 1 << i);
    printbits(bits);
    printf("%f", FromBits(bits));

    return 0;
}
```