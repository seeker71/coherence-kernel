#!/usr/bin/env python3
# dsv4-mla-core-oracle.py — a RENTED ORACLE for the DeepSeek-V4-Flash MLA attention core.
#
# WHY IT EXISTS (twinblind, corpus row 868). Stone 35 proved the MLA PROJECTION surface at real dims by
# self-carve: the GPU result vs an independent CPU decode of the SAME bytes. That is a real falsifier there
# because a matvec and an RMSNorm are CANONICAL — one right answer, no recipe choice. The attention CORE is
# not canonical. Where the sink enters the softmax, how the trailing-64 RoPE splits, whether the heads are
# un-roped after attending, which output path is taken, whether the KV row is fp8-then-f16 rounded before it
# is attended to — these are CHOICES. A self-carve inherits the choice on both sides: it confirms GPU and
# CPU are bound and is BLIND to whether the choice is right. So the choosing half is proven against THIS —
# an INDEPENDENT fp64 transcription written from ds4.c's C control flow, not from the Form kernel or the
# Metal harness.
#
# PROVENANCE. Transcribed from ds4.c (/Users/ursmuff/models/ds4-engine/ds4.c, 64525 lines, MIT — the file's
# own general.license KV is "mit"). Anchors, each re-expressed here, never copied:
#     layer_forward_self_one              :13793   the attention half's whole order of operations
#     layer_attn_norm_one                  :9981   rms_norm_weight over attn_norm
#     layer_q_projection_normed_one       :10002   q_a -> q_a_norm -> q_b -> head_rms_norm_inplace
#     layer_kv_projection_normed_one      :10041   attn_kv -> kv_a_norm
#     head_rms_norm_inplace                :6646   per-head RMSNorm, NO weight, fp64 sumsq
#     rms_norm_weight                      :6637   fp64 sumsq, f32 scale, out = x*scale*w
#     rope_tail_layer_inplace             :10166   the per-layer freq_base / freq_scale / ext_factor choice
#     rope_tail_ext_inplace               :10102   the trailing-n_rot rotation, sin_sign for the inverse
#     ds4_expected_layer_compress_ratio    :1065   FLASH: il < 2 -> ratio 0 -> uncompressed RoPE
#     dsv4_fp8_kv_quantize_row_inplace_cpu :3211   the NOPE head is fp8-round-tripped in 64-wide groups
#     dsv4_e4m3fn_dequant_cpu              :3181   nearest-even E4M3FN round, 448 clamp
#     f16_round_inplace_cpu                :3162   then the WHOLE kv row is f16-round-tripped
#     layer_attention_rows_one            :10305   sink-aware softmax: the sink is in the DENOMINATOR only
#     layer_grouped_out_one               :10356   8 groups -> rank-1024 low -> attn_output_b -> n_embd
#     matvec_q8_0_grouped_worker           :7123   the grouped row indexing (row = group*rank + r)
#
# INDEPENDENCE, stated exactly. This oracle parses the GGUF header itself, finds the tensors by name in the
# file's own table, decodes F16 / F32 / MXFP8 (type 41) itself, and computes everything in Python fp64. It
# shares NO code, no buffer and no arithmetic with the Form band, the MSL kernels, or the Swift carrier. It
# reads only the same 85 GiB read-only file. The type-41 decode is the one thing it re-derives that the
# carrier also re-derives — but that decode is CANONICAL and was already self-carve-proven at real dims in
# Stones 33/34/35, so it is not the choosing surface this oracle exists to falsify.
#
# WHAT IT IS NOT. It is not an engine: ds4/llama.cpp/ollama all REFUSE this file's types 40/41, so the WHOLE
# forward remains unfalsifiable against a reference (selfgauge). This oracle falsifies exactly the attention
# core's recipe at layer 0, at real dims, for a given token and a given position.
#
# Run:  dsv4-mla-core-oracle.py <gguf> <token> <pos> [layer]
# Emits a flat keyed stream on stdout (ORA <key> <index> <value>) plus summary lines the harness parses.

import sys, os, math, struct, mmap

# ------------------------------------------------------------------ GGUF header, parsed independently
GGUF_MAGIC = b"GGUF"

class Gguf:
    def __init__(self, path):
        self.f = open(path, "rb")
        self.mm = mmap.mmap(self.f.fileno(), 0, prot=mmap.PROT_READ)
        m = self.mm
        if m[0:4] != GGUF_MAGIC:
            raise SystemExit("not a GGUF")
        self.version = struct.unpack_from("<I", m, 4)[0]
        n_tensors = struct.unpack_from("<Q", m, 8)[0]
        n_kv = struct.unpack_from("<Q", m, 16)[0]
        off = 24
        self.kv = {}
        for _ in range(n_kv):
            key, off = self._rd_str(off)
            vtype = struct.unpack_from("<I", m, off)[0]; off += 4
            val, off = self._rd_val(vtype, off)
            self.kv[key] = val
        self.tensors = {}
        for _ in range(n_tensors):
            name, off = self._rd_str(off)
            nd = struct.unpack_from("<I", m, off)[0]; off += 4
            dims = list(struct.unpack_from("<%dQ" % nd, m, off)); off += 8 * nd
            ttype = struct.unpack_from("<I", m, off)[0]; off += 4
            toff = struct.unpack_from("<Q", m, off)[0]; off += 8
            self.tensors[name] = (ttype, dims, toff)
        align = self.kv.get("general.alignment", 32)
        self.data_start = (off + align - 1) // align * align

    def _rd_str(self, off):
        n = struct.unpack_from("<Q", self.mm, off)[0]
        s = self.mm[off + 8: off + 8 + n].decode("utf-8", "replace")
        return s, off + 8 + n

    def _rd_val(self, vt, off):
        m = self.mm
        F = {0: ("<B", 1), 1: ("<b", 1), 2: ("<H", 2), 3: ("<h", 2), 4: ("<I", 4), 5: ("<i", 4),
             6: ("<f", 4), 7: ("<B", 1), 10: ("<Q", 8), 11: ("<q", 8), 12: ("<d", 8)}
        if vt in F:
            fmt, sz = F[vt]
            return struct.unpack_from(fmt, m, off)[0], off + sz
        if vt == 8:
            return self._rd_str(off)
        if vt == 9:
            et = struct.unpack_from("<I", m, off)[0]; off += 4
            n = struct.unpack_from("<Q", m, off)[0]; off += 8
            out = []
            for _ in range(n):
                v, off = self._rd_val(et, off)
                out.append(v)
            return out, off
        raise SystemExit("unknown KV type %d" % vt)

    def abs_off(self, name):
        return self.data_start + self.tensors[name][2]

    def dims(self, name):
        return self.tensors[name][1]

    def ttype(self, name):
        return self.tensors[name][0]

# ------------------------------------------------------------------ the decodes, re-derived here
def f16_to_f64(h):
    s = -1.0 if (h >> 15) else 1.0
    e = (h >> 10) & 0x1F
    f = h & 0x3FF
    if e == 0:
        return s * (f / 1024.0) * (2.0 ** -14) if f else s * 0.0
    if e == 0x1F:
        return s * float("inf") if f == 0 else float("nan")
    return s * (1.0 + f / 1024.0) * (2.0 ** (e - 15))

# E4M3 payload, exactly ds4.c's dsv4_e4m3fn_value_cpu :3166 read as a byte-indexed table.
def _e4m3_table():
    exp_scale = [0.0, 0.015625, 0.03125, 0.0625, 0.125, 0.25, 0.5, 1.0,
                 2.0, 4.0, 8.0, 16.0, 32.0, 64.0, 128.0, 256.0]
    t = []
    for b in range(256):
        sgn = -1.0 if (b & 0x80) else 1.0
        i = b & 0x7F
        e = (i >> 3) & 0x0F
        mant = i & 0x07
        mag = mant * 0.001953125 if e == 0 else (1.0 + mant * 0.125) * exp_scale[e]
        t.append(sgn * mag)
    return t

E4M3 = _e4m3_table()
E8M0 = [2.0 ** (e - 127) for e in range(256)]

# ------------------------------------------------------------------ tensor readers
def read_f32(g, name, n):
    o = g.abs_off(name)
    return list(struct.unpack_from("<%df" % n, g.mm, o))

def read_f16_row(g, name, row, n):
    o = g.abs_off(name) + row * n * 2
    return [f16_to_f64(h) for h in struct.unpack_from("<%dH" % n, g.mm, o)]

