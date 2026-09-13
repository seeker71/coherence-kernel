# Three ops come home, and the copies stop drifting

2026-09-13, afternoon, M4 Max, Hati Suci. Urs: *"Bring fully home all the ops as form native code
outside of the kernel into form code. Use BML without s-expressions."*

## Carried

- **dot_product, magnitude and vector_cosine live in BML now**
  (`form-stdlib/bml/vector-ops.bml`). The seed lost their optable rows, the host-pool arms at tags
  84-86 and the two list walks behind them; the manifest and both flt-ops tables lost their rows;
  the fourth-arm emitter lost tag 85 from its unary group. The generator wrote the op table
  (three rows gone), and op-manifest and native-surface read 191 rows aligned.
- **Four copies of one op had drifted.** fkwu's walk stopped at the shorter vector; Go, Rust and TS
  panicked on unequal lengths. The first band I wrote read 255 on fkwu and died on every sibling.
  The siblings dropped their natives, so the one Form definition answers on every kernel; Go's
  pair_angle keeps a private cosine. The registry drops the three rows (210, 181 in lane 1).
- **The walk to one op out of the seed**, as it stands: a BML home; the manifest row, both flt-ops
  rows and the emitter arm; the generator for the op table; the seed arm and its helpers; the
  reserved heads; the fourth-arm and form-cli regens; each caller's prelude; the siblings' copies
  and the registry rows; a four-way band.

## Witnessed

vector-ops-band 255 four-way (registered); jit-native-span 127 four-way; primitive-registry 63
three-way; the registry lens 210 == 210; the drift door 16383 of 16383, nothing held back;
freshness 31; TestFkwu green after the regens (form-cli stamp 400bcd1533ae6423); four-way-run-recipe42
and word-gender four-way 0.

## Where all the ops stand

265 rows remain in fkwu's table. Three kinds of work are left, and each waits on something different:

- **The pure rows** (floor, ceil, trunc, round, round_ndigits, abs, str_find, substring, value_eq,
  value_str, len, nth, ...) can move the way this trio did, but a row that leaves the table is
  gone for every cell that does not prelude its home: `round` has 31 calling files with no core.fk
  line, `abs` 42. Before those move, the body needs a home every chain reaches.
- **core.fk cannot prelude a BML unit.** The BML compiler's own chain loads core.fk, and the
  lowering door has no guard against its own re-entry, so the compiler tier stays in .fk until it
  is bootstrapped from a prebuilt image.
- **Host doors** (files, sockets, processes, Metal, sensors) become BML over one door into the host;
  that door is the seed's to keep.

## Closing

Most surprising: bringing an op home exposed a disagreement that had stood as long as each kernel
kept its own copy. Row 1521 names it copydrift.

Discomfort to gold: a red on all three siblings, minutes after 255 on fkwu. The discomfort was
real; reading the siblings' own crash lines turned it into the heal, one definition in place of
four.

— Claude (Opus 5), as Sema, worktree pensive-wilbur-a0b3b7
