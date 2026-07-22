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

def rope_tail(x, n_head, head_dim, n_rot, pos, freq_base, freq_scale, inverse):   # ds4.c:10102
    # Layer 0 of FLASH is UNCOMPRESSED (ds4.c:1065 -> ratio 0), so ext_factor = 0 and attn_factor = 1:
    # no YaRN ramp, no magnitude scale. Only the TRAILING n_rot of each head rotate; the leading
    # head_dim - n_rot (the NOPE part) are untouched.
    n_nope = head_dim - n_rot
    theta_scale = freq_base ** (-2.0 / n_rot)
    sin_sign = -1.0 if inverse else 1.0
    for h in range(n_head):
        tail = h * head_dim + n_nope
        theta_extrap = float(pos)
        for i in range(0, n_rot, 2):
            theta = freq_scale * theta_extrap
            c = math.cos(theta)
            s = sin_sign * math.sin(theta)
            x0 = x[tail + i]
            x1 = x[tail + i + 1]
            x[tail + i] = x0 * c - x1 * s
            x[tail + i + 1] = x0 * s + x1 * c
            theta_extrap *= theta_scale
    return x

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

# ------------------------------------------------------------------ the core
def main():
    if len(sys.argv) < 4:
        raise SystemExit("usage: dsv4-mla-core-oracle.py <gguf> <token> <pos> [layer]")
    path, token, pos = sys.argv[1], int(sys.argv[2]), int(sys.argv[3])
    il = int(sys.argv[4]) if len(sys.argv) > 4 else 0
    g = Gguf(path)
    K = g.kv
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

    # --- the probe vector: the token's own F16 embedding (Stone 33/34/35's knownsolved input class).
    x0 = read_f16_row(g, "token_embd.weight", token, n_embd)

    # --- ds4.c:9981  attn_norm
    xn = rms_norm_weight(x0, read_f32(g, P + "attn_norm.weight", n_embd), eps)

    # --- ds4.c:10002  Q: q_a -> q_a_norm -> q_b -> per-head RMSNorm (no weight)
    qlat = mx8_matvec(g, P + "attn_q_a.weight", xn, q_rank, n_embd)
    qlatn = rms_norm_weight(qlat, read_f32(g, P + "attn_q_a_norm.weight", q_rank), eps)
    q = mx8_matvec(g, P + "attn_q_b.weight", qlatn, n_head * head_dim, q_rank)
    q_preheadrms = list(q)
    q = head_rms_norm(q, n_head, head_dim, eps)

    # --- ds4.c:10041  KV: attn_kv -> kv_a_norm
    kvraw = mx8_matvec(g, P + "attn_kv.weight", xn, head_dim, n_embd)
    kv = rms_norm_weight(kvraw, read_f32(g, P + "attn_kv_a_norm.weight", head_dim), eps)

    # --- ds4.c:13793  RoPE forward on q and on kv, then the kv row's fp8 + f16 round-trip
    q = rope_tail(q, n_head, head_dim, n_rot, pos, freq_base, freq_scale, False)
    kv = rope_tail(kv, n_head_kv, head_dim, n_rot, pos, freq_base, freq_scale, False)
    kv_prequant = list(kv)
    kv = fp8_kv_quantize_row(kv, head_dim, n_rot)
    kv = [f16_round(v) for v in kv]

    # --- ds4.c:10305  the sink-aware softmax; this diagnostic path attends to ONE row (itself)
    heads = attention_rows_one(q, [kv], read_f32(g, P + "attn_sinks.weight", n_head), n_head, head_dim)

    # --- ds4.c:13793  the heads are UN-roped (inverse rotation) before the output projection
    heads = rope_tail(heads, n_head, head_dim, n_rot, pos, freq_base, freq_scale, True)

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

    emit("xn", xn, 8)
    emit("qlatn", qlatn, 8)
    emit("q_preheadrms", q_preheadrms, 8)
    emit("q", q, 8)
    emit("kv_prequant", kv_prequant, 8)
    emit("kv", kv, 8)
    emit("heads", heads, 8)
    emit("low", low, 8)
    emit("attn_out", attn_out, 8)
    # the full vectors the harness compares elementwise, written to a side file for exactness
    outdir = os.environ.get("DSV4_ORACLE_OUT")
    if outdir:
        for key, vec in (("q", q), ("kv", kv), ("heads", heads), ("low", low), ("attn_out", attn_out)):
            with open(os.path.join(outdir, "oracle-%s.f64" % key), "w") as fh:
                for v in vec:
                    fh.write("%.17g\n" % v)
    print("END")

if __name__ == "__main__":
    main()
