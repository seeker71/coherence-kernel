#define _DARWIN_C_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/clonefile.h>
#include <sys/stat.h>
#include <unistd.h>

enum { GGUF_STRING = 8, GGUF_ARRAY = 9 };

typedef struct {
    char *name;
    uint32_t ndim;
    uint64_t ne[4];
    uint32_t type;
    uint64_t offset;
    off_t type_pos;
} tensor;

static void fail(const char *what) {
    fprintf(stderr, "ds4-metalize: %s: %s\n", what, strerror(errno));
    exit(1);
}

static void read_exact(FILE *f, void *p, size_t n) {
    if (n && fread(p, 1, n, f) != n) fail("short read");
}

static uint32_t u32(FILE *f) {
    uint32_t v;
    read_exact(f, &v, sizeof(v));
    return v;
}

static uint64_t u64(FILE *f) {
    uint64_t v;
    read_exact(f, &v, sizeof(v));
    return v;
}

static char *string(FILE *f) {
    uint64_t n = u64(f);
    if (n > SIZE_MAX - 1) fail("string too large");
    char *s = malloc((size_t)n + 1);
    if (!s) fail("string allocation");
    read_exact(f, s, (size_t)n);
    s[n] = 0;
    return s;
}

static size_t scalar_size(uint32_t type) {
    static const uint8_t sizes[] = {1, 1, 2, 2, 4, 4, 4, 1, 0, 0, 8, 8, 8};
    return type < sizeof(sizes) ? sizes[type] : 0;
}

static void skip(FILE *f, uint64_t n) {
    if (fseeko(f, (off_t)n, SEEK_CUR) != 0) fail("seek");
}

static void skip_value(FILE *f, uint32_t type) {
    if (type == GGUF_STRING) {
        skip(f, u64(f));
    } else if (type == GGUF_ARRAY) {
        uint32_t et = u32(f);
        uint64_t n = u64(f);
        if (et == GGUF_STRING) {
            while (n--) skip(f, u64(f));
        } else {
            size_t z = scalar_size(et);
            if (!z) fail("unsupported GGUF array type");
            skip(f, n * z);
        }
    } else {
        size_t z = scalar_size(type);
        if (!z) fail("unsupported GGUF scalar type");
        skip(f, z);
    }
}

static size_t sf_offset(uint64_t row, uint64_t kb, uint64_t kbp) {
    return (size_t)((row / 128) * (kbp / 4) + kb / 4) * 512
         + (size_t)(row % 32) * 16
         + (size_t)((row % 128) / 32) * 4
         + (size_t)(kb % 4);
}

static void metalize_40(const uint8_t *src, uint8_t *dst, const tensor *t) {
    uint64_t k = t->ne[0], n = t->ne[1];
    uint64_t experts = t->ndim > 2 ? t->ne[2] : 1;
    uint64_t kb = k / 32, kbp = (kb + 3) & ~UINT64_C(3);
    uint64_t rows_pad = (n + 127) & ~UINT64_C(127);
    size_t data_bytes = (size_t)n * (size_t)k / 2;
    size_t sf_bytes = (size_t)rows_pad * (size_t)kbp;
    size_t expert_bytes = data_bytes + sf_bytes;
    if (k % 32 || n % 128 || kb % 4) {
        fprintf(stderr, "ds4-metalize: unaligned type-40 tensor %s\n", t->name);
        exit(1);
    }
    for (uint64_t e = 0; e < experts; e++) {
        const uint8_t *in = src + e * expert_bytes;
        const uint8_t *sf = in + data_bytes;
        uint8_t *out = dst + e * expert_bytes;
        for (uint64_t r = 0; r < n; r++) {
            for (uint64_t b = 0; b < kb; b++) {
                size_t block = ((size_t)r * kb + b) * 17;
                out[block] = sf[sf_offset(r, b, kbp)];
                memcpy(out + block + 1, in + (size_t)r * k / 2 + b * 16, 16);
            }
        }
    }
}