# MXFP8 (type 41) is PLANE-SPLIT: nel payload bytes, then nel/32 E8M0 scale bytes.
# y[r] = sum_g scale[r*cols/32 + g] * sum_{m<32} E4M3[pay[r*cols + g*32 + m]] * x[g*32 + m]
def mx8_matvec(g, name, x, rows, cols):
    base = g.abs_off(name)
    nel = rows * cols
    mm = g.mm
    pay = mm[base: base + nel]
    sca = mm[base + nel: base + nel + nel // 32]
    tget = E4M3.__getitem__
    ngrp = cols // 32
    xs = [x[i * 32:(i + 1) * 32] for i in range(ngrp)]
    out = [0.0] * rows
    mul = float.__mul__
    for r in range(rows):
        rp = r * cols
        g0 = rp // 32
        acc = 0.0
        for gi in range(ngrp):
            p = rp + gi * 32
            acc += E8M0[sca[g0 + gi]] * sum(map(mul, map(tget, pay[p:p + 32]), xs[gi]))
        out[r] = acc
    return out

# ------------------------------------------------------------------ ds4.c primitives, re-expressed
def rms_norm_weight(x, w, eps):                        # ds4.c:6637
    ss = 0.0
    for v in x:
        ss += v * v
    scale = 1.0 / math.sqrt(ss / len(x) + eps)
    return [x[i] * scale * w[i] for i in range(len(x))]

def head_rms_norm(x, n_head, head_dim, eps):           # ds4.c:6646 — NO weight
    for h in range(n_head):
        b = h * head_dim
        ss = 0.0
        for i in range(b, b + head_dim):
            ss += x[i] * x[i]
        s = 1.0 / math.sqrt(ss / head_dim + eps)
        for i in range(b, b + head_dim):
            x[i] *= s
    return x

def rope_yarn_ramp(low, high, i0):                     # ds4.c:10086
    y = (float(i0 // 2) - low) / max(0.001, high - low)
    return 1.0 - min(1.0, max(0.0, y))


def rope_yarn_corr_dim(n_dims, n_ctx_orig, beta, base):  # ds4.c:10091
    return float(n_dims) * math.log(float(n_ctx_orig) / (beta * 2.0 * math.pi)) / (2.0 * math.log(base))


def rope_yarn_corr_dims(n_dims, n_ctx_orig, freq_base, beta_fast, beta_slow):   # ds4.c:10095
    start = math.floor(rope_yarn_corr_dim(n_dims, n_ctx_orig, beta_fast, freq_base))
    end = math.ceil(rope_yarn_corr_dim(n_dims, n_ctx_orig, beta_slow, freq_base))
    return (max(0.0, start), min(float(n_dims - 1), end))


def rope_pair_factors(n_rot, pos, n_ctx_orig, freq_base, freq_scale, ext_factor,
                      attn_factor, beta_fast, beta_slow, inverse):
    """ds4.c:10102 rope_tail_ext_inplace's per-PAIR (cos*mscale, sin_sign*sin*mscale).

    The whole YaRN body reduces to a per-pair angle and a per-pair magnitude, and both are the same for
    every head, so they are computed once. On a COMPRESSED layer ext_factor is 1 and

        theta = theta_interp*(1 - ramp) + theta_extrap*ramp
              = theta_extrap * (freq_scale*(1 - ramp) + ramp)

    -- a per-pair SCALE of the same theta_extrap. And mscale is attn_factor*(1 + 0.1*ln(1/freq_scale))
    where rope_tail_layer_inplace (:10175) set attn_factor to the reciprocal of exactly that, so the
    magnitude cancels back to 1 (to within one ulp of the reciprocal round-trip). That is why a
    compressed layer needs NO new kernel: it is the same rotation with a different per-pair freq.
    """
    theta_scale = freq_base ** (-2.0 / n_rot)
    sin_sign = -1.0 if inverse else 1.0
    corr = (0.0, 0.0)
    if ext_factor != 0.0:
        corr = rope_yarn_corr_dims(n_rot, n_ctx_orig, freq_base, beta_fast, beta_slow)
    out = []
    theta_extrap = float(pos)
    for i in range(0, n_rot, 2):
        theta_interp = freq_scale * theta_extrap
        theta = theta_interp
        mscale = attn_factor
        if ext_factor != 0.0:
            ramp_mix = rope_yarn_ramp(corr[0], corr[1], i) * ext_factor
            theta = theta_interp * (1.0 - ramp_mix) + theta_extrap * ramp_mix
            mscale *= 1.0 + 0.1 * math.log(1.0 / freq_scale)
        out.append((math.cos(theta) * mscale, sin_sign * math.sin(theta) * mscale))
        theta_extrap *= theta_scale
    return out


def rope_unit_freqs(n_rot, n_ctx_orig, freq_base, freq_scale, ext_factor, beta_fast, beta_slow):
    """The per-pair FREQ the GPU kernel needs: theta = pos * freqs[k]. Host-side, once per layer."""
    theta_scale = freq_base ** (-2.0 / n_rot)
    corr = (0.0, 0.0)
    if ext_factor != 0.0:
        corr = rope_yarn_corr_dims(n_rot, n_ctx_orig, freq_base, beta_fast, beta_slow)
    out = []
    f = 1.0
    for i in range(0, n_rot, 2):
        if ext_factor != 0.0:
            r = rope_yarn_ramp(corr[0], corr[1], i) * ext_factor
            out.append(f * (freq_scale * (1.0 - r) + r))
        else:
            out.append(f * freq_scale)
        f *= theta_scale
    return out


def rope_apply(x, n_head, head_dim, n_rot, factors):
    n_nope = head_dim - n_rot
    for h in range(n_head):
        tail = h * head_dim + n_nope
        for k in range(len(factors)):
            c, s = factors[k]
            i = tail + 2 * k
            x0 = x[i]
            x1 = x[i + 1]
            x[i] = x0 * c - x1 * s
            x[i + 1] = x0 * s + x1 * c
    return x


def rope_layer_params(K, il):                          # ds4.c:10155/:10162/:10166
    ratios = K.get("deepseek4.attention.compress_ratios", [])
    ratio = ratios[il] if il < len(ratios) else 0
    compressed = ratio != 0
    scale_factor = K.get("deepseek4.rope.scaling.factor", 0.0)
    cbase = K.get("deepseek4.attention.compress_rope_freq_base", 0.0)
    freq_base = cbase if (compressed and cbase > 0.0) else K["deepseek4.rope.freq_base"]
    freq_scale = 1.0 if (not compressed or scale_factor <= 0.0) else 1.0 / scale_factor
    ext_factor = 1.0 if (compressed and scale_factor > 1.0) else 0.0
    attn_factor = 1.0
    if ext_factor != 0.0 and freq_scale > 0.0:
        attn_factor /= 1.0 + 0.1 * math.log(1.0 / freq_scale)
    return {
        "ratio": ratio, "freq_base": freq_base, "freq_scale": freq_scale,
        "ext_factor": ext_factor, "attn_factor": attn_factor,
        "n_ctx_orig": K.get("deepseek4.rope.scaling.original_context_length", 0) if compressed else 0,
        "beta_fast": K.get("deepseek4.rope.scaling.yarn_beta_fast", 32.0),
        "beta_slow": K.get("deepseek4.rope.scaling.yarn_beta_slow", 1.0),
    }


def rope_tail(x, n_head, head_dim, n_rot, pos, freq_base, freq_scale, inverse):   # ds4.c:10102
    # The UNCOMPRESSED call (ds4.c:1065 -> ratio 0 for il < 2): ext_factor 0, attn_factor 1, no ramp and
    # no magnitude scale. Only the TRAILING n_rot of each head rotate; the leading head_dim - n_rot (the
    # NOPE part) are untouched. Expressed through the general body so there is ONE rotation here.
    f = rope_pair_factors(n_rot, pos, 0, freq_base, freq_scale, 0.0, 1.0, 32.0, 1.0, inverse)
    return rope_apply(x, n_head, head_dim, n_rot, f)

def f32_round(v):
    return struct.unpack("<f", struct.pack("<f", v))[0]

def f16_round(v):
    try:
        return f16_to_f64(struct.unpack("<H", struct.pack("<e", v))[0])
    except (OverflowError, struct.error):
        return math.copysign(65504.0, v)

_E4M3_POS = sorted(set(abs(v) for v in E4M3 if not math.isnan(v)))

def e4m3fn_dequant(v):                                  # ds4.c:3181 — nearest, ties handled by the C's rule
    sign = -1.0 if v < 0.0 else 1.0
    ax = min(abs(v), 448.0)
    # the C binary-searches its own 0..126 value ladder; the ladder is monotone, so a nearest lookup
    # over the same ladder is the same answer.
    lo, hi = 0, len(_E4M3_POS) - 1
    while lo < hi:
        mid = (lo + hi + 1) >> 1
        if _E4M3_POS[mid] <= ax:
            lo = mid
        else:
            hi = mid - 1
    best = lo
    if best < len(_E4M3_POS) - 1:
        if abs(ax - _E4M3_POS[best + 1]) < abs(ax - _E4M3_POS[best]):
            best += 1
    return sign * _E4M3_POS[best]

def fp8_kv_quantize_row(x, head_dim, n_rot):            # ds4.c:3211 — the NOPE part only, 64-wide groups
    n_nope = head_dim - n_rot
    off = 0
    while off < n_nope:
        amax = 0.0
        for i in range(off, off + 64):
            a = abs(x[i])
            if a > amax:
                amax = a
        if amax < 1.0e-4:
            amax = 1.0e-4
        scale = 2.0 ** math.ceil(math.log2(amax / 448.0))
        for i in range(off, off + 64):
            v = x[i] / scale
            v = 448.0 if v > 448.0 else (-448.0 if v < -448.0 else v)
            x[i] = e4m3fn_dequant(v) * scale
        off += 64
    return x

def attention_rows_one(q, kv_rows, sinks, n_head, head_dim):   # ds4.c:10305
    # The learned per-head sink logit sits in the softmax DENOMINATOR and contributes NO value vector.
    kq = 1.0 / math.sqrt(head_dim)
    n_kv = len(kv_rows)
    out = [0.0] * (n_head * head_dim)
    for h in range(n_head):
        qb = h * head_dim
        qh = q[qb: qb + head_dim]
        scores = []
        mx = sinks[h]
        for r in range(n_kv):
            kv = kv_rows[r]
            s = 0.0
            for i in range(head_dim):
                s += qh[i] * kv[i]
            s *= kq
            scores.append(s)
            if s > mx:
                mx = s
        denom = math.exp(sinks[h] - mx)
        acc = [0.0] * head_dim
        for r in range(n_kv):
            w = math.exp(scores[r] - mx)
            denom += w
            kv = kv_rows[r]
            for i in range(head_dim):
                acc[i] += kv[i] * w
        inv = 1.0 / denom
        for i in range(head_dim):
            out[qb + i] = acc[i] * inv
    return out

# ------------------------------------------------------------------ the HYPER-CONNECTION half (ds4.c)
# STONE 36 STAGE 4. A complete layer is not the attention block: it is HC-pre -> block -> HC-post, twice.
# ds4.c anchors, re-expressed here the same way as the core above:
#   hc_from_plain_embedding  :9764   the plain embedding is BROADCAST to all n_hc streams
#   hc_pre_from_state_one    :9690   rms(no weight) over the whole n_hc*n_embd state -> f16 matvec ->
#                                    sinkhorn split -> weighted sum of the streams
#   hc_split_sinkhorn_one    :9592   pre = sigmoid+eps, post = 2*sigmoid, comb = row-softmax then 20
#                                    alternating column/row normalisations
#   hc_weighted_sum_one      :9673
#   hc_post_one              :9772   out[dst][d] = block_out[d]*post[dst] + sum_src comb[dst+src*n_hc]*resid[src][d]
def matvec_f16(g, name, x, rows, cols):
    base = g.abs_off(name)
    out = [0.0] * rows
    for r in range(rows):
        o = base + r * cols * 2
        hs = struct.unpack_from("<%dH" % cols, g.mm, o)
        acc = 0.0
        for j in range(cols):
            acc += f16_to_f64(hs[j]) * x[j]
        out[r] = acc
    return out

def rms_norm_no_weight(x, eps):                        # ds4.c:6628
    ss = 0.0
    for v in x:
        ss += v * v
    s = 1.0 / math.sqrt(ss / len(x) + eps)
    return [v * s for v in x]

def sigmoid(z):
    return 1.0 / (1.0 + math.exp(-z)) if z >= 0 else math.exp(z) / (1.0 + math.exp(z))

def hc_split_sinkhorn(mix, scale, base, n_hc, iters, eps):     # ds4.c:9592
    out = [0.0] * (2 * n_hc + n_hc * n_hc)
    for i in range(n_hc):
        out[i] = sigmoid(mix[i] * scale[0] + base[i]) + eps
    for i in range(n_hc):
        o = n_hc + i
        out[o] = 2.0 * sigmoid(mix[o] * scale[1] + base[o])
    c = [0.0] * (n_hc * n_hc)
    for dst in range(n_hc):
        rmax = -1e300
        for src in range(n_hc):
            idx = src + dst * n_hc
            v = mix[2 * n_hc + idx] * scale[2] + base[2 * n_hc + idx]
            c[idx] = v
            if v > rmax:
                rmax = v
        rsum = 0.0
        for src in range(n_hc):
            idx = src + dst * n_hc
            c[idx] = math.exp(c[idx] - rmax)
            rsum += c[idx]
        for src in range(n_hc):
            c[src + dst * n_hc] = c[src + dst * n_hc] / rsum + eps
    for src in range(n_hc):                              # the FIRST normalisation is by COLUMN
        s = sum(c[src + dst * n_hc] for dst in range(n_hc))
        inv = 1.0 / (s + eps)
        for dst in range(n_hc):
            c[src + dst * n_hc] *= inv
    for _ in range(1, iters):
        for dst in range(n_hc):
            s = sum(c[src + dst * n_hc] for src in range(n_hc))
            inv = 1.0 / (s + eps)
            for src in range(n_hc):
                c[src + dst * n_hc] *= inv
        for src in range(n_hc):
            s = sum(c[src + dst * n_hc] for dst in range(n_hc))
            inv = 1.0 / (s + eps)
            for dst in range(n_hc):
                c[src + dst * n_hc] *= inv
    for k in range(n_hc * n_hc):
        out[2 * n_hc + k] = c[k]
    return out

def hc_pre(g, P, kind, residual_hc, n_embd, n_hc, eps, iters, hc_eps):    # ds4.c:9690
    flat = rms_norm_no_weight(residual_hc, eps)
    fn = P + "hc_%s_fn.weight" % kind
    rows = g.dims(fn)[1]
    cols = g.dims(fn)[0]
    mix = matvec_f16(g, fn, flat, rows, cols)
    scale = read_f32(g, P + "hc_%s_scale.weight" % kind, 3)
    base = read_f32(g, P + "hc_%s_base.weight" % kind, rows)
    split = hc_split_sinkhorn(mix, scale, base, n_hc, iters, hc_eps)
    cur = [0.0] * n_embd
    for d in range(n_embd):
        acc = 0.0
        for h in range(n_hc):
            acc += residual_hc[h * n_embd + d] * split[h]
        cur[d] = acc
    post = split[n_hc:2 * n_hc]
    comb = split[2 * n_hc:]
    return cur, post, comb, mix, flat

def hc_post(block_out, residual_hc, post, comb, n_embd, n_hc):           # ds4.c:9772
    out = [0.0] * (n_hc * n_embd)
    for dst in range(n_hc):
        for d in range(n_embd):
            acc = block_out[d] * post[dst]
            for src in range(n_hc):
                acc += comb[dst + src * n_hc] * residual_hc[src * n_embd + d]
            out[dst * n_embd + d] = acc
    return out


# ------------------------------------------------------------------ IQ2_XXS tables, rented
# Transcribed verbatim from ds4.c:873 (ksigns_iq2xs) and ds4.c:884 (iq2xxs_grid). These are
# TRAINED tables with no closed form; a 'simplified' re-derivation would be right most of the
# time and silently wrong on the rest, so they are carried, not computed.
KSIGNS_IQ2XS = (
    0, 129, 130, 3, 132, 5, 6, 135, 136, 9, 10, 139, 12, 141, 142, 15,
    144, 17, 18, 147, 20, 149, 150, 23, 24, 153, 154, 27, 156, 29, 30, 159,
    160, 33, 34, 163, 36, 165, 166, 39, 40, 169, 170, 43, 172, 45, 46, 175,
    48, 177, 178, 51, 180, 53, 54, 183, 184, 57, 58, 187, 60, 189, 190, 63,
    192, 65, 66, 195, 68, 197, 198, 71, 72, 201, 202, 75, 204, 77, 78, 207,
    80, 209, 210, 83, 212, 85, 86, 215, 216, 89, 90, 219, 92, 221, 222, 95,
    96, 225, 226, 99, 228, 101, 102, 231, 232, 105, 106, 235, 108, 237, 238, 111,
    240, 113, 114, 243, 116, 245, 246, 119, 120, 249, 250, 123, 252, 125, 126, 255,
)

IQ2XXS_GRID = (
    0x0808080808080808, 0x080808080808082b, 0x0808080808081919, 0x0808080808082b08,
    0x0808080808082b2b, 0x0808080808190819, 0x0808080808191908, 0x08080808082b0808,
    0x08080808082b082b, 0x08080808082b2b08, 0x08080808082b2b2b, 0x0808080819080819,
    0x0808080819081908, 0x0808080819190808, 0x0808080819192b08, 0x08080808192b0819,
    0x08080808192b1908, 0x080808082b080808, 0x080808082b08082b, 0x080808082b082b2b,
    0x080808082b2b082b, 0x0808081908080819, 0x0808081908081908, 0x0808081908190808,
    0x0808081908191919, 0x0808081919080808, 0x080808192b081908, 0x080808192b192b08,
    0x0808082b08080808, 0x0808082b0808082b, 0x0808082b082b082b, 0x0808082b2b08082b,
    0x0808190808080819, 0x0808190808081908, 0x0808190808190808, 0x08081908082b0819,
    0x08081908082b1908, 0x0808190819080808, 0x080819081908082b, 0x0808190819082b08,
    0x08081908192b0808, 0x080819082b080819, 0x080819082b081908, 0x080819082b190808,
    0x080819082b2b1908, 0x0808191908080808, 0x080819190808082b, 0x0808191908082b08,
    0x08081919082b0808, 0x080819191908192b, 0x08081919192b2b19, 0x080819192b080808,
    0x080819192b190819, 0x0808192b08082b19, 0x0808192b08190808, 0x0808192b19080808,
    0x0808192b2b081908, 0x0808192b2b2b1908, 0x08082b0808080808, 0x08082b0808081919,
    0x08082b0808082b08, 0x08082b0808191908, 0x08082b08082b2b08, 0x08082b0819080819,
    0x08082b0819081908, 0x08082b0819190808, 0x08082b081919082b, 0x08082b082b082b08,
    0x08082b1908081908, 0x08082b1919080808, 0x08082b2b0808082b, 0x08082b2b08191908,
    0x0819080808080819, 0x0819080808081908, 0x0819080808190808, 0x08190808082b0819,
    0x0819080819080808, 0x08190808192b0808, 0x081908082b081908, 0x081908082b190808,
    0x081908082b191919, 0x0819081908080808, 0x0819081908082b08, 0x08190819082b0808,
    0x0819081919190808, 0x0819081919192b2b, 0x081908192b080808, 0x0819082b082b1908,
    0x0819082b19081919, 0x0819190808080808, 0x0819190808082b08, 0x08191908082b0808,
    0x08191908082b1919, 0x0819190819082b19, 0x081919082b080808, 0x0819191908192b08,
    0x08191919192b082b, 0x0819192b08080808, 0x0819192b0819192b, 0x08192b0808080819,
    0x08192b0808081908, 0x08192b0808190808, 0x08192b0819080808, 0x08192b082b080819,
    0x08192b1908080808, 0x08192b1908081919, 0x08192b192b2b0808, 0x08192b2b19190819,
    0x082b080808080808, 0x082b08080808082b, 0x082b080808082b2b, 0x082b080819081908,
    0x082b0808192b0819, 0x082b08082b080808, 0x082b08082b08082b, 0x082b0819082b2b19,
    0x082b081919082b08, 0x082b082b08080808, 0x082b082b0808082b, 0x082b190808080819,
    0x082b190808081908, 0x082b190808190808, 0x082b190819080808, 0x082b19081919192b,
    0x082b191908080808, 0x082b191919080819, 0x082b1919192b1908, 0x082b192b2b190808,
    0x082b2b0808082b08, 0x082b2b08082b0808, 0x082b2b082b191908, 0x082b2b2b19081908,
    0x1908080808080819, 0x1908080808081908, 0x1908080808190808, 0x1908080808192b08,
    0x19080808082b0819, 0x19080808082b1908, 0x1908080819080808, 0x1908080819082b08,
    0x190808081919192b, 0x19080808192b0808, 0x190808082b080819, 0x190808082b081908,
    0x190808082b190808, 0x1908081908080808, 0x19080819082b0808, 0x19080819192b0819,
    0x190808192b080808, 0x190808192b081919, 0x1908082b08080819, 0x1908082b08190808,
    0x1908082b19082b08, 0x1908082b1919192b, 0x1908082b192b2b08, 0x1908190808080808,
    0x1908190808082b08, 0x19081908082b0808, 0x190819082b080808, 0x190819082b192b19,
    0x190819190819082b, 0x19081919082b1908, 0x1908192b08080808, 0x19082b0808080819,
    0x19082b0808081908, 0x19082b0808190808, 0x19082b0819080808, 0x19082b0819081919,
    0x19082b1908080808, 0x19082b1919192b08, 0x19082b19192b0819, 0x19082b192b08082b,
    0x19082b2b19081919, 0x19082b2b2b190808, 0x1919080808080808, 0x1919080808082b08,
    0x1919080808190819, 0x1919080808192b19, 0x19190808082b0808, 0x191908082b080808,
    0x191908082b082b08, 0x1919081908081908, 0x191908191908082b, 0x191908192b2b1908,
    0x1919082b2b190819, 0x191919082b190808, 0x191919082b19082b, 0x1919191908082b2b,
    0x1919192b08080819, 0x1919192b19191908, 0x19192b0808080808, 0x19192b0808190819,
    0x19192b0808192b19, 0x19192b08192b1908, 0x19192b1919080808, 0x19192b2b08082b08,
    0x192b080808081908, 0x192b080808190808, 0x192b080819080808, 0x192b0808192b2b08,
    0x192b081908080808, 0x192b081919191919, 0x192b082b08192b08, 0x192b082b192b0808,
    0x192b190808080808, 0x192b190808081919, 0x192b191908190808, 0x192b19190819082b,
    0x192b19192b081908, 0x192b2b081908082b, 0x2b08080808080808, 0x2b0808080808082b,
    0x2b08080808082b2b, 0x2b08080819080819, 0x2b0808082b08082b, 0x2b08081908081908,
    0x2b08081908192b08, 0x2b08081919080808, 0x2b08082b08190819, 0x2b08190808080819,
    0x2b08190808081908, 0x2b08190808190808, 0x2b08190808191919, 0x2b08190819080808,
    0x2b081908192b0808, 0x2b08191908080808, 0x2b0819191908192b, 0x2b0819192b191908,
    0x2b08192b08082b19, 0x2b08192b19080808, 0x2b08192b192b0808, 0x2b082b080808082b,
    0x2b082b1908081908, 0x2b082b2b08190819, 0x2b19080808081908, 0x2b19080808190808,
    0x2b190808082b1908, 0x2b19080819080808, 0x2b1908082b2b0819, 0x2b1908190819192b,
    0x2b1908192b080808, 0x2b19082b19081919, 0x2b19190808080808, 0x2b191908082b082b,
    0x2b19190819081908, 0x2b19191919190819, 0x2b192b082b080819, 0x2b192b19082b0808,
    0x2b2b08080808082b, 0x2b2b080819190808, 0x2b2b08082b081919, 0x2b2b081908082b19,
    0x2b2b082b08080808, 0x2b2b190808192b08, 0x2b2b2b0819190808, 0x2b2b2b1908081908,
)

# ------------------------------------------------------------------ the FFN half (ds4.c), STONE 37
# A complete layer is HC-pre -> attention -> HC-post THEN HC-pre -> ffn_norm -> MoE+shared -> HC-post.
# The second half's ds4.c anchors, re-expressed here the same way as the attention half above:
#   layer_forward_self_one            :13835   the attention half hands after_attn_hc to layer_ffn_one
#   layer_ffn_one                     :11437   hc_pre(ffn) -> ffn_norm -> routed MoE -> shared -> sum -> hc_post
#   layer_hash_selected_experts       :10566   layers 0..2 select by the I32 tid2eid table on the TOKEN id
#   layer_router_probs_one            :10588   probs[i] = sqrt(softplus(logit[i])) -- gating func 4
#   layer_hash_router_weights_from_probs :10600  w = probs[sel]/max(sum,6.103515625e-5) * expert_weights_scale
#   swiglu                            :10430   clamp gate ABOVE only, up to [-lim, lim], out = silu(g)*u
#   layer_routed_moe_one              :10697   the router weight multiplies the MID, before the down matvec
#   layer_shared_ffn_one              :10444   the shared expert runs for EVERY token and is simply ADDED
#   hc_post_one                       :9772    the SECOND hc_post, over the FFN's own post/comb
#
# THE RECIPE GAP, named (aporon). ds4.c cannot actually execute this file's FFN: its expert gate/up
# dispatcher (matvec_experts_mid_prequant :9349) raises "unsupported gate/up expert tensor type" for
# type 40 (MXFP4), and layer_shared_ffn_one :10460 dies unless the shared expert is Q8_0 -- this file's
# shexp is type 41. So the ORDER OF OPERATIONS and every scalar choice above are rented from ds4.c, but
# the three quantised decodes are NOT ds4.c's prequantised paths: where ds4.c would quantise the
# activation to Q8_K before an IQ2_XXS down projection, this oracle uses the EXACT fp64 activation, which
# is ds4.c's own ds4_vec_dot_iq2_xxs_f32 :3779 control flow. That is a stated deviation, not a hidden one.
# The decodes themselves (MXFP4 E2M1+E8M0, IQ2_XXS, MXFP8) are CANONICAL -- one right answer -- and are
# what Stones 33/34/35 self-carve-proved at real dims; the oracle re-derives them independently anyway.

# --- MXFP4 (GGUF type 40): E2M1 nibbles, plane-split, then nel/32 E8M0 scale bytes. The E2M1 ladder is
# ds4.c:3231 dsv4_e2m1fn_value_cpu's, re-expressed as the same arithmetic the body's mx4_val uses.
def _e2m1_table():
    t = []
    for c in range(16):
        mant = c % 2
        ex = (c // 2) % 4
        sgn = c // 8
        frac = mant / 2.0
        mag = frac if ex == 0 else (2.0 ** (ex - 1)) * (1.0 + frac)
        t.append(-mag if sgn == 1 else mag)
    return t

E2M1 = _e2m1_table()
MX4_LO = [E2M1[b & 15] for b in range(256)]      # even flat index takes the LOW nibble
MX4_HI = [E2M1[b >> 4] for b in range(256)]

def mx4_matvec_expert(g, name, x, rows, cols, expert):
    """y[r] = sum_c w(r*cols+c)*x[c] over the expert's own byte slice of an [in, out, experts] stack."""
    nel = rows * cols
    stride = nel // 2 + nel // 32
    base = g.abs_off(name) + expert * stride
    pay = g.mm[base: base + nel // 2]
    sca = g.mm[base + nel // 2: base + stride]
    xe = x[0::2]
    xo = x[1::2]
    ngrp = cols // 32
    half = cols // 2
    lo_get = MX4_LO.__getitem__
    hi_get = MX4_HI.__getitem__
    mul = float.__mul__
    xes = [xe[i * 16:(i + 1) * 16] for i in range(ngrp)]
    xos = [xo[i * 16:(i + 1) * 16] for i in range(ngrp)]
    out = [0.0] * rows
    for r in range(rows):
        rb = r * half
        row = pay[rb: rb + half]
        g0 = (r * cols) // 32
        acc = 0.0
        for gi in range(ngrp):
            blk = row[gi * 16:(gi + 1) * 16]
            s = E8M0[sca[g0 + gi]]
            a = sum(map(mul, map(lo_get, blk), xes[gi])) + sum(map(mul, map(hi_get, blk), xos[gi]))
            acc += s * a
        out[r] = acc
    return out

# --- IQ2_XXS (GGUF type 16): 66-byte / 256-element superblock. The 256 eight-tuple grid and the
# 128-entry sign table are TRAINED -- no closed form -- so they are transcribed from ds4.c:884
# (iq2xxs_grid) and ds4.c:873 (ksigns_iq2xs); the 8th sign rides in the table's 8th bit (paritylock,
# corpus row 855). The dot's control flow is ds4.c:3779 ds4_vec_dot_iq2_xxs_f32, re-expressed.
KMASK_IQ2XS = (1, 2, 4, 8, 16, 32, 64, 128)

def _iq2_signed_grid():
    sg = []
    for gi in range(256):
        v = IQ2XXS_GRID[gi]
        gb = [(v >> (8 * j)) & 0xFF for j in range(8)]
        for s in range(128):
            signs = KSIGNS_IQ2XS[s]
            sg.append(tuple(float(-gb[j] if (signs & KMASK_IQ2XS[j]) else gb[j]) for j in range(8)))
    return sg

IQ2_SIGNED = None

def iq2_matvec_expert(g, name, x, rows, cols, expert):
    global IQ2_SIGNED
    if IQ2_SIGNED is None:
        IQ2_SIGNED = _iq2_signed_grid()
    sg = IQ2_SIGNED
    nblk_row = cols // 256
    row_bytes = nblk_row * 66
    stride = rows * row_bytes
    base = g.abs_off(name) + expert * stride
    mm = g.mm
    mul = float.__mul__
    xs = [x[i * 8:(i + 1) * 8] for i in range(cols // 8)]
    out = [0.0] * rows
    for r in range(rows):
        rb = base + r * row_bytes
        acc = 0.0
        for b in range(nblk_row):
            off = rb + b * 66
            d = f16_to_f64(struct.unpack_from("<H", mm, off)[0])
            for ib32 in range(8):
                gbase = off + 2 + 8 * ib32
                gidx = struct.unpack_from("<4B", mm, gbase)
                aux1 = struct.unpack_from("<I", mm, gbase + 4)[0]
                scale = 0.125 * d * float(2 * (aux1 >> 28) + 1)
                xbase = (b * 256 + ib32 * 32) // 8
                sub = 0.0
                for l in range(4):
                    sub += sum(map(mul, sg[gidx[l] * 128 + ((aux1 >> (7 * l)) & 127)], xs[xbase + l]))
                acc += scale * sub
        out[r] = acc
    return out

def read_i32_row(g, name, row, n):
    o = g.abs_off(name) + row * n * 4
    return list(struct.unpack_from("<%di" % n, g.mm, o))

def softplus_stable(z):                                # ds4.c:10424
    if z > 20.0:
        return z
    if z < -20.0:
        return math.exp(z)
    return math.log1p(math.exp(z))

def silu(z):                                           # ds4.c:10420
    return z * sigmoid(z)

def swiglu(gate, up, clamp):                           # ds4.c:10430
    out = [0.0] * len(gate)
    for i in range(len(gate)):
        gv = gate[i]
        uv = up[i]
        if clamp > 1.0e-6:
            if gv > clamp:
                gv = clamp
            if uv > clamp:
                uv = clamp
            if uv < -clamp:
                uv = -clamp
        out[i] = silu(gv) * uv
    return out

def topk_desc(score, n, k):                            # ds4.c:10630 — verbatim control flow
    idx = [-1] * k
    for i in range(n):
        for j in range(k):
            if idx[j] < 0 or score[i] > score[idx[j]]:
                for mm in range(k - 1, j, -1):
                    idx[mm] = idx[mm - 1]
                idx[j] = i
                break
    return idx


# STONE 39. The expert matvec DISPATCHES on the tensor's own declared type. A blk.0-shaped stack is
# silently wrong: gate/up and down flip between GGUF 40 (MXFP4) and 16 (IQ2_XXS) INDEPENDENTLY across six
# layer groups. The type is read from the file's tensor table, never assumed.
def expert_matvec(g, name, x, rows, cols, expert):
    t = g.ttype(name)
    if t == 40:
        return mx4_matvec_expert(g, name, x, rows, cols, expert)
    if t == 16:
        return iq2_matvec_expert(g, name, x, rows, cols, expert)
    raise SystemExit("expert tensor %s carries type %d, which this oracle does not decode" % (name, t))


def layer_route(g, P, K, probs, token, n_exp_router, n_exp_stack, n_used, wscale):
    """The two routing regimes, chosen the way ds4.c:10745 chooses: by whether the layer CARRIES a table.

    Layers 0..2 (deepseek4.hash_layer_count = 3) hold ffn_gate_tid2eid and select by an I32 table read on
    the TOKEN id (forepick, corpus row 867). Layers 3+ hold exp_probs_b.bias and select by BIASED top-k
    while weighting by the UNbiased prob. Both then divide by the floored sum and scale by 1.5.
    """
    if (P + "ffn_gate_tid2eid.weight") in g.tensors:
        selected = read_i32_row(g, P + "ffn_gate_tid2eid.weight", token, n_used)
        regime = "hash"
    else:
        bias = read_f32(g, P + "exp_probs_b.bias", n_exp_router)
        selection = [probs[i] + bias[i] for i in range(n_exp_router)]
        selected = topk_desc(selection, n_exp_router, n_used)
        regime = "topk"
    s0 = 0.0
    for e in selected:
        if e < 0 or e >= n_exp_router:
            raise SystemExit("selected expert %d is outside the router's %d logits" % (e, n_exp_router))
        if e >= n_exp_stack:
            # REAP-25 pruned this layer to n_exp_stack experts while the router still emits n_exp_router
            # logits. The file keeps the two consistent by carrying exp_probs_b.bias = -1e30 on every
            # pruned index, so biased top-k can never reach one. If one is ever reached, say so — do not
            # index off the end of the stack.
            raise SystemExit("layer routing chose expert %d but the stack holds only %d" % (e, n_exp_stack))
        s0 += probs[e]
    if s0 < 6.103515625e-5:
        s0 = 6.103515625e-5
    return selected, [probs[e] / s0 * wscale for e in selected], regime


def ffn_half(g, P, after_attn_hc, K, n_embd, n_hc, eps, hc_iters, hc_eps, token, il):
    """ds4.c:11437 layer_ffn_one, re-expressed. Returns a dict of every named intermediate."""
    n_ff = K["deepseek4.expert_feed_forward_length"]
    n_exp = K["deepseek4.expert_count"]
    n_used = K["deepseek4.expert_used_count"]
    wscale = K["deepseek4.expert_weights_scale"]
    clamp = K["deepseek4.swiglu_clamp_exp"][il]
    # the PER-LAYER expert count is the tensor's own dim[2], NEVER the KV's expert_count.
    n_exp_stack = g.dims(P + "ffn_gate_exps.weight")[2]
    ffn_cur, post, comb, mix, flat = hc_pre(g, P, "ffn", after_attn_hc, n_embd, n_hc,
                                            eps, hc_iters, hc_eps)
    norm = rms_norm_weight(ffn_cur, read_f32(g, P + "ffn_norm.weight", n_embd), eps)

    logits = matvec_f16(g, P + "ffn_gate_inp.weight", norm, n_exp, n_embd)
    probs = [math.sqrt(softplus_stable(v)) for v in logits]

    selected, ew, regime = layer_route(g, P, K, probs, token, n_exp, n_exp_stack, n_used, wscale)

    moe = [0.0] * n_embd
    per_expert_down = []
    for i, e in enumerate(selected):
        gate = expert_matvec(g, P + "ffn_gate_exps.weight", norm, n_ff, n_embd, e)
        up = expert_matvec(g, P + "ffn_up_exps.weight", norm, n_ff, n_embd, e)
        mid = swiglu(gate, up, clamp)
        mid = [v * ew[i] for v in mid]
        down = expert_matvec(g, P + "ffn_down_exps.weight", mid, n_embd, n_ff, e)
        per_expert_down.append((e, gate, up, mid, down))
        for d in range(n_embd):
            moe[d] += down[d]

    sg = mx8_matvec(g, P + "ffn_gate_shexp.weight", norm, n_ff, n_embd)
    su = mx8_matvec(g, P + "ffn_up_shexp.weight", norm, n_ff, n_embd)
    smid = swiglu(sg, su, clamp)
    shared = mx8_matvec(g, P + "ffn_down_shexp.weight", smid, n_embd, n_ff)

    ffn_out = [moe[d] + shared[d] for d in range(n_embd)]
    out_hc = hc_post(ffn_out, after_attn_hc, post, comb, n_embd, n_hc)
    return {
        "ffn_flat": flat, "ffn_mix": mix, "ffn_post_w": post, "ffn_comb": comb, "ffn_cur": ffn_cur,
        "ffn_norm": norm, "router_logits": logits, "router_probs": probs,
        "selected": selected, "expert_w": ew, "clamp": clamp, "regime": regime,
        "n_exp_stack": n_exp_stack,
        "moe": moe, "shared": shared, "ffn_out": ffn_out, "out_hc": out_hc,
        "per_expert": per_expert_down,
        "sh_gate": sg, "sh_up": su, "sh_mid": smid,
    }

# ------------------------------------------------------------------ STONE 39: the heterogeneous stack
# ds4.c:13793 layer_forward_self_one, re-expressed WHOLE and driven per layer from the file's own table.
# forward_first_token_cpu (:13849) calls exactly this for every one of the 43 layers, handing the n_hc
# streams forward, and it never touches a compressor or an indexer -- so the compressor tensors blk.2+
# carry are not on this path. Grounded, not assumed.
def attn_half(g, P, K, GEO, residual_hc, il, pos):
    n_embd = GEO["n_embd"]; n_head = GEO["n_head"]; n_head_kv = GEO["n_head_kv"]
    head_dim = GEO["head_dim"]; n_rot = GEO["n_rot"]; eps = GEO["eps"]
    n_hc = GEO["n_hc"]; q_rank = GEO["q_rank"]; n_groups = GEO["n_groups"]; rank = GEO["rank"]

    x_in, post, comb, hc_mix, hc_flat = hc_pre(g, P, "attn", residual_hc, n_embd, n_hc,
                                               eps, GEO["hc_iters"], GEO["hc_eps"])
    xn = rms_norm_weight(x_in, read_f32(g, P + "attn_norm.weight", n_embd), eps)
    qlat = mx8_matvec(g, P + "attn_q_a.weight", xn, q_rank, n_embd)
    qlatn = rms_norm_weight(qlat, read_f32(g, P + "attn_q_a_norm.weight", q_rank), eps)
    q = mx8_matvec(g, P + "attn_q_b.weight", qlatn, n_head * head_dim, q_rank)
    q_headrms = head_rms_norm(list(q), n_head, head_dim, eps)
    kvraw = mx8_matvec(g, P + "attn_kv.weight", xn, head_dim, n_embd)
    kv_norm = rms_norm_weight(kvraw, read_f32(g, P + "attn_kv_a_norm.weight", head_dim), eps)

    R = rope_layer_params(K, il)
    fwd = rope_pair_factors(n_rot, pos, R["n_ctx_orig"], R["freq_base"], R["freq_scale"],
                            R["ext_factor"], R["attn_factor"], R["beta_fast"], R["beta_slow"], False)
    inv = rope_pair_factors(n_rot, pos, R["n_ctx_orig"], R["freq_base"], R["freq_scale"],
                            R["ext_factor"], R["attn_factor"], R["beta_fast"], R["beta_slow"], True)
    q = rope_apply(list(q_headrms), n_head, head_dim, n_rot, fwd)
    kv_roped = rope_apply(list(kv_norm), n_head_kv, head_dim, n_rot, fwd)
    kv = fp8_kv_quantize_row(list(kv_roped), head_dim, n_rot)
    kv = [f16_round(v) for v in kv]

    heads_attn = attention_rows_one(q, [kv], read_f32(g, P + "attn_sinks.weight", n_head),
                                    n_head, head_dim)
    heads = rope_apply(list(heads_attn), n_head, head_dim, n_rot, inv)

    group_dim = head_dim * (n_head // n_groups)
    a_rows = g.dims(P + "attn_output_a.weight")[1]
    a_cols = g.dims(P + "attn_output_a.weight")[0]
    if a_cols != group_dim or a_rows != n_groups * rank:
        raise SystemExit("attn_output_a layout %dx%d != grouped %dx%d"
                         % (a_rows, a_cols, n_groups * rank, group_dim))
    low = [0.0] * (n_groups * rank)
    base = g.abs_off(P + "attn_output_a.weight")
    nel = a_rows * a_cols
    pay = g.mm[base: base + nel]
    sca = g.mm[base + nel: base + nel + nel // 32]
    tget = E4M3.__getitem__
    mul = float.__mul__
    ngrp32 = group_dim // 32
    for grp in range(n_groups):
        xg = heads[grp * group_dim:(grp + 1) * group_dim]
        xs = [xg[i * 32:(i + 1) * 32] for i in range(ngrp32)]
        for r in range(rank):
            tensor_row = grp * rank + r                 # ds4.c:7129
            rp = tensor_row * group_dim
            g0 = rp // 32
            acc = 0.0
            for gi in range(ngrp32):
                p = rp + gi * 32
                acc += E8M0[sca[g0 + gi]] * sum(map(mul, map(tget, pay[p:p + 32]), xs[gi]))
            low[tensor_row] = acc
    b_rows = g.dims(P + "attn_output_b.weight")[1]
    b_cols = g.dims(P + "attn_output_b.weight")[0]
    attn_out = mx8_matvec(g, P + "attn_output_b.weight", low, b_rows, b_cols)
    after_attn_hc = hc_post(attn_out, residual_hc, post, comb, n_embd, n_hc)
    return {
        "hc_flat": hc_flat, "hc_mix": hc_mix, "hc_post_w": post, "hc_comb": comb, "hc_cur": x_in,
        "xn": xn, "qlatn": qlatn, "q_headrms": q_headrms, "q": q, "kv_norm": kv_norm,
        "kv_roped": kv_roped, "kv": kv, "heads_attn": heads_attn, "heads": heads, "low": low,
        "attn_out": attn_out, "after_attn_hc": after_attn_hc, "rope": R,
    }


def geometry(K):
    return {
        "n_embd": K["deepseek4.embedding_length"],
        "n_head": K["deepseek4.attention.head_count"],
        "n_head_kv": K["deepseek4.attention.head_count_kv"],
        "head_dim": K["deepseek4.attention.key_length"],
        "n_rot": K["deepseek4.rope.dimension_count"],
        "eps": K["deepseek4.attention.layer_norm_rms_epsilon"],
        "q_rank": K["deepseek4.attention.q_lora_rank"],
        "n_groups": K["deepseek4.attention.output_group_count"],
        "rank": K["deepseek4.attention.output_lora_rank"],
        "n_hc": K.get("deepseek4.hyper_connection.count", 4),
        "hc_iters": K.get("deepseek4.hyper_connection.sinkhorn_iterations", 20),
        "hc_eps": K.get("deepseek4.hyper_connection.epsilon", 1e-6),
    }


def run_stack(g, token, pos, n_layers, outdir):
    """The 43 heterogeneous layers, carrying the four hyper-connection streams between them.

    Every per-layer decision is READ FROM THE FILE and printed, never inherited from blk.0:
      * n_exp_stack  = ffn_gate_exps dim[2]     (256 for 0..2, 192 after REAP-25 pruning)
      * gate/up and down TYPES                  (GGUF 40 MXFP4 or 16 IQ2_XXS, independently)
      * the routing regime                      (ffn_gate_tid2eid table present, or biased top-k)
      * the RoPE compress ratio                 (0 for 0..1, then 4/128 alternating)
    Each layer's vectors are flushed to disk as it completes, so a partial run still gates a prefix.
    """
    K = g.kv
    GEO = geometry(K)
    n_embd = GEO["n_embd"]; n_hc = GEO["n_hc"]
    x0 = read_f16_row(g, "token_embd.weight", token, n_embd)
    # THE SENSITIVITY PROBE. A 43-layer stack is a composed map, and its own conditioning is a FACT about
    # the model that has to be measured before any GPU-vs-oracle disagreement can be read. Setting
    # DSV4_ORACLE_PERTURB to a relative size (one f32 ulp is 1.1920929e-7) tilts the layer-0 input by that
    # much, in fp64 throughout, and runs the SAME recipe. The distance between the two fp64 trajectories is
    # then the envelope a one-ulp difference opens by itself -- the honest yardstick for an f32 carrier,
    # and a yardstick derived from the model rather than chosen to make a harness green.
    f32_state = os.environ.get("DSV4_ORACLE_F32_STATE", "") == "1"
    if f32_state:
        print("F32STATE 1 (the inter-layer state is rounded to f32; all arithmetic stays fp64)")
    pert_every = float(os.environ.get("DSV4_ORACLE_PERTURB_EVERY", "0") or "0")
    if pert_every != 0.0:
        print("PERTURB_EVERY %r (applied to the state after every layer)" % pert_every)
    pert = float(os.environ.get("DSV4_ORACLE_PERTURB", "0") or "0")
    if pert != 0.0:
        x0 = [v * (1.0 + pert * (1.0 if (i % 2 == 0) else -1.0)) for i, v in enumerate(x0)]
        print("PERTURB %r (one-sided per element, alternating sign)" % pert)
    resid = list(x0) * n_hc                                # ds4.c:9764 broadcast
    print("STACKGEOM n_embd %d n_hc %d layers %d token %d pos %d" % (n_embd, n_hc, n_layers, token, pos))
    if outdir:
        with open(os.path.join(outdir, "oracle-embed.f64"), "w") as fh:
            for v in x0:
                fh.write("%.17g\n" % v)
    for il in range(n_layers):
        P = "blk.%d." % il
        A = attn_half(g, P, K, GEO, resid, il, pos)
        F = ffn_half(g, P, A["after_attn_hc"], K, n_embd, n_hc, GEO["eps"],
                     GEO["hc_iters"], GEO["hc_eps"], token, il)
        R = A["rope"]
        print("LAYER %d ratio %d freq_base %r freq_scale %r n_exp %d gate_t %d up_t %d down_t %d "
              "regime %s selected %s weights %s"
              % (il, R["ratio"], R["freq_base"], R["freq_scale"], F["n_exp_stack"],
                 g.ttype(P + "ffn_gate_exps.weight"), g.ttype(P + "ffn_up_exps.weight"),
                 g.ttype(P + "ffn_down_exps.weight"), F["regime"], F["selected"],
                 ["%.9g" % w for w in F["expert_w"]]))
        sys.stdout.flush()
        if outdir:
            e0 = F["per_expert"][0]
            named = [("after_attn_hc", A["after_attn_hc"]), ("ffn_cur", F["ffn_cur"]),
                     ("ffn_normed", F["ffn_norm"]), ("router_logits", F["router_logits"]),
                     ("expert_w", F["expert_w"]), ("selected", [float(v) for v in F["selected"]]),
                     ("exp0_gate", e0[1]), ("exp0_up", e0[2]), ("exp0_mid", e0[3]),
                     ("exp0_down", e0[4]), ("moe", F["moe"]), ("shared", F["shared"]),
                     ("ffn_out", F["ffn_out"]), ("out_hc", F["out_hc"])]
            for key, vec in named:
                with open(os.path.join(outdir, "oracle-L%d-%s.f64" % (il, key)), "w") as fh:
                    for v in vec:
                        fh.write("%.17g\n" % v)
            with open(os.path.join(outdir, "oracle-done.txt"), "a") as fh:
                fh.write("%d\n" % il)
        resid = F["out_hc"]
        # THE f32-STATE PROBE. DSV4_ORACLE_F32_STATE=1 rounds ONLY the state handed from one layer to the
        # next to f32, and leaves every arithmetic step in fp64. That is the one thing an f32 carrier
        # cannot avoid doing, so the distance between this trajectory and the pure-fp64 one is the floor
        # under ANY f32 implementation of this stack — measured from the reference, not chosen.
        if f32_state:
            resid = [f32_round(v) for v in resid]
        # THE COMPOSED-TRAJECTORY ENVELOPE. DSV4_ORACLE_PERTURB_EVERY tilts the state by a relative amount
        # after EVERY layer, in fp64 throughout. Set to the per-layer gap an f32 carrier was MEASURED to
        # have when each layer is run alone from this oracle's own input, it answers the only question a
        # 43-layer comparison can honestly ask: how far apart do two runs of the SAME recipe drift when
        # one of them is nudged, each layer, by exactly as much as f32 arithmetic nudges it?
        if pert_every != 0.0:
            resid = [v * (1.0 + pert_every * (1.0 if (i % 2 == 0) else -1.0)) for i, v in enumerate(resid)]
    print("STACKEND %d" % n_layers)


# ------------------------------------------------------------------ the core
def main():
    if len(sys.argv) < 4:
        raise SystemExit("usage: dsv4-mla-core-oracle.py <gguf> <token> <pos> [layer]")
    path, token, pos = sys.argv[1], int(sys.argv[2]), int(sys.argv[3])
    il = int(sys.argv[4]) if len(sys.argv) > 4 else 0
    g = Gguf(path)
    K = g.kv
    # STONE 39: mode `stack` runs N heterogeneous layers, carrying the four hyper-connection streams,
    # every per-layer decision read from the file's own tensor table. argv[4] is the LAYER COUNT here.
    if os.environ.get("DSV4_ORACLE_MODE") == "stack":
        n_layers = il if len(sys.argv) > 4 else K["deepseek4.block_count"]
        print("ORACLE dsv4-stack token %d pos %d layers %d" % (token, pos, n_layers))
        run_stack(g, token, pos, n_layers, os.environ.get("DSV4_ORACLE_OUT"))
        print("END")
        return
    n_embd = K["deepseek4.embedding_length"]
    n_head = K["deepseek4.attention.head_count"]
    n_head_kv = K["deepseek4.attention.head_count_kv"]
    head_dim = K["deepseek4.attention.key_length"]
    n_rot = K["deepseek4.rope.dimension_count"]
    eps = K["deepseek4.attention.layer_norm_rms_epsilon"]
    freq_base = K["deepseek4.rope.freq_base"]
    n_groups = K["deepseek4.attention.output_group_count"]
    rank = K["deepseek4.attention.output_lora_rank"]
    q_rank = K["deepseek4.attention.q_lora_rank"]

    # ds4.c:1065 — FLASH, il < 2 -> compress ratio 0 -> uncompressed RoPE (base 10000, scale 1, no YaRN).
    ratios = K.get("deepseek4.attention.compress_ratios", [])
    ratio = ratios[il] if il < len(ratios) else 0
    if ratio != 0:
        raise SystemExit("this oracle covers the UNCOMPRESSED layers only; layer %d has ratio %d" % (il, ratio))
    freq_scale = 1.0

    P = "blk.%d." % il
    print("ORACLE dsv4-mla-core layer %d token %d pos %d" % (il, token, pos))
    print("GEOM n_embd %d n_head %d n_head_kv %d head_dim %d n_rot %d q_rank %d groups %d rank %d"
          % (n_embd, n_head, n_head_kv, head_dim, n_rot, q_rank, n_groups, rank))
    print("ROPE freq_base %r freq_scale %r ratio %d" % (freq_base, freq_scale, ratio))
    print("OUTPATH grouped a=%s b=%s dense_present=%d"
          % (g.dims(P + "attn_output_a.weight"), g.dims(P + "attn_output_b.weight"),
             1 if (P + "attn_output.weight") in g.tensors else 0))

    # --- the token's own F16 embedding.
    x0 = read_f16_row(g, "token_embd.weight", token, n_embd)

    # --- STAGE 4: the HC half. mode "probe" feeds the MLA the raw embedding (Stone 33/34/35's
    # knownsolved input class); mode "hc" builds the REAL layer-0 input — the embedding broadcast to
    # n_hc streams (ds4.c:9764) and collapsed by hc_pre (ds4.c:9690) — which removes that bound.
    n_hc = K.get("deepseek4.hyper_connection.count", 4)
    hc_iters = K.get("deepseek4.hyper_connection.sinkhorn_iterations", 20)
    hc_eps = K.get("deepseek4.hyper_connection.epsilon", 1e-6)
    mode = os.environ.get("DSV4_ORACLE_MODE", "probe")
    residual_hc = list(x0) * n_hc                                     # ds4.c:9764 broadcast
    hc_flat = []
    hc_mix = []
    post = []
    comb = []
    if mode in ("hc", "layer"):
        x_in, post, comb, hc_mix, hc_flat = hc_pre(g, P, "attn", residual_hc, n_embd, n_hc,
                                                   eps, hc_iters, hc_eps)
    else:
        x_in = x0
    print("MODE %s n_hc %d sinkhorn_iters %d hc_eps %r" % (mode, n_hc, hc_iters, hc_eps))

    # --- ds4.c:9981  attn_norm
    xn = rms_norm_weight(x_in, read_f32(g, P + "attn_norm.weight", n_embd), eps)

    # --- ds4.c:10002  Q: q_a -> q_a_norm -> q_b -> per-head RMSNorm (no weight)
    qlat = mx8_matvec(g, P + "attn_q_a.weight", xn, q_rank, n_embd)
    qlatn = rms_norm_weight(qlat, read_f32(g, P + "attn_q_a_norm.weight", q_rank), eps)
    q = mx8_matvec(g, P + "attn_q_b.weight", qlatn, n_head * head_dim, q_rank)
    q_preheadrms = list(q)
    q_headrms = head_rms_norm(list(q), n_head, head_dim, eps)

    # --- ds4.c:10041  KV: attn_kv -> kv_a_norm
    kvraw = mx8_matvec(g, P + "attn_kv.weight", xn, head_dim, n_embd)
    kv_norm = rms_norm_weight(kvraw, read_f32(g, P + "attn_kv_a_norm.weight", head_dim), eps)

    # --- ds4.c:13793  RoPE forward on q and on kv, then the kv row's fp8 + f16 round-trip
    q = rope_tail(list(q_headrms), n_head, head_dim, n_rot, pos, freq_base, freq_scale, False)
    kv_roped = rope_tail(list(kv_norm), n_head_kv, head_dim, n_rot, pos, freq_base, freq_scale, False)
    kv = fp8_kv_quantize_row(list(kv_roped), head_dim, n_rot)
    kv = [f16_round(v) for v in kv]

    # --- ds4.c:10305  the sink-aware softmax; this diagnostic path attends to ONE row (itself)
    heads_attn = attention_rows_one(q, [kv], read_f32(g, P + "attn_sinks.weight", n_head), n_head, head_dim)

    # --- ds4.c:13793  the heads are UN-roped (inverse rotation) before the output projection
    heads = rope_tail(list(heads_attn), n_head, head_dim, n_rot, pos, freq_base, freq_scale, True)

    # --- ds4.c:10356  the GROUPED output: 8 groups of 8 heads -> rank-1024 low each, then one 8192->4096
    group_dim = head_dim * (n_head // n_groups)
    a_rows, a_cols = g.dims(P + "attn_output_a.weight")[1], g.dims(P + "attn_output_a.weight")[0]
    if a_cols != group_dim or a_rows != n_groups * rank:
        raise SystemExit("attn_output_a layout %dx%d != grouped %dx%d" % (a_rows, a_cols, n_groups * rank, group_dim))
    low = [0.0] * (n_groups * rank)
    base = g.abs_off(P + "attn_output_a.weight")
    nel = a_rows * a_cols
    pay = g.mm[base: base + nel]
    sca = g.mm[base + nel: base + nel + nel // 32]
    tget = E4M3.__getitem__
    mul = float.__mul__
    ngrp32 = group_dim // 32
    for grp in range(n_groups):
        xg = heads[grp * group_dim:(grp + 1) * group_dim]
        xs = [xg[i * 32:(i + 1) * 32] for i in range(ngrp32)]
        for r in range(rank):
            tensor_row = grp * rank + r              # ds4.c:7129
            rp = tensor_row * group_dim
            g0 = rp // 32
            acc = 0.0
            for gi in range(ngrp32):
                p = rp + gi * 32
                acc += E8M0[sca[g0 + gi]] * sum(map(mul, map(tget, pay[p:p + 32]), xs[gi]))
            low[tensor_row] = acc

    b_rows, b_cols = g.dims(P + "attn_output_b.weight")[1], g.dims(P + "attn_output_b.weight")[0]
    attn_out = mx8_matvec(g, P + "attn_output_b.weight", low, b_rows, b_cols)

    # ------------------------------------------------------------------ the stream
    def emit(key, vec, n=None):
        n = len(vec) if n is None else n
        s = 0.0
        for v in vec:
            s += v * v
        print("SUM %s n=%d sumsq=%.17g min=%.17g max=%.17g" % (key, len(vec), s, min(vec), max(vec)))
        for i in range(min(n, len(vec))):
            print("ORA %s %d %.17g" % (key, i, vec[i]))

    named = [("xn", xn), ("qlatn", qlatn), ("q_preheadrms", q_preheadrms), ("q_headrms", q_headrms),
             ("q", q), ("kv_norm", kv_norm), ("kv_roped", kv_roped), ("kv", kv),
             ("heads_attn", heads_attn), ("heads", heads), ("low", low), ("attn_out", attn_out)]
    if mode in ("hc", "layer"):
        # ds4.c:13793 — the attention half CLOSES with hc_post over the same residual and the same
        # post/comb the SAME hc_pre produced. This is the complete attention half of a real layer.
        after_attn_hc = hc_post(attn_out, residual_hc, post, comb, n_embd, n_hc)
        named = ([("hc_resid", residual_hc), ("hc_flat", hc_flat), ("hc_mix", hc_mix),
                  ("hc_post_w", post), ("hc_comb", comb), ("hc_cur", x_in)] + named
                 + [("after_attn_hc", after_attn_hc)])
    if mode == "layer":
        # ds4.c:11437 — the SECOND half. The layer's output is out_hc, the n_hc streams the next layer
        # receives. Nothing here is a probe: this is layer 0's real state for this token at this position.
        F = ffn_half(g, P, after_attn_hc, K, n_embd, n_hc, eps, hc_iters, hc_eps, token, il)
        print("FFN clamp %r selected %s weights %s"
              % (F["clamp"], F["selected"], ["%.9g" % w for w in F["expert_w"]]))
        named = named + [("ffn_flat", F["ffn_flat"]), ("ffn_mix", F["ffn_mix"]),
                         ("ffn_post_w", F["ffn_post_w"]), ("ffn_comb", F["ffn_comb"]),
                         ("ffn_cur", F["ffn_cur"]), ("ffn_normed", F["ffn_norm"]),
                         ("router_logits", F["router_logits"]), ("router_probs", F["router_probs"]),
                         ("expert_w", F["expert_w"]),
                         ("sh_gate", F["sh_gate"]), ("sh_up", F["sh_up"]), ("sh_mid", F["sh_mid"]),
                         ("shared", F["shared"]), ("moe", F["moe"]), ("ffn_out", F["ffn_out"]),
                         ("out_hc", F["out_hc"]),
                         ("selected", [float(v) for v in F["selected"]])]
        for i, (e, gt, up, mid, dn) in enumerate(F["per_expert"]):
            named = named + [("exp%d_gate" % i, gt), ("exp%d_up" % i, up),
                             ("exp%d_mid" % i, mid), ("exp%d_down" % i, dn)]
    for key, vec in named:
        emit(key, vec, 8)
    # the full vectors the harness compares elementwise, written to a side file for exactness
    outdir = os.environ.get("DSV4_ORACLE_OUT")
    if outdir:
        for key, vec in named:
            with open(os.path.join(outdir, "oracle-%s.f64" % key), "w") as fh:
                for v in vec:
                    fh.write("%.17g\n" % v)
    print("END")

if __name__ == "__main__":
    main()
