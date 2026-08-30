/* iq2xs-ggml-oracle.c — independent llama.cpp b10686 oracle for Form's type-17 carver.
 *
 * This does not transcribe the IQ2_XS formula or tables. It asks the pinned ggml library's
 * public type-traits entry to dequantize one deterministic 74-byte block, then emits rounded
 * samples and a whole-block rounded sum. The Form band constructs the same bytes by recipe.
 *
 * Build on the comparator checkout used by the receipt:
 *   cc -O2 -I.../ggml/include iq2xs-ggml-oracle.c -L.../build/bin -lggml-base \
 *      -Wl,-rpath,.../build/bin -o /tmp/iq2xs-ggml-oracle
 */

#include "ggml.h"

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static void put_u16_le(uint8_t * dst, uint16_t v) {
    dst[0] = (uint8_t) (v & 0xffu);
    dst[1] = (uint8_t) (v >> 8);
}

int main(void) {
    uint8_t block[74] = {0};
    float out[256];
    const int samples[] = {0, 7, 8, 31, 32, 63, 64, 95, 96, 127, 128, 159, 160, 191, 192, 223, 224, 255};

    /* d = f16(1.0). qword k visits independent grid/sign fields. */
    block[0] = 0x00;
    block[1] = 0x3c;
    for (int k = 0; k < 32; ++k) {
        const uint16_t grid = (uint16_t) ((17 * k) % 512);
        const uint16_t sign = (uint16_t) ((13 * k) % 128);
        put_u16_le(block + 2 + 2 * k, (uint16_t) (grid | (sign << 9)));
    }
    for (int ib32 = 0; ib32 < 8; ++ib32) {
        block[66 + ib32] = (uint8_t) (ib32 | ((15 - ib32) << 4));
    }

    const struct ggml_type_traits * traits = ggml_get_type_traits(GGML_TYPE_IQ2_XS);
    if (traits == NULL || traits->to_float == NULL) {
        fputs("IQ2_XS trait unavailable\n", stderr);
        return 2;
    }
    traits->to_float(block, out, 256);

    long long rounded_sum = 0;
    for (int i = 0; i < 256; ++i) {
        rounded_sum += llround((double) out[i] * 1000000.0);
    }
    for (size_t k = 0; k < sizeof(samples) / sizeof(samples[0]); ++k) {
        const int i = samples[k];
        printf("SAMPLE %d %lld\n", i, llround((double) out[i] * 1000000.0));
    }
    printf("ROUND_SUM %lld\n", rounded_sum);
    return 0;
}
