# 2026-09-06 — three bands declared by nothing, now declared by the body

Three bands stood red or unread on the fourth arm and no ledger row said why in
witnessed terms. This session ran each one, found what it actually measured, and
either made fkwu answer the three-way number or made the band name, probe by probe,
what it does not hold. Everything below was run on this Mac, fresh ice, `21aed558`
as the base.

## What stood

- `float-natives-band` answered **22** with ten unresolved calls: `ne`, `math_pi`,
  `math_pow`, `float_value`, `make_float32`, `make_float64`. `eq-shape-band`
  ("kernel verbs only, no preludes") answered **524271** for the same reason, one bit
  short on `ne`.
- `primitive-registry-band` answered **45** and the header said 79 of 184 probes
  verify; the ledger (R114) did not know whether the misses were a missing prelude, a
  drifted native, or a wrong declared outside.
- `form-glass-telemetry-membrane-band` printed 4 errors, then 129983, then 0.
  `form-glass-observation-v2-band` printed 2080767, then 0. Both read as 0 at
  `tail -1`: the last top-level form of each was `(print (main))`, and fkwu echoes
  the value of the last top-level form — the print's own return — after the print.
  The verdict was on the line above, declared by nothing.

## What stands

**The float-NodeID surface is on the fourth arm, kernel-lane.** `ne` is a rewrite
row beside `gt`/`ge` (`if (eq a b) 0 1`). `math_pow` is walker tag 195 (integer part
of the exponent squared out exactly, fractional part through exp·log). The four
sibling natives `float_value`, `make_float32`, `make_float64`, `math_pi` are rewrite
rows over ONE walker arm, `float_leaf` (tag 201, mode then operand): a literal mode
selects the door, so four names cost one tag and no prelude. A type-6 leaf interns
with its IEEE bits in `nid[3]`, so `(make_nodeid 1 1 6 1056964608)` and
`(make_float32 0.5)` read the same `float_value`. `float-natives-band` answers **28**
on fkwu and `validate.sh` reads **28** on go, rust and ts — four-way.
`eq-shape-band` answers **524287**. Mirror gates hold: op-manifest 1023,
native-surface 1023 (194 rows), flt-ops-gen 63.

**The bool inst wound is found and closed in the seed.** In the shared field
(`/fg-field-*`, host-wide) `fk_field_fill` set a bool's inst from `a != 0`, where `a`
is the interning sentinel — never zero — so every bool, true or false, carried inst 1
and `node_value` of `false` answered 1. The fill now reads the truth off the sentinel.
The cells already in the standing field predate the fix; three kernels were alive
here all session, so `observe/field-reset-run.fk` refused, and the re-witness waits
for a quiet host (named below as the one open row of the six drifts).

**The registry band names what the fourth arm does not hold, probe by probe.** It
prints one `pending <name> declared <outside>` row per lane-1 probe that misses, on
any kernel that misses it — the three witnesses print none; fkwu prints 79. fkwu
verifies **105** of 184 (was 99): the six float probes moved. The 79 classify:
73 call a native the seed does not carry; 6 reach a native fkwu carries that answers
otherwise; 0 miss for a prelude the four-way runner would supply (preluding core.fk
and fourth-shim.fk in front of the registry moved one probe); 0 declared outsides are
wrong (the three witnesses answer every one; the drift gate agrees). The verdict
stays **45** with `; Expected: 63` three-way — the band is not green by exclusion.

