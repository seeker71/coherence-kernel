/* fk-mlx-carrier.c — MLX as an organ of THIS fkwu, not a second binary.
 *
 * STANDING PREFERENCE, 2026-09-09: NATIVE METAL IS PREFERRED OVER THIS LANE
 * WHEREVER IT CAN DO THE WORK. Urs's word, and it is a direction for new work,
 * not a verdict on what is here: the doors and recipes below were repaired that
 * same day because they were red and lying, and an honest organ is worth more
 * than a quietly broken one whichever lane it sits in. But this carrier borrows
 * a library's kernels, and `form/native/metal/` writes the body's own — the
 * quantized tiers below (q8/q4k/q6k) dequantize what q6k-msl.fk and q8-0-msl.fk
 * already dequantize in kernels this body emits and can read back. Two paths to
 * one meaning is the parallel path the minimum law exists to prevent, one level
 * up from the op table. So: heal MLX when it is wrong, and REACH FOR METAL when
 * the work is new.
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
 *   l2norm softmax scale axpy recip square min centre
 *   pow mod where clamp rope-pair attn
 *
 * THIS LIST WAS AN OVERCLAIM FOR TWO WEEKS. From 2026-08-25 it named gelu,
 * layernorm, scale, axpy, shift, select, clamp, rope-pair and attn — NINE ops —
 * and mlx-derived.fk contained none of them. An overclaim in a header is not a
 * lie that fails, it is a lie that CLOSES THE QUESTION: a reader checking whether
 * this carrier is minimal reads the list, sees the op accounted for, and stops.
 * All nine are written and pinned as of 2026-09-09 (mlx-derived-band 1073741823,
 * up from 16777215). `shift` and `select` are gone from the list rather than
 * written: select IS `where`, and a shift by k IS scale by 2^k, so adding rows
 * for them would break the same law from the Form side.
 *
 * all of those are Form-emitted graphs over the twenty-seven forms below.
 * `sub` used to live here and was retired on 2026-08-24 to prove the law cuts
 * both ways — a carrier row is not kept because it is convenient. The law cuts
 * a third way too, and that one took longest to see: a row can be lost without
 * anyone deciding to lose it. `tf32` passed the law, was written in #470, and
 * went out in the 2026-08-25 consolidation as collateral — restored 2026-09-09
 * only because someone asked why its band was still red.
 *
 * THE TWENTY-SEVEN, and why each is irreducible:
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
 *   q8 / q4k / q6k <path> <off> <r> <c>    — the same door at the quantized
 *                                            tiers, dequantizing on the way in.
 *                                            f32 tensors are a GGUF's norms; the
 *                                            weight lives here. Removed by the
 *                                            same 2026-08-25 consolidation and
 *                                            restored 2026-09-09 — mlx-q8-band
 *                                            and mlx-kquant-band had been
 *                                            reading 49/63 and 53/63 throughout
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

/* MLX'S DEFAULT ERROR HANDLER ABORTS. A Form program asking for a shape that has
 * no product — a (1,3) matmul (2,2) — prints one line and takes the WHOLE PROCESS
 * with it, rc 255, which is the opposite of every other door in this body: an
 * uncovered shape is DECLINED and the caller walks. So the carrier owns the
 * handler: the message is kept where mlx_status can speak it, and control comes
 * back. A refusal has to be survivable or it is not a refusal.
 *
 * WRITTEN ONCE AND LOST ONCE. This was here before 2026-08-25 with that same
 * paragraph, and the consolidation that cut this file from 942 lines to 195 took
 * it out along with the five file doors. From that day until 2026-09-09 any cell
 * that handed MLX a bad shape killed fkwu outright — and in a pipeline the 255
 * launders to 0, so it could kill the body and read as success. Nothing found it
 * because the bands that would have hit a shape error were themselves failing one
 * token earlier on `mRxC`, a literal the same commit retired: two wounds in a row
 * where the first hid the second. That is the ordinary way a fatal path stays
 * unnoticed — not that nobody looked, but that nobody could REACH it. */
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
            if (!fk_mlx_failed) { fk_mlx_seterr("dup failed"); }
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
        /* MLX already spoke through the handler; do not paper over its words with ours. */
        if (!fk_mlx_failed) { fk_mlx_seterr("mlx op failed"); }
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

