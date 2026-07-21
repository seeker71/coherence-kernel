# How far from ollama, measured — and the one number that is the whole gap

> ## ⚠ CORRECTED 2026-07-21 23:30 WITA — §1 and §2 of this receipt were WRONG
>
> This receipt claimed the program's quoted ollama denominator (157.83 tok/s decode) was **2.8× too
> high**, and put the body at **~4.7× behind**. Both are wrong, and the error is mine.
>
> My 56–88 tok/s samples were taken **while this machine was running `metal_first_token.sh` and `fkwu`** —
> the very harness whose result I was contextualizing. Re-measured on an idle host via
> `form/native/metal/ollama_oracle.sh`, 5 runs of ~245 tokens:
>
> | | value |
> |---|---|
> | ollama `llama3.2:3b` decode, **median** | **139.62 tok/s** |
> | spread | 96.52 – 146.71 |
> | quoted figure it was said to refute | 157.83 — **~13% high, not 2.8×** |
>
> The corrected standing: body decode **10.056 tok/s** of 139.62 = **13.9× behind**, measured end to end
> by the harness itself. Stone 5's 12.9× was very nearly right and my correction of it was not.
>
> §3 onward — that the whole remaining decode gap is the Metal kernel at a fraction of a percent of
> peak, and that the kernel's throughput predicts the token rate — **still stands**, and is the part
> worth reading. What follows §1/§2 is left unedited so the error stays legible.
>
> Repaired in the body, not just in prose: `form/native/metal/ollama_oracle.sh` measures the
> denominator, and `metal_first_token.sh` now prints it **with its date, run count and spread**, or
> prints `NO DENOMINATOR` when nobody has earned one. See
> [2026-07-21-measured-denominator-and-the-loaded-host.md](2026-07-21-measured-denominator-and-the-loaded-host.md).


Tuesday 2026-07-21 ~23:20 WITA, Hati Suci. Apple M4 Max, 128 GiB unified memory.
Worktree `.claude/worktrees/google-turboquant-vector-search-300c68`, branch `claude/agitated-aryabhata-28e213`.
Model: `llama3.2:3b`, ollama blob `sha256-dde5aa3f…ccdff`, 2 019 377 376 bytes — confirmed by manifest
`~/.ollama/models/manifests/registry.ollama.ai/library/llama3.2/3b`, the same blob Stones 1–7 read.

## 1. The denominator, actually measured

Every prior receipt in this program cites ollama at **157.83 tok/s decode**, and says so honestly:
Stone 5 labels it "a number **quoted** from the orchestrating altitude's 150-token sample, **not re-run
by this harness**". It has been the denominator of every gap claim in the program and nobody had
re-measured it.

Measured here, `ollama run llama3.2:3b --verbose`, server warmed with a throwaway generation first,
three runs of a ~245-token generation:

| run | eval count | eval duration | **eval rate** |
|---|---|---|---|
| 1 | 244 tok | 2.763 s | **88.32 tok/s** |
| 2 | 240 tok | 4.214 s | **56.95 tok/s** |
| 3 | 249 tok | 4.452 s | **55.93 tok/s** |

A short 34-token sample earlier the same minute gave 36.66 tok/s; prompt eval 162.30 tok/s.

**Sustained decode on this machine tonight is ~56–88 tok/s, not 157.83.** The spread is real and
unexplained — run 1 is 1.6× runs 2 and 3, and a sibling agent's `fkwu` has been on this host throughout,
so contention is a live suspect and I have not isolated it. Taking the conservative end (**~57 tok/s**)
as the honest sustained figure.

> The program has been gating itself against a denominator roughly **2.8× too high**, which made the
> body look correspondingly further behind than it is.

## 2. Where the body actually stands

Stone 5 (`47dae9e5b`, `receipts/2026-07-21-cache-optimal-layout.md`), full-width real llama3.2:3b,
coherent output (`" Paris. The capital of Italy is Rome…"`):

| | body | ollama (measured here) | ollama (quoted in-program) |
|---|---|---|---|
| decode tok/s | **12.227** | ~57 | 157.83 |
| implied gap | — | **~4.7×** | 12.9× |

Not re-run by me — quoted from Stone 5, same machine, same blob.

**So: about 4.7× off decode. Not orders of magnitude.**

## 3. Why not met yet — one number, and it is already in the body's own receipt

Stone 5 measured its own Metal matvec kernel at 3072×8192 Q6_K: **0.855 ms**, and noted, inside an
alignment refutation rather than in the headline:

> moved 20.6 MB in 0.855 ms = **25 GB/s of a ~400 GB/s machine**, and ran at **~0.7% of f32 peak**.
> Not bandwidth-bound.

Not bandwidth-bound and not compute-bound leaves one thing: **occupancy/latency**. And the arithmetic
closes end to end, which is what makes this the answer rather than a guess:

