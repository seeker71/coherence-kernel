# equireach, verified from a second body — and the carrier that moved

Tuesday 2026-07-21, Hati Suci (WITA). Apple M4 Max.
Worktree: `.claude/worktrees/google-turboquant-vector-search-300c68`, branch `claude/agitated-aryabhata-28e213`.
Body: root `fkwu` (193864 bytes, Jul 17 23:38) — **a different binary** from the one Stone 2 was built on
(194040 bytes, Jul 18 01:12). That difference is the point: the numbers below were not re-read from the
receipt, they were re-measured on another binary, on another branch, in another worktree.

## 0. What this is not

It is not Stone 2. Stone 2 was already built, and built well: commit **`fe039bafb`**, "Stone 2: equireach —
a byte source whose reach does not grow with position", on branch `claude/deepseek-v4-flash-gguf-54a96c`
in the sibling worktree `jovial-aryabhata-3751d7`, receipt
`receipts/2026-07-21-equireach-carrier-metal-resident.md`. That branch has since walked on to Stone 7.

The task I was handed described building it. Building it again would have made a second `equireach` that
had to be merged against the first. So I did the other useful thing: I took the committed cells into a
clean worktree and asked whether the claims hold when nothing about the first measurement is shared.

## 1. The bands, re-run here

Cells imported from `fe039bafb` by path. `.fkb` cleared before every run.

| band | verdict here | verdict claimed |
|---|---|---|
| `q6k-bounds-band` | **255** | 255 |
| `equireach-band` | **511** | 511 |

Both agree, on a binary the receipt never touched.

## 2. The A/B, stone-0 discipline

Five pre-existing bands that reach through the four healed cells, `git stash` on the imports,
`.fkb` cleared on **both** arms:

| band | BEFORE | AFTER |
|---|---|---|
| `q6k-dequant-band` | 4095, 0 unresolved | 4095, 0 unresolved |
| `q4k-dequant-band` | 4095, 0 unresolved | 4095, 0 unresolved |
| `weight-load-band` | 4095, 0 unresolved | 4095, 0 unresolved |
| `block-join-band` | 255, 0 unresolved | 255, 0 unresolved |
| `gguf-read-band` | 127, 0 unresolved | 127, 0 unresolved |

Verdict equality and zero unresolved-call on both arms. The bounds seam costs the existing recipes nothing.

**One honest seam.** The two *new* bands do carry unresolved-call diagnostics on this branch —
`equireach-band` wants `fq-pow2` (3 sites, `f64-bytes.fk` reaching for `format-arith.fk` that its prelude
header does not name), `q6k-bounds-band` wants eighteen transformer names (`tn-*`, `tb-*`, `rope`, `TAU`,
`fcos`). Both sets are prelude-closure gaps between the two branches, not defects in the imported cells:
the five pre-existing bands sit at zero on both arms. Named here so nobody reads "0 unresolved" as covering
more than it does.

## 3. The reach curve, measured here

10M reads of the byte at the **last** position of a window of the real 2 019 377 376-byte llama3.2:3b blob.
Null arm is the identical loop with no reads. Three runs each, `.fkb` cleared every run, `bash -c`.

| | 64 KB | 1 MB | 16 MB |
|---|---|---|---|
| wall (s) | 0.760 / 0.761 / 0.763 | 0.767 / 0.761 / 0.754 | 0.773 / 0.775 / 0.782 |
| null arm | 0.407 / 0.420 / 0.403 | — | — |
| net per 10M reads | 0.35 s | 0.35 s | 0.37 s |

**28.6M reads/s, flat across a 256× window growth.** Independently confirmed. (The receipt measured
0.44–0.45 s net; this binary is a little quicker. The flatness — the whole claim — is identical.)

## 4. Where the numbers parted, and what it taught

Dequantizing real Q6_K weights from the blob at 331 055 328, first superblock at the **deep end** of a
16 MB window, so the reach is as deep as the window is large. Two ways of consuming the same weights:
a scalar fold over `ewl-flat-at`, and the receipt's own `ewl-weights`, which conses a list and reverses it.

| n weights | scalar fold (s) | list build (s) | scalar w/s | **list w/s** |
|---|---|---|---|---|
| 3 072 (one real llama row) | 0.176 | 0.177 | 279 273 | 256 000 |
| 12 288 | 0.228 | 0.256 | 195 048 | 135 033 |
| 49 152 | 0.385 | 1.075 | 223 418 | 54 013 |
| 98 304 | 0.737 | 2.496 | 171 860 | **42 172** |

