/* q2koracle.c — THROWAWAY independent Q2_K reference. The decomposition and the
 * struct offsets are ds4.c's own (q2_k_value_f32 :3466, block_q2_K :759, whose
 * size is asserted 84 there), transcribed verbatim. Exists to be diffed against
 * form-stdlib/q2k-dequant.fk on identical bytes — the askalike pattern (row 931). */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#define QK_K 256
typedef struct { uint8_t scales[QK_K/16]; uint8_t qs[QK_K/4]; uint16_t d; uint16_t dmin; } block_q2_K;

static float f16_to_f32(uint16_t h) {          /* ds4.c:3090 portable branch */
    uint32_t sign=(uint32_t)(h&0x8000)<<16, exp=(h>>10)&0x1f, mant=h&0x03ff, bits;
    if (exp==0) { if(mant==0) bits=sign; else { exp=1; while(!(mant&0x0400)){mant<<=1;exp--;}
                  mant&=0x03ff; bits=sign|((exp+112)<<23)|(mant<<13); } }
    else if (exp==31) bits=sign|0x7f800000|(mant<<13);
    else bits=sign|((exp+112)<<23)|(mant<<13);
    float f; memcpy(&f,&bits,4); return f;
}
static float q2_k_value_f32(const block_q2_K *blocks, uint32_t k) {   /* ds4.c:3466 verbatim */
    const uint32_t block = k / QK_K;
    const uint32_t idx = k - block * QK_K;
    const block_q2_K *xb = blocks + block;
    const uint32_t group = idx / 16u;
    const uint32_t l = idx - group * 16u;
    const uint32_t q_base = 32u * (group / 8u) + 16u * (group & 1u);
    const uint32_t shift = ((group / 2u) & 3u) * 2u;
    const uint32_t q = ((uint32_t)xb->qs[q_base + l] >> shift) & 0x03u;
    const uint32_t sc = xb->scales[group];
    return f16_to_f32(xb->d) * (float)(sc & 0x0fu) * (float)q -
           f16_to_f32(xb->dmin) * (float)(sc >> 4u);
}
int main(int argc, char **argv) {
    if (argc < 4) { fprintf(stderr,"usage: q2koracle FILE OFFSET COUNT\n"); return 2; }
    long long off = atoll(argv[2]); int count = atoi(argv[3]);
    int nblk = (count + QK_K - 1)/QK_K;
    block_q2_K *b = malloc((size_t)nblk*sizeof(block_q2_K));
    FILE *f = fopen(argv[1],"rb"); if(!f){perror("open");return 1;}
    fseeko(f, off, SEEK_SET);
    if (fread(b,sizeof(block_q2_K),nblk,f) != (size_t)nblk) { fprintf(stderr,"short read\n"); return 1; }
    for (int k=0;k<count;k++) printf("%d %.9g\n", k, q2_k_value_f32(b,(uint32_t)k));
    return 0;
}
