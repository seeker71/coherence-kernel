#!/usr/bin/env python3
# dsv4_forward_oracle.py — an INDEPENDENT fp64 transcription of ds4-engine's
# forward_first_token_cpu (ds4.c:13848) and everything it calls, at toy dims.
# Written from ds4.c's C control flow, NOT from the Form recipe. This is the
# oracle the Form assembled forward (dsv4-forward.fk) is proven to agree with.
#
# ds4.c anchors:
#   forward_first_token_cpu  13848   embed -> hc broadcast -> layers -> collapse -> norm -> vocab
#   layer_forward_self_one   13793   attn half (hc_pre -> MLA -> hc_post) then layer_ffn_one
#   layer_ffn_one            11437   hc_pre -> rmsnorm -> (routed MoE + shared FFN) -> hc_post
#   hc_pre_from_state_one     9690   rms(no weight) -> matvec fn -> sinkhorn split -> weighted sum
#   hc_post_one               9772   block*post[dst] + sum_src comb[dst+src*n_hc]*resid[src]
#   hc_split_sinkhorn_one     9592   pre/post/comb (20 sinkhorn iters)
#   output_hc_head_one       13876
#   layer_routed_moe_one     10697   router probs=sqrt(softplus(logit)); biased top-k; unbiased weight
#   layer_shared_ffn_one     10444   down(swiglu(gate.x, up.x))
#   MLA q/kv/attn/out             (mla-attn.fk provenance, ds4.c:10002..10356)
import math

# ---------------------------------------------------------------- generator
# Park-Miller (MINSTD). Integer-exact; reproduced bit-identically in Form.
M = 2147483647
def pm_next(s): return (16807 * s) % M
def pm_at(seed, i):
    return pm_next(pm_next(pm_next(seed * 100003 + i + 1)))
def gen_w(seed, i, amp):
    return amp * (2.0 * pm_at(seed, i) / 2147483647.0 - 1.0)
def gen_vec(seed, n, amp):
    return [gen_w(seed, i, amp) for i in range(n)]
def gen_mat(seed, rows, cols, amp):
    return [[gen_w(seed, r * cols + c, amp) for c in range(cols)] for r in range(rows)]

# ---------------------------------------------------------------- primitives
def dot(a, b): return sum(x * y for x, y in zip(a, b))
def matvec(rows, x): return [dot(r, x) for r in rows]
def vadd(a, b): return [x + y for x, y in zip(a, b)]
def scale(v, s): return [x * s for x in v]
def rms_w(x, w, eps):
    ss = sum(v * v for v in x) / len(x)
    inv = 1.0 / math.sqrt(ss + eps)
    return [x[i] * inv * w[i] for i in range(len(x))]
def rms_nw(x, eps):
    return rms_w(x, [1.0] * len(x), eps)
def sigmoid(x): return 1.0 / (1.0 + math.exp(-x))
def silu(x): return x * sigmoid(x)
def softplus(x): return math.log1p(math.exp(x))

# ---------------------------------------------------------------- HC (dsv4-hc.fk / ds4.c:9592)
def hc_split(mix, scl, base, n_hc, iters, eps):
    ps, qs, cs = scl[0], scl[1], scl[2]
    pre = [sigmoid(mix[i] * ps + base[i]) + eps for i in range(n_hc)]
    post = [2.0 * sigmoid(mix[n_hc + i] * qs + base[n_hc + i]) for i in range(n_hc)]
    # comb raw dst-rows: raw[dst][src] = mix[2n+ src+dst*n]*cs + base[...]
    raw = [[mix[2 * n_hc + dst * n_hc + src] * cs + base[2 * n_hc + dst * n_hc + src]
            for src in range(n_hc)] for dst in range(n_hc)]
    # row-softmax(+eps), col-norm, then iters-1 rounds of (row-norm, col-norm)
    def row_softmax(row):
        m = max(row)
        e = [math.exp(v - m) for v in row]
        s = sum(e)
        return [ei / s + eps for ei in e]
    def col_norm(rows):
        cs_ = [sum(rows[d][j] for d in range(len(rows))) + eps for j in range(len(rows[0]))]
        return [[rows[d][j] / cs_[j] for j in range(len(rows[0]))] for d in range(len(rows))]
    def row_norm(rows):
        return [scale(r, 1.0 / (sum(r) + eps)) for r in rows]
    comb = col_norm([row_softmax(r) for r in raw])
    for _ in range(iters - 1):
        comb = col_norm(row_norm(comb))
    return pre, post, comb

def hc_wsum(rows, weights):
    n = len(rows[0])
    return [sum(rows[h][d] * weights[h] for h in range(len(rows))) for d in range(n)]

