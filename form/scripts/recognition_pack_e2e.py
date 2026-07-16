#!/usr/bin/env python3
"""recognition_pack_e2e.py — real-data witness: the mic fleet's speaker book and
the face store, packed through the parity-witnessed TurboQuant lane.

READ-ONLY on the live stores under ~/.coherence-network (the running body's
field; a launchd watcher is actively growing the speaker book). All person
names are anonymized to speaker-N / profile-N BEFORE anything is printed —
the private-circle law holds in every artifact this emits.

Pack algorithm = form/scripts/librarian_pack_e2e.py's mirrored cell functions
(parity-witnessed against fkwu on real vectors, 2026-07-17). numpy is absent
in this environment, so evaluation uses stated random samples for
tractability; sizes are printed, never hidden.

Usage: python3 form/scripts/recognition_pack_e2e.py audio|face
"""

import json
import os
import random
import sys
import time

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from librarian_pack_e2e import (  # noqa: E402
    pack_vec, kac_rotate, table_2bit, unit, dot, newton_sqrt, SEED)

HOME = os.path.expanduser("~/.coherence-network")


def decode(pack, table):
    norm = pack[0] / 1000000.0
    return norm, [table[c] for c in pack[1:]]


def packed_score(qrot, pack, table):
    norm, dec = decode(pack, table)
    return norm * dot(qrot, dec)


def cmd_audio():
    sdir = os.path.join(HOME, "speakers", "samples")
    rows = []
    for name in sorted(os.listdir(sdir)):
        if not name.endswith(".json"):
            continue
        with open(os.path.join(sdir, name)) as f:
            row = json.load(f)
        if row.get("person") and row.get("embedding"):
            rows.append((row["person"], row["embedding"]))
    people = sorted({p for p, _ in rows})
    anon = {p: f"speaker-{i+1}" for i, p in enumerate(people)}
    counts = {anon[p]: sum(1 for q, _ in rows if q == p) for p in people}
    d = len(rows[0][1])
    majority = max(counts.values()) / len(rows)

    t0 = time.time()
    packs = [pack_vec(v) for _, v in rows]
    t_pack = time.time() - t0
    table = table_2bit(d)

    rng = random.Random(SEED)
    n_q = 300
    queries = rng.sample(range(len(rows)), n_q)
    acc_full = acc_packed = agree = 0
    for qi in queries:
        qv = rows[qi][1]
        qu = unit(qv)
        qrot = kac_rotate(qu, SEED)
        best_f, bf = -1, -1e18
        best_p, bp = -1, -1e18
        for gi, (_, gv) in enumerate(rows):
            if gi == qi:
                continue
            sf = dot(qu, gv)
            if sf > bf:
                best_f, bf = gi, sf
            sp = packed_score(qrot, packs[gi], table)
            if sp > bp:
                best_p, bp = gi, sp
        acc_full += (rows[best_f][0] == rows[qi][0])
        acc_packed += (rows[best_p][0] == rows[qi][0])
        agree += (best_f == best_p)

    full_bytes = len(rows) * d * 8
    packed_bytes = len(rows) * (8 + (2 * d) // 8)
    print(json.dumps({
        "store": "speakers/samples (labeled only)",
        "samples": len(rows), "dim": d,
        "speakers": counts, "majority_baseline_pct": round(100 * majority, 1),
        "loo_queries": n_q,
        "full_top1_accuracy_pct": round(100 * acc_full / n_q, 1),
        "packed_top1_accuracy_pct": round(100 * acc_packed / n_q, 1),
        "full_vs_packed_top1_agreement_pct": round(100 * agree / n_q, 1),
        "full_bytes": full_bytes, "packed_bytes": packed_bytes,
        "compression_x": round(full_bytes / packed_bytes, 2),
        "pack_time_s": round(t_pack, 1),
    }, indent=2))


def cmd_face():
    path = os.path.join(HOME, "face-training", "samples.jsonl")
    labeled, pooled = [], []
    with open(path) as f:
        for line in f:
            row = json.loads(line)
            emb = row.get("embedding")
            if not emb:
                continue
            (labeled if row.get("person") else pooled).append(emb)
    d = len(labeled[0])

    rng = random.Random(SEED)
    gallery_idx = rng.sample(range(len(labeled)), 1000)
    gallery = [labeled[i] for i in gallery_idx]
    t0 = time.time()
    gpacks = [pack_vec(v) for v in gallery]
    t_pack = time.time() - t0
    table = table_2bit(d)

    # nearest-sample agreement: live query vs packed vs full gallery
    n_q = 100
    qidx = rng.sample([i for i in range(len(labeled)) if i not in set(gallery_idx)], n_q)
    agree = 0
    score_mae = 0.0
    for qi in qidx:
        qu = unit(labeled[qi])
        qrot = kac_rotate(qu, SEED)
        best_f, bf = -1, -1e18
        best_p, bp = -1, -1e18
        for gi in range(len(gallery)):
            sf = dot(qu, gallery[gi])
            if sf > bf:
                best_f, bf = gi, sf
            sp = packed_score(qrot, gpacks[gi], table)
            if sp > bp:
                best_p, bp = gi, sp
        agree += (best_f == best_p)
        score_mae += abs(bf - bp)

    full_bytes = len(labeled) * d * 8
    packed_bytes = len(labeled) * (8 + (2 * d) // 8)
    print(json.dumps({
        "store": "face-training/samples.jsonl",
        "labeled_samples": len(labeled), "pooled_samples": len(pooled),
        "profiles": 1, "dim": d,
        "gallery": len(gallery), "queries": n_q,
        "full_vs_packed_top1_agreement_pct": round(100 * agree / n_q, 1),
        "best_score_mae": round(score_mae / n_q, 4),
        "full_bytes_whole_store": full_bytes,
        "packed_bytes_whole_store": packed_bytes,
        "compression_x": round(full_bytes / packed_bytes, 2),
        "pack_time_s_gallery": round(t_pack, 1),
    }, indent=2))


if __name__ == "__main__":
    {"audio": cmd_audio, "face": cmd_face}[sys.argv[1]]()
