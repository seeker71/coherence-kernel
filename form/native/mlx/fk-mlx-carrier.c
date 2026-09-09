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
 * all of those are Form-emitted graphs over the twenty-four forms below.
 * `sub` used to live here and was retired on 2026-08-24 to prove the law cuts
 * both ways — a carrier row is not kept because it is convenient. The law cuts
 * a third way too, and that one took longest to see: a row can be lost without
 * anyone deciding to lose it. `tf32` passed the law, was written in #470, and
 * went out in the 2026-08-25 consolidation as collateral — restored 2026-09-09
 * only because someone asked why its band was still red.
 *
 * THE TWENTY-FOUR, and why each is irreducible:
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
 *   tf32 <path> <off> <r> <c>              — a tensor by REFERENCE. No graph
 *                                            over the other tokens can name a
 *                                            byte offset in a file; this is not
 *                                            a computation, it is a DOOR, and
 *                                            it is what makes this a generation
 *                                            lane instead of a calculator
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

/* tf32 <path> <off> <r> <c> — A TENSOR ARRIVES BY REFERENCE.
 *
 * Without this the only way data reaches the GPU is as literal lanes in the
 * program text. That is fine for proving a matmul and useless for a forward
 * pass: a program is a string, and a string is not how gigabytes of weights
 * should ever travel. This token names a file, a byte offset and a shape, and
 * the carrier reads r*c float32 from where they already lie. It is what makes
 * the MLX lane a GENERATION lane rather than a calculator.
 *
 * IT EARNS ITS ROW under the minimum law at the top of this file, and by the
 * strictest reading of that law: no graph over the other tokens can name a
 * byte offset in a file. It is not derivable, it is a DOOR.
 *
 * WHY IT IS BEING WRITTEN A SECOND TIME. It landed in #470 as `f32`, and the
 * 2026-08-25 consolidation that cut this file from 942 lines to 195 took it out
 * — not by the law, which it passes, but as collateral, and nothing said so.
 * `mlx-tensor-band.fk` kept asking for it and kept scoring 49 of 63, because
 * its refusal bits accepted any 0 and 0 is also what an unparseable program
 * returns. A capability can be deleted, its band can keep running, and the band
 * can keep reading most of the way to green: that is the whole failure, and it
 * took a person asking why the red was still there to close it.
 *
 * IT IS `tf32` AND NOT `f32` because `f32` now means the astype cast, which is
 * also irreducible and also earns its row. Two irreducible meanings cannot
 * share one token; the dtype prefix leaves room for the named next stone, the
 * quantized tiers (`tq8` and the K-quants) where the model's weight actually
 * lives.
 *
 * Bounded, and the bound is honest: FK_MLX_TENSOR_CAP elements, and the file
 * must actually contain the extent asked for — a short read is a REFUSAL, never
 * a partial tensor padded with whatever the buffer last held. */
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
        fk_mlx_seterr("tf32 needs a path");
        return -1;
    }
    if (!fk_mlx_tok(p, end, tok, 64) || !fk_mlx_num(tok)) {
        fk_mlx_seterr("tf32 needs a byte offset");
        return -1;
    }
    off = atoll(tok);
    if (!fk_mlx_tok(p, end, tok, 64) || !fk_mlx_num(tok)) {
        fk_mlx_seterr("tf32 needs rows");
        return -1;
    }
    r = atoll(tok);
    if (!fk_mlx_tok(p, end, tok, 64) || !fk_mlx_num(tok)) {
        fk_mlx_seterr("tf32 needs cols");
        return -1;
    }
    c = atoll(tok);
    n = r * c;
    if (r < 1 || c < 1 || n > FK_MLX_TENSOR_CAP || off < 0) {
        fk_mlx_seterr("tf32 shape out of range");
        return -1;
    }
    f = fopen(path, "rb");
    if (f == 0) {
        fk_mlx_seterr("tf32 cannot open path");
        return -1;
    }
    data = (float *)malloc((size_t)n * sizeof(float));
    if (data == 0) {
        fclose(f);
        fk_mlx_seterr("tf32 out of memory");
        return -1;
    }
    if (fseek(f, (long)off, SEEK_SET) != 0) {
        free(data);
        fclose(f);
        fk_mlx_seterr("tf32 cannot seek to offset");
        return -1;
    }
    got = fread(data, sizeof(float), (size_t)n, f);
    fclose(f);
    if (got != (size_t)n) {
        /* A SHORT READ IS A REFUSAL. Padding here would hand the GPU a tensor
         * whose tail is whatever malloc last held, and every number downstream
         * would be confidently wrong with no instrument saying so. */
        free(data);
        fk_mlx_seterr("tf32 file is shorter than the shape asked for");
        return -1;
    }
    {
        int shape[2];
        int rc;
        shape[0] = (int)r;
        shape[1] = (int)c;
        rc = fk_mlx_push(mlx_array_new_data(data, shape, 2, MLX_FLOAT32), st, sp);
        free(data);
        return rc;
    }
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
        } else if (strcmp(tok, "tf32") == 0) {
            if (fk_mlx_push_tensor(&p, end, st, &sp) != 0) {
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
    /* MLX's own allocator ledger (mlx/c/memory.h). Each door answers 0 on
     * success; a door that refuses leaves its line out of the status entirely,
     * so a reader sees the named missing door rather than a fabricated 0. */
    size_t active = 0, peak = 0, cached = 0, limit = 0;
    int active_ok = mlx_get_active_memory(&active);
    int peak_ok = mlx_get_peak_memory(&peak);
    int cache_ok = mlx_get_cache_memory(&cached);
    int limit_ok = mlx_get_memory_limit(&limit);
    int n = snprintf(out, (size_t)cap,
        "mlx_owner=fkwu-form-cli\n"
        "mlx_linked=true\n"
        "mlx_metal_available=%s\n"
        "mlx_gpu_available=%s\n"
        "mlx_device=%s\n"
        "mlx_version=%s\n"
        "mlx_ops=24\n"
        "mlx_dispatch=%lld\n"
        "last_error=%s\n",
        metal ? "true" : "false",
        gpu_ok ? "true" : "false",
        gpu_ok ? "gpu" : "cpu",
        vs,
        fk_mlx_dispatch,
        fk_mlx_err);
    if (n > 0 && (long long)n < cap && active_ok == 0) {
        n = n + snprintf(out + n, (size_t)(cap - n),
            "mlx_active_memory=%llu\n", (unsigned long long)active);
    }
    if (n > 0 && (long long)n < cap && peak_ok == 0) {
        n = n + snprintf(out + n, (size_t)(cap - n),
            "mlx_peak_memory=%llu\n", (unsigned long long)peak);
    }
    if (n > 0 && (long long)n < cap && cache_ok == 0) {
        n = n + snprintf(out + n, (size_t)(cap - n),
            "mlx_cache_memory=%llu\n", (unsigned long long)cached);
    }
    if (n > 0 && (long long)n < cap && limit_ok == 0) {
        n = n + snprintf(out + n, (size_t)(cap - n),
            "mlx_memory_limit=%llu\n", (unsigned long long)limit);
    }
    mlx_string_free(ver);
    mlx_device_free(gpu);
    if (n < 0) {
        return 0;
    }
    return (long long)n;
}

/* the same state as words, for a glass that reads and never parses:
 * 0 linked 1 metal available 2 gpu available 3 device (1 gpu, 0 cpu) 4 version major 5 minor 6 patch
 * 7 ops 8 dispatches 9 last error present 10 last error length 11 reserved. Answers the word count. */
long long fk_mlx_live_external(long long *out) {
    if (out == 0) {
        return 0;
    }
    int k = 0;
    while (k < 16) { out[k] = 0; k = k + 1; }
    bool metal = 0;
    mlx_metal_is_available(&metal);
    mlx_device gpu = mlx_device_new_type(MLX_GPU, 0);
    bool gpu_ok = 0;
    mlx_device_is_available(&gpu_ok, gpu);
    mlx_string ver = mlx_string_new();
    mlx_version(&ver);
    const char *vs = mlx_string_data(ver);
    long long part[3] = {0, 0, 0};
    int pi = 0;
    while (vs != 0 && *vs != 0 && pi < 3) {
        if (*vs >= '0' && *vs <= '9') { part[pi] = part[pi] * 10 + (*vs - '0'); }
        else if (*vs == '.') { pi = pi + 1; }
        else { break; }
        vs = vs + 1;
    }
    out[0] = 1;
    out[1] = metal ? 1 : 0;
    out[2] = gpu_ok ? 1 : 0;
    out[3] = gpu_ok ? 1 : 0;
    out[4] = part[0]; out[5] = part[1]; out[6] = part[2];
    out[7] = 23;
    out[8] = fk_mlx_dispatch;
    out[9] = strcmp(fk_mlx_err, "none") == 0 ? 0 : 1;
    out[10] = (long long)strlen(fk_mlx_err);
    out[11] = 0;
    /* the MLX allocator ledger as words, beside the status text */
    size_t lactive = 0, lpeak = 0, lcached = 0, llimit = 0;
    out[12] = mlx_get_active_memory(&lactive) == 0 ? (long long)lactive : -1;
    out[13] = mlx_get_peak_memory(&lpeak) == 0 ? (long long)lpeak : -1;
    out[14] = mlx_get_cache_memory(&lcached) == 0 ? (long long)lcached : -1;
    out[15] = mlx_get_memory_limit(&llimit) == 0 ? (long long)llimit : -1;
    mlx_string_free(ver);
    mlx_device_free(gpu);
    return 16;
}
