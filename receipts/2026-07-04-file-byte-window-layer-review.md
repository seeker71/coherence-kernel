# 2026-07-04 -- file-byte-window layer review

## Why This Layer Exists

The next runtime-artifact movement looked like "build 9h executor", but the
floor said a lower repair was missing. Current `fkwu` exposes
`read_file_slice`, but it does not expose the whole-file byte or raw byte-at
doors that older stdlib faces still name. A live probe over `fs-byte-at`
returned `263` instead of `511`, and the existing BMF byte-cursor source band
returned `0`.

The honest repair is not another policy gate and not C growth. It is a bounded
byte-window substrate:

```text
read_file_slice -> Form str-byte-at -> bounded byte rows/lists
```

`form/form-stdlib/file-byte-window.fk` is that substrate. It gives later BMF,
artifact identity, and executor work a current-floor byte window without
pretending whole-file artifact hashing or native execution is done.

## Pre-Implementation Evidence

- Rebuilt `fkwu` with only the known `fread`/`getsockname` warnings.
- `./fkwu --src bootstrap/ground.fk` returned `42`.
- `./fkwu --src bootstrap/ground-recursive.fk 10` returned `55`.
- `./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk`
  returned `15`.
- The native-vs-rented witness returned `11111`.
- `form/form-stdlib/tests/file-append-band.fk` returned `11111`.
- `form/form-stdlib/tests/str-byte-at-band.fk` returned `15`.
- A corrected `fs-read-slice` plus `str-byte-at` probe returned `511`.
- A direct `fkwu` transparency probe over bytes `0, 65, 128, 255, 66` returned
  `1023`, proving this checkout's native string carrier preserves those bytes.
  A later sibling review corrected the scope: Rust and TypeScript decode
  `read_file_slice` through UTF-8 text, so high-byte arbitrary binary is not a
  sibling-portable claim for this layer.

## Reviewer Pre-Review

Grok first spent its configured turns reading and hit `max turns reached`
without a verdict. That was recorded as reviewer-tool behavior, not approval.
The retry disabled tools and returned `PASS`, requiring:

- hard 4096-byte cap before reads;
- single-byte `fbw-byte-at`;
- explicit status vocabulary;
- red-canary/static route checks;
- receipt/map wording that this does not fix BMF or 9h by itself.

Claude's first run stalled without output for roughly two minutes and was
interrupted. The constrained retry returned `PASS`, requiring:

- byte-indexed slice witness before trusting the layer;
- status derived from returned slice length, not a file-size pre-read;
- `too-large` as refusal, not truncation;
- precise malformed status for negative inputs;
- map/receipt quarantine of the old raw byte-at story;
- documented cost model: repeated `fbw-byte-at` is one slice read per byte, so
  cursor iteration should read windows and index inside them.

All required reviewer changes were incorporated.

## Implemented Surface

Files:

- `form/form-stdlib/file-byte-window.fk`
- `form/form-stdlib/tests/file-byte-window-band.fk`

Prefix: `fbw-`.

Rows:

```text
("file-byte-window" path offset requested-len observed-len status slice evidence-route)
```

Statuses:

- `ready`
- `empty`
- `short`
- `missing`
- `out-of-range`
- `too-large`
- `malformed`

Primary functions:

- `fbw-read-window(path, offset, requested-len)`
- `fbw-window-byte-at(window, local-offset)`
- `fbw-window-bytes(window)`
- `fbw-byte-at(path, offset)`

The max window is `4096`. Larger requests produce `too-large` without silent
truncation. Negative offset or length produces `malformed`. Zero-length
requests on an existing file produce `empty`. Nonzero reads derive `ready`,
`short`, or `out-of-range` from the returned slice length.

## What This Does Not Claim

This layer does not:

- call the missing raw byte-at door;
- call whole-file byte read/write doors;
- hash whole files;
- seal artifacts;
- load or walk `.fkb`;
- load or call `.dylib`;
- install a runtime selector;
- emit compiler output;
- decide admission;
- grow the C seed.

