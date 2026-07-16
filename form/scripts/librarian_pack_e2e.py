#!/usr/bin/env python3
"""librarian_pack_e2e.py — end-to-end real-data witness for the TurboQuant lane.

The body's second-brain librarian, exercised on REAL data: every receipt in
receipts/ (the body's own documents) is embedded with the rag lane's named
bootstrap carrier (nomic-embed-text via ollama), packed through the SAME
algorithm as form/form-stdlib/turboquant-as-recipe.fk + rag-turboquant-lane.fk,
and queried with real questions. This carrier is HANDS ONLY: the Form cells are
the law, and --parity emits a Form program so fkwu can witness that carrier
packs and cell packs are code-for-code identical on sampled real vectors.

Metrics reported (no fabrication — all measured here):
  - index bytes: full float64 vs packed (norm-micro int + 2-bit codes)
  - top-1 / top-5 agreement between full-precision and packed ranking over
    title-derived queries (how much the 2-bit codes blur real retrieval)
  - hit@1 / hit@5 on hand-labeled natural-language questions, full vs packed

Usage (from repo root):
  python3 form/scripts/librarian_pack_e2e.py embed     # build/refresh cache
  python3 form/scripts/librarian_pack_e2e.py parity    # emit + run fkwu witness
  python3 form/scripts/librarian_pack_e2e.py eval      # measured report
"""

import json
import math
import os
import random
import subprocess
import sys
import time
import urllib.request

REPO = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
CACHE = os.environ.get(
    "LIB_E2E_CACHE",
    os.path.join(REPO, "form", "librarian-e2e-cache.json"))
OLLAMA = "http://localhost:11434/api/embeddings"
MODEL = "nomic-embed-text"
SEED = 20260716          # rtq-seed — index schema, shared with the Form cell
EMBED_CHARS = 1500       # retrieval granularity: title + head of each receipt

# ── the Form cell's algorithm, mirrored exactly (parity-witnessed) ──────────

def lcg(x):
    # turboquant-as-recipe.fk tqr-lcg: Park–Miller minstd, normalized domain
    return ((x % 2147483646) + 1) * 16807 % 2147483647