static float fk_mlx_f16(unsigned short h) {
    int sign = (h >> 15) & 1;
    int exp = (h >> 10) & 0x1f;
    int mant = h & 0x3ff;
    float v;
    if (exp == 0) {
        v = (float)mant * 5.9604644775390625e-8f;   /* 2^-24, the subnormal step */
    } else if (exp == 31) {
        v = 0.0f;                                   /* no finite value to carry */
    } else {
        float m = 1.0f + (float)mant / 1024.0f;
        int e = exp - 15;
        float scale = 1.0f;
        int k = 0;
        if (e > 0) {
            while (k < e) { scale = scale * 2.0f; k = k + 1; }
        } else {
            while (k > e) { scale = scale * 0.5f; k = k - 1; }
        }
        v = m * scale;
    }
    return sign ? -v : v;
}

static int fk_mlx_q4k_scale(const unsigned char *sc, int j) {
    if (j < 4) {
        return sc[j] & 63;
    }
    return (sc[j + 4] & 15) | ((sc[j - 4] >> 6) << 4);
}

static int fk_mlx_q4k_minv(const unsigned char *sc, int j) {
    if (j < 4) {
        return sc[j + 4] & 63;
    }
    return (sc[j + 4] >> 4) | ((sc[j] >> 6) << 4);
}


/* q8 / q4k / q6k <path> <off> <r> <c> — THE QUANTIZED TIERS, where the weight lives.
 *
 * `tf32` above reads the f32 tensors, which in a real GGUF are the norms — a few thousand floats out
 * of billions. Everything that matters by volume is quantized, so a lane that can only reach f32 can
 * read a model's punctuation and not its words. These three dequantize on the way in: a Q8_0 block is
 * 34 bytes (an f16 scale and 32 int8 weights), Q4_K and Q6_K are the 144- and 210-byte superblocks
 * whose arithmetic q6k-dequant.fk carries four-way, and the reference for all three is the body's own
 * transcription, not a library's.
 *
 * They are file doors, so they earn their rows the same way tf32 does — no graph over the other
 * tokens names a byte offset — and they were removed by the same 2026-08-25 consolidation, for the
 * same reason: collateral, not law. mlx-q8-band and mlx-kquant-band have been reading 49/63 and 53/63
 * ever since, which is the shape a partial score always has (row 1385 `namedaway`): high enough to
 * look like a rough edge, never low enough to get opened.
 *
 * Restored under their ORIGINAL names, not renamed like tf32, because nothing in the current
 * vocabulary collides with them — and because two bands written against the old carrier then go green
 * UNTOUCHED, which is a stronger proof the restoration is faithful than any edit I could make to them.
 *
 * A SHORT READ IS A REFUSAL here too, and so is a shape that is not a whole number of blocks: a
 * partial superblock has no honest reading, and rounding one would hand the GPU a tensor whose tail is
 * whatever malloc last held. */
static int fk_mlx_push_q8(const char **p, const char *end, mlx_array *st, int *sp) {
    int rc = 0;
    char path[512];
    char tok[64];
    long long off = 0;
    long long r = 0;
    long long c = 0;
    long long n = 0;
    long long blocks = 0;
    unsigned char *raw = 0;
    float *data = 0;
    FILE *f = 0;
    size_t got = 0;
    long long bi = 0;
    if (!fk_mlx_tok(p, end, path, 512)) {
        fk_mlx_seterr("q8 needs a path");
        return -1;
    }
    if (!fk_mlx_tok(p, end, tok, 64) || !fk_mlx_num(tok)) {
        fk_mlx_seterr("q8 needs a byte offset");
        return -1;
    }
    off = atoll(tok);
    if (!fk_mlx_tok(p, end, tok, 64) || !fk_mlx_num(tok)) {
        fk_mlx_seterr("q8 needs rows");
        return -1;
    }
    r = atoll(tok);
    if (!fk_mlx_tok(p, end, tok, 64) || !fk_mlx_num(tok)) {
        fk_mlx_seterr("q8 needs cols");
        return -1;
    }
    c = atoll(tok);
    n = r * c;
    if (r < 1 || c < 1 || n > FK_MLX_TENSOR_CAP || off < 0) {
        fk_mlx_seterr("q8 shape out of range");
        return -1;
    }
    if ((n % 32) != 0) {
        /* A Q8_0 block is 32 weights. A shape that is not a whole number of
         * blocks has no honest reading, so it is refused rather than rounded. */
        fk_mlx_seterr("q8 shape is not a whole number of 32-weight blocks");
        return -1;
    }
    blocks = n / 32;
    f = fopen(path, "rb");
    if (f == 0) {
        fk_mlx_seterr("q8 cannot open path");
        return -1;
    }
    raw = (unsigned char *)malloc((size_t)blocks * 34);
    data = (float *)malloc((size_t)n * sizeof(float));
    if (raw == 0 || data == 0) {
        free(raw);
        free(data);
        fclose(f);
        fk_mlx_seterr("q8 out of memory");
        return -1;
    }
    if (fseek(f, (long)off, SEEK_SET) != 0) {
        free(raw);
        free(data);
        fclose(f);
        fk_mlx_seterr("q8 cannot seek to offset");
        return -1;
    }
    got = fread(raw, 1, (size_t)blocks * 34, f);
    fclose(f);
    if (got != (size_t)blocks * 34) {
        free(raw);
        free(data);
        fk_mlx_seterr("q8 file is shorter than the shape asked for");
        return -1;
    }
    while (bi < blocks) {
        const unsigned char *b = raw + bi * 34;
        unsigned short hb = (unsigned short)(b[0] | (b[1] << 8));
        float d = fk_mlx_f16(hb);
        int j = 0;
        while (j < 32) {
            signed char q = (signed char)b[2 + j];
            data[bi * 32 + j] = d * (float)q;
            j = j + 1;
        }
        bi = bi + 1;
    }
    free(raw);
    {
        int shape[2];
        shape[0] = (int)r;
        shape[1] = (int)c;
        rc = fk_mlx_push(mlx_array_new_data(data, shape, 2, MLX_FLOAT32), st, sp);
    }
    free(data);
    return rc;
}

