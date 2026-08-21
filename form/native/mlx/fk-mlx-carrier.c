/* fk-mlx-carrier.c — MLX as an organ of THIS fkwu, not a second binary.
 *
 * ONE generic door: mlx_run(postfix). Form emits the program. The carrier
 * is a stack machine over MLX arrays. New shapes are new tokens in the
 * program (and a table row here), not new opcodes in fkwu-uni.c.
 * mlx_add is sugar: it writes "a b add" and calls the same runner.
 *
 * Weak stubs in runtime/fkwu-uni.c speak mlx_linked=false when unlinked.
 * Shrink target: the Form walker owns the call.
 */
#include "mlx/c/mlx.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

static long long fk_mlx_dispatch = 0;
static char fk_mlx_err[256] = "none";

static void fk_mlx_seterr(const char *m) {
    snprintf(fk_mlx_err, sizeof(fk_mlx_err), "%s", m ? m : "none");
}

/* MLX's DEFAULT ERROR HANDLER ABORTS. A Form program asking for a shape that has
 * no product — `m1x3 ... m2x2 ... matmul` — printed one line and took the whole
 * process with it, which is the opposite of every other door in this body: an
 * uncovered shape is DECLINED and the caller walks. So the carrier owns the
 * handler: the message is kept where mlx_status can speak it, and control comes
 * back. A refusal has to be survivable or it is not a refusal. */
static int fk_mlx_failed = 0;
static void fk_mlx_on_error(const char *msg, void *data) {
    (void)data;
    fk_mlx_failed = 1;
    fk_mlx_seterr(msg);
}
static void fk_mlx_arm_handler(void) {
    static int armed = 0;
    if (!armed) {
        mlx_set_error_handler(fk_mlx_on_error, 0, 0);
        armed = 1;
    }
}

static int fk_mlx_space(char c) {
    return c == ' ' || c == '\n' || c == '\t' || c == '\r';
}

static int fk_mlx_tok(const char **p, const char *end, char *buf, int cap) {
    while (*p < end && fk_mlx_space(**p)) {
        (*p)++;
    }
    if (*p >= end) {
        return 0;
    }
    int i = 0;
    while (*p < end && !fk_mlx_space(**p) && i + 1 < cap) {
        buf[i++] = *(*p)++;
    }
    buf[i] = 0;
    return 1;
}

static int fk_mlx_num(const char *s) {
    if (s[0] == '-' || s[0] == '+') {
        s++;
    }
    if (s[0] == 0) {
        return 0;
    }
    while (s[0]) {
        if (s[0] < '0' || s[0] > '9') {
            return 0;
        }
        s++;
    }
    return 1;
}

static void fk_mlx_drop(mlx_array *st, int *sp) {
    while (*sp > 0) {
        (*sp)--;
        mlx_array_free(st[*sp]);
    }
}

static int fk_mlx_binop(const char *op, mlx_array *st, int *sp, mlx_stream s) {
    if (*sp < 2) {
        fk_mlx_seterr("stack underflow");
        return -1;
    }
    mlx_array b = st[--(*sp)];
    mlx_array a = st[--(*sp)];
    mlx_array c = mlx_array_new();
    int rc = -1;
    if (strcmp(op, "add") == 0) {
        rc = mlx_add(&c, a, b, s);
    } else if (strcmp(op, "mul") == 0) {
        rc = mlx_multiply(&c, a, b, s);
    } else if (strcmp(op, "sub") == 0) {
        rc = mlx_subtract(&c, a, b, s);
    } else if (strcmp(op, "max") == 0) {
        rc = mlx_maximum(&c, a, b, s);
    } else if (strcmp(op, "matmul") == 0) {
        /* The op generation is MADE of. Everything above is elementwise and
         * could as well have run on the CPU; this is the one that has to be on
         * the GPU, and MLX's own kernel is what makes asking worth it. */
        rc = mlx_matmul(&c, a, b, s);
    } else {
        mlx_array_free(a);
        mlx_array_free(b);
        mlx_array_free(c);
        fk_mlx_seterr("unknown op");
        return -1;
    }
    mlx_array_free(a);
    mlx_array_free(b);
    if (rc != 0) {
        mlx_array_free(c);
        fk_mlx_seterr("mlx binop failed");
        return -1;
    }
    if (*sp >= 32) {
        mlx_array_free(c);
        fk_mlx_seterr("stack overflow");
        return -1;
    }
    st[(*sp)++] = c;
    return 0;
}

