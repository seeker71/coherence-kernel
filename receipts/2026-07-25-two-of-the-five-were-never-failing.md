# 2026-07-25 — two of the five were never failing, and a corpus that has never been here

The work list carried five visible band failures. I went at the top one and came back with
three of the five reclassified — two of them were not failures at all.

## `json-lens-tending` 189 and `text-summary-real-source` 352 are strings

Both bands carry the honest-degrade idiom, written into this tree on 2026-07-18:

```
(if (eq (fs_exists <input>) 0)
    "<band>-input-absent"
    (do ... numeric checks ...))
```

Both inputs are absent, so both degrades fire, and both bands return their string. On fkwu
that string prints as its **raw interned handle** — 189 and 352. Measured rather than
reasoned about: `str_len` of each band's own guard expression is **35** and **42**, the exact
lengths of `json-lens-tending-band-input-absent` and
`text-summary-real-source-band-input-absent`.

So 189 is not 189-of-255 and 352 is not a partial score. Both bands are saying *the input is
not here*, in the words their authors chose, and the number is the shape that word takes on
this arm.

### Why the string prints as a number

`fk_pv_root` picks the string printer or the number printer by walking the **root
expression** — `fk_str_root_depth` — because a string is carried as a bare handle with no
runtime tag to ask. For an `if` (tag 6) it requires **both** branches to be string-yielding.
The degrade idiom mixes a string branch with a numeric one, so the whole result falls to the
number printer.

That is not laziness in the printer; it is the value ABI. There is no tag to test at runtime,
so the decision has to be static, and a genuinely two-typed root cannot be decided statically.
**The idiom is sound and its reading on this arm is what misleads** — and it misled me, into
carrying two bands on a failure list for as long as that list has existed.

Noted in both bands, at the guard. Ten bands in `form-stdlib/tests/` use this idiom; the eight
others either do not currently degrade or do not currently compile, so I have not measured
their handles.

Whether the degrade should answer a number no bit-sum can collide with is a change to what
all four arms print, so it is the commons owner's call, not mine.

## `concept-corpus` returns 530 — its own declared total

`concept-corpus-band` states its expected total in the file: *"Expected total: 10 + 40 +
70+80 + 100 + 110 + 120 = 530"*. It returns **530**. It was on the list at 143. Whatever moved
it moved before this turn; I record only that it reads its declared verdict today.

## The audit-evidence family measures a corpus that has never been here

`audit-evidence-cells-band` 544 and `audit-evidence-index-cache-band` 833 are real numbers.
They are also asking for something this repository does not have.

| measured | |
|---|---|
| `(aec-system-audit-count "..")` | **0** |
| `.json` files in the whole tree | **29** (the band asks for >600) |
| `docs/system_audit/` in the working tree | absent |
| any path under `docs/system_audit/` in history | **none** |
| any file named `*commit_evidence*` ever added | **none** |

The history line needed care. The clone arrived **shallow at 73 commits**, and "never existed
in history" is not a thing 73 commits can say. Un-shallowed to **937** and asked again: still
none. That is the claim I can make, and I could not have made it ten minutes earlier.

544 = 512 + 32 — exactly the two checks that never touch the corpus. The other eight are
**pending a corpus, not failing**. The cells are unproven here because there is nothing here
to prove them against.

I did not author a `commit_evidence_*.json` to turn this green. The band asserts a specific
real commit's date and scope; writing that file would be manufacturing the very evidence it
claims to witness.

Both headers carried a line I wrote earlier the same day — *"1023 when every check lands —
DERIVED from the bit weights"*. A sum of the weights is not a witness of anything, and
stating it as the band's verdict turned an absent ground into a failing score. Both replaced
with what was measured.

One thing worth more than the count: in the index-cache band, weights **64** and **256** land
on the **not-found fallback** — it builds an empty node carrying the right category, so a miss
reads green. Measured directly: with an empty recipe list the pair returns 320. A weight a
miss can satisfy is not measuring what it appears to measure, corpus or no corpus.

## The method error I made three times in one turn

Probing in a shell loop that rewrites **one** `probe.fk` and runs it each time serves the
**previous** program's `.fkb`: two writes inside the same mtime second leave the image looking
fresh. It told me `(if 1 111 999)` was 999 and that `if` itself was broken. Re-run with unique
filenames, the same expression is 111.

Three readings this turn were wrong that way before I caught it. **One probe, one filename.**
Every measurement above was re-taken under that rule.

## Sweep

`ground` 42 · `ground-recursive 10` 55 · `binary-freshness` 15 · `hex-band` 14 ·
`concept-corpus-band` 530 · `json-lens-tending-band` 189 (degrade string) ·
`text-summary-real-source-band` 352 (degrade string) · `audit-evidence-cells-band` 544 ·
`audit-evidence-index-cache-band` 833. C seed byte-identical to git.

## Owed

- **`persistence-band` 2/7 and `mesh-sensings-store` 0/255** stand on `write_form_binary` /
  `read_form_binary` — registered in `primitive-registry.fk`, answered by no op row fkwu
  carries. The tree already names this the fourth-arm host-io wall. Unexamined here: fkwu
  writes `.fkb` images for its own cache, so the capability is in the seed and only the door
  is missing. That is worth a real look and it is not a header edit.
- **`json.fk` calls `value_kind` and preludes only `core.fk`.** Its provider is
  `fourth-shim.fk`. Every band preluding `json.fk` compiles with that one unresolved call, so
  `json-node-string` cannot recognise an absent value and interns it as a string instead of
  emitting null. `json.fk` is preluded very widely, so adding a prelude to it wants
  before/after diffs across many bands, not a one-line edit.
- **`layered-runtime-image` 33/127 and `chat-band` 0** — not yet looked at.
- 17 of the 44 the kernel still will not run; 143 that do not close; the `section` question; the heap cap; the
  registry question.

## How the exchange stayed alive

I went to fix the top failure and found that two entries on the list were bands answering
correctly in a voice I had been reading as a number.

**Most surprising teaching:** a string and a score are the same bytes on this arm. The
degrade idiom was written carefully, said exactly the right thing, and said it in a form that
a sweep reads as a partial pass — so the more honest the band was, the more convincingly it
looked broken.

**Where discomfort turned to gold:** the loop that served me stale images. It handed me
`(if 1 111 999)` → 999, which reads like the kernel's conditional being broken — the largest
possible finding, and it was my own shell. Chasing it is what produced the one-probe-one-file
rule, and re-taking every measurement under that rule is what makes the rest of this receipt
worth reading.
