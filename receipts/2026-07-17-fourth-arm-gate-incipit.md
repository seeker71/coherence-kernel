# 2026-07-17 — the gate read only the incipit

**Ground:** `cc -O2 -o fkwu runtime/fkwu-uni.c && ./fkwu --src bootstrap/ground.fk` → **42**
(Friday 2026-07-17, ~11:45 WITA; worktree `mystifying-mestorf-6a0fb7`.)

## The wound

`form/scripts/fourth-arm-gate.sh` built each band's prelude list with
`grep -E '^; preludes:' | head -1` — the opening line only. Two distinct failures hid
in that one read:

1. **Multi-line headers truncated.** `form-cli-request-band.fk` declares 19 preludes
   across 10 `; preludes:` lines; `rag-ask-grounded-band.fk` declares 9 across 5.
   Everything after line one was dropped, and the reference kernels crashed unbound
   (`ra-answer-key-of-hex` and kin).
2. **A single-line header still crashed.** `shamballa-channel-band.fk` declares its
   preludes on ONE line — but omits `core.fk` (the fourth arm's shim mirrors core, so
   fourth-leg bands need not declare it). The header's mere existence suppressed the
   gate's core.fk fallback, and the three reference kernels — which have no shim —
   died on unbound `nil?` at `sc-scan@form-stdlib/grammars/shamballa-codes.fk:76`.

Both shapes printed **DIVERGENT** — a false verdict on a body that agreed four ways.

## The heal

`fourth-arm.sh` already owned the honest reader (`fourth_band_prelude_mods`:
continuation lines handled, block stopped at the first non-path comment line) — but
it drops core.fk for the shim's sake. Split it:

- `fourth_band_prelude_mods_raw` — every declared path, **declared order, core.fk kept**;
- `fourth_band_prelude_mods` — the raw list minus core.fk (byte-identical behavior
  for every existing fourth-leg caller).

The gate now sources `fourth-arm.sh`, reads the raw list, and prepends
`form-stdlib/core.fk` **only when absent** — because `form-debug-band.fk` declares
core.fk *fourth*, after `minimal-surface.fk` and `hati-os-kernel.fk`, and a blind
prepend would have silently reordered a proven load order inside the very gate whose
job is proving agreement. The script is now executable (mode `100755`).

## Witnessed

| stem | before | after |
|---|---|---|
| shamballa-channel | DIVERGENT (unbound `nil?`) | **PASS-4WAY** |
| form-cli-request | DIVERGENT (truncated to line 1 of 10) | **PASS-4WAY** |
| rag-ask-grounded | DIVERGENT (truncated to line 1 of 5) | **PASS-4WAY** |
| form-debug (control: mid-list core.fk) | — | **PASS-4WAY**, declared order intact |
| int-literal-width, go-jit (control: no header) | — | **PASS-4WAY** via fallback |
| nl-reason | DIVERGENT | DIVERGENT — **pre-existing**, identical error (`bp: unreviewed bootstrap name: property` via `norm-en@nl-translate.fk:75` → `bp@form-ontology-loader.fk:343`) with and without core.fk; flagged as its own task |

## Corpus

Row **761** landed: *what one word names a text known only by its opening line* →
**incipit** (0 hits in corpus and body before the row). The body attests natively:
`hdc-field-code` → **1621622761** = 162 rows · 162 admissible · 2 foundings ·
max-mid **761**.

## Most surprising teaching

The bug announced itself as "multi-line headers truncate" — but shamballa-channel's
header is single-line and still produced the false verdict, because the header's
*existence* suppressed the core.fk fallback. The law underneath is not about
continuation lines at all: **two readers of one declaration will disagree, and the
disagreement surfaces wherever their defaults differ, not only where the obvious
feature is used.** One declaration deserves exactly one reader; the gate's sin was
having its own.

## Where discomfort turned to gold

The urge was to blindly prepend core.fk and move on — all three named stems would
have passed and the work would have *looked* done. Sitting with the small discomfort
of "does declared order matter?" and asking the body (a grep over every band for
non-first core.fk) surfaced `form-debug-band.fk`, whose core.fk sits deliberately
after `minimal-surface.fk` — the blind prepend would have reordered a proven load
inside the proving gate itself. The same reflex (grep before change) then killed the
first fresh-word candidate: "synecdoche" was already landed (3 hits); the collision
avoided became the more precise word, *incipit* — the gate's exact sin, naming the
whole text by its opening line.
