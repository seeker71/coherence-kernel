# Where the Python word stands — 2026-09-15

Claude (Opus 5), embodying Sema, in worktree `agent-ae84493a28c27049e`. The
direction: no CPython anywhere, and every known gap closed rather than
documented.

## The reading

```
before  native authoring guide: Python implementations=0 execution candidates=194 grammar inputs=10 unread=0
after   native authoring guide: Python implementations=0 execution candidates=2 grammar inputs=10 name references=163 compiled artifacts=22 unread=0
```

Both lines are `./fkwu observe/native-authoring-guide-run.fk`, the second on
the rebased tree. The two candidates still standing are the voice (below).

## Families, one by one

**The word `python` naming the language** (about 160 rows: language tables,
lens rows, `(let python ...)`, prose such as `printf("... no python ...")`,
JSON values) — classifier healed at its root, `702976b43`. In a program
source a Python word is a launch only where it stands as a command: a program
path, an interpreter literal or a bare name with an argument literal beside
it, a line with a launch door in code, or a command position followed by a
command's shape. Shell, config and extensionless files stay command languages.
A door named inside a string is prose (`release-ledger.bml:77` and `:133`
taught that). `form/form-stdlib/tests/native-authoring-guide-band.fk` reads
1048575; its twenty readings include the launches that stay launches.

**Compiled images** (`form/form-cli`, `form/form-stdlib/bootstrap/form-cli-darwin-arm64`)
— classifier healed, same commit: a file holding a NUL byte is a compiled
artifact whose text copies sources the reading meets at their own paths.

**The guide's own specimens** — classifier healed, same commit. A file that
declares the guide's reference role in its head replaces the guide's
two-path list; `form/form-stdlib/tests/form-cli-native-guide-band.fk` declares
it and still reads 7. A declared line that hands the word to a door in code
stays a launch.

**MLX trainer files hashed as lineage** (`form/scripts/native_model_lineage.sh:165`,
`:166`, `:327`, `:330`) — classifier healed: an `mlx_lm/` directory inside a
path is a file being read for its digest; an entry that runs or imports it
still reads as one.

**Dead doors, released.** `cognition/steiner-neutral-emit.fk` checked its
template with `python3` and nothing called it (`a73108ec4`);
`cbl-local-proof-command` in `form/form-stdlib/current-branch-landing.fk`
carried a `python3 scripts/validate_commit_evidence.py ...` line with no
caller (`8539091be`; `current-branch-landing-band` 16383 on all four kernels
after the rebase).

**Sibling `bp` panics** told a person to run `python3
scripts/scan_form_blueprints.py`, a file not in CK — healed to name the
registry row and the kernel's own `bp_table` (`6cb334f87`).

**Metal witnesses, home** (`e8e77e79f`). `metal_uncertainty.sh`'s anchored
splice is `form/native/metal/metal-uncertainty-splice.bml`: on the real host
harness it turns 77692 bytes into 82951, four insert-only hunks, each anchor
counted once. `ollama_oracle.sh`'s busy check and medians are
`form/native/metal/ollama-oracle-stats.bml` (`56.95 55.93 88.32 1041.00 1032.10
1050.20 1645 3` on three recorded rates; busy 1 at load 3.1 on 8 cores, 0 at
2.0). The residency audit's compile clock is `observe/clock-ms-run.bml`. The
full GPU and ollama runs were not repeated here (a 2 GB blob, the device, a
quiet host); the doors were witnessed on their own.

**e2e driver witness, home** (`31a283713`). The body's clock times both real
stages (23 ms and 82 ms, warm) and `form/scripts/e2e-driver-receipt.bml` writes
the receipt: seed `[0,1]`, generated ids `[0,2,2,2,2,2,2,2]`,
`cached_equals_recompute` true, 255 tensors.

**form-asm trig witness, home** (`ed3026197`). The body writes `_fam_sin.o`
(442 bytes), `_fam_cos.o` (434) and both point sets; `ld`, the loader and libm
do the rest. Named points: max error 6.63e-10 (sin), 6.32e-09 (cos). RoPE
sweep: N=74944, 6.62e-10 and 6.32e-09.

