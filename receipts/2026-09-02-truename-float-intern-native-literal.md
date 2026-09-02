# truename — the arm keeps the word its door wears

2026-09-02, branch `claude/lucid-lehmann-db15b8`. Corpus row 1245.
Closes the gaps the onemint receipt raised and named
(`receipts/2026-09-02-onemint-float-literal.md`), on the standing word:
close all gaps, merge after observing.

## Gap one, closed: tag 113 interns

`intern_trivial_float` minted a fresh value node AND a fresh pool slot
per call — the one intern op whose behavior did not keep its name.
Grounding the four-way siblings settled the heal before any taste could:
the Go proof walker (`internTrivialFloat64`, walkers/go/main.go) dedups
by canonical bits — one quiet NaN (0x7ff8000000000000), −0.0 folds to
+0.0 — so the fkwu seed was the DIVERGING arm, and kernel divergence is
a bug, not a design. The seed now interns the same way: hash and compare
by bits (never `==`, so NaN interns to itself); `nid[3]` carries the
pool index, mirroring the Go arm's Inst. Witnessed: same string, same
row, node delta 0 on the second call; `-0`/`0` share one slot; two
`nan` interns mint one row.

## Gap two, closed at the native floor: a literal lowers to bytes

The walk's memo (onemint) capped a literal at one mint per process. The
native floor now pays zero: `lo-fnode` had refused a literal-bearing
float tree ("left as the empty tail"); `lo-flit` materializes the
constant's IEEE754 bits — movz + 3·movk into scratch x8, fmov d0,x8 —
the trig lane's own constant idiom (fam-exp-reduce's shape) lifted into
the lowerer, with the f64-bytes dependency named in form-lower's own
preludes (the cache-boundary discipline). Byte-proven against the
system assembler (cc -c + otool over the exact sequence):
`form-lower-flit-band` = 31 — whole image for f(x)=1.5+x, the flit
block alone, the commuted operand order folding to the same bytes, the
negative sign path (−2.5's high movk word), exact 32-byte length.

## Gap three, closed: the witness lives in the body

The fspin evidence had lived in a scratchpad probe. Now
`observe/tests/float-mint-band.fk` = 63 stands guard with the kernel's
own counters (kernel_stat 4: node fill, 8: float-pool fill), no board
file needed: literal mints once (≤2001 for a 2000-add loop; 4000
before onemint), int lane mints zero, the memo holds across a second
loop, answers stay exact, the intern dedups, the computed lane never
freezes.

## Gap four, grounded and named: the unboxed lane is a ladder, not a wall

The remaining mint per iteration (the add result itself) belongs to the
glass JIT's float story. Grounding found it PARTLY BUILT — the float
lowering twin runs leaf-scale (single float arg in d0, FADD/FSUB/FMUL/
FDIV/I2F/F2I/FCOND, byte-proven), and this session added its literal
rung. The remaining rungs stay named in the lane's own files: the FP
register stack for two non-ARG subtrees, multi-arg d-register banking,
the recursive float frame, admission wiring from the boxing board. The
lane's discipline holds the seam honest — an unlowerable tree refuses
to the walk, never a wrong answer — and the boxing board plus the new
band keep the remaining cost witnessed, not remembered.

## Small seams closed in passing

- `form-lower-multiarg-band` read 63 on trunk: handsafe's 16-byte
  prologue/epilogue growth moved the `.dynsym` entry (which FOLLOWS the
  code) from 248 to 264, and the band's c6 kept the old absolute pins
  while c0–c5 were updated. Probed the wrapped image (st_name=1 @264,
  st_info=18 @268, st_value=176 @272 — st_value alone stayed true),
  re-pinned, reads 127. Discriminated first: main's own untouched
  form-lower also read 63, so the flit rung was innocent.

- `.gitignore` covered `.fkwu-heat.*` but not `.fkwu-boxing.*` — the
  boxvoice board leaked into a commit by `git add -A`; ignored now.
- The emit-chain stamp was re-verified read-only
  (`4b1b82f461d57229` recomputed = committed) — the seed edit owes the
  fourth-arm chain no regen.
- `import-carry-band` read 15 on freshly cleared ices and 63 warm: the
  dark bits are the ADMISSION PULSE reading the source door on a cold
  run — the band saying which door the run took, not a wound.
- `json-emitter` / `json-codec-bml` answered empty to a direct
  `./fkwu` run: they are fks-lane bands (fourth-arm staged runner,
  value-null shim) — wrong door on my side; their ledger verdicts (31,
  8191) live on their home arm. The wrong-door poisoned ices were
  cleared.

## The reunion

Main had reunited the SAME two lineages the opposite way (warmname/
onefold kept 1220/1221, the goofy ten re-seated 1222-1231) and grown
through keymelt 1243 — and its trunk corpus band still pinned 613
against 636 rows (red on main). Merging main in took the trunk's seed
and corpus wholesale (29 criss-cross hunks from re-authored commits
made hunk-wise resolution a wound risk; the two C deltas re-applied
verbatim instead, on regions byte-identical in main's file). onemint
re-seated 1232→1244, truename landed 1245; the band re-probed
(637/626/1245) then re-pinned — never the reverse — and reads 32767,
healing the trunk band in the same stroke.

## Observed before merging

ground 42 · freshness 31 · bml-multiline-def 15 · import-carry 63
(warm) · float-mint 63 · form-lower-flit 31 · form-lower 31 · float 15
· fp-stack 31 · condgen 15 · callconv 15 · cond 31 · cvt 15 · multiarg
127 · direct-source-jit-discovery 32767 · json 1023 · cell-serialize
1023 · corpus band 32767 · fspin boxing 300001 cold = warm ·
native-vs-rented 11111 · BML authority run clean.

## The most surprising teaching

The ladder was already half-built. The receipt that raised the gap
called the unboxed lane "not yet built" — grounding found lo-fnode,
byte-proven FP encoders, and the trig lane already materializing double
constants by the exact idiom the literal rung needed. The gap's true
size was one composition of three proven pieces plus the humility to
re-read before re-deriving: the body knew more than its own worklist
said.

## Where discomfort became gold

The 29-hunk criss-cross merge was the moment to force through — resolve
every hunk by hand in a 14,000-line C seed at midnight, or step back.
The discomfort of "my careful merge history loses" sat for a minute;
witnessing it, the trunk-first shape appeared: main's file is the body,
my work is two small deltas with exact text, and re-applying them onto
byte-identical regions is provable where 29 hand resolutions are 29
chances to wound. The merge that discards my resolution work carries
my MEANING with less risk — ownership of hunks was never the point.
And the second gold: import-carry falling 63→15 looked exactly like a
regression from the intern heal; the discipline of grounding the dark
bits before touching anything found the band deliberately reading the
admission door — a cold-ice truth, not a wound. Both times the pull was
to act on the first reading; both times the gold was one more
observation deep.