static int fk_mlx_vecn(const char *tok, int *n) {
    if (tok[0] != 'v' || tok[1] == 0) {
        return 0;
    }
    if (!fk_mlx_num(tok + 1)) {
        return 0;
    }
    *n = atoi(tok + 1);
    return *n >= 1 && *n <= 16;
}

/* mRxC — a two-dimensional shape, R*C lanes following. The vector token vN is
 * the 1-d case and stays as it was; this is the shape a weight matrix needs, and
 * a matvec is m1xK against mKx1. Bounded on purpose: a program is a string, and
 * a string is not how gigabytes of weights should ever arrive. That door — real
 * tensors from a mapped file — is the next stone, and it is named rather than
 * faked by raising this cap. */
static int fk_mlx_matn(const char *tok, int *r, int *c) {
    const char *x;
    if (tok[0] != 'm' || tok[1] == 0) {
        return 0;
    }
    x = tok + 1;
    while (*x && *x != 'x') {
        x++;
    }
    if (*x != 'x' || x == tok + 1 || x[1] == 0) {
        return 0;
    }
    {
        char lhs[32];
        long long len = (long long)(x - (tok + 1));
        if (len <= 0 || len > 30) {
            return 0;
        }
        memcpy(lhs, tok + 1, (size_t)len);
        lhs[len] = 0;
        if (!fk_mlx_num(lhs) || !fk_mlx_num(x + 1)) {
            return 0;
        }
        *r = atoi(lhs);
        *c = atoi(x + 1);
    }
    return *r >= 1 && *c >= 1 && *r <= 64 && *c <= 64 && (*r) * (*c) <= 256;
}

static int fk_mlx_push_mat(const char **p, const char *end, int r, int c,
                           mlx_array *st, int *sp) {
    int32_t data[256];
    char tok[64];
    int i = 0;
    int n = r * c;
    while (i < n) {
        if (!fk_mlx_tok(p, end, tok, 64) || !fk_mlx_num(tok)) {
            fk_mlx_seterr("mat needs integer lanes");
            return -1;
        }
        data[i++] = (int32_t)atoi(tok);
    }
    if (*sp >= 32) {
        fk_mlx_seterr("stack overflow");
        return -1;
    }
    {
        float fdata[256];
        int shape[2];
        int j = 0;
        while (j < n) {
            fdata[j] = (float)data[j];
            j = j + 1;
        }
        shape[0] = r;
        shape[1] = c;
        /* FLOAT32, not int: MLX's matmul is floating point only ("Only inexact
         * types are supported"), and so is every matmul generation performs.
         * The lanes are still written as integers in the program because a
         * program is text; the shape is what decides the tier. */
        st[(*sp)++] = mlx_array_new_data(fdata, shape, 2, MLX_FLOAT32);
    }
    return 0;
}

/* f32 <path> <off> <r> <c> — A TENSOR ARRIVES BY REFERENCE.
 *
 * Until now the only way data reached this GPU was as literal lanes in the
 * program text, capped at 256 of them. That is fine for proving a matmul and
 * useless for a forward pass: a program is a string, and a string is not how
 * gigabytes of weights should ever travel. This token names a file, a byte
 * offset and a shape, and the carrier reads r*c float32 from there.
 *
 * It is the door the walk-home receipt named as the next stone, and it is what
 * makes the MLX lane a GENERATION lane rather than a calculator: a GGUF's f32
 * tensors (every norm in the file) can now be multiplied on the GPU from where
 * they already lie, without being spelled out.
 *
 * Bounded, and the bound is honest: FK_MLX_TENSOR_CAP elements, and the file
 * must actually contain the extent asked for — a short read is a REFUSAL, never
 * a partial tensor padded with whatever was in the buffer. Quantized tiers
 * (Q8_0 and the K-quants, which is where the model's weight actually lives) are
 * the next stone after this one, and are named rather than faked. */
#define FK_MLX_TENSOR_CAP 4000000

