/* mxoracle.c — a THROWAWAY independent dequantizer for GGUF types 40/41 as this
 * file lays them out. Not a fork of ds4 and not maintained: it exists to be read
 * against form-stdlib/mxfp4-plane-dequant.fk, whose own header says it is "NOT
 * bit-exactness against an independent implementation — none exists". This is
 * that implementation, and the two value maps below are ds4.c's, verbatim
 * (dsv4_e2m1fn_value_cpu :3231, dsv4_e4m3fn_value_cpu :3166) — written by
 * another hand, which is the whole point.
 * Geometry from mxfp4/mxfp8-plane-dequant.fk: a slice of n elements is
 *   type 40: [ n/2 bytes E2M1 nibbles ][ n/32 bytes E8M0 ]
 *   type 41: [ n   bytes E4M3        ][ n/32 bytes E8M0 ] */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

static float e2m1_value(int i) {                      /* ds4.c:3231, verbatim */
    static const float v[8] = {0.0f,0.5f,1.0f,1.5f,2.0f,3.0f,4.0f,6.0f};
    return v[i & 7];
}
static float e4m3_value(int i) {                      /* ds4.c:3166, verbatim */
    static const float exp_scale[16] = {
        0.0f,0.015625f,0.03125f,0.0625f, 0.125f,0.25f,0.5f,1.0f,
        2.0f,4.0f,8.0f,16.0f, 32.0f,64.0f,128.0f,256.0f };
    const int e = (i >> 3) & 0x0f, m = i & 0x07;
    return e == 0 ? (float)m * 0.001953125f : (1.0f + (float)m * 0.125f) * exp_scale[e];
}
static float e8m0(unsigned b) { return ldexpf(1.0f, (int)b - 127); }

int main(int argc, char **argv) {
    if (argc < 6) { fprintf(stderr,"usage: mxoracle FILE TYPE ABS N COUNT\n"); return 2; }
    const char *path = argv[1];
    int type = atoi(argv[2]);
    long long abs_off = atoll(argv[3]);
    long long n = atoll(argv[4]);          /* elements in ONE slice */
    int count = atoi(argv[5]);

    long long pay_bytes = (type == 40) ? n/2 : n;
    long long scale_off = abs_off + pay_bytes;
    long long need_pay = (type == 40) ? (count+1)/2 : count;
    long long need_sc  = (count + 31)/32 + 1;

    FILE *f = fopen(path, "rb");
    if (!f) { perror("open"); return 1; }
    unsigned char *pay = malloc(need_pay), *sc = malloc(need_sc);
    fseeko(f, abs_off, SEEK_SET);  if (fread(pay,1,need_pay,f) != (size_t)need_pay) { fprintf(stderr,"short payload\n"); return 1; }
    fseeko(f, scale_off, SEEK_SET); if (fread(sc,1,need_sc,f) != (size_t)need_sc) { fprintf(stderr,"short scales\n"); return 1; }

    printf("# type %d  n %lld  payload_bytes %lld  scale_base %lld\n", type, n, pay_bytes, scale_off);
    printf("# scale bytes seen:"); for (int i=0;i<4 && i<need_sc;i++) printf(" 0x%02X(2^%d)", sc[i], sc[i]-127); printf("\n");
    for (int i = 0; i < count; i++) {
        unsigned s = sc[i/32];
        if (type == 40) {
            unsigned byte = pay[i/2];
            unsigned lo = byte & 0x0f, hi = (byte >> 4) & 0x0f;
            unsigned c_loFirst = (i & 1) ? hi : lo;      /* even element -> low nibble */
            unsigned c_hiFirst = (i & 1) ? lo : hi;      /* even element -> high nibble */
            float a = (c_loFirst & 8 ? -1.f : 1.f) * e2m1_value(c_loFirst & 7) * e8m0(s);
            float b = (c_hiFirst & 8 ? -1.f : 1.f) * e2m1_value(c_hiFirst & 7) * e8m0(s);
            printf("%d byte=0x%02X  lowFirst=%.9g  highFirst=%.9g\n", i, byte, a, b);
        } else {
            unsigned c = pay[i];
            float v = (c & 0x80 ? -1.f : 1.f) * e4m3_value(c & 0x7f) * e8m0(s);
            printf("%d byte=0x%02X  value=%.9g\n", i, c, v);
        }
    }
    return 0;
}
