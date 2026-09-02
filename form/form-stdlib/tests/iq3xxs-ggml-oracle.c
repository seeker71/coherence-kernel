/* iq3xxs-ggml-oracle.c — independent llama.cpp b10686 oracle for GGUF type 18. */
#include "ggml.h"

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static void put_u32_le(uint8_t * dst, uint32_t v) {
    dst[0] = (uint8_t) v;
    dst[1] = (uint8_t) (v >> 8);
    dst[2] = (uint8_t) (v >> 16);
    dst[3] = (uint8_t) (v >> 24);
}

int main(void) {
    uint8_t block[98] = {0};
    float out[256];
    const int samples[] = {0, 3, 4, 7, 8, 31, 32, 63, 64, 95, 96, 127, 128, 159, 160, 191, 192, 223, 224, 255};

    block[0] = 0x00;
    block[1] = 0x3c; /* d=f16(1) */
    for (int k = 0; k < 64; ++k) block[2 + k] = (uint8_t) ((13*k + 7) & 255);
    for (int ib = 0; ib < 8; ++ib) {
        uint32_t aux = (uint32_t) ((3*ib + 1) & 15) << 28;
        for (int l = 0; l < 4; ++l) aux |= (uint32_t) ((11*ib + 17*l) & 127) << (7*l);
        put_u32_le(block + 66 + 4*ib, aux);
    }

    const struct ggml_type_traits * traits = ggml_get_type_traits(GGML_TYPE_IQ3_XXS);
    if (traits == NULL || traits->to_float == NULL) return 2;
    traits->to_float(block, out, 256);

    long long rounded_sum = 0;
    for (int i = 0; i < 256; ++i) rounded_sum += llround((double) out[i] * 1000000.0);
    for (size_t k = 0; k < sizeof(samples)/sizeof(samples[0]); ++k) {
        const int i = samples[k];
        printf("SAMPLE %d %lld\n", i, llround((double) out[i] * 1000000.0));
    }
    printf("ROUND_SUM %lld\n", rounded_sum);
    return 0;
}
