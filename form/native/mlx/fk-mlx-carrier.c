/* fk-mlx-carrier.c — MLX as an organ of THIS fkwu, not a second binary.
 *
 * ONE generic door: mlx_run(postfix). Form emits the program. The carrier
 * is a stack machine over MLX arrays. New shapes are new tokens in the
 * program (and a table row here), not new opcodes in fkwu-uni.c.
 * mlx_add is sugar: it writes "a b add" and calls the same runner.
 *
 * THE MINIMUM LAW. A token earns its place here only when no graph over the
 * other tokens computes it. Everything that CAN be composed is composed in
 * Form (form-stdlib/mlx-derived.fk) and costs this file nothing:
 *
 *   sub neg sigmoid silu swiglu tanh gelu mean rmsnorm layernorm
 *   l2norm softmax scale axpy recip square
 *   pow mod shift select clamp rope-pair              (added 2026-08-24)
 *
 * all of those are Form-emitted graphs over the twenty-three forms below.
 * `sub` used to live here and was retired on 2026-08-24 to prove the law cuts
 * both ways — a carrier row is not kept because it is convenient.
 *
 * THE TWENTY-THREE, and why each is irreducible:
 *   <int>       push int32 scalar          — the only literal
 *   vN a1..aN   push int32 vector          — the only shaped literal
 *   f32 / i32   astype                     — dtype is not computable
 *   dup / swap  stack shuffles             — a stack cannot express a DAG
 *                                            without them (silu needs x twice)
 *   add mul div                            — div is not reachable from add/mul
 *   max                                    — binary max; a compare/select pair
 *                                            would cost two rows, not one
 *   exp log                                — irreducible, and log is what
 *                                            retires `pow`: a^b = exp(b log a)
 *   sin cos                                — RoPE. Trigonometry does not come
 *                                            from exp over the reals; the
 *                                            2026-08-24 op census found four
 *                                            Metal kernels needing it and MLX
 *                                            unable to say any of them
 *   rsqrt                                  — transcendental; irreducible
 *   gt                                     — the only comparison; `where` is
 *                                            then a*c + b*(1-c), and `mod` is
 *                                            a - (a/n)*n, both Form lines
 *   iota                                   — a stream of any length; a billion
 *                                            lanes cannot be written as vN
 *   sum rmax                               — reductions over every axis
 *   matmul                                 — contraction is not elementwise
 *   rN d1..dN   reshape                    — shape is not computable
 *   take                                   — gather (the embedding row)
 *   argmax                                 — index-of-max; deriving it needs
 *                                            eq+where, two rows for one
 *
 * A program lands ONE int32. A float pipeline scales and says `i32` before it
 * ends: the carrier owns no float return path and needs no float parser.
 *
 * HONEST FLOOR: the reductions are over-all-axes. One decoded token is a
 * vector, so rmsnorm/softmax over it are whole-array reductions and this is
 * enough. A batched prefill over a matrix would want axis reductions; they
 * are named here, not claimed.
 *
 * Weak stubs in runtime/fkwu-uni.c speak mlx_linked=false when unlinked.
 * Shrink target: the Form walker owns the call.
 */
#include "mlx/c/mlx.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#define FK_MLX_STACK 64
#define FK_MLX_RANK 4

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

/* push takes ownership of c; on a full stack it frees rather than leaks. */
static int fk_mlx_push(mlx_array c, mlx_array *st, int *sp) {
    if (*sp >= FK_MLX_STACK) {
        mlx_array_free(c);
        fk_mlx_seterr("stack overflow");
        return -1;
    }
    st[(*sp)++] = c;
    return 0;
}

/* a prefixed token whose letter names a kind and whose digits name a count:
 * v3 is three int32 lanes, r2 is a two-dimensional reshape. */
static int fk_mlx_counted(const char *tok, char letter, int hi, int *n) {
    if (tok[0] != letter || tok[1] == 0 || !fk_mlx_num(tok + 1)) {
        return 0;
    }
    *n = atoi(tok + 1);
    return *n >= 1 && *n <= hi;
}

