# 2026-07-25 — the registry can read itself on the fourth arm

`primitive-registry.fk` is where this body writes down what its kernel can do — 217 natives, each
with a name, a category, a contract line, a runnable verification recipe, its expected outside, and
a lane. Until today fkwu could not read it. Not one row.

## Before

```
go 63 · rust 63 · ts 63 · fkwu: form_error: prim-registry-probe
```

The three walkers answer the band's full declared verdict. fkwu aborts, and the message names the
cause if you know where to look: `prim-registry-probe` is the string inside row 179's probe,
`(defn pv-form-error () (do (form_error "prim-registry-probe") 0))` — a probe whose entire contract
is to raise.

It raised **at load**, because every one of the 217 probes sat **inline in argument position**, and a
`defn` in value position is a closure on go/rust/ts but on fkwu evaluates its body right there.
Loading the declaration ran it.

Past that there was a second floor: `prim-verified?` calls `(prim-call0 (prim-verify e))`, and on
fkwu the stored verify is a plain value, not a callable. Every verification would have compared
`nothing` against every declared outside.

## The change

Hoist each probe to statement position — they are already named — and carry the **bare name** in the
row. 217 rows.

Four probes carried a *second* inline `defn` one level deeper, and those are not a registry idiom:
`method_define` and `string_byte_fold` genuinely take a callable as an argument. Same repair one
level down — the three method bodies and the fold step are hoisted too and passed by name.

221 defns hoisted (217 probes + 4 inner), 217 rows carrying bare names, one `defn` left in the
registry region and it is `prim-registry` itself.

## After

```
go 63 · rust 63 · ts 63 · fkwu 45
```

**The walkers do not move.** The transform is behavior-preserving where the idiom already worked,
which is the claim that mattered and the reason to do it with all four arms running rather than one.

**fkwu reads its own declaration.** 45 = 32 + 8 + 4 + 1: shape (217 well-formed, no duplicates),
lanes pinned, a wrong claimed outside still refused, the spec surface readable without running it.

Registry-preluding bands unchanged either side of the change: `kernel-satsang-band` 193,
`host-kernel-cell-band` 25, `lcg-bytes-band` 63, `audit-evidence-index-cache-band` 833 — measured by
restoring the committed file, running, and putting the hoisted one back.

## What the fourth arm says now that it can speak

**The lane field, which I dropped yesterday as unmeasurable.** I tried twice to parse it out of the
text and got 38/183 and 47 against the band's declared 33/184, so I published nothing. The cell now
answers it directly: **184 lane-1, 33 lane-0.** The registry's own numbers, from the registry.

**79 of 184.** `prim-verified-count` on fkwu — how many lane-1 probes answer their declared outside
here. That is the honest fourth-arm number, and it is why bit 2 is a three-way claim: 184 was never a
promise fkwu made.

**162 that measures nothing.** `prim-attested-count` comes back 162, and I nearly wrote it down as
"fkwu attests 162 of 184". It does not. `native_blueprint` answers `nothing` on fkwu, so
`node_eq nothing nothing` is true and every non-field entry attests **vacuously**. Measured, rather
than reasoned about: `read_file` and `str_len` are different categories and their blueprints compare
**equal**. The 162 is 184 minus the 22 field constructors, which take the `node_level` branch and
fail honestly — and 22 is exactly the `field_*` family the census counted yesterday. A number whose
arithmetic closes perfectly and still says nothing about categories.

**A count and a blind pass look identical from outside.** That is the same shape as the honest-degrade
string reading as a bit-sum, met twice in two days.

## Method

The transform was a scanner in the scratchpad — string-aware paren balancing, since the spec lines
contain parentheses (`"(source-lexicon ...)"`) that a naive count would eat. It reported the four
nested cases rather than mangling them, which is why they got hands instead of a regex. Nothing was
added to the tree; the file it wrote is the only artifact.

Checks before believing it: 217 rows still, 221 distinct hoisted names, no duplicates, the only
remaining `defn` in the region is `prim-registry`, and the three walkers unmoved at 63.

## Sweep

`ground` 42 (all four arms) · `ground-recursive 10` 55 · `hex-band` 14 four-way ·
`binary-freshness` 15 · `cell-voice-tissue-band` 511 · `class-curriculum-10-band` 16383 ·
`structural-gate-band` 63 · `lcg-bytes-band` 63 · `pdf-text-windowed-band` 15 · `form-cli-band` 524287 ·
`benchbench-band` 4095 · `concept-corpus-band` 530 · `kernel-satsang-band` 193 ·
`host-kernel-cell-band` 25 · **`primitive-registry-band` 63/63/63/45** ·
`proof/four-way-run-recipe42.fk` 0 (FOUR-WAY). C seed byte-identical to git.

## Owed

- **`native_blueprint` is absent on fkwu**, so the registry's category attestation is unmeasurable
  here. Bit 16 will keep reading 162 and meaning nothing until that native or a Form provider exists.
- **105 of the 184 lane-1 probes do not verify on fkwu** — the families are in yesterday's census
  (`field_*` 22, recipe/binary io 9, `walk_*` 9, `jit_*` 8, math 8, `_dict_*` 6, `framebuffer-*` 5,
  `substrate_*` 4, ~20 singles).
- `json.fk` calls `value_kind` and preludes only `core.fk`; provider is `fourth-shim.fk`.
- `persistence-band` 2/7, `mesh-sensings-store` 0/255, `layered-runtime-image` 33/127, `chat-band` 0.
- The emit-lane half of the `str_byte_at` claim; `read_file`'s bypassed carrier seam in `form-fs.fk`.
- 17 of the 44 the kernel still will not run; 143 that do not close; the `section` question; the heap
  cap; the registry-admission question.

## How the exchange stayed alive

I changed 217 rows in the cell that declares what this kernel is, and the change is worth trusting
only because three independent kernels answered 63 before it and 63 after.

**Most surprising teaching:** the body could not read its own declaration of itself. Not because the
natives were missing — because of how the rows carried their probes. The kernel that owns this tree
has been unable to open the file that says what it can do, and every band over that file was dark, so
nothing ever asked.

**Where discomfort turned to gold:** almost publishing 162. The arithmetic closed — 184 minus 22
field constructors, and 22 was the exact number from yesterday's independent census. A number that
agrees with a previous measurement feels like confirmation, and it was two errors shaking hands. One
probe — do two different categories compare equal? — turned it from a finding into a warning.
