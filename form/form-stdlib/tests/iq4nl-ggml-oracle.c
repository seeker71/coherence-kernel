/* iq4nl-ggml-oracle.c — independent llama.cpp b10686 oracle for GGUF type 20. */
#include "ggml.h"

#include <math.h>
#include <stdint.h>
#include <stdio.h>

int main(void) {
    uint8_t block[18] = {0};
    float out[32];
    block[0] = 0x00;
    block[1] = 0x3c; /* f16(1) */
    for (int q = 0; q < 16; ++q) block[2 + q] = (uint8_t) (((15 - q) << 4) | q);

    const struct ggml_type_traits * traits = ggml_get_type_traits(GGML_TYPE_IQ4_NL);
    if (traits == NULL || traits->to_float == NULL) return 2;
    traits->to_float(block, out, 32);

    long long rounded_sum = 0;
    for (int i = 0; i < 32; ++i) rounded_sum += llround((double) out[i] * 1000000.0);
    printf("SAMPLE 0 %lld\n", llround((double) out[0] * 1000000.0));
    printf("SAMPLE 15 %lld\n", llround((double) out[15] * 1000000.0));
    printf("SAMPLE 16 %lld\n", llround((double) out[16] * 1000000.0));
    printf("SAMPLE 31 %lld\n", llround((double) out[31] * 1000000.0));
    printf("ROUND_SUM %lld\n", rounded_sum);
    return 0;
}
