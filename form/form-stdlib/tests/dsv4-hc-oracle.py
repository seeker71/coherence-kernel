#!/usr/bin/env python3
# hc_oracle.py — INDEPENDENT fp64 transcription of DeepSeek-V4 hyper-connections
# from ds4-engine (MIT) ds4.c, written from the C control flow, NOT from the Form
# recipe. Used as the oracle for form/form-stdlib/tests/dsv4-hc-band.fk.
#
# Transcribed function-by-function:
#   ds4.c:6628  rms_norm_no_weight
#   ds4.c:6697  matvec_f16            (here: plain fp64 matvec, boundborrow: no f16)
#   ds4.c:9592  hc_split_sinkhorn_one
#   ds4.c:9673  hc_weighted_sum_one
#   ds4.c:9690  hc_pre_from_state_one_scratch
#   ds4.c:9764  hc_from_plain_embedding
#   ds4.c:9772  hc_post_one
#   ds4.c:13876 output_hc_head_one
#
# Everything is Python float (fp64). The C reference accumulates rms in double and
# rounds intermediates to f32; this oracle stays fp64 throughout, so agreement is
# expected at ~1e-9, the residual being f32-vs-f64 rounding, NOT the algebra.
import math

NEG_INF = -1.0e30
EPS_SINK = 1.0e-6   # the eps hc_split_sinkhorn_one is called with (ds4.c:9716)
RMS_EPS  = 1.0e-6   # DS4_RMS_EPS for this file (Stone 21: layer_norm_rms_epsilon=1e-06)
HC_EPS   = 1.0e-6   # DS4_DEFAULT_HC_EPS

def rms_norm_no_weight(x, eps=RMS_EPS):
    n = len(x)
    ss = sum(v*v for v in x)
    scale = 1.0 / math.sqrt(ss/n + eps)
    return [v*scale for v in x]

def matvec(rows, x):
    # rows: list of output-rows, each len(x). out[r] = dot(rows[r], x)
    return [sum(a*b for a, b in zip(row, x)) for row in rows]

def hc_split_sinkhorn_one(mix, scale, base, n_hc, iters, eps):
    pre_scale, post_scale, comb_scale = scale[0], scale[1], scale[2]
    out = [0.0]*(2*n_hc + n_hc*n_hc)
    # pre weights
    for i in range(n_hc):
        z = mix[i]*pre_scale + base[i]
        out[i] = 1.0/(1.0 + math.exp(-z)) + eps
    # post weights
    for i in range(n_hc):
        off = n_hc + i
        z = mix[off]*post_scale + base[off]
        out[off] = 2.0/(1.0 + math.exp(-z))
    # comb matrix, flat: c[src + dst*n_hc]
    c = [0.0]*(n_hc*n_hc)
    for dst in range(n_hc):
        row_max = NEG_INF
        for src in range(n_hc):
            idx = src + dst*n_hc
            off = 2*n_hc + idx
            v = mix[off]*comb_scale + base[off]
            c[idx] = v
            if v > row_max: row_max = v
        row_sum = 0.0
        for src in range(n_hc):
            idx = src + dst*n_hc
            v = math.exp(c[idx] - row_max)
            c[idx] = v
            row_sum += v
        inv = 1.0/row_sum
        for src in range(n_hc):
            idx = src + dst*n_hc
            c[idx] = c[idx]*inv + eps
    # first column normalization (fix src, sum over dst)
    for src in range(n_hc):
        s = sum(c[src + dst*n_hc] for dst in range(n_hc))
        inv = 1.0/(s + eps)
        for dst in range(n_hc):
            c[src + dst*n_hc] *= inv
    # iters-1 more double normalizations
    for _ in range(1, iters):
        for dst in range(n_hc):
            s = sum(c[src + dst*n_hc] for src in range(n_hc))
            inv = 1.0/(s + eps)
            for src in range(n_hc):
                c[src + dst*n_hc] *= inv
        for src in range(n_hc):
            s = sum(c[src + dst*n_hc] for dst in range(n_hc))
            inv = 1.0/(s + eps)
            for dst in range(n_hc):
                c[src + dst*n_hc] *= inv
    for i in range(n_hc*n_hc):
        out[2*n_hc + i] = c[i]
    return out

