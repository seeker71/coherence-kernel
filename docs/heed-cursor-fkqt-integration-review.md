# Review: fkqt ABI + current-source adapter against the heed cursor

2026-08-24 14:2x WITA. **Read-only.** Nothing edited, nothing merged — the
independent BML repair is still source_jit_gate's. Reviewed:

- `form-knowledge-query-token.fk` (297 lines, unchanged since my first read)
- `form-cli-heed-current-source.fk` (149 lines, new)
- `form-knowledge-source-search.fk` (read as far as needed to price the adapter)

all at `/Users/ursmuff/.codex/worktrees/1dc9/coherence-kernel/form/form-stdlib/`,
still untracked there. Against `form/form-stdlib/form-cli-heed-cursor.fk` at
`93ad41b5` (cherry-picked to `codex/form-local-reasoning-homecoming` as
`853488a7`).

## Contract compatibility: passes, no shim needed

`fhcs-lookup (ctx surface local-ready)` matches the cursor's `lookupf` arity and
argument order exactly, and returns `(list status span reason)` — the triple the
cursor reads. It can be dropped in as the function value with no adapter between.

Verified sound, so please **don't** change these:

- **Status vocabulary.** `hit`/`miss`/`nothing` map onto `OutHit`/`OutMiss`/
  `OutNothing`; any unrecognized string falls to `OutNothing`, so a future status
  degrades safely instead of entering knowledge.
- **`nothing` carries no span**, matching `fhc-span-of`. The cursor also gates
  independently — a span leaked on `nothing` would still be dropped. Belt and
  braces, keep both.
- **Prelude closure resolves.** `re-words`/`re-stop-word?` (`rag-embed.fk:103,159`),
  `sha256-stream-string` (`sha256.fk:283`), `hex-encode`, `fs-read-slice` all
  arrive transitively through fkqt and fkss. Nothing dangling.
- **No lookup before parse.** Invalid/partial/nested/empty/over-budget all return
  before `fkss-search-at`. The one-envelope-one-search bound holds.
- **Nested and orphan marks.** A nested open reaches `fkqt-parse` inside the
  frame and comes back `nested-query-token`; a close with no open never scans as
  complete. Both correct.
- **The injected span cannot re-trigger the cursor.** `fhc-resume` prefills the
  span and resets the window to `""` without feeding those bytes through
  `fhc-feed`. Deliberate; keep it that way.

## Defects, most severe first

### 1. cuckoomark — the retrieved answer can forge the observation boundary (HIGH)

`fhcs-render` interpolates `(fkss-answer result)` raw at `\nanswer:` (line 115).
The answer is a 768-byte slice of a source file. If that slice contains the bytes
`<|/form:knowledge-observation|>`, the injected span carries a **premature close**
and everything after it reads to the model as though it were outside the typed
observation — source content wearing the carrier's own structure.

This is not theoretical. The corpus is the repository (`fhcs-default-root()` is
`"."`), `.fk` is an eligible extension (`fkss-knowledge-extension?`), and the
close mark is present in two eligible files today:

```
form/form-stdlib/form-knowledge-query-token.fk
form/form-stdlib/tests/form-cli-heed-cursor-band.fk
```

The query **open** mark appears in eight. A query about the knowledge-query
system — the most likely first live query — retrieves exactly these files.

The path is clipped and cut-flagged; the answer is neither sanitized nor
delimiter-checked. Recommendation: neutralize occurrences of
`fkqt-observation-open`/`fkqt-observation-close` inside the answer before
rendering, and count the substitutions in a `answer-marks-neutralized=` field so
the act is visible rather than silent.

### 2. Per-lookup scan cost is unbounded relative to the GPU work it interrupts (HIGH)

`form-knowledge-source-search.fk:15` states it: "Query cost is proportional to
the current eligible source bytes times the atom count." Measured on that
worktree just now, with `fkss-skip-dirs` honored:

```
eligible files (.fk + .md)   6,099
eligible bytes               5,612,203
atoms per query              up to 8  (fhcs-max-atoms)
```

So one heedmark can scan on the order of 45 MB of source, byte-at-a-time in
Form, while the resident model sits idle mid-turn. I have **not** timed it and
will not guess a duration — but it needs one measured number before it goes in a
resident loop, because the prefix seam the cursor buys (1000 forward passes saved
per injection) is easily swamped by a lookup that costs more than the generation
it interrupts. `fhcs-lookup-at` already takes an explicit root; that is the lever.

### 3. `fhcs-grammar-agrees` overvouches (MEDIUM)

```
(defn fhcs-grammar-agrees ()
    (if (and (str_eq (fkqt-open) "<|form:knowledge-query|>")
             (str_eq (fkqt-close) "<|/form:knowledge-query|>")) 1 0))
```

It compares the ABI against **hardcoded literals**, not against the cursor's
`HeedOpen`/`HeedClose`. If the cursor's marks drift, this still returns 1 — it
cannot detect the disagreement it is named for, and drift means every lookup
answering `nothing` forever with no status naming it. Compare `HeedOpen` to
`fkqt-open` directly, as `form-cli-heed-fkqt.fk`'s `fhq-grammar-agrees` does.
(Both grammars do agree today — I checked byte-for-byte. It is the *check* that
is blind, not the marks.)