static int fk_mlx_lanes(const char **p, const char *end, int n, int32_t *out) {
    char tok[64];
    int i = 0;
    while (i < n) {
        if (!fk_mlx_tok(p, end, tok, 64) || !fk_mlx_num(tok)) {
            fk_mlx_seterr("counted token needs integer lanes");
            return -1;
        }
        out[i++] = (int32_t)atoi(tok);
    }
    return 0;
}

/* fk_mlx_apply — the whole vocabulary in one place, so the count is countable.
 * Returns 0 applied, 1 not-an-op (caller keeps looking), -1 failed. */
static int fk_mlx_apply(const char *op, mlx_array *st, int *sp, mlx_stream s) {
    int arity = 0;
    if (strcmp(op, "add") == 0 || strcmp(op, "mul") == 0 ||
        strcmp(op, "div") == 0 || strcmp(op, "max") == 0 ||
        strcmp(op, "gt") == 0 ||
        strcmp(op, "matmul") == 0 || strcmp(op, "take") == 0 ||
        strcmp(op, "swap") == 0) {
        arity = 2;
    } else if (strcmp(op, "exp") == 0 || strcmp(op, "log") == 0 ||
               strcmp(op, "sin") == 0 || strcmp(op, "cos") == 0 ||
               strcmp(op, "rsqrt") == 0 ||
               strcmp(op, "sum") == 0 || strcmp(op, "rmax") == 0 ||
               strcmp(op, "argmax") == 0 || strcmp(op, "f32") == 0 ||
               strcmp(op, "i32") == 0 || strcmp(op, "dup") == 0 ||
               strcmp(op, "iota") == 0) {
        arity = 1;
    } else {
        return 1;
    }
    if (*sp < arity) {
        fk_mlx_seterr("stack underflow");
        return -1;
    }

    /* the two shuffles move handles and touch no stream */
    if (strcmp(op, "dup") == 0) {
        mlx_array c = mlx_array_new();
        if (mlx_array_set(&c, st[*sp - 1]) != 0) {
            mlx_array_free(c);
            fk_mlx_seterr("dup failed");
            return -1;
        }
        return fk_mlx_push(c, st, sp);
    }
    if (strcmp(op, "swap") == 0) {
        mlx_array t = st[*sp - 1];
        st[*sp - 1] = st[*sp - 2];
        st[*sp - 2] = t;
        return 0;
    }

    /* iota reads its length as a value, not as a token, so a stream of any
     * size is expressible — a billion lanes cannot be written as literals. */
    if (strcmp(op, "iota") == 0) {
        mlx_array a = st[--(*sp)];
        int32_t n = 0;
        int rc = mlx_array_eval(a);
        if (rc == 0) {
            rc = mlx_array_item_int32(&n, a);
        }
        mlx_array_free(a);
        if (rc != 0 || n < 1) {
            fk_mlx_seterr("iota needs a positive int32 length");
            return -1;
        }
        mlx_array c = mlx_array_new();
        if (mlx_arange(&c, 0.0, (double)n, 1.0, MLX_INT32, s) != 0) {
            mlx_array_free(c);
            fk_mlx_seterr("iota failed");
            return -1;
        }
        return fk_mlx_push(c, st, sp);
    }

    mlx_array b = (arity == 2) ? st[--(*sp)] : mlx_array_new();
    mlx_array a = st[--(*sp)];
    mlx_array c = mlx_array_new();
    int rc = -1;
    if (strcmp(op, "add") == 0) {
        rc = mlx_add(&c, a, b, s);
    } else if (strcmp(op, "mul") == 0) {
        rc = mlx_multiply(&c, a, b, s);
    } else if (strcmp(op, "div") == 0) {
        rc = mlx_divide(&c, a, b, s);
    } else if (strcmp(op, "max") == 0) {
        rc = mlx_maximum(&c, a, b, s);
    } else if (strcmp(op, "matmul") == 0) {
        rc = mlx_matmul(&c, a, b, s);
    } else if (strcmp(op, "take") == 0) {
        rc = mlx_take(&c, a, b, s);
    } else if (strcmp(op, "gt") == 0) {
        rc = mlx_greater(&c, a, b, s);
    } else if (strcmp(op, "exp") == 0) {
        rc = mlx_exp(&c, a, s);
    } else if (strcmp(op, "log") == 0) {
        rc = mlx_log(&c, a, s);
    } else if (strcmp(op, "sin") == 0) {
        rc = mlx_sin(&c, a, s);
    } else if (strcmp(op, "cos") == 0) {
        rc = mlx_cos(&c, a, s);
    } else if (strcmp(op, "rsqrt") == 0) {
        rc = mlx_rsqrt(&c, a, s);
    } else if (strcmp(op, "sum") == 0) {
        rc = mlx_sum(&c, a, false, s);
    } else if (strcmp(op, "rmax") == 0) {
        rc = mlx_max(&c, a, false, s);
    } else if (strcmp(op, "argmax") == 0) {
        rc = mlx_argmax(&c, a, false, s);
    } else if (strcmp(op, "f32") == 0) {
        rc = mlx_astype(&c, a, MLX_FLOAT32, s);
    } else if (strcmp(op, "i32") == 0) {
        rc = mlx_astype(&c, a, MLX_INT32, s);
    }
    mlx_array_free(a);
    mlx_array_free(b);
    if (rc != 0) {
        mlx_array_free(c);
        fk_mlx_seterr("mlx op failed");
        return -1;
    }
    return fk_mlx_push(c, st, sp);
}