def hc_weighted_sum_one(x_hc, weights, n_embd, n_hc):
    out = [0.0]*n_embd
    for d in range(n_embd):
        acc = 0.0
        for h in range(n_hc):
            acc += x_hc[h*n_embd + d] * weights[h]
        out[d] = acc
    return out

def hc_from_plain_embedding(x, n_embd, n_hc):
    out = [0.0]*(n_embd*n_hc)
    for h in range(n_hc):
        for d in range(n_embd):
            out[h*n_embd + d] = x[d]
    return out

def hc_post_one(block_out, residual_hc, post, comb, n_embd, n_hc):
    out_hc = [0.0]*(n_embd*n_hc)
    for dst in range(n_hc):
        for d in range(n_embd):
            acc = block_out[d]*post[dst]
            for src in range(n_hc):
                acc += comb[dst + src*n_hc] * residual_hc[src*n_embd + d]
            out_hc[dst*n_embd + d] = acc
    return out_hc

def hc_pre_from_state_one(fn_rows, scale, base, residual_hc, n_hc, n_embd, iters):
    hc_dim = n_embd*n_hc
    flat = rms_norm_no_weight(residual_hc, RMS_EPS)
    mix = matvec(fn_rows, flat)              # len 2*n_hc + n_hc*n_hc
    split = hc_split_sinkhorn_one(mix, scale, base, n_hc, iters, EPS_SINK)
    out = hc_weighted_sum_one(residual_hc, split[:n_hc], n_embd, n_hc)
    post = split[n_hc:2*n_hc]
    comb = split[2*n_hc:]
    return out, post, comb

def output_hc_head_one(fn_rows, scale, base, inp_hc, n_hc, n_embd):
    hc_dim = n_embd*n_hc
    flat = rms_norm_no_weight(inp_hc, RMS_EPS)
    pre = matvec(fn_rows, flat)              # len n_hc
    w = [1.0/(1.0+math.exp(-(pre[i]*scale[0] + base[i]))) + HC_EPS for i in range(n_hc)]
    return hc_weighted_sum_one(inp_hc, w, n_embd, n_hc)

# ---------- deterministic invented inputs (defined ONCE here, copied to the band) ----------
def frange(vals):
    return "(list " + " ".join(fmt(v) for v in vals) + ")"
def fmt(v):
    return repr(float(v))
def flist2d(rows):
    return "(list " + " ".join(frange(r) for r in rows) + ")"

# ==== component tests at n_hc=4, n_embd=3 (real Sinkhorn width, tiny embd) ====
n_hc, n_embd, ITERS = 4, 3, 20
mix = [0.3, -0.7, 0.5, 0.1,            # pre (4)
       -0.2, 0.6, -0.4, 0.8,           # post (4)
       0.2, -0.5, 0.9, -0.1,           # comb dst0 (src0..3)
       -0.3, 0.7, 0.1, -0.6,           # comb dst1
       0.4, -0.2, -0.8, 0.5,           # comb dst2
       0.1, 0.3, -0.4, 0.6]            # comb dst3
scale = [1.3, 0.7, 1.1]
base  = [0.05, -0.1, 0.2, 0.0,
         0.1, -0.05, 0.15, -0.2,
         0.0, 0.1, -0.1, 0.05,
         -0.05, 0.2, 0.0, 0.1,
         0.15, -0.1, 0.05, 0.0,
         0.1, 0.0, -0.15, 0.2]
residual_hc = [ 0.5, -0.3, 0.9,
               -0.4, 0.7, -0.1,
                0.2, 0.1, -0.8,
                0.6, -0.5, 0.3]