static int fk_mlx_push_tensor(const char **p, const char *end, mlx_array *st, int *sp) {
    char path[512];
    char tok[64];
    long long off = 0;
    long long r = 0;
    long long c = 0;
    long long n = 0;
    float *data = 0;
    FILE *f = 0;
    size_t got = 0;
    if (!fk_mlx_tok(p, end, path, 512)) {
        fk_mlx_seterr("f32 needs a path");
        return -1;
    }
    if (!fk_mlx_tok(p, end, tok, 64) || !fk_mlx_num(tok)) {
        fk_mlx_seterr("f32 needs a byte offset");
        return -1;
    }
    off = atoll(tok);
    if (!fk_mlx_tok(p, end, tok, 64) || !fk_mlx_num(tok)) {
        fk_mlx_seterr("f32 needs rows");
        return -1;
    }
    r = atoll(tok);
    if (!fk_mlx_tok(p, end, tok, 64) || !fk_mlx_num(tok)) {
        fk_mlx_seterr("f32 needs cols");
        return -1;
    }
    c = atoll(tok);
    n = r * c;
    if (r < 1 || c < 1 || n > FK_MLX_TENSOR_CAP || off < 0) {
        fk_mlx_seterr("f32 shape out of range");
        return -1;
    }
    if (*sp >= 32) {
        fk_mlx_seterr("stack overflow");
        return -1;
    }
    f = fopen(path, "rb");
    if (f == 0) {
        fk_mlx_seterr("f32 cannot open path");
        return -1;
    }
    data = (float *)malloc((size_t)n * sizeof(float));
    if (data == 0) {
        fclose(f);
        fk_mlx_seterr("f32 out of memory");
        return -1;
    }
    if (fseek(f, (long)off, SEEK_SET) != 0) {
        free(data);
        fclose(f);
        fk_mlx_seterr("f32 cannot seek to offset");
        return -1;
    }
    got = fread(data, sizeof(float), (size_t)n, f);
    fclose(f);
    if (got != (size_t)n) {
        /* A SHORT READ IS A REFUSAL. Padding here would hand the GPU a tensor
         * whose tail is whatever malloc last held, and every number downstream
         * would be confidently wrong with no instrument saying so. */
        free(data);
        fk_mlx_seterr("f32 file is shorter than the shape asked for");
        return -1;
    }
    {
        int shape[2];
        shape[0] = (int)r;
        shape[1] = (int)c;
        st[(*sp)++] = mlx_array_new_data(data, shape, 2, MLX_FLOAT32);
    }
    free(data);
    return 0;
}

static int fk_mlx_unop(const char *op, mlx_array *st, int *sp, mlx_stream s) {
    if (strcmp(op, "sum") != 0) {
        return 1;
    }
    if (*sp < 1) {
        fk_mlx_seterr("stack underflow");
        return -1;
    }
    mlx_array a = st[--(*sp)];
    mlx_array c = mlx_array_new();
    int rc = mlx_sum(&c, a, false, s);
    mlx_array_free(a);
    if (rc != 0) {
        mlx_array_free(c);
        fk_mlx_seterr("mlx unop failed");
        return -1;
    }
    if (*sp >= 32) {
        mlx_array_free(c);
        fk_mlx_seterr("stack overflow");
        return -1;
    }
    st[(*sp)++] = c;
    return 0;
}

static int fk_mlx_push_vec(const char **p, const char *end, int n, mlx_array *st, int *sp) {
    int32_t data[16];
    char tok[64];
    int i = 0;
    while (i < n) {
        if (!fk_mlx_tok(p, end, tok, 64) || !fk_mlx_num(tok)) {
            fk_mlx_seterr("vec needs integer lanes");
            return -1;
        }
        data[i++] = (int32_t)atoi(tok);
    }
    if (*sp >= 32) {
        fk_mlx_seterr("stack overflow");
        return -1;
    }
    int shape[1];
    shape[0] = n;
    st[(*sp)++] = mlx_array_new_data(data, shape, 1, MLX_INT32);
    return 0;
}