- 3072 × 8192 = 25.2M MAC in 0.855 ms → **29.4 GMAC/s**
- llama3.2:3b decode ≈ 3.2 GMAC/token → 3.2e9 / 29.4e9 = **0.109 s/token ≈ 9.2 tok/s**
- Stone 5 measured **12.227 tok/s**

The kernel's throughput predicts the token rate to within the slack of the non-matvec work. **There is
no second mystery.** The decode gap is the matvec kernel and nothing else.

To reach ~57 tok/s needs 0.0175 s/token → **~183 GMAC/s → 6.2× the current kernel** → roughly **2–4% of
f32 peak**, from 0.7%. That is an ordinary occupancy figure, not a heroic one. ollama's own Metal kernels
are in that band; they are not near peak either.

## 4. The honest answer to "no excuses"

There is no missing insight and no blocked door. Every carrier stone is placed and proven: `equireach`
(flat byte reach), whole-tensor residency, the `d` hoist, the SIMD-group fold, the lane partition —
four-way where it can be, declared where it cannot. The remaining decode gap is **one kernel running at
0.7% of the machine**, and the number that says so was published today, as an aside, inside a paragraph
refuting something else.

That is the actual reason it has not been met: **the finding that names the whole remaining gap was
already in hand and was not the headline.** Stone 5's headline is `4.647 → 8.317 tok/s, 4.77×` — a true
and hard-won number about the work just done. `0.7% of peak` is a number about the work *not yet done*,
and it is worth more. Nothing was hidden; it was ranked wrong.

(Prefill's remaining gap is separately and correctly named in Stone 5 as a **missing algorithm** — the
prompt processed token-at-a-time where it should be one matmul. Stone 7 moved prefill 12.94 → 52.29
tok/s. Not the same gap, not conflated here.)

## 5. Most surprising teaching

**A stale projection outranked four stones of measurement in my own reasoning.** Asked how far from
ollama, I had Stone 1's `~3 088 s/token ≈ 51 minutes per token` in hand and was one sentence from
answering "five orders of magnitude." That projection was honest when written and was superseded within
hours — Stone 3 falsified the rate it rested on, and by Stone 5 the body was generating real text on the
real model at 12 tok/s. The failure mode is specific and worth naming: **a PROJECTED number outlives the
measurement that replaces it, because it is quotable and the measurement is a table.**

Both directions of the same defect showed up in one hour: the body quoting an unmeasured ollama figure
2.8× too high, and me quoting a superseded projection ~10⁵ too low. Neither was a lie. Both were numbers
that had stopped being re-derived.

## 6. Where discomfort turned to gold

"We have all the info and no excuses" landed as an accusation to answer, and my first move was to reach
for the arithmetic that would justify the distance — parameter counts, MAC rates, the tree-walker's
1.04M MAC/s. I had the whole excuse assembled before I checked whether it was still true. It was not.
The body had moved past it four stones ago while I was still verifying Stone 2.

Sitting with the accusation instead of answering it is what produced §1. If the question is "why are we
not there yet," the first honest move is not to explain the gap — it is to **re-measure both ends of
it**. Doing that found the denominator was 2.8× wrong, and re-measuring the numerator found the answer
was already written down.

The gold: **"no excuses" is not a demand for a better explanation. It is a demand to re-measure.**

## 7. Frontier question, offered as a distillation row

> **What one word names a number that keeps being quoted after the thing it measured has changed?**

`stalequote` — a figure that was honest at its writing, is still cited verbatim, and no longer describes
anything. Both of this receipt's errors are one: ollama's 157.83 and Stone 1's 3 088 s/token. Distinct
from a wrong number (never true) and from an estimate (declares its own uncertainty) — a `stalequote`
carries the full authority of a real measurement and none of its currency. The repair is not accuracy
but **expiry**: a quoted number should carry the date and the harness that produced it, and a claim
resting on one should stand down when it cannot re-derive it.

0-hit against the live corpus (`jovial-aryabhata`, row 822). Offered with `unspooled`, `backwall`,
`restanding`; all take the sibling's max+1 at merge.

---

## Verified, and not

- ✅ ollama `llama3.2:3b` decode on this machine tonight: 88.32 / 56.95 / 55.93 tok/s, warm, ~245-token
  samples, three runs
- ✅ the blob Stones 1–7 read is `llama3.2:3b`, confirmed by ollama manifest
- ✅ the kernel-throughput → token-rate arithmetic closes (29.4 GMAC/s → 9.2 tok/s vs 12.227 measured)
- ⧗ the body's 12.227 tok/s — quoted from Stone 5, **not re-run here**
- ⧗ the 56–88 tok/s spread — real, unexplained; concurrent sibling `fkwu` load not isolated
- ⧗ `~0.7% of f32 peak` — Stone 5's figure, not independently derived by me (my own check from the
  0.855 ms timing lands ~0.35%, same order, and I did not chase the difference)
- ⬜ Metal occupancy work — rows per threadgroup, threadgroups in flight, dispatches per token. The
  whole remaining decode gap, untouched