block_out = [0.35, -0.25, 0.65]

split = hc_split_sinkhorn_one(mix, scale, base, n_hc, ITERS, EPS_SINK)
wsum  = hc_weighted_sum_one(residual_hc, split[:n_hc], n_embd, n_hc)
post_out = hc_post_one(block_out, residual_hc, split[n_hc:2*n_hc], split[2*n_hc:], n_embd, n_hc)

# ==== full pipeline at n_hc=2, n_embd=2 (small fn) ====
n_hc2, n_embd2 = 2, 2
# fn: (2*2 + 2*2)=8 output rows, each len n_hc2*n_embd2 = 4
fn2 = [[ 0.10, -0.20, 0.30, 0.15],
       [-0.25, 0.40, 0.05, -0.10],
       [ 0.35, 0.20, -0.15, 0.25],
       [-0.05, 0.30, 0.10, -0.20],
       [ 0.20, -0.35, 0.25, 0.05],
       [ 0.15, 0.10, -0.30, 0.40],
       [-0.20, 0.25, 0.15, -0.05],
       [ 0.30, -0.10, 0.20, 0.35]]
scale2 = [1.2, 0.9, 1.05]
base2  = [0.05, -0.1, 0.1, -0.05, 0.0, 0.15, -0.2, 0.1]
residual2 = [0.6, -0.4, 0.3, 0.8]   # 2 streams x 2 embd
out2, post2, comb2 = hc_pre_from_state_one(fn2, scale2, base2, residual2, n_hc2, n_embd2, ITERS)

# output head at n_hc=2, n_embd=2
fnh = [[0.2, -0.3, 0.4, 0.1],
       [-0.1, 0.25, 0.15, -0.2]]   # n_hc rows
scaleh = [1.1]
baseh  = [0.05, -0.1]
inph   = [0.5, -0.2, 0.7, 0.4]
headout = output_hc_head_one(fnh, scaleh, baseh, inph, n_hc2, n_embd2)

# comb 4x4 as list-of-rows [dst][src] for the doubly-stochastic checks
comb4 = split[2*n_hc:]
def col_sum(c, src, n): return sum(c[src + dst*n] for dst in range(n))
def row_sum(c, dst, n): return sum(c[src + dst*n] for src in range(n))

print("=== PASTE INTO BAND ===")
print(";; component n_hc=4 n_embd=3")
print("mix   ", frange(mix))
print("scale ", frange(scale))
print("base  ", frange(base))
print("resid ", frange(residual_hc))
print("blk   ", frange(block_out))
print("REF-split", frange(split))
print("REF-wsum ", frange(wsum))
print("REF-post ", frange(post_out))
print(";; pipeline n_hc=2 n_embd=2")
print("fn2   ", flist2d(fn2))
print("scale2", frange(scale2))
print("base2 ", frange(base2))
print("resid2", frange(residual2))
print("REF-out2 ", frange(out2))
print("REF-post2", frange(post2))
print("REF-comb2", frange(comb2))
print(";; output head")
print("fnh   ", flist2d(fnh))
print("scaleh", frange(scaleh))
print("baseh ", frange(baseh))
print("inph  ", frange(inph))
print("REF-head ", frange(headout))
print("=== diagnostics ===")
print("comb4 col sums (should be ~1):", [round(col_sum(comb4,s,4),12) for s in range(4)])
print("comb4 row sums (approx):     ", [round(row_sum(comb4,d,4),12) for d in range(4)])
# iters=1 vs iters=20 to show the count is load-bearing
split1 = hc_split_sinkhorn_one(mix, scale, base, n_hc, 1, EPS_SINK)
maxdiff = max(abs(a-b) for a,b in zip(split[2*n_hc:], split1[2*n_hc:]))
print("comb maxdiff iters20 vs iters1:", maxdiff)