static int fk_mlx_push_vec(const char **p, const char *end, int n,
                           mlx_array *st, int *sp) {
    int32_t data[16];
    if (fk_mlx_lanes(p, end, n, data) != 0) {
        return -1;
    }
    int shape[1];
    shape[0] = n;
    return fk_mlx_push(mlx_array_new_data(data, shape, 1, MLX_INT32), st, sp);
}

static int fk_mlx_reshape_n(const char **p, const char *end, int n,
                            mlx_array *st, int *sp, mlx_stream s) {
    int32_t dims[FK_MLX_RANK];
    if (fk_mlx_lanes(p, end, n, dims) != 0) {
        return -1;
    }
    if (*sp < 1) {
        fk_mlx_seterr("stack underflow");
        return -1;
    }
    int shape[FK_MLX_RANK];
    for (int i = 0; i < n; i++) {
        shape[i] = (int)dims[i];
    }
    mlx_array a = st[--(*sp)];
    mlx_array c = mlx_array_new();
    int rc = mlx_reshape(&c, a, shape, (size_t)n, s);
    mlx_array_free(a);
    if (rc != 0) {
        mlx_array_free(c);
        fk_mlx_seterr("reshape failed");
        return -1;
    }
    return fk_mlx_push(c, st, sp);
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
    mlx_array st[FK_MLX_STACK];
    int sp = 0;
    const char *p = src;
    const char *end = src + n;
    char tok[64];
    int fail = 0;
    while (!fail && fk_mlx_tok(&p, end, tok, 64)) {
        int cn = 0;
        int applied = 0;
        if (fk_mlx_num(tok)) {
            if (fk_mlx_push(mlx_array_new_int(atoi(tok)), st, &sp) != 0) {
                fail = 1;
            }
        } else if (fk_mlx_counted(tok, 'v', 16, &cn)) {
            if (fk_mlx_push_vec(&p, end, cn, st, &sp) != 0) {
                fail = 1;
            }
        } else if (fk_mlx_counted(tok, 'r', FK_MLX_RANK, &cn)) {
            if (fk_mlx_reshape_n(&p, end, cn, st, &sp, s) != 0) {
                fail = 1;
            }
        } else if ((applied = fk_mlx_apply(tok, st, &sp, s)) != 1) {
            if (applied != 0) {
                fail = 1;
            }
        } else {
            fk_mlx_seterr("unknown op");
            fail = 1;
        }
    }
    long long outv = 0;
    if (!fail) {
        if (sp != 1) {
            fk_mlx_seterr("program did not leave one value");
            fail = 1;
        } else if (mlx_array_dtype(st[0]) != MLX_INT32) {
            fk_mlx_seterr("program did not land an int32 — say i32 before it ends");
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
        "mlx_ops=23\n"
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