**Stone 44 instrument, home** (`719103eea`). `tools/local-authoring-capability/ask.bml`
writes each request body and reads each answer; curl times itself. Witnessed
on a recorded answer (`m__t1: 3.1s (warm 0.4s) load 2.7 chars 33 eval 9`); a
live ollama round was not run. `grade.sh` reads compact JSON too.

**The body's own Python model takes the judging seats** (`834c958c7`). The
lift now reads one-line def bodies, slices, keyword arguments and decorators;
eval carries list and string `+`, whole-exponent powers and slices, and its
eleven `trace` sites now answer with the name they cannot carry instead of
making up a value. On that model: `gen-conformance-band` judges its Python
page in the body (262143 before the rebase; 2097151 after it, beside
upstream's Form judge); the emitter round trip agrees with fkwu
(`[5 120 3628800]`); `emit_native_python.sh` runs end to end and the body
reads its `objects.py` (6 statements). The five Python-lane bands agree on
three arms before and after. The Python model shares Form's runtime, so the
cells say plainly that it is not a third independent executor.

## Still open

`form/form-stdlib/voice-say.bml:128` (`VSPy`, the `.mlx-venv` Python that runs
piper's espeak phonemizer) and `:51` (`VSPiper`, the venv's piper console
script for zh, ja, th and he). Letters-to-phonemes is espeak-ng: a rule engine
and one compiled dictionary per tongue (118 `*_dict` files, `en_dict` 168 KB,
`phondata` 594 KB), and this Mac holds it only inside piper's CPython
extension. The next code point is a reader of `espeak-ng-data`
(`phontab`, `phonindex`, the `<lang>_dict` rules and lists) in
`form/form-stdlib/voice-phoneme.bml`, beside `vp-ids`, answering the phoneme
list piper's `EspeakPhonemizer` answers.

## Seams seen on the way, outside the count

- `emit_native_python.sh` and the residency audit still run body cells on
  `bin-go`, and the category image comes from `bin-go --emit-binary`.
- The sibling `bp_table.*` headers name `scripts/gen_bp_table.py`, which is not
  in CK.
- The bootstrap form-cli image still carries the previous guide and the
  released landing command until its next regeneration
  (`form-cli.dependencies:149`, `:200`); nothing at landing or at startup
  reads that seal.
- The structural gate walks gitignored caches, so a run of
  `emit_native_python.sh` leaves an unclassified `objects.py` under
  `form/.cache` until it is cleared.
- `gen-conformance-band`'s proof line said Go answered its verdict too. The
  Python model's chain carries BML, which a sibling reads only through
  validate.sh's lowering, and validate runs this band on fkwu alone; the line
  now says where the verdict stands.

## The most surprising teaching

The count was mostly a name problem. Of 194 lines, about 160 were the word
`python` naming a language, and only seventeen started a process. And the
body already carried nearly all of Python: 4418 lines of evaluator stood a
handful of shapes short (one-line bodies, slices, list `+`, keyword
arguments, decorators) of judging the pages CPython was lent for.

## Where discomfort turned to gold

Moving gen-conformance's Python judge into the body felt like trading an
independent executor for a relative. I held that discomfort in view: the five
Python-lane bands before and after, the band's verdict on fkwu, and the loss
of independence written into the cell rather than smoothed over. The gold was
underneath it: eleven places where the model used to trace and walk on with a
made-up value now stop and say what they cannot carry.

## A frontier word

**wordseat** — the seat a word occupies in a line: the command's seat or the
name's seat. Question: can a reader tell a launch from a mention without
running anything? Answer: yes, when it reads the seat rather than the word.
The same seven letters are a program in a command's seat and a language's name
in a name's seat, and the guide now reads which seat the word is in.

## How the exchange stayed alive

Every claim above names the command that showed it, and the two lines still
in the count name the next code point rather than a reason to wait.
