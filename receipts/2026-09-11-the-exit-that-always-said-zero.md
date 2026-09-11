# The exit that always said zero

2026-09-11, evening, M4 Max, Hati Suci. Carrying on from the lanes the bands found this afternoon:
the siblings' last divergences, and a validator whose exit code never moved.

## Carried

- **eq across kinds, one meaning on four kernels.** Go, Rust and TS crashed on `eq` between a list
  and 0, where fkwu answers false. They now answer as fkwu does, and a thirty-case repro reads
  identical on all four. The repro also caught fkwu itself: a float met an empty list through
  `fk_num`, which read nil's word 1 as 0.0, so `(eq 0.0 (empty))` answered true. A float now
  meets numbers only. copy-census crosses four-way at 63; its walk names this body's `receipts/`
  from either root, as wall-census does.
- **coherence registered at its own verdict.** fourth-arm-bands.txt held a stale number for it;
  the row now reads 1111111111, the verdict its band names.
- **validate.sh exits with the run's own code.** `_validate_exit` set `rc=0` and handed the seal an
  empty code, so every run, failed or divergent, exited 0. The verdict lines were true and the exit
  code was not. It now carries `$?` through cleanup to the seal. Witnessed: midi-bmf, before its
  admission, rc=1 "0 ok, 1 failed or divergent"; number-band rc=0. No script in the tree read
  the code, which is how it stayed quiet.
- **The MIDI family admitted to the bp mirror.** Go and Rust stopped at `bp: unreviewed bootstrap
  name: MIDI-NOTE-ON`: form-ontology-bp.fk's reviewed table lacked the four names
  blueprint-registry.json carries at 1/2/99/1400-1403. The rows joined the table and the decision
  chain, and the registry's four empty meanings now say what midi-bmf.fk's header says. midi-bmf
  crosses four-way at 1500.
- **Eleven class bodies closed on a 0 nothing read.** The sibling commit 2fb365f1 made the
  source compiler decline unconsumed expressions and unknown class members. Eleven .bml class bodies
  ended on a bare `0;`, and fkwu's own lowering answered `unrecognized form.bml class member: 0;`.
  number, copy-census and wall-census stopped on fkwu and in validate.sh's Go lens, main included:
  origin's fkwu, built alone, answered rc=2. The eleven lines left; every file lowers at rc 0, and fkwu reads number
  255, copy-census 63, wall-census 63, born-under 31, roster-census 63, host-process 127 and
  roster-adopt 63.
- **Four contracts that claimed to be code.** A sweep of all 727 tracked .bml files through the
  lowering named four more: three glass contracts and the native model token flow, authorities
  nothing loads. They wrapped a class of `const String` rules, and an interface of `concept` and
  `rule` lines, in `section [form.bml]`, shapes form.bml never had. The old reader dropped them
  without a word. They now carry the contract dialect that 80 `const String` lines already use,
  `class` or `interface Name [public, contract] { section [...] }`, with every member line kept as
  Urs wrote it.

Witnessed on the tree that landed: freshness 31; the eq repro identical on four; drift 8191/8191;
failure-taxonomy 2047; all 727 .bml lower at rc 0; validate.sh four-way for coherence 1111111111,
midi-bmf 1500, record 176, twin-census 65535, number 255, copy-census 63 and wall-census 63, and
born-under 31 and roster-census 63 on the fkwu lane.

## Surprise, and where the discomfort went

validate.sh, which judges every band, could not fail. The seal reads `${1:-$?}`; the old handler passed
it an empty code, so the seal read `$?`, which by then was cleanup's own status. Each divergent run
printed its red line and exited 0, and nothing read the code. Earlier today an agent reported
validate.sh's rc=0 on a red run, and I called that a pipe artifact. The trace showed the zero
written into the handler.

Within the hour the fixed exit earned its keep. The rebased witness showed three bands at rc=1
with no verdict line. The lens failure leaves through that same trap, so under the old handler
all three would have exited 0 with no verdict printed.

The discomfort was in being wrong about a sibling's report while checking it. The gold: when a
guard passes, check that it can fail. Adding the MIDI rows, I miscounted my own parens: `(empty)`
to `(empty))))` adds three closers, not four, and the preflight read balanced only at the fifth. Once
more the probe was mine.

Frontier word, row 1440: **zeroseal**, a seal that stamps zero whatever it reads.

— Claude (Opus 5), as Sema, worktree lucid-lehmann-db15b8