def draw(state, n):
    return (state // 65536) % n

def newton_sqrt(x):
    # tqr-sqrt: 40 iterations, y0 = 0.5x + 0.5, y' = 0.5*(y + x/y)
    if x <= 0.0:
        return 0.0
    y = 0.5 * x + 0.5
    for _ in range(40):
        y = 0.5 * (y + x / y)
    return y

def kac_rotate(v, seed):
    # tqr-kac: 5*d seeded rational-Givens steps, exact op order preserved
    v = list(v)
    d = len(v)
    state = seed
    for _ in range(5 * d):
        s1 = lcg(state); s2 = lcg(s1); s3 = lcg(s2); s4 = lcg(s3)
        i = draw(s1, d)
        j0 = draw(s2, d - 1)
        j = j0 + 1 if j0 >= i else j0
        u = 1 + draw(s3, 97)
        w = 1 + draw(s4, 97)
        c = float((u * u) - (w * w)) / float((u * u) + (w * w))
        s = (2.0 * (u * w)) / float((u * u) + (w * w))
        xi, xj = v[i], v[j]
        v[i] = (c * xi) + (s * xj)
        v[j] = (c * xj) - (s * xi)
        state = s4
    return v

def table_2bit(d):
    sigma = 1.0 / newton_sqrt(1.0 * d)
    return [-1.5104 * sigma, -0.4528 * sigma, 0.4528 * sigma, 1.5104 * sigma]

def nearest_code(x, table):
    # tensor-quant tq-nearest-walk: strictly-less replaces → lowest index wins ties
    best_i, best_d = 0, 1000000000.0
    for i, t in enumerate(table):
        dist = abs(x - t)
        if dist < best_d:
            best_i, best_d = i, dist
    return best_i

def round_half_away(x):
    # the kernels' half-away round (Python round() is banker's — not the law)
    return math.floor(x + 0.5) if x >= 0 else -math.floor(-x + 0.5)

def dot(a, b):
    s = 0.0
    for x, y in zip(a, b):
        s += x * y
    return s

def pack_vec(v):
    # rag-turboquant-lane.fk rtq-pack-vec: [norm-microunits] + codes
    n = newton_sqrt(dot(v, v))
    unit = v if n <= 0.0 else [x * (1.0 / n) for x in v]
    rot = kac_rotate(unit, SEED)
    table = table_2bit(len(v))
    return [round_half_away(n * 1000000.0)] + [nearest_code(x, table) for x in rot]

def score_packed(qrot, qn_ignored, pack, table):
    # rtq-score-rot: stored-norm × ⟨q̂rot, decoded⟩  (estimates ⟨q̂, x⟩)
    norm = pack[0] / 1000000.0
    dec = [table[c] for c in pack[1:]]
    return norm * dot(qrot, dec)

def score_full(q_unit, x):
    return dot(q_unit, x)

def unit(v):
    n = newton_sqrt(dot(v, v))
    return v if n <= 0.0 else [x * (1.0 / n) for x in v]

# ── real data: the receipts shelf ───────────────────────────────────────────

def receipt_docs():
    rdir = os.path.join(REPO, "receipts")
    docs = []
    for name in sorted(os.listdir(rdir)):
        if not name.endswith(".md"):
            continue
        path = os.path.join(rdir, name)
        with open(path, encoding="utf-8", errors="replace") as f:
            text = f.read()
        docs.append((name, text[:EMBED_CHARS]))
    return docs

def embed(text):
    body = json.dumps({"model": MODEL, "prompt": text}).encode()
    req = urllib.request.Request(
        OLLAMA, data=body, headers={"Content-Type": "application/json"})
    with urllib.request.urlopen(req, timeout=120) as resp:
        return json.load(resp)["embedding"]

def cmd_embed():
    docs = receipt_docs()
    cache = {}
    if os.path.exists(CACHE):
        with open(CACHE) as f:
            cache = json.load(f)
    t0 = time.time()
    fresh = 0
    for name, text in docs:
        if name in cache:
            continue
        cache[name] = embed(text)
        fresh += 1
        if fresh % 50 == 0:
            print(f"  embedded {fresh} … ({time.time()-t0:.0f}s)", flush=True)
            with open(CACHE, "w") as f:
                json.dump(cache, f)
    with open(CACHE, "w") as f:
        json.dump(cache, f)
    dim = len(next(iter(cache.values()))) if cache else 0
    print(f"embedded: {len(cache)} receipts, dim={dim}, "
          f"{fresh} fresh in {time.time()-t0:.0f}s -> {CACHE}")

# ── parity witness: the Form cell must produce the SAME pack ────────────────

def cmd_parity():
    with open(CACHE) as f:
        cache = json.load(f)
    names = sorted(cache.keys())
    rng = random.Random(SEED)
    picks = [names[rng.randrange(len(names))] for _ in range(3)]
    # shared input = floats rounded to 9 decimals (both sides consume these)
    ok_all = True
    for pick in picks:
        v = [round(x, 9) for x in cache[pick]]
        carrier = pack_vec(v)
        lits = " ".join(f"{x:.9f}" for x in v)
        prog = (
            "(do (let v (list " + lits + "))\n"
            "    (let p (rtq-pack-vec v))\n"
            "    p)\n")
        tmp = os.path.join(REPO, "form", "parity-witness.fk")
        parts = ["core.fk", "tensor-quant.fk", "turboquant-as-recipe.fk",
                 "rag-retrieve.fk", "rag-turboquant-lane.fk"]
        src = ""
        for part in parts:
            with open(os.path.join(REPO, "form", "form-stdlib", part)) as f:
                src += f.read() + "\n"
        with open(tmp, "w") as f:
            f.write(src + prog)
        t0 = time.time()
        # invoke with a BARE relative name from form/ — the path-prefixed form
        # trips the fk_fkb truncated-string diagnostic seam (receipted 2026-07-16)
        out = subprocess.run(
            ["../fkwu", "--src", os.path.basename(tmp)],
            capture_output=True, text=True, timeout=1800,
            cwd=os.path.join(REPO, "form"))
        last = out.stdout.strip().splitlines()[-1] if out.stdout.strip() else ""
        cell = [int(t) for t in
                last.strip("[]").replace(",", " ").split()] if last else []
        match = cell == carrier
        ok_all = ok_all and match
        print(f"parity {pick}: dim={len(v)} fkwu={time.time()-t0:.1f}s "
              f"match={'YES' if match else 'NO'}")
        if not match:
            print(f"  carrier[:6]={carrier[:6]}  cell[:6]={cell[:6]}")
        for ext in (".fk", ".fkb", ".sym"):
            p = tmp.replace(".fk", ext)
            if os.path.exists(p):
                os.remove(p)
    print("PARITY:", "WITNESSED — carrier == cell on real vectors"
          if ok_all else "FAILED — carrier is NOT the law; fix before eval")
    sys.exit(0 if ok_all else 1)

# ── labeled real questions (written blind to rankings, before eval ran) ─────

LABELED = [
    ("why did a stale fkwu binary silently cost a day of false discoveries",
     "2026-07-01-stale-binary-root-cause.md"),
    ("the first token the body ever generated natively",
     "2026-07-02-first-native-token.md"),
    ("ingesting what is healthy from microsoft's memora memory system",
     "2026-07-03-frontier-ingest-memora.md"),
    ("the offer to serve sema through a chatgpt plugin",
     "2026-07-05-chatgpt-plugin-offer.md"),
    ("transmuting a satsang gathering into executable practice",
     "2026-07-04-satsang-transmute.md"),
    ("brain to text typing decoded from neural signals ingest",
     "2026-07-02-frontier-ingest-brain2qwerty-dspark.md"),
    ("speech fingerprints for recognizing who is speaking",
     "2026-07-05-frontier-ingest-speech-fingerprints.md"),
    ("google turboquant vector quantization comes home as a cell",
     "2026-07-16-frontier-ingest-turboquant.md"),
    ("the packed third ranking lane and a typescript divergence",
     "2026-07-16-turboquant-rag-lane.md"),
    ("cross voice eighty percent and the two levers",
     "2026-07-02-cross-voice-80-two-levers.md"),
    ("the decoder forward pass matching bit for bit",
     "2026-06-30-decoder-forward-bitexact.md"),
    ("floats arriving in source programs and the harvest",
     "2026-06-29-src-floats-and-harvest.md"),
]

def cmd_eval():
    with open(CACHE) as f:
        cache = json.load(f)
    names = sorted(cache.keys())
    vecs = {n: cache[n] for n in names}
    d = len(vecs[names[0]])
    table = table_2bit(d)
    t0 = time.time()
    packs = {n: pack_vec(vecs[n]) for n in names}
    t_pack = time.time() - t0

    full_bytes = len(names) * d * 8
    packed_bytes = len(names) * (8 + (2 * d) // 8)

    def rank_full(qv, k):
        qu = unit(qv)
        scored = sorted(names, key=lambda n: -score_full(qu, vecs[n]))
        return scored[:k]

    def rank_packed(qv, k):
        qrot = kac_rotate(unit(qv), SEED)
        scored = sorted(names, key=lambda n: -score_packed(qrot, 0, packs[n], table))
        return scored[:k]

    # agreement over title-derived queries (first heading line of each receipt)
    rng = random.Random(SEED)
    sample = [names[i] for i in rng.sample(range(len(names)), 100)]
    agree1 = agree5 = 0
    for n in sample:
        with open(os.path.join(REPO, "receipts", n), encoding="utf-8",
                  errors="replace") as f:
            title = f.readline().lstrip("# ").strip()
        qv = embed(title)
        f1, p1 = rank_full(qv, 5), rank_packed(qv, 5)
        agree1 += (f1[0] == p1[0])
        agree5 += len(set(f1) & set(p1)) / 5.0
    # labeled natural questions
    hits = {"full@1": 0, "full@5": 0, "packed@1": 0, "packed@5": 0}
    for q, label in LABELED:
        qv = embed(q)
        f5, p5 = rank_full(qv, 5), rank_packed(qv, 5)
        hits["full@1"] += (f5[0] == label)
        hits["full@5"] += (label in f5)
        hits["packed@1"] += (p5[0] == label)
        hits["packed@5"] += (label in p5)

    print(json.dumps({
        "docs": len(names), "dim": d,
        "full_index_bytes": full_bytes, "packed_index_bytes": packed_bytes,
        "compression_x": round(full_bytes / packed_bytes, 2),
        "pack_time_s": round(t_pack, 2),
        "title_queries": len(sample),
        "top1_agreement_pct": round(100.0 * agree1 / len(sample), 1),
        "top5_overlap_pct": round(100.0 * agree5 / len(sample), 1),
        "labeled_questions": len(LABELED),
        "full_hit@1": hits["full@1"], "full_hit@5": hits["full@5"],
        "packed_hit@1": hits["packed@1"], "packed_hit@5": hits["packed@5"],
    }, indent=2))

if __name__ == "__main__":
    {"embed": cmd_embed, "parity": cmd_parity, "eval": cmd_eval}[sys.argv[1]]()