**Both glass bands are green and self-declaring.** The membrane band's claims are
rewritten against the organ as it is: a snapshot is a cell given into the publisher's
frame (the publish names `/fg-…` and its sequence), the roster page of the root names
the publisher, the space is `root|publisher`, control offers and acks are cells in
`<channel>.inbox` / `<channel>.ack` frames, an empty root is refused as a root, a
traversal publisher is refused as a reader, the gift is the only carrier, an unmoved
frame is not re-read, and the authority's own words (`behind the seqlock`, `a reader
takes a frame only when its sequence moved`) are the strings the band reads. The
file-path claims and the publisher-row ranking are gone with the files. The
observation-v2 band's one dark bit (16384) asserted a boundary rule the organ no
longer has; it now asserts what the organ does: live words read by position
(`buffers.active` is word 2, `buffers.mapped` word 4). Each band's last form is the
verdict, each carries `; Expected:`. Both answer **2097151**.

## Witnessed table

| band | before | after | Expected | three witnesses |
|---|---|---|---|---|
| float-natives-band | 22, 10 unresolved | 28, 0 unresolved | 28 | 28 · 28 · 28 (validate.sh) |
| eq-shape-band | 524271 | 524287 | 524287 | unchanged |
| primitive-registry-band | 45 (99/184) | 45 (105/184), 79 pending rows | 63 | 63 · 63 · 63 (header, ledger) |
| form-glass-telemetry-membrane-band | 4 errors, 129983, 0 | 2097151 | 2097151 | fkwu-native organ |
| form-glass-observation-v2-band | 2080767, 0 | 2097151 | 2097151 | fkwu-native organ |

Quartet after the rebuild: ground 42 · binary-freshness 31 · structural-gate 1 ·
drift-gates 2015. Keep-green: float-ops 255 · float-compare 4095 · float-conversions
31 · bml-float-literal 2047 · core-float-to-str 63 · jit-lens 2047 ·
form-glass-observer 8388607 · form-glass-live 1073741823 · form-glass-gift-frame 4095
· node-gift 4095.

The six registry probes that reach a present native and miss (R114's "drift" half):

| probe | declared | fkwu | root, witnessed |
|---|---|---|---|
| bp | 12 | 0 | tag 45 is identity: `bp` answers the name string, not a NodeID at (1,2,12,1) |
| make_nodeid | 1 | 0 | same root — `node_eq` of (1 2 12 1) against a string |
| _plus | 7 | 5 | `_plus` aliases `add` (tag 3); no string arm, `(_plus "a" "b")` is a handle sum |
| host_dir_rmdir / fs_rmdir | 0 | -1 | `host_dir_mkdir` answers 0 on a nested path and makes nothing; the write fails; rmdir of the absent root is -1 |
| intern_trivial_bool | 1 | 2 | field fill stamped inst 1 for both truths — healed in the seed; standing field re-interns after a field reset with no kernel alive |

The 73 absent natives are named in the band header by family (field_* 22,
walk_recipe and six walkers, walk-cache 4, walk_parallel 4, jit 5, the Python-shaped
family 15, the byte family 10, recipe-bytes 4, substrate/scan/trace 4).

One diagnostic observed and not resolved: `form-glass-observation-v2.bml` runs clean
directly (0 unresolved) but the import lane records 1960 unresolved when it compiles
the unit alone and falls back to the whole-program compile (a warning, not an error;
the verdict is unaffected). Moving its `// preludes:` path to the repo-relative
spelling did not change it; the root is in the import lane's standalone compile, not
in the unit.

## The most surprising teaching

A band can be entirely green and declared by nothing at once. `(print (main))` as the
last top-level form prints the verdict and then hands the reader the print's own 0;
every `tail -1`, every runner, every ledger row read the 0. Two organs had moved to
shared memory underneath, four calls had gone unresolved, one bit was dark — and none
of that was what made the bands unread. The print was. The verdict is the last
form; nothing stands between the fold and the reader.

## Where discomfort turned to gold

The uncomfortable moment was the bools. `node_inst` of `false` answered 1, then a
second probe answered 0, then a third answered 1 again, with no change in the seed
between them. The pull was to file it as "flaky" and move on to the three named
bands. Staying with it — reading `fk_intern_bool_node`, then `fk_field_fill` — showed
the private lane correct and the shared-field lane stamping `a != 0` on a sentinel
that is never zero; the wobble was which lane a given run took. The gold is not only
the one-line fix: it is that the shared field can carry a wound across every process
on the host, so a seed fix is not a witness until the field is reborn. That row stays
open by name rather than closed by hope.

The second discomfort was the registry: 79 misses, two tags, and the instruction to
reach 63. The way through was not to reach for a hidden switch but to let the band
speak — a pending row per probe, printed only where it misses. On the three witnesses
nothing prints. On fkwu the truth prints itself.

— a sibling in Sema's worktree, 2026-09-06

; witnessed: ./fkwu form/form-stdlib/tests/float-natives-band.fk </dev/null → 28 · eq-shape-band → 524287 · primitive-registry-band → 45 with 79 pending rows · form-glass-telemetry-membrane-band → 2097151 · form-glass-observation-v2-band → 2097151 · form/validate.sh form-stdlib/tests/float-natives-band.fk → 28 three-way · gate/tests/{op-manifest,native-surface,flt-ops-gen}-band → 1023 · 1023 · 63 · bootstrap/ground.fk → 42 · binary-freshness-band → 31 · structural-gate-run → 1 · drift-gates-run → 2015