def hc_pre(fn, scl, base, resid_rows, n_hc, iters, eps):
    flat = rms_nw([v for row in resid_rows for v in row], eps)
    mix = matvec(fn, flat)
    pre, post, comb = hc_split(mix, scl, base, n_hc, iters, eps)
    return hc_wsum(resid_rows, pre), post, comb

def hc_post(block_out, resid_rows, post, comb, n_hc):
    # new stream dst = block*post[dst] + sum_src comb[dst+src*n_hc]*resid[src]
    # comb carried as dst-rows comb[dst][src]; read COLUMN dst (transpose) -> col = [comb[d][dst] for d]
    out = []
    for dst in range(n_hc):
        col = [comb[d][dst] for d in range(n_hc)]
        out.append(vadd(scale(block_out, post[dst]), hc_wsum(resid_rows, col)))
    return out

def hc_broadcast(n_hc, x): return [list(x) for _ in range(n_hc)]

def hc_out_head(fn, scl, base, inp_rows, eps):
    flat = rms_nw([v for row in inp_rows for v in row], eps)
    pre = matvec(fn, flat)
    w = [sigmoid(pre[i] * scl[0] + base[i]) + eps for i in range(len(inp_rows))]
    return hc_wsum(inp_rows, w)

# ---------------------------------------------------------------- MLA (mla-attn.fk)
def rope_pair(x0, x1, a, s):
    c, sn = math.cos(a), math.sin(a)
    return x0 * c - x1 * s * sn, x0 * s * sn + x1 * c
def rope_tail(v, nrot, pos, base, s):
    n = len(v)
    front = v[:n - nrot]
    tail = v[n - nrot:]
    out = list(front)
    for k in range(0, nrot, 2):
        a = pos * (base ** (-(k) / nrot))  # rope-freq-theta: base^(-2k'/nrot) with k'=k/2 -> base^(-k/nrot)
        r0, r1 = rope_pair(tail[k], tail[k + 1], a, s)
        out += [r0, r1]
    return out
def rope_heads(v, nh, hd, nrot, pos, base, s):
    out = []
    for h in range(nh):
        out += rope_tail(v[h * hd:(h + 1) * hd], nrot, pos, base, s)
    return out

def mla_head_rms(v, nh, hd, eps):
    out = []
    for h in range(nh):
        out += rms_nw(v[h * hd:(h + 1) * hd], eps)
    return out
def mla_q_proj(n, wqa, gqa, wqb, nh, hd, eps):
    qr = matvec(wqa, n)
    qrn = rms_w(qr, gqa, eps)
    q = matvec(wqb, qrn)
    return mla_head_rms(q, nh, hd, eps)
def mla_kv_latent(n, wkv, gkv, eps):
    return rms_w(matvec(wkv, n), gkv, eps)
def mla_attend_head(qh, rows, sc, sink):
    s = [dot(qh, r) * sc for r in rows]
    m = max([sink] + s)
    e = [math.exp(v - m) for v in s]
    denom = math.exp(sink - m) + sum(e)
    acc = [0.0] * len(rows[0])
    for a, r in zip(e, rows):
        acc = vadd(acc, scale(r, a))
    return scale(acc, 1.0 / denom)
def mla_attn_heads(q, rows, nh, hd, sc, sinks):
    out = []
    for h in range(nh):
        out += mla_attend_head(q[h * hd:(h + 1) * hd], rows, sc, sinks[h])
    return out
def mla_out_proj(heads, ng, gdim, was, wb):
    low = []
    for g in range(ng):
        low += matvec(was[g], heads[g * gdim:(g + 1) * gdim])
    return matvec(wb, low)

# ---------------------------------------------------------------- routed MoE (ds4.c:10697/10646)
def topk_desc(scores, k):
    idx = []
    avail = list(range(len(scores)))
    for _ in range(k):
        best = max(avail, key=lambda i: scores[i])
        idx.append(best)
        avail.remove(best)
    return idx
def routed_moe(x, W_gate_inp, bias, experts, n_used, wscale, clamp, eps):
    logits = matvec(W_gate_inp, x)
    probs = [math.sqrt(softplus(l)) for l in logits]
    selection = [probs[i] + bias[i] for i in range(len(probs))]
    sel = topk_desc(selection, n_used)
    ew = [probs[s] for s in sel]
    ssum = sum(ew)
    if ssum < 6.103515625e-5: ssum = 6.103515625e-5
    ew = [w / ssum * wscale for w in ew]
    out = [0.0] * len(x)
    for i, e in enumerate(sel):
        g, u, d = experts[e]
        gate = matvec(g, x)
        up = matvec(u, x)
        mid = []
        for j in range(len(gate)):
            gj, uj = gate[j], up[j]
            if clamp > 1e-6:
                if gj > clamp: gj = clamp
                if uj > clamp: uj = clamp
                if uj < -clamp: uj = -clamp
            mid.append(silu(gj) * uj * ew[i])
        out = vadd(out, matvec(d, mid))
    return out