static int fk_mlx_push_q4k(const char **p, const char *end, mlx_array *st, int *sp) {
    int rc = 0;
    char path[512];
    char tok[64];
    long long off = 0, r = 0, c = 0, n = 0, blocks = 0, bi = 0;
    unsigned char *raw = 0;
    float *data = 0;
    FILE *f = 0;
    size_t got = 0;
    if (!fk_mlx_tok(p, end, path, 512)) {
        fk_mlx_seterr("q4k needs a path");
        return -1;
    }
    if (!fk_mlx_tok(p, end, tok, 64) || !fk_mlx_num(tok)) {
        fk_mlx_seterr("q4k needs a byte offset");
        return -1;
    }
    off = atoll(tok);
    if (!fk_mlx_tok(p, end, tok, 64) || !fk_mlx_num(tok)) {
        fk_mlx_seterr("q4k needs rows");
        return -1;
    }
    r = atoll(tok);
    if (!fk_mlx_tok(p, end, tok, 64) || !fk_mlx_num(tok)) {
        fk_mlx_seterr("q4k needs cols");
        return -1;
    }
    c = atoll(tok);
    n = r * c;
    if (r < 1 || c < 1 || n > FK_MLX_TENSOR_CAP || off < 0) {
        fk_mlx_seterr("q4k shape out of range");
        return -1;
    }
    if ((n % 256) != 0) {
        fk_mlx_seterr("q4k shape is not a whole number of 256-weight superblocks");
        return -1;
    }
    blocks = n / 256;
    f = fopen(path, "rb");
    if (f == 0) {
        fk_mlx_seterr("q4k cannot open path");
        return -1;
    }
    raw = (unsigned char *)malloc((size_t)blocks * 144);
    data = (float *)malloc((size_t)n * sizeof(float));
    if (raw == 0 || data == 0) {
        free(raw); free(data); fclose(f);
        fk_mlx_seterr("q4k out of memory");
        return -1;
    }
    if (fseek(f, (long)off, SEEK_SET) != 0) {
        free(raw); free(data); fclose(f);
        fk_mlx_seterr("q4k cannot seek to offset");
        return -1;
    }
    got = fread(raw, 1, (size_t)blocks * 144, f);
    fclose(f);
    if (got != (size_t)blocks * 144) {
        free(raw); free(data);
        fk_mlx_seterr("q4k file is shorter than the shape asked for");
        return -1;
    }
    while (bi < blocks) {
        const unsigned char *b = raw + bi * 144;
        float d = fk_mlx_f16((unsigned short)(b[0] | (b[1] << 8)));
        float dmin = fk_mlx_f16((unsigned short)(b[2] | (b[3] << 8)));
        const unsigned char *sc = b + 4;
        const unsigned char *qs = b + 16;
        int i = 0;
        while (i < 256) {
            int chunk = i / 64;
            int within = i - chunk * 64;
            int hf = within / 32;
            int l = within - hf * 32;
            int sidx = 2 * chunk + hf;
            int qbyte = qs[chunk * 32 + l];
            int nib = (hf == 0) ? (qbyte & 15) : (qbyte >> 4);
            int scv = fk_mlx_q4k_scale(sc, sidx);
            int mnv = fk_mlx_q4k_minv(sc, sidx);
            data[bi * 256 + i] = (d * (float)scv) * (float)nib - (dmin * (float)mnv);
            i = i + 1;
        }
        bi = bi + 1;
    }
    free(raw);
    {
        int shape[2];
        shape[0] = (int)r;
        shape[1] = (int)c;
        rc = fk_mlx_push(mlx_array_new_data(data, shape, 2, MLX_FLOAT32), st, sp);
    }
    free(data);
    return rc;
}

