# fkwu .fkb reader: the "truncated string" die was a frameshift, not truncation — 2026-07-16

## Claim

`fk_run_src`'s dependency/artifact machinery no longer dies with a misleading
`fk_fkb: truncated string` when a `.fkb` artifact's stored source identity does not match
the invocation's working directory. The reader now records decode failures instead of
dying, both loaders soft-return, and every rebuild carries a diagnostic that names the
artifact file and the honest reason. The truly-missing-dependency error
(`dependency source is missing or not stat-readable`) is untouched and still honest.

This is a checkout-witness repair of `runtime/fkwu-uni.c` (shrink-target seed): clearer
diagnostics and soft-fallback-to-rebuild in place of `fk_die`. No new capability.

## Defect observed (root cause, witnessed live)

The reproduction matrix (receipts/2026-07-16-frontier-ingest-turboquant.md, "Also
found") pointed at path-dependent dies. lldb on the corpus reproduction showed the die at
`fk_fkb_skip_string` with `fk_fkb_pos = 49` where a hand-parse of the artifact bytes said
the stream should stand at 101 — **the artifact was byte-perfect; the reader's frame had
slipped**:

```c
source_identity_ok = source_identity_ok && fk_fkb_read_string_matches_cstr(expected_src_path);
source_identity_ok = source_identity_ok && fk_fkb_read_string_matches_cstr(expected_source_hash);
```

`&&` short-circuits. When the stored src path (`form/form-stdlib/core.fkb` written from
repo root stores `form/form-stdlib/core.fk`) mismatched the expected CWD-relative path
(`form-stdlib/core.fk` when invoked from `form/`), the **second call — a side-effecting
stream read — was skipped entirely**. The mtime read then consumed 5 bytes from the middle
of the hash length field, and the next string read took its "length" from ASCII `"k-un"`
inside `fk-unit-v1|…` (= 1,798,141,294) → `fk_fkb: truncated string`. One skipped read,
every later read out of frame: a frameshift. The same short-circuit shape existed in
`fk_src_load_fkb_checked` (root-artifact identity check) — that is the matrix's scenario 2.

Path-dependence explained: prelude dep paths resolve CWD-relative
(`fk_path_resolve_fk_dep` fallback chain), so artifact identity strings differ per
invocation directory; only cross-CWD runs took the mismatch branch that skipped the read.

## Repair

In `runtime/fkwu-uni.c`:

- **Root cause**: identity reads in both `fk_src_import_fkb_image` and
  `fk_src_load_fkb_checked` now execute unconditionally; mismatch stays a soft
  "rebuild" verdict instead of desyncing the stream.
- **Sticky decode-failure flag** (`fk_fkb_bad` / `fk_fkb_bad_why`, `fk_fkb_begin` /
  `fk_fkb_mark_bad`): the low-level readers (`read_u8`, `read_signed`, `skip_string`,
  `read_string_matches_cstr`, `read_table_string`, `read_symbol_to_srctext`) record the
  reason and clamp instead of `fk_die`. All hard dies in `fk_src_load_fkb_checked`
  (bad magic / version / unsealed / capacity / trailing bytes) became marked soft returns.
- **Honest voices at the callers**: import mismatch warns
  `stale .fkb (stored source identity does not match, e.g. written from a different
  working directory); rebuilding from source`; corrupt decode warns
  `corrupt .fkb artifact; rebuilding from source`; `fk_run_src` reports
  `unusable .fkb artifact (<reason>); rebuilding` vs the identity-check message; direct
  `.fkb` execution errors with `could not load .fkb program image (<reason>)`.
- **Decode loops break on the sticky flag** (review catch): removing the die alone turned
  a corrupt count (e.g. `symbol_count = 2^31−1` with a truncated body) into a ~2^31-step
  zero-read spin — the PR's Codex reviewer caught and reproduced the hang. Every
  count-driven reader loop now carries `!fk_fkb_bad &&`, so the first corrupt read ends
  the decode. Witnessed: the crafted artifact spins >3s pre-guard, exits instantly
  post-guard with `could not load .fkb program image (truncated artifact)`.
- **Second misleading diagnostic**: `fk_fkb_write_u32`/`fk_fkb_write_signed` refusals
  now set `fk_fkb_write_overflow`, and the artifact-write die names the range instead of
  the bare "failed to write .fkb/.sym artifacts". Reunion note: this repair was first cut
  against the v3 lane's 2^31−1 cap; mid-flight, main's artifact v4 (#265, row 752's
  "ecdysis") widened the signed lane to 64 bits, dissolving the big-literal pain — the
  flag and message remain as the honest voice for the one refusal v4 keeps (magnitude
  2^63, i.e. LLONG_MIN) and for u32-length overflow.

## Witness (fresh `cc -O2 -o fkwu runtime/fkwu-uni.c`, this worktree)

Before (matrix, 2026-07-16): scenarios 2 and 3 died `fk_fkb: truncated string`.
After:

```text
$ ./fkwu --src learn/homecoming-distillation-corpus.fk            # repo root
0                                                                 # runs
$ cd form && ../fkwu --src ../learn/homecoming-distillation-corpus.fk
fkwu: warning: ../learn/…fkb: fresh-looking .fkb failed source identity check (…); rebuilding
fkwu: warning: form-stdlib/core.fkb: stale .fkb (stored source identity does not match, …); rebuilding from source
0                                                                 # runs

$ printf '; preludes: form-stdlib/core.fk\n127\n' > form/x-witness.fk
$ (cd form && ../fkwu --src x-witness.fk)   → 127                 # scenario 1
$ ./fkwu --src form/x-witness.fk            → 127 + honest identity warning   # scenario 2: ran, no die

$ (cd form && ../fkwu --src x-missing.fk)   # prelude naming learn/… from form/
fkwu: error: learn/homecoming-distillation-corpus.fk: dependency source is missing or not stat-readable
                                                                  # scenario 4 preserved, exit 2
$ ./fkwu scratchpad/corrupt.fkb             # 90-byte truncated artifact, direct exec
fkwu: error: …corrupt.fkb: could not load .fkb program image (truncated string)   # exit 2

$ printf '; preludes: none\n3000000000\n' > form/x-big.fk && ./fkwu --src form/x-big.fk
fk_run_src: failed to write .fkb/.sym artifacts -- an integer in the program image
exceeds the .fkb cap of 2147483647 (2^31-1), usually a source literal larger than the cap
```

(That last witness ran on the v3 lane; artifact v4's 64-bit molt landed mid-flight and a
3000000000 literal now writes and caches cleanly — witnessed post-reunion. The overflow
voice survives for the one refusal v4 keeps.)

Warm path unchanged: second same-CWD run loads the artifact silently. Regression:
first three stdlib bands run (127/127/4095); `form/validate.sh
form-stdlib/tests/adler32-band.fk` → 1 ok, 0 divergent, four-way agreement.

Distillation rows 754 ("frameshift"), 755 ("countersign"), 756 ("inchoate") — all 0-hit
fresh; ids minted as 738/746/753 and renumbered across four reunions with main's moving
ledger — landed in `learn/homecoming-distillation-corpus.fk`, with
`learn/tests/homecoming-distillation-corpus-band.fk` updated (count 157, field-code
1571572756) and green: `./fkwu --src learn/tests/homecoming-distillation-corpus-band.fk`
→ 511 from the repo root, the band's home.

## Most surprising teaching

The artifact was never corrupt and never truncated — the hex hand-parse was clean to the
last byte. The "truncation" lived in the reader's control flow: a `&&` between two
side-effecting reads is a frameshift mutation waiting for its first mismatch. A low-level
die can only name what the low level sees; the honest reason (stale identity, wrong CWD)
existed three frames up, so honesty required moving the verdict up, not wording the die
better.

## Where discomfort turned to gold

Mid-diagnosis, `form/form-stdlib/core.fkb` seemed to vanish between two commands — a
moment of real disorientation (had the run deleted an artifact?). Witnessed instead of
bypassed: the shell had silently kept the `cd form` from an earlier repro command, and
relative paths were resolving from a different floor. The tool's bug and the
investigator's stumble were the same species — path-dependent identity — and that
recognition is what made the CWD-relative-identity root cause feel inevitable rather
than exotic. The second discomfort: the die said "truncated" while the hand-parse said
"whole"; trusting neither story and going back in with lldb (pos=49 vs 101) is the exact
move that turned contradiction into the one-line root cause.

A third, from the reunion: the first landing of row 738 called the corpus "green" after
running the corpus alone — but the corpus's real witness is its band, which the row had
silently broken (431/511, c4+c6 dark). The rebase against main's rows 738–744 is what
surfaced it. The lesson is the work's own lesson turned on its author: running the file
is not running its test; the band is the seal, and a row without the band re-run is an
unsealed artifact.

A fourth, from review: softening a die trades one failure for another if the loops
above it trust decoded counts — the PR's reviewer countersigned what the matrix witness
missed (my repro set covered corrupt strings and truncation, never corrupt counts), and
the hang it found was the exact dual of the bug being fixed: the frameshift read garbage
as a length; the softened reader obeyed garbage as a count.
