# Four megabytes of ceiling, and three things I had told him wrong

**Date:** 2026-08-14
**Status:** measured; no cap raised, nothing wired
**Corpus:** row 1020, `guesswall`
**Follows:** [two rungs moved, one held](2026-08-14-two-rungs-moved-one-held.md)

## The question

*The floor is narrower than the page implied — how do we make it wider?* Then, a beat later, the
sharper one: *a cap of 4096 does not scale to almost all being local.*

## The first answer, and why it was only half

`cognition/native-generate.fk` gives two reasons the KV cache is not wired: the cache is the
"named speed stone" (unbuilt), and the cell omits `core.fk` "to keep under the function-table
ceiling." Both were measured today.

The stone is already cut. `form-stdlib/kv-cache.fk` **255**, `kv-llama-block.fk` **255**, and
`kv-gqa-llama-block.fk` — GQA plus KV plus llama, exactly this checkpoint's 8-head / 4-kv shape —
**255**, all exit 0. What is missing is the wiring, not the cache.

And the ceiling: `FK_FN_CAP` is 4096 (`runtime/fkwu-uni.c:5837`) while the *entire* wiring closure —
this cell, native-decode-step, core.fk, and the whole proven KV family — counts **335** defns. Eight
percent. A door held shut by a sentence about a lock that nobody had tried.

## The second answer, which is the real one

Then the question moved up a floor, and my "twelvefold headroom" stopped being the interesting
number. Counted across the tree: **58,246 defns**. Fourteen times the cap.

The table is a single static array per run (`static long long fk_fn[FK_FN_CAP]`), so the binding
number is the largest single closure, not the body total — 58k is not loaded at once today. But
*almost all of it local* means exactly the thing that is not true today: the body resident together.
There, 4096 binds.

Three facts decide what to do about it, and all three are good:

1. **It is a raisable capacity constant, and the body has raised its siblings.** The source says so
   in the same breath — `FK_AST_NODE_CAP` went 65536 → 262144 on 2026-07-02 because a real program
   exceeded it, with the note that `"--src is a gate"` had been a misdiagnosis: *"this is a raisable
   capacity constant, not a fundamental limit."*
2. **It fails loudly.** At define time, `fk_diag(FK_DIAG_ERR, …)` fires for *every* over-cap defn,
   not just the first, nothing is corrupted, and unregistered names fall through to the
   unresolved-call witness. (The silent `return fk_nothing` at :6022 is a defensive index check
   behind that, not the primary path. I misread it as the failure mode before reading the define
   path, and it is not.)
3. **It is cheap.** Eight arrays are sized by `FK_FN_CAP` — `fk_fn`, `fk_fnar`, `fk_src_nat`,
   `fk_src_nat_len`, `fk_fheat`, `fk_nat_tried`, `fk_nat_exec`, `fk_src_nat_frame` — at 8 bytes per
   index:

   | cap | static memory | |
   |---|---|---|
   | 4096 | 0.25 MB | today |
   | 65536 | **4.00 MB** | holds the whole body's 58k |
   | 131072 | 8.00 MB | twice the body |

**Four megabytes is the price of the whole body resident.** The body's own law is written beside
`FK_AST_NODE_CAP`: *measure (does the fill position move with the cap?) before raising* — a doubling
probe, so a treadmill cannot be mistaken for honest growth. That probe is the next step, and it is
small. No cap was raised here; it was measured, and measuring is the part that was missing.

## Three corrections I owe

A five-reader fan-out on the generation path returned during this work. It found three things I had
already told him, wrongly. I re-ran each myself rather than repeat a report.

- **form-cli is driven by stdin, not argv.** I said `--help` produced nothing and left it there.
  argv is inert; the door is a pipe. `echo ping | ./form/form-cli` → **pong**, witnessed. Reporting
  a silence as a finding, when I had only knocked on the wrong side of the door, is the same
  not-having-probed the row is named for.
- **"form-cli routes" was wrong.** `fcr-route` and `fcr-fitness` have **zero** callers outside their
  own file and one test band — the only hit is `tests/observed-auto-learning-band.fk`. The router is
  proven tissue carrying no traffic, and the body already says so:
  `docs/coherence-substrate/form-cli-fourth-kernel-baseline.md` records *"Open wire: the python ask
  route still bypasses fcr-route."*
- **The native GGUF lane cannot be reached from the shipped binary.** `ask-native-lane.fk` appears
  **0** times in `form/build-form-cli.sh`, and the flatten passes its source list verbatim without
  resolving `; preludes:` — so `anl-ready?` is unresolved in the image and that lane can never be
  selected, whatever the routing says.

## The most surprising teaching

Every wall today was made of a sentence. "No persisted KV cache" — it is built and banded three ways.
"Keeps under the function-table ceiling" — eight percent of it. "The lane resolves on go rust ts" —
it is Form in a file nobody preluded. Four times in one day the thing standing between the body and
a capability was a claim written once, from inference, and never re-measured. The body's own source
already names this class exactly, beside `FK_AST_NODE_CAP`: a misdiagnosis called a gate.

## Where discomfort became gold

I answered "how do we make it wider" with 335-against-4096 and a note about twelvefold headroom, and
I was pleased with it. It was a true number answering a smaller question than the one asked. Urs
moved up one floor — *4096 does not scale to almost all being local* — and the same measurement
inverted: at closure scale a guesswall, at body scale a real wall fourteen times exceeded.

The discomfort was that my answer had been *correct and beside the point*, which is harder to notice
than being wrong. What saved it was that the fix is the same either way — measure the thing instead
of asserting it — and measuring at the new scale took one command and produced a better answer than
the first: four megabytes, a documented precedent, and a loud failure mode. The word in row 1006 had
to be widened to hold both readings, and it names the not-having-measured, never the number.