static int fk_mlx_push_q6k(const char **p, const char *end, mlx_array *st, int *sp) {
    int rc = 0;
    char path[512];
    char tok[64];
    long long off = 0, r = 0, c = 0, n = 0, blocks = 0, bi = 0;
    unsigned char *raw = 0;
    float *data = 0;
    FILE *f = 0;
    size_t got = 0;
    if (!fk_mlx_tok(p, end, path, 512)) {
        fk_mlx_seterr("q6k needs a path");
        return -1;
    }
    if (!fk_mlx_tok(p, end, tok, 64) || !fk_mlx_num(tok)) {
        fk_mlx_seterr("q6k needs a byte offset");
        return -1;
    }
    off = atoll(tok);
    if (!fk_mlx_tok(p, end, tok, 64) || !fk_mlx_num(tok)) {
        fk_mlx_seterr("q6k needs rows");
        return -1;
    }
    r = atoll(tok);
    if (!fk_mlx_tok(p, end, tok, 64) || !fk_mlx_num(tok)) {
        fk_mlx_seterr("q6k needs cols");
        return -1;
    }
    c = atoll(tok);
    n = r * c;
    if (r < 1 || c < 1 || n > FK_MLX_TENSOR_CAP || off < 0) {
        fk_mlx_seterr("q6k shape out of range");
        return -1;
    }
    if ((n % 256) != 0) {
        fk_mlx_seterr("q6k shape is not a whole number of 256-weight superblocks");
        return -1;
    }
    blocks = n / 256;
    f = fopen(path, "rb");
    if (f == 0) {
        fk_mlx_seterr("q6k cannot open path");
        return -1;
    }
    raw = (unsigned char *)malloc((size_t)blocks * 210);
    data = (float *)malloc((size_t)n * sizeof(float));
    if (raw == 0 || data == 0) {
        free(raw); free(data); fclose(f);
        fk_mlx_seterr("q6k out of memory");
        return -1;
    }
    if (fseek(f, (long)off, SEEK_SET) != 0) {
        free(raw); free(data); fclose(f);
        fk_mlx_seterr("q6k cannot seek to offset");
        return -1;
    }
    got = fread(raw, 1, (size_t)blocks * 210, f);
    fclose(f);
    if (got != (size_t)blocks * 210) {
        free(raw); free(data);
        fk_mlx_seterr("q6k file is shorter than the shape asked for");
        return -1;
    }
    while (bi < blocks) {
        const unsigned char *ql = raw + bi * 210;
        const unsigned char *qh = ql + 128;
        const signed char *scales = (const signed char *)(ql + 192);
        float d = fk_mlx_f16((unsigned short)(ql[208] | (ql[209] << 8)));
        int i = 0;
        while (i < 256) {
            int h = i / 128;
            int wi = i - h * 128;
            int l = wi % 32;
            int g = wi / 32;
            int is_ = l / 16;
            int qlidx = h * 64 + l + (g % 2) * 32;
            int nib = (g / 2 == 0) ? (ql[qlidx] & 15) : (ql[qlidx] >> 4);
            int hi = (qh[h * 32 + l] >> (2 * g)) & 3;
            int q = (nib | (hi << 4)) - 32;
            int scv = (int)scales[h * 8 + is_ + 2 * g];
            data[bi * 256 + i] = d * (float)scv * (float)q;
            i = i + 1;
        }
        bi = bi + 1;
    }
    free(raw);
    {
        int shape[2];
        shape[0] = (int)r;
        shape[1] = (int)c;
        rc = fk_mlx_push(mlx_array_new_data(data, shape, 2, MLX_FLOAT32), st, sp);
    }
    free(data);
    return rc;
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
        if (!fk_mlx_failed) { fk_mlx_seterr("reshape failed"); }
        return -1;
    }
    return fk_mlx_push(c, st, sp);
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
        } else if (strcmp(tok, "q8") == 0) {
            if (fk_mlx_push_q8(&p, end, st, &sp) != 0) {
                fail = 1;
            }
        } else if (strcmp(tok, "q4k") == 0) {
            if (fk_mlx_push_q4k(&p, end, st, &sp) != 0) {
                fail = 1;
            }
        } else if (strcmp(tok, "q6k") == 0) {
            if (fk_mlx_push_q6k(&p, end, st, &sp) != 0) {
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
        "mlx_ops=27\n"
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