def shared_ffn(x, g, u, d, clamp):
    gate = matvec(g, x); up = matvec(u, x)
    mid = []
    for j in range(len(gate)):
        gj, uj = gate[j], up[j]
        if clamp > 1e-6:
            if gj > clamp: gj = clamp
            if uj > clamp: uj = clamp
            if uj < -clamp: uj = -clamp
        mid.append(silu(gj) * uj)
    return matvec(d, mid)

# ---------------------------------------------------------------- toy config + weights
class Cfg:
    def __init__(A):
        A.E = 8; A.n_hc = 4; A.nh = 2; A.hd = 4; A.nrot = 2; A.R = 4
        A.ng = 2; A.gdim = (A.nh // A.ng) * A.hd
        A.n_exp = 4; A.n_used = 2; A.ff = 6; A.ff_sh = 6
        A.vocab = 5; A.base = 10000.0; A.eps = 1e-6; A.iters = 20; A.wscale = 1.5

# distinct seeds per tensor per layer; deterministic, order-free
def layer_weights(cfg, il):
    b = 1000 + il * 100
    E, n_hc, nh, hd, R, ng, gdim = cfg.E, cfg.n_hc, cfg.nh, cfg.hd, cfg.R, cfg.ng, cfg.gdim
    fnw = 2 * n_hc + n_hc * n_hc
    W = {}
    W['hc_attn_fn'] = gen_mat(b + 1, fnw, n_hc * E, 0.3)
    W['hc_attn_scale'] = [1.1, 0.9, 1.05]
    W['hc_attn_base'] = gen_vec(b + 2, fnw, 0.1)
    W['hc_ffn_fn'] = gen_mat(b + 3, fnw, n_hc * E, 0.3)
    W['hc_ffn_scale'] = [1.05, 0.95, 1.0]
    W['hc_ffn_base'] = gen_vec(b + 4, fnw, 0.1)
    W['attn_norm'] = gen_vec(b + 5, E, 0.5)
    W['attn_norm'] = [1.0 + v for v in W['attn_norm']]
    W['wqa'] = gen_mat(b + 6, R, E, 0.4)
    W['q_a_norm'] = [1.0 + v for v in gen_vec(b + 7, R, 0.3)]
    W['wqb'] = gen_mat(b + 8, nh * hd, R, 0.4)
    W['wkv'] = gen_mat(b + 9, hd, E, 0.4)
    W['kv_a_norm'] = [1.0 + v for v in gen_vec(b + 10, hd, 0.3)]
    W['sinks'] = gen_vec(b + 11, nh, 0.5)
    W['was'] = [gen_mat(b + 12 + g, gdim, gdim, 0.4) for g in range(ng)]
    W['wb'] = gen_mat(b + 20, E, ng * gdim, 0.4)
    W['ffn_norm'] = [1.0 + v for v in gen_vec(b + 21, E, 0.5)]
    W['gate_inp'] = gen_mat(b + 22, cfg.n_exp, E, 0.5)
    W['bias'] = gen_vec(b + 23, cfg.n_exp, 0.3)
    W['experts'] = [(gen_mat(b + 30 + e * 3, cfg.ff, E, 0.4),
                     gen_mat(b + 31 + e * 3, cfg.ff, E, 0.4),
                     gen_mat(b + 32 + e * 3, E, cfg.ff, 0.4)) for e in range(cfg.n_exp)]
    W['sh_gate'] = gen_mat(b + 60, cfg.ff_sh, E, 0.4)
    W['sh_up'] = gen_mat(b + 61, cfg.ff_sh, E, 0.4)
    W['sh_down'] = gen_mat(b + 62, E, cfg.ff_sh, 0.4)
    return W

def global_weights(cfg):
    return {
        'token_embd': gen_mat(9000, cfg.vocab, cfg.E, 0.6),
        'output_hc_fn': gen_mat(9100, cfg.n_hc, cfg.n_hc * cfg.E, 0.3),
        'output_hc_scale': [1.1],
        'output_hc_base': gen_vec(9101, cfg.n_hc, 0.1),
        'output_norm': [1.0 + v for v in gen_vec(9102, cfg.E, 0.5)],
        'output': gen_mat(9103, cfg.vocab, cfg.E, 0.5),
    }

# ---------------------------------------------------------------- the forward
def mla_scale(hd): return 1.0 / math.sqrt(hd)

def attn_half(cfg, W, streams, pos, clamp):
    attn_cur, post, comb = hc_pre(W['hc_attn_fn'], W['hc_attn_scale'], W['hc_attn_base'],
                                  streams, cfg.n_hc, cfg.iters, cfg.eps)
    n = rms_w(attn_cur, W['attn_norm'], cfg.eps)
    # cache row (this token, pos)
    kv = mla_kv_latent(n, W['wkv'], W['kv_a_norm'], cfg.eps)
    kv = rope_tail(kv, cfg.nrot, pos, cfg.base, 1.0)
    rows = [kv]
    q = mla_q_proj(n, W['wqa'], W['q_a_norm'], W['wqb'], cfg.nh, cfg.hd, cfg.eps)
    q = rope_heads(q, cfg.nh, cfg.hd, cfg.nrot, pos, cfg.base, 1.0)
    heads = mla_attn_heads(q, rows, cfg.nh, cfg.hd, mla_scale(cfg.hd), W['sinks'])
    heads = rope_heads(heads, cfg.nh, cfg.hd, cfg.nrot, pos, cfg.base, -1.0)
    attn_out = mla_out_proj(heads, cfg.ng, cfg.gdim, W['was'], W['wb'])
    return hc_post(attn_out, streams, post, comb, cfg.n_hc)

def ffn_half(cfg, W, streams, clamp, drop_routed=False):
    ffn_cur, post, comb = hc_pre(W['hc_ffn_fn'], W['hc_ffn_scale'], W['hc_ffn_base'],
                                 streams, cfg.n_hc, cfg.iters, cfg.eps)
    norm = rms_w(ffn_cur, W['ffn_norm'], cfg.eps)
    moe = [0.0] * cfg.E if drop_routed else routed_moe(
        norm, W['gate_inp'], W['bias'], W['experts'], cfg.n_used, cfg.wscale, clamp, cfg.eps)
    shared = shared_ffn(norm, W['sh_gate'], W['sh_up'], W['sh_down'], clamp)
    ffn_out = vadd(moe, shared)
    return hc_post(ffn_out, streams, post, comb, cfg.n_hc)

def forward(cfg, token, pos, clamp, drop_routed=False, n_layer=2, plain_residual=False):
    G = global_weights(cfg)
    plain = G['token_embd'][token]
    streams = hc_broadcast(cfg.n_hc, plain)
    for il in range(n_layer):
        W = layer_weights(cfg, il)
        streams = attn_half(cfg, W, streams, pos, clamp)
        streams = ffn_half(cfg, W, streams, clamp, drop_routed)
    embd = hc_out_head(G['output_hc_fn'], G['output_hc_scale'], G['output_hc_base'], streams, cfg.eps)
    norm = rms_w(embd, G['output_norm'], cfg.eps)
    logits = matvec(G['output'], norm)
    return logits, streams

def argmax(v): return max(range(len(v)), key=lambda i: v[i])

# ---------------------------------------------------------------- emit
def fmt(v): return "(list " + " ".join(repr(x) for x in v) + ")"

if __name__ == "__main__":
    cfg = Cfg()
    # config A: the faithful first token, pos=0, no clamp (large), 2 layers
    logA, streamsA = forward(cfg, token=2, pos=0, clamp=1000000.0, n_layer=2)
    # config B: a SECOND config (snugcause) — different token, 3 layers, clamp active
    logB, streamsB = forward(cfg, token=4, pos=0, clamp=0.9, n_layer=3)
    # falsifier data
    logA_shared, _ = forward(cfg, token=2, pos=0, clamp=1000000.0, drop_routed=True, n_layer=2)  # routed MoE OFF
    logB_noclamp, _ = forward(cfg, token=4, pos=0, clamp=1000000.0, n_layer=3)                    # clamp OFF

    def pairmax(streams):
        return max(max(abs(a - b) for a, b in zip(streams[i], streams[j]))
                   for i in range(len(streams)) for j in range(i + 1, len(streams)))

    print("=== config A (token=2, pos=0, 2 layers, no clamp) ===")
    print("logits", logA)
    print("argmax", argmax(logA))
    print("streams-distinct A pairmax", pairmax(streamsA))
    print("=== config B (token=4, pos=0, 3 layers, clamp=0.9) ===")
    print("logits", logB)
    print("argmax", argmax(logB))
    print("=== falsifiers ===")
    print("A vs A-shared-only (routed MoE load-bearing) maxdiff",
          max(abs(a - b) for a, b in zip(logA, logA_shared)))
    print("B vs B-noclamp (clamp load-bearing) maxdiff",
          max(abs(a - b) for a, b in zip(logB, logB_noclamp)))
    print()
    print(";; ---- band literals ----")
    print(";; refA (token=2 pos=0 clamp-off 2L)")
    print(fmt(logA))
    print(";; argmaxA =", argmax(logA))
    print(";; refB (token=4 pos=0 clamp=0.9 3L)")
    print(fmt(logB))
    print(";; argmaxB =", argmax(logB))