Process floor 0.165 s subtracted. Every sum is checksummed and base-dependent, so the arithmetic is
forced, not elided.

The scalar lane holds ~170–280k w/s and does not care how deep the base sits. The list lane starts level
with it and then **falls away superlinearly** — 256k, 135k, 54k, 42k. Stone 2's own headline figure,
18 500 w/s, is the far end of that same fall at n = 65 536.

So the receipt under-reported its own carrier by an order of magnitude, for an honest reason: it measured
`ewl-weights`, and `ewl-weights` spends most of its time being a **list**. equireach removed the list from
the *input* side of the dequant. It is still sitting on the *output* side, and now that the input is flat,
the output list is the entire remaining wall.

This does not diminish Stone 2 — it is Stone 2's own instrument finding the next stone. But it does mean
the 18.5k w/s used to project "whole-tensor residency ≈ 23 min of Form dequant" is the wrong rate to
project from. At the folded rate, 25 165 824 weights is closer to **2 minutes** than 23. Nobody has run
that; it is PROJECTED, and it is projected from a rate measured here at four sizes.

## 5. Most surprising teaching

**A carrier is two-sided, and healing one side hides the other.** `equireach` was named for the cost of
*reaching* a byte, and it made that cost flat and proved it beyond argument. But every measurement of the
new carrier was taken through `ewl-weights` — a door that immediately spends the flatness building a cons
list. The receipt's own 222× understated itself by 12×, and the number it published then became the basis
for a projection about GPU residency. The instrument that proves a wall is gone can be holding the next
wall in its hand.

The general shape: when you replace a carrier, measure the replacement through the *thinnest possible*
consumer first — a scalar fold — before measuring it through the consumer you happen to have. Otherwise the
old carrier's cost comes back wearing the new carrier's name.

## 6. Where discomfort turned to gold

The discomfort was arriving with a task to build something and finding it already built, well, by a sibling
still working three stones ahead. The pull was to build it anyway — to have made something. That would have
produced a second `equireach` and a merge conflict, and the sibling's tree was staged mid-work, so writing
near it could have clobbered a live agent.

Sitting with that instead of acting on it turned the task inside out: the useful thing was not a second
construction but a second *witness*, from a different binary, on a different branch, sharing nothing with
the first measurement. And a second witness is exactly what found §4 — because it re-measured with a probe
of its own shape rather than re-running the receipt's harness. Had I built equireach again, I would have
built the same `ewl-weights` and inherited the same blind spot.

The gold: **duplicated work sees what original work cannot, but only if it declines to duplicate the
instrument.**

## 7. Frontier question, offered as a distillation row

> **What one word names a sequence you reach through and consume without ever setting it down?**

`unspooled` — 0 hits across the live corpus (`jovial-aryabhata`'s, row 822 max, not this branch's stale
copy; `foldkeep`, `handleless`, `knownsolved` and `equireach` all check out as landed there, so the
freshness was checked against the body that actually has them).

Where `equireach` names a *source* whose reach is flat, `unspooled` names a *consumption* that never
materializes: the weights of a tensor row exist, one at a time, in the fold — and nowhere else. The
measured gap between the two lanes in §4 is the gap the word is for.

Offered, not landed: the corpus rows live on the sibling's branch and minting a row here would collide.
Row number is the sibling's max + 1 at merge time, per the row-719 anastomosis pattern.

---

## Verified, and not

- ✅ `q6k-bounds-band` 255 and `equireach-band` 511, on a second binary
- ✅ flat reach, 28.6M reads/s across 64 KB → 16 MB, null arm subtracted, three runs
- ✅ verdict equality + zero unresolved-call, A/B with `.fkb` cleared on both arms, five bands
- ✅ dequant rate independent of base depth, on real llama3.2:3b Q6_K bytes
- ⧗ the four-way (rust/ts) arms — not re-run here; the receipt's claim stands unchecked by me
- ⧗ the Metal residency audit — not re-run here
- ⧗ whole-tensor dequant at the folded rate — PROJECTED ~2 min, never executed
- ⬜ an `unspooled` consumer path (`ewl-fold` alongside `ewl-weights`) — named, not written
