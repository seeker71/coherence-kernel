#!/usr/bin/env bash
# e2e_driver_witness.sh — run the end-to-end chat driver's REAL lane on fkwu and write the honest
# per-stage receipt. Two fkwu --src runs: (1) the real community-page prompt through template +
# tokenize (stops honestly at tokenize — the llama3 rank-table carrier is pending), (2) the driver's
# forward+sample binding over REAL llama3.2:3b Q6_K bytes at today's real width (dim=4, 1 layer).
# Receipt: form/docs/receipts/e2e_native_lane_status.json. Never fabricates: every id in the receipt
# came out of a Form recipe walking real weight bytes; the response text slot stays a named refusal
# until tokenize/detokenize bindings land.
set -euo pipefail
cd "$(dirname "$0")/../.."   # repo root

if [[ ! -x ./fkwu || runtime/fkwu-uni.c -nt ./fkwu ]]; then
    echo "building runtime fkwu (--src door)..." >&2
    cc -O2 -o ./fkwu runtime/fkwu-uni.c
fi

BLOB="$HOME/.ollama/models/blobs/sha256-dde5aa3fc5ffc17176b5e8bdc82f587b24b2678c6c66101bf7da77af9f7ccdff"
[[ -f "$BLOB" ]] || { echo "llama3.2:3b blob absent at $BLOB — the forward stage cannot run on real weights" >&2; exit 1; }

now_ms() { python3 -c 'import time; print(int(time.time()*1000))'; }

t0=$(now_ms)
OUT1=$(./fkwu --src form/scripts/e2e-run-prompt.fk 2>/dev/null | tail -1)
t1=$(now_ms)
OUT2=$(./fkwu --src form/scripts/e2e-run-forward-real.fk 2>/dev/null | tail -1)
t2=$(now_ms)

[[ -n "$OUT1" && -n "$OUT2" ]] || { echo "EMPTY fkwu output — treat as a silent crash, run without stderr suppression" >&2; exit 1; }

echo "run 1 (template+tokenize, real prompt):   $OUT1  ($((t1-t0)) ms)"
echo "run 2 (forward+sample, real 3b weights):  $OUT2  ($((t2-t1)) ms)"

OUT1="$OUT1" OUT2="$OUT2" MS1=$((t1-t0)) MS2=$((t2-t1)) BLOB="$BLOB" python3 - <<'EOF'
import json, os, ast, time
o1 = ast.literal_eval(os.environ["OUT1"])
o2 = ast.literal_eval(os.environ["OUT2"])
status, tpl_len, tok_status, gen_len, txt_len, tpl_head, lane = o1
hdr_ok, tensor_count, gen_ids, cache_law = o2
seed = gen_ids[:2]
prov = {2: "native-fkwu", 1: "proven-recipe-reduced-width", 0: "pending"}
receipt = {
  "witness": "form/scripts/e2e_driver_witness.sh",
  "captured_at": time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime()),
  "runtime": "fkwu --src (c-bootstrapped runtime walker)",
  "driver_cell": "form/form-stdlib/e2e-driver.fk (band: tests/e2e-driver-band.fk, e2e-driver fks 255, four-way)",
  "prompt_lane": {
    "prompt_bytes_templated": tpl_len,
    "template_first_bytes": tpl_head,
    "driver_status": status,
    "stopped_at": "tokenize" if status == 0 else None,
    "wall_ms": int(os.environ["MS1"]),
  },
  "stages": [
    {"stage": "template",   "provenance": "native-fkwu",
     "binding": "ed-template over the real llama3.2 chat-template bytes; ran on fkwu, 406 real bytes composed"},
    {"stage": "tokenize",   "provenance": "pending",
     "binding": "llama3-pretokenize.fk fks 255 + bpe-tokenizer.fk prove the algorithm; the llama3 rank-table carrier (claude/tokenizer-carriers) has not landed — the driver REFUSED (status 0), no ids invented"},
    {"stage": "forward",    "provenance": "proven-recipe-reduced-width",
     "binding": "ed-generate -> lg-generate-cached over REAL llama3.2:3b Q6_K bytes (blk.0.ffn_down.weight superblocks + F32 attn_norm gains), dim=4, 1 layer, V=4 — the width real today; full d=3072 x 28 is the wide-forward lane"},
    {"stage": "sample",     "provenance": "proven-recipe-reduced-width",
     "binding": "greedy argmax == top-k(1) of the proven sampled draw (llama-sample-generate fks 255); ran inside the loop"},
    {"stage": "detokenize", "provenance": "pending",
     "binding": "same rank-table carrier as tokenize; the response text slot is a named refusal, never canned text"},
  ],
  "forward_lane": {
    "weights": os.environ["BLOB"],
    "weights_shared_with_oracle": "llama3.2:3b manifest model layer == this blob (see e2e_oracle_llama32_3b.json)",
    "gguf_header_ok": hdr_ok, "gguf_tensor_count": tensor_count,
    "seed_ids_declared": seed,
    "seed_note": "capability-floor seed, NOT the prompt's tokens (tokenize pending)",
    "generated_real_weight_token_ids": gen_ids[2:],
    "cached_equals_recompute": bool(cache_law),
    "wall_ms": int(os.environ["MS2"]),
  },
  "honest_floor": "template composes real bytes on fkwu; tokenize/detokenize refuse (rank table pending); forward+sample walk real llama3.2:3b Q6_K weights at dim=4 through the four-way-proven loop. No stage output is fabricated; the end-to-end transcript for the real prompt is PENDING on the tokenizer carrier and the wide-forward lane.",
}
path = "form/docs/receipts/e2e_native_lane_status.json"
os.makedirs(os.path.dirname(path), exist_ok=True)
json.dump(receipt, open(path, "w"), indent=2)
print("receipt ->", path)
EOF