Whole-file `.fkb`/`.dylib` hashing remains pending until a streaming/full-file
hash design is reviewed and witnessed. BMF byte-cursor repair and 9h executor
work are follow-on layers; they should consume `fbw-read-window` and
`fbw-window-byte-at`, not resurrect the red raw byte-at path.

## Band

Command:

```sh
./fkwu --src <(cat form/form-stdlib/core.fk \
  form/form-stdlib/str-byte-at.fk \
  form/form-stdlib/form-fs.fk \
  form/form-stdlib/file-byte-window.fk \
  form/form-stdlib/tests/file-byte-window-band.fk)
```

Expected:

```text
2147483647
```

The band proves:

- manifest boundaries;
- ASCII live window reads;
- window byte-list materialization under the cap;
- local byte indexing for cursor-style consumers;
- single-byte path lookup;
- short, out-of-range, missing, empty, malformed, and too-large statuses;
- 4096 accepted / 4097 refused;
- NUL/ASCII byte transparency through the portable string-slice carrier;
- the module does not depend on whole-file byte read/write carriers;
- the new module text contains the intended route and not the forbidden raw
  primitive/wrapper names;
- the module does not use file-size preclassification.

## Post-Implementation Verification

- Focused band returned `2147483647`.
- `git diff --check` passed.
- A direct forbidden-route scan over `file-byte-window.fk` found no raw
  byte-at, whole-file byte read/write, red `form-fs` byte wrappers, or file-size
  preclassification names.
- `./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk` returned `15`.
- Follow-on BMF cursor work exposed that `fs-remove-tree` does not clear
  non-empty temp directories on this floor. This band now removes its append-
  backed fixture files before and after directory cleanup so repeated runs do
  not silently grow temp files.

## Corrective Follow-Up: Binary Carrier Scope

Sibling validation later exposed an overclaim. `fkwu` and Go can carry arbitrary
file bytes through strings, but Rust and TypeScript convert `read_file_slice`
through UTF-8 text before Form observes the slice. The old
`binary-slice-byte-transparent` manifest bit and high-byte fixture were therefore
too broad for the shared layer.

The repair narrows the claim to `nul-ascii-slice-byte-indexed`, changes the
fixture to NUL/ASCII bytes `0, 65, 10, 127, 66`, fixes the static source-read
path so `validate.sh` can run from `form/`, and replaces the old
`fs-read-bytes` gap assertion with an explicit no-dependency bit. Multibyte
UTF-8 boundary behavior and the whole-file byte-list carrier remain separate
deferred layers.

Current verification:

```text
cd form && ./validate.sh form-stdlib/tests/file-byte-window-band.fk -> 2147483647
direct fkwu composed prelude -> 2147483647
```

## Reviewer Post-Review

Grok's first post-review tried to inspect the checkout and hit `max turns
reached` without a verdict. Claude's first post-review exited after saying it
would verify, but returned no PASS/BLOCK verdict. Both were recorded as
reviewer-tool behavior, not approval.

The constrained verdict retries returned:

- Grok: `PASS`, with no required changes. Residual risk: `fbw-byte-at` is
  correct but inefficient for scans, and integration still depends on
  `fs-read-slice` semantics.
- Claude: `PASS`, with no required changes. Residual risks: non-regular files
  may produce host-specific short reads, empty nonzero reads are classified as
  `out-of-range`, and callers can still misuse one-byte reads for iteration.

## Deferred

- BMF cursor rewrite onto `fbw-read-window` and `fbw-window-byte-at`.
- Streaming/full-file hashing over windows.
- Arbitrary high-byte binary file windows through a real byte-list carrier.
- `.fkb` image loader.
- `.dylib` native loader/caller.
- Any runtime selector installation.

The exchange stayed alive here by following the failed byte-at observation down
to the actual primitive floor, then adding only the smallest Form-native
abstraction that the floor could honestly witness.