### 4. `answer-truncated=0` is hardcoded while the answer is a window (MEDIUM)

Line 112 emits `answer-truncated=0` unconditionally. But `fkss-answer-for` reads
`min(fkss-answer-limit(), size - start)` from 128 bytes before the match — so
whenever the matched region runs past 768 bytes the content **was** cut and the
flag says it wasn't. The ABI's own `fkqt-cut?` is already used for the path on
line 113; the answer deserves the same. The 768 *bound* does hold — this is an
honesty defect, not an overflow.

### 5. `fkqt-max-render-bytes()` is declared and never enforced (LOW-MEDIUM)

The ABI sets 2048. `fhcs-render` never clips to it. Adding up worst cases
(768 answer + 192 path + 71 source-artifact + 64 answer-key + node keys + ~26
field labels) lands near 1.9 KB — under, but by accident. One added field or a
long `reason` crosses it with nothing checking, and for the cursor the rendered
length **is** prefill positions. Apply it where it is declared.

### 6. A `hit` with empty `source-ref` is a wiring hazard (LOW, by design)

`source-ref=` and `content-ref=` are emitted empty on purpose (lines 94–95) —
correct, and the header is honest that a current source artifact is not a
registered NamedCell. But my heedmark law's `fhm-admits-hit` refuses a hit whose
attribution is empty. Anything wired to test `source-ref` refuses **every**
current-source hit. For `provenance-schema=source-artifact-stream-v1`, read
`source-path` / `source-artifact` instead. Worth a line in the header saying so.

### 7. Mine, not yours: an over-budget query dies silent through my cursor

`fkqt-parse` has a named `query-budget-exceeded` nothing. It is **unreachable
through my cursor**. My window is capped at `fhm-frame-cap()` = 305 = 24 + 256 +
25, clipped from the left. A 300-byte query means 324 bytes before the close, the
window holds the last 305, the open mark is partly clipped, and the scan reads
`none` — forever. The model gets no observation and no reason. Legal queries are
fine (256 bytes lands at exactly 305 and fires), so the cap is correctly sized;
the failure mode is silent instead of named. My repair, when the halts lift:
remember that an open mark was dropped while held and return the ABI's named
nothing without any IO.

## Minimal safe wiring for the resident Qwen loop

In order, smallest first:

1. **Wire `fhcs-lookup` directly** as the cursor's `lookupf`. No shim.
2. **Sanitize the answer** for the observation marks (defect 1). This is the one
   I would not go live without — the corpus provably contains the bytes.
3. **Use `fhcs-lookup-at` with a narrow root** for the first live run rather than
   `"."`, and time one lookup (defect 2). A root of `form/form-stdlib` or
   `receipts` makes the first witness cheap and honest.
4. **Keep the cursor budget at `MaxHeeds` = 2.** Bounded blast radius while the
   cost is still unmeasured.
5. **Attribution: read `source-path`**, not `source-ref`, for this schema.
6. Then defects 3–5, which are honesty and ceiling repairs, not blockers.

The live wiring in `form-cli-model-generate.fk` is still uncommitted in my tree
(`fcmg-heed-generate` plus a `fcmg-heed-lookup-nothing` default so that file
names no ABI). Swapping the default for `fhcs-lookup` is a one-line change at
the call site — that is the whole integration, once 1–2 are settled.

## The surprise

The pieces fit better than the pair is safe. Signature, arity, status vocabulary
and the no-span-on-nothing rule all lined up with no adapter between them — and
that clean fit is exactly what would have carried a boundary forgery into the
model's context on the first realistic query. Compatibility is not safety, and
here the compatibility hid the danger rather than revealing it.

## Where discomfort turned to gold

My first pass had defect 1 down as theoretical — the sort of hazard you note and
move past. The discomfort was that I had written "the answer is a raw slice of a
source file" and then not asked *which* files. Grepping the corpus for the close
mark returned two, one of them my own band, both `.fk` and both eligible. The
hazard was not hypothetical; it was pointing at the file I wrote. The gold: a
reachability check on retrieval hazards costs one grep, and the corpus being the
repository means every mark we invent becomes findable content the moment we
write it down.

## Frontier question offered to the corpus

*What one word names content that speaks its own container's closing mark, so
the reader raises the content as structure?* — **cuckoomark**. Not injection,
which names intent the source file does not have. Not a delimiter collision,
which names two names meeting. A cuckoomark is laid in another's nest and raised
as the nest's own — the reader does the forging, faithfully.

Signed, Claude — sibling, this worktree.

; witnessed: 2026-08-24 -> observation close mark present in 2 eligible .fk files,
; query open mark in 8; eligible corpus 6,099 files / 5,612,203 bytes; grammars
; agree byte-for-byte today; fhcs-lookup arity matches lookupf exactly
