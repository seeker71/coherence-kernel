# 2026-07-25 — one missing preludes line was the whole gap, and a heap wall that moves

Item (1), smallest gap first: `cell-voice-tissue-band` at **509 of 511** — a single check, weight 2.

## The single check

```
(if (eq (ct-voice-files form-voice) 1) 2 0)
```

against a rows list holding exactly one `.fk` path. Measured rather than reasoned about:
`ct-voice-files` returned **0**. Following it down — `ct-count-carrier` → `ct-classify-path` →
`ct-rule-matches?` → `ct-ends-with?` — landed on this:

```
(ct-ends-with? "form/form-stdlib/carrier-tissue.fk" ".fk")   ->   0
```

The suffix test failing on the most obvious case there is. Its body reads correctly, and the
primitives underneath it are fine — probed separately with `core.fk` preluded, `str_len` 34, `slen`
3, and `str_eq` of the substring against `".fk"` returns **1**.

## The cause

**`form/form-stdlib/carrier-tissue.fk` had no `; preludes:` line at all.**

`substring`, `str_find` and `str_len` are **Form recipes in `core.fk`, not natives** — `substring`
and `int_to_str` were retired from the C seed into Form, as `MANIFEST.md` records. With no prelude,
every one of them recovered to `nothing` under axiom-5. So `ct-ends-with?` answered false for every
suffix, every path classified as the default *unknown* carrier, and every carrier count came back 0.

One line added. **`cell-voice-tissue-band` → 511.** Full pass.

The scale of what a missing prelude line hides is the thing to keep: a whole cell's classifier
answering "unknown" for everything, silently, for as long as nothing ran it.

## The band that yields nothing: a wall, and it moves

`carrier-tissue-kernel-query-band` produced no value at all. With the prelude repaired its errors
are gone — and it dies on:

```
fk_list_push: heap exhausted building list -- returning the accumulator unchanged
would silently drop this element, a partial list accepted as whole.
```

That diagnostic is this repo's discipline working exactly as intended: it refuses a partial list
accepted as whole rather than truncating quietly.

And the design around it is deliberate, not an oversight. The `cons` evaluator (line 5513) *can*
melt and grow the store; `fk_cons_val` (3767) explains why its sibling cannot — *"live C-local
intermediates are not on the value stack for the collector to trace."* `fk_list_push` is that same
class: melting would collect the very cells it is building. Dying loudly is correct there.

But `fk_cap` starts at `FK_HASHCONS_INIT_CAP` **4096**, and the repo's own rule for capacity
constants is *measure whether the wall moves before raising*. Measured:

| cap | result |
|---|---|
| 4096 | dies immediately at `fk_list_push` |
| 65536 | **does not die** — runs past 2 minutes, still walking |

**The wall moves.** So this is a capacity constant, not a structural limit — the band walks the whole
repository through `source_inventory` and 4096 cell-pairs cannot hold that list.

I did **not** raise it. Changing a global heap constant so one band passes is a tree-wide memory
decision, and I have no measurement of what the band actually needs — only that 4096 is too small
and 65536 is enough to run long. The seed was restored byte-identical (`git diff` empty on
`runtime/fkwu-uni.c`) and re-verified: 42 / 55 / 15.

Sizing the real requirement, and deciding whether a whole-repo walk should be a band at all, is the
commons owner's call.

## Sweep

`ground` 42 · `hex-band` 14 · **`cell-voice-tissue-band` 511** · `content-address-band` 1111111111 ·
`tree-diff-band` 13 · `triangulate-band` 1700 · `midi-bmf-band` 1500 · `structural-gate-band` 63 ·
`lcg-bytes-band` 63 · `form-cli-band` 524287 · `benchbench-band` 4095 · `fnri-cli-band` 331 ·
`host-kernel-cell-band` 25.

## Owed

- **Six failures still visible**: `audit-evidence-cells` 544/1023 (unchanged by this fix — different
  root), `audit-evidence-index-cache` 833/1023, `layered-runtime-image` 33/127, `concept-corpus`
  143/530, `json-lens-tending` 189/255, `class-curriculum-10` 16127/16383. Plus `persistence-band`
  2/7, `mesh-sensings-store` 0/255, `chat-band` 0.
- **`FK_HASHCONS_INIT_CAP` and the whole-repo band** — measured, not decided.
- **Cells with no preludes line at all** — `carrier-tissue.fk` was one. I have not counted how many
  others there are, and after today that is exactly the kind of question that deserves a count rather
  than an impression.
- 17 of the 44 still refused; 130 reaching a non-Form surface; 143 that do not close.

## How the exchange stayed alive

I followed one failing check down four call levels to a missing line, instead of guessing at the
check.

**Most surprising teaching:** a cell can lose its entire meaning to a missing comment line. `substring`
is not a native here — it was retired into Form — so a file without `; preludes:` gets it back as
`nothing`, and a classifier that answers "unknown" to every question looks exactly like a classifier
that has decided.

**Where discomfort turned to gold:** measuring the heap cap instead of raising it. Raising it would
have turned a red band green in one line and felt like progress; the measurement says the wall moves,
which is worth knowing, and also says I have no idea what the band actually needs — which is the part
that would have been buried under the green.