long long fk_mlx_run_external(const char *src, long long n) {
    if (src == 0 || n <= 0) {
        fk_mlx_seterr("empty program");
        return 0;
    }
    fk_mlx_arm_handler();
    fk_mlx_failed = 0;
    mlx_device gpu = mlx_device_new_type(MLX_GPU, 0);
    bool gpu_ok = 0;
    mlx_device_is_available(&gpu_ok, gpu);
    mlx_stream s = gpu_ok ? mlx_default_gpu_stream_new() : mlx_default_cpu_stream_new();
    mlx_array st[32];
    int sp = 0;
    const char *p = src;
    const char *end = src + n;
    char tok[64];
    int fail = 0;
    while (!fail && fk_mlx_tok(&p, end, tok, 64)) {
        int vn = 0;
        int mr = 0;
        int mc = 0;
        int u = 0;
        if (fk_mlx_num(tok)) {
            if (sp >= 32) {
                fk_mlx_seterr("stack overflow");
                fail = 1;
                break;
            }
            st[sp++] = mlx_array_new_int(atoi(tok));
        } else if (fk_mlx_vecn(tok, &vn)) {
            if (fk_mlx_push_vec(&p, end, vn, st, &sp) != 0) {
                fail = 1;
            }
        } else if (fk_mlx_matn(tok, &mr, &mc)) {
            if (fk_mlx_push_mat(&p, end, mr, mc, st, &sp) != 0) {
                fail = 1;
            }
        } else if (strcmp(tok, "f32") == 0) {
            if (fk_mlx_push_tensor(&p, end, st, &sp) != 0) {
                fail = 1;
            }
        } else if ((u = fk_mlx_unop(tok, st, &sp, s)) != 1) {
            if (u != 0) {
                fail = 1;
            }
        } else if (fk_mlx_binop(tok, st, &sp, s) != 0) {
            fail = 1;
        }
        if (fk_mlx_failed) {
            fail = 1;
        }
    }
    long long outv = 0;
    if (fk_mlx_failed) {
        fail = 1;
    }
    if (!fail) {
        if (sp != 1) {
            fk_mlx_seterr("program did not leave one value");
            fail = 1;
        } else {
            int ev = mlx_array_eval(st[0]);
            int32_t v = 0;
            int it = -1;
            if (ev == 0) {
                /* ASK THE DTYPE, never the return code. mlx_array_item_int32 on
                 * a float32 array SUCCEEDS and hands back the raw bits: a matvec
                 * that really computed 32.0 read as 1107296256, which is
                 * 0x42000000. The arithmetic was right and the reading was
                 * wrong, and nothing in the status line would have said so.
                 * Anything through matmul is a float tier, because MLX's matmul
                 * is floating point only. */
                mlx_dtype dt = mlx_array_dtype(st[0]);
                if (dt == MLX_FLOAT32) {
                    float fv = 0.0f;
                    if (mlx_array_item_float32(&fv, st[0]) == 0) {
                        it = 0;
                        v = (int32_t)fv;
                    }
                } else {
                    it = mlx_array_item_int32(&v, st[0]);
                }
            }
            if (ev != 0 || it != 0) {
                fk_mlx_seterr("eval/item failed");
                fail = 1;
            } else {
                fk_mlx_seterr("none");
                fk_mlx_dispatch++;
                outv = (long long)v;
            }
        }
    }
    fk_mlx_drop(st, &sp);
    mlx_stream_free(s);
    mlx_device_free(gpu);
    return fail ? 0 : outv;
}

long long fk_mlx_add_external(long long a, long long b) {
    char buf[80];
    int n = snprintf(buf, sizeof(buf), "%lld %lld add", a, b);
    if (n <= 0 || n >= (int)sizeof(buf)) {
        fk_mlx_seterr("add encode failed");
        return 0;
    }
    return fk_mlx_run_external(buf, n);
}

long long fk_mlx_status_external(char *out, long long cap) {
    if (out == 0 || cap <= 0) {
        return 0;
    }
    bool metal = 0;
    mlx_metal_is_available(&metal);
    mlx_device gpu = mlx_device_new_type(MLX_GPU, 0);
    bool gpu_ok = 0;
    mlx_device_is_available(&gpu_ok, gpu);
    mlx_string ver = mlx_string_new();
    mlx_version(&ver);
    const char *vs = mlx_string_data(ver);
    if (vs == 0) {
        vs = "";
    }
    int n = snprintf(out, (size_t)cap,
        "mlx_owner=fkwu-form-cli\n"
        "mlx_linked=true\n"
        "mlx_metal_available=%s\n"
        "mlx_gpu_available=%s\n"
        "mlx_device=%s\n"
        "mlx_version=%s\n"
        "mlx_dispatch=%lld\n"
        "last_error=%s\n",
        metal ? "true" : "false",
        gpu_ok ? "true" : "false",
        gpu_ok ? "gpu" : "cpu",
        vs,
        fk_mlx_dispatch,
        fk_mlx_err);
    mlx_string_free(ver);
    mlx_device_free(gpu);
    if (n < 0) {
        return 0;
    }
    return (long long)n;
}