static void metalize_41(const uint8_t *src, uint8_t *dst, const tensor *t) {
    uint64_t k = t->ne[0], n = t->ne[1], kb = k / 32;
    uint64_t kbp = (kb + 3) & ~UINT64_C(3);
    uint64_t rows_pad = (n + 127) & ~UINT64_C(127);
    size_t data_bytes = (size_t)n * (size_t)k;
    const uint8_t *sf = src + data_bytes;
    if (k % 32 || n % 128 || kb % 4) {
        fprintf(stderr, "ds4-metalize: unaligned type-41 tensor %s\n", t->name);
        exit(1);
    }
    for (uint64_t r = 0; r < n; r++) {
        for (uint64_t b = 0; b < kb; b++) {
            size_t block = ((size_t)r * kb + b) * 33;
            dst[block] = sf[sf_offset(r, b, kbp)];
            memcpy(dst + block + 1, src + (size_t)r * k + b * 32, 32);
        }
    }
    (void)rows_pad;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "usage: ds4-metalize INPUT.gguf OUTPUT.gguf\n");
        return 2;
    }
    FILE *f = fopen(argv[1], "rb");
    if (!f) fail("open input");
    char magic[4];
    read_exact(f, magic, 4);
    if (memcmp(magic, "GGUF", 4) != 0) fail("not GGUF");
    uint32_t version = u32(f);
    uint64_t nt = u64(f), nkv = u64(f), alignment = 32;
    if (version < 2 || nt > SIZE_MAX / sizeof(tensor)) fail("unsupported GGUF");
    for (uint64_t i = 0; i < nkv; i++) {
        char *key = string(f);
        uint32_t type = u32(f);
        if (!strcmp(key, "general.alignment") && type == 4) alignment = u32(f);
        else skip_value(f, type);
        free(key);
    }
    tensor *ts = calloc((size_t)nt, sizeof(*ts));
    if (!ts) fail("tensor metadata allocation");
    for (uint64_t i = 0; i < nt; i++) {
        ts[i].name = string(f);
        ts[i].ndim = u32(f);
        if (!ts[i].ndim || ts[i].ndim > 4) fail("unsupported tensor rank");
        for (uint32_t d = 0; d < ts[i].ndim; d++) ts[i].ne[d] = u64(f);
        ts[i].type_pos = ftello(f);
        ts[i].type = u32(f);
        ts[i].offset = u64(f);
    }
    uint64_t meta_end = (uint64_t)ftello(f);
    uint64_t data_offset = (meta_end + alignment - 1) / alignment * alignment;
    fclose(f);

    if (clonefile(argv[1], argv[2], 0) != 0) fail("APFS clone");
    int in_fd = open(argv[1], O_RDONLY), out_fd = open(argv[2], O_RDWR);
    if (in_fd < 0 || out_fd < 0) fail("open mapped files");
    struct stat st;
    if (fstat(in_fd, &st) != 0) fail("stat input");
    uint8_t *src = mmap(NULL, (size_t)st.st_size, PROT_READ, MAP_PRIVATE, in_fd, 0);
    uint8_t *dst = mmap(NULL, (size_t)st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, out_fd, 0);
    if (src == MAP_FAILED || dst == MAP_FAILED) fail("mmap");

    uint64_t changed = 0;
    for (uint64_t i = 0; i < nt; i++) {
        tensor *t = &ts[i];
        if (t->type != 40 && t->type != 41) continue;
        fprintf(stderr, "metalize %" PRIu64 "/%" PRIu64 " %s type=%u\n",
                i + 1, nt, t->name, t->type);
        uint8_t *out = dst + data_offset + t->offset;
        const uint8_t *in = src + data_offset + t->offset;
        if (t->type == 40) metalize_40(in, out, t);
        else metalize_41(in, out, t);
        uint32_t replacement = t->type == 40 ? 39 : 38;
        memcpy(dst + t->type_pos, &replacement, sizeof(replacement));
        changed++;
    }
    if (msync(dst, (size_t)st.st_size, MS_SYNC) != 0) fail("msync");
    fprintf(stderr, "ds4-metalize: changed=%" PRIu64 " tensors bytes=%" PRIu64 "\n",
            changed, (uint64_t)st.st_size);
    return changed ? 0 : 3;
}
