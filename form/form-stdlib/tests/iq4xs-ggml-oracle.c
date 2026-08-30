/* iq4xs-ggml-oracle.c — independent llama.cpp b10686 oracle for GGUF type 20. */
#include "ggml.h"

#include <math.h>
#include <stdint.h>
#include <stdio.h>

int main(void) {
    uint8_t block[136] = {0};
    float out[256];
    const int samples[] = {0, 15, 16, 31, 32, 63, 64, 95, 96, 127, 128, 159, 160, 191, 192, 223, 224, 255};

    /* d=f16(1). Eight 6-bit scales visit both low and high scale planes. */
    block[0] = 0x00;
    block[1] = 0x3c;
    uint16_t scales_h = 0;
    for (int ib = 0; ib < 8; ++ib) {
        const int sc = (7 * ib + 3) & 63;
        block[4 + ib/2] |= (uint8_t) ((sc & 15) << (4 * (ib & 1)));
        scales_h |= (uint16_t) ((sc >> 4) << (2 * ib));
    }
    block[2] = (uint8_t) scales_h;
    block[3] = (uint8_t) (scales_h >> 8);
    for (int q = 0; q < 128; ++q) {
        block[8 + q] = (uint8_t) (((15 - (q % 16)) << 4) | (q % 16));
    }

    const struct ggml_type_traits * traits = ggml_get_type_traits(GGML_TYPE_IQ4_XS);
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
