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
        } else if ((u = fk_mlx_unop(tok, st, &sp, s)) != 1) {
            if (u != 0) {
                fail = 1;
            }
        } else if (fk_mlx_binop(tok, st, &sp, s) != 0) {
            fail = 1;
        }
    }
    long long outv = 0;
    if (!fail) {
        if (sp != 1) {
            fk_mlx_seterr("program did not leave one value");
            fail = 1;
        } else {
            int ev = mlx_array_eval(st[0]);
            int32_t v = 0;
            int it = (ev == 0) ? mlx_array_item_int32(&v, st[0]) : -1;
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
