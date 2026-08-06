# The measured denominator — and the host that was measuring itself

Tuesday 2026-07-21 ~23:30 WITA, Hati Suci. Apple M4 Max, 128 GiB unified memory.
Worktree `.claude/worktrees/google-turboquant-vector-search-300c68`, branch `claude/agitated-aryabhata-28e213`
after merging `claude/deepseek-v4-flash-gguf-54a96c` (Stones 1–7) at `099df4a8a`.
Model: `llama3.2:3b`, blob `sha256-dde5aa3f…ccdff`, 2 019 377 376 bytes.

## 1. The merge

Stones 1–7 merged clean into this branch — 22 commits, no conflicts, my 3 receipt commits kept.
The merged body re-verified here, `.fkb` cleared:

```
qk-matvec-lane-band  255   ·  q6k-bounds-band  255  ·  equireach-band  511
llama-decode-msl-band 511  ·  homecoming-distillation-corpus-band  4095
```

`metal_first_token.sh 24` → **VERDICT PASS, 13 gates**, same 24 token ids, same text
(`" Paris. The capital of Italy is Rome. The capital of Spain is Madrid…"`).

## 2. The denominator, now MEASURED — and my own retraction

An hour ago I re-measured ollama and reported **56–88 tok/s**, concluded the program's quoted 157.83 was
**2.8× too high**, and put the body at **4.7× behind** instead of 12.9×. I committed that.

It was wrong. Those samples were taken **while this machine was running `metal_first_token.sh` and
`fkwu`** — I benchmarked the reference implementation against a host that my own benchmark was saturating.

`form/native/metal/ollama_oracle.sh`, idle host, warm server, 5 runs of ~245 tokens:

| run | decode tok/s |
|---|---|
| 1 | 146.71 |
| 2 | 96.52 |
| 3 | 142.94 |
| 4 | 139.62 |
| 5 | 138.09 |
| **median** | **139.62** |

| | quoted (Stone 5) | measured here | my retracted claim |
|---|---|---|---|
| ollama decode | 157.83 | **139.62** (spread 96.52–146.71) | 56–88 |
| error | ~13% high | — | **~2.4× low** |

**The quoted figure was very nearly right.** The stale constant was off by 13%; my correction of it was
off by 2.4× in the other direction, and mine was the one presented as a measurement.

Corrected standing, from the harness itself, `metal_first_token.sh 24`:

```
decode 10.056 of 139.62 tok/s  ->  13.9x behind
end-to-end 8.413 tok/s
```

## 3. What was built, so this cannot recur silently

Not a better constant — **expiry**.

| path | what |
|---|---|
| `form/native/metal/ollama_oracle.sh` | **new** — runs ollama on this host, warms first, takes the median of N ~245-token samples, and writes `.ollama-oracle.env` carrying the value **plus its date, host, run count and min/max spread** |
| `form/native/metal/metal_first_token.sh` | the two hardcoded constants are gone; the runner reads the oracle from the environment and prints the denominator **with when it was taken and how far it spread**. With no oracle it prints `NO DENOMINATOR — run ollama_oracle.sh first` rather than dividing by a number nobody can re-derive |

Live output, this run:

```
external denominator: ollama llama3.2:3b decode 139.62 tok/s
                      (measured 2026-07-21 23:29 WITA, 5 runs, spread 96.52-146.71)
vs the world (ollama MEASURED here 2026-07-21 23:29 WITA, decode spread 96.5-146.7):
     decode 10.056 of 139.62 tok/s (13.9x behind)  |  end-to-end 8.413 tok/s
```

The spread is now printed beside the median on purpose. A single figure would have hidden run 2's 96.52
— and run 2 is the whole lesson.

## 4. The prefill denominator is NOT trustworthy, and is now labelled as measured anyway

The oracle reports prefill 4685.41 tok/s (vs the quoted 640.94), which would put the body 364× behind
rather than 49×. **I do not believe that number and it should not be quoted.** ollama's prompt-eval rate
here is taken over a ~10-token prompt; run 1 gave 1187.76 against runs 3–5's 4500–5071. A per-token rate
over a batch that small is dominated by fixed cost. The decode figure has ~245 tokens under it and is
sound; the prefill figure has ten. It is written to the env file because hiding it would be worse, and
it is named here as untrustworthy so nobody builds on it. **Measuring prefill needs a long prompt and
that harness does not exist.**

## 5. Most surprising teaching

**A benchmark that runs on the machine it is benchmarking against will lie, and lie in the flattering
direction.** Every condition of my measurement was stated honestly — warm server, three runs, ~245-token
samples, and I even *wrote down* that a sibling `fkwu` was on the host and that I had not isolated it. I
listed it as a ⧗. Then I used the number anyway and published a conclusion resting entirely on it.

The failure was not ignorance of the confound. **The confound was named, filed as pending, and then
stepped over inside the same document.** A ⧗ next to the load-bearing number is not a disclosure — it is
a reason to stop, and I treated it as a reason to annotate.

Two receipts in a row now, same root: `fkwu --src` without declared-import expansion, then ollama on a
saturated host. **Both times the finding was real-looking and the harness was uncontrolled.** The
discipline that catches this is not more care in reading results; it is refusing to publish a number
whose conditions I could not state as *controlled*, rather than merely *known*.

## 6. Where discomfort turned to gold

The discomfort was writing §2 — retracting, publicly and in the tree, a correction I had made
confidently against someone else's number an hour earlier, having accused the program of "gating itself
against a denominator 2.8× too high." The sibling's figure was fine. Mine was the bad one, and I had
dressed it in three runs and a table.

What made it bearable was that the instruction was *use ollama* — build the thing that measures. Building
`ollama_oracle.sh` is what produced the 139.62 that refuted me. The repair and the refutation were the
same act, so the retraction arrived already carrying its own fix.

The gold: **the fastest way to discover you are wrong is to automate the measurement you were doing by
hand.** A number I produced twice by hand differed by 2.4×; the moment it had to be produced by a script
with its conditions recorded, the truth was immediate and unarguable. **Automation is not a convenience
here — it is the honesty mechanism.**

## 7. Frontier question, offered as a distillation row

> **What one word names a measurement corrupted by the act of taking it?**

`selfload` — the observer's own cost landing inside the observed quantity. Distinct from noise (unbiased)
and from `stalequote` (was true once): a `selfload` reading is wrong *at the moment of reading*, biased
in one direction, and reproducible — which is what makes it so convincing. Three runs agreed with each
other and all three were wrong together. The repair is not repetition but **quiescence**: a measurement
of an external reference states what else was running, or it does not count.

0-hit against the merged corpus (row 837 max after the anastomosis reunion). Offered with `unspooled`,
`backwall`, `restanding`, `stalequote`.

---

## Verified, and not

- ✅ Stones 1–7 merged clean; five bands green post-merge (255/255/511/511/4095)
- ✅ `metal_first_token.sh 24` VERDICT PASS, 13 gates, on the merged tree
- ✅ ollama `llama3.2:3b` decode 139.62 tok/s median, 5 runs, idle host, spread recorded
- ✅ the denominator is now measured-with-provenance in the harness, or absent and said to be absent
- ✅ my prior claim of 2.8× / 4.7× is retracted in the receipt that made it
- ⧗ the 96.52 outlier in run 2 — real, unexplained, not chased
- ⬜ a trustworthy **prefill** denominator — needs a long-prompt harness that does not exist
- ⬜ Metal occupancy — the 13.9× decode gap, still the whole remaining story, still untouched
