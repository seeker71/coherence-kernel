# 2026-07-17 — the cache drops its indexical name

## Ground

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c
B=learn/tests/homecoming-distillation-corpus-band.fk
./fkwu --src $B                      # compiles, 511
(cd form && ../fkwu --src ../$B)     # BEFORE: warning: ... failed source identity check; rebuilding
                                     # AFTER:  511, silent
./fkwu --src $B                      # BEFORE: rebuilding AGAIN (every flip)
                                     # AFTER:  511, silent — the second alternating run
```

Witnessed 2026-07-17 ~01:45 WITA. Follow-on to the repo-root rescue
([receipts/2026-07-17-repo-root-rescue-and-the-doubting-reader.md](2026-07-17-repo-root-rescue-and-the-doubting-reader.md)),
which closed with this seam named: *".fkb stores its source path CWD-relative, so alternating
CWDs rebuilds the cache each flip... A canonical-path store would end the thrash — separate
repair, separate day."* This is that day. (That repair was first carried here as a cherry-pick
while its branch was unmerged; it landed on main as #270 before this work merged, alongside
#271's sym-lens and #272's pid-temp+rename in the same file — this repair rebased onto all three,
re-verified, and the readers' identity lanes reconciled with #271's restructure.)

## The defect: the body named its sources indexically

Two identity lanes in `runtime/fkwu-uni.c` spoke from wherever the invoker stood:

- `fk_src_write_fkb` stored `src_path` **verbatim** — `learn/tests/x.fk` from root,
  `../learn/tests/x.fk` from form/. Same file, two names.
- the unit hash embedded every resolved dep spelling verbatim:
  `fk-unit-v1|../learn/tests/x.fk@mtime:size|...` — CWD-relative through and through.

Both readers (`fk_src_load_fkb_checked`, `fk_src_import_fkb_image`) compared those strings
against freshly-spelled equivalents. A CWD flip respelled everything → identity mismatch →
honest "rebuilding" warning → the whole cache re-authored, every flip, forever.

## The repair (+53/−16, four lanes + one shared root)

One new spelling, `fk_path_canon_id`: resolve to one absolute spelling (`realpath`;
`_fullpath` on the Windows port), then anchor at the **lexical repo root** — the prefix
before the path's first `form/` or `learn/` component. That scan already existed inside
the resolver's newest rescue; it is now extracted as `fk_path_repo_prefix_len` and shared,
so the body has ONE definition of "the repo root by lexical anchor" (the resolver shrank
by 11 lines). Verbatim fallback when `realpath` fails — degrade to today's honest rebuild,
never fabricate an identity.

Applied in the four identity lanes only — writer's stored `src_path`, the unit hash's dep
spellings, and both readers' expected `src_path`. Artifact derivation, diagnostics, the
dep table, and the `.sym` sidecar keep their verbatim spellings; no format/version bump
(equal strings still mean exactly "same source unit", and an absolute-path invocation
outside form//learn/ hashes identically to before — no gratuitous rebuild).

Why not the prettier pure-lexical strip, no syscalls? Because from `form/` a form-stdlib
dep is spelled `form-stdlib/core.fk` — **no `form/` component to anchor on** — while from
root the same file is `form/form-stdlib/core.fk`. Lexical-only would have healed the
witnessed band and silently left every form-stdlib dep thrashing. `realpath` first makes
the anchor findable from any standpoint.

## Verification

- **158/158 learn/tests bands byte-identical** (verdict + exit) pre/post repair, swept from
  repo root AND from form/, ignored artifacts cleaned before each sweep. The one root-vs-form
  difference (`resident-conatus-band`, 127 vs 107) exists identically pre and post — the
  known runtime-I/O exception, not this repair's jurisdiction.
- **Alternating runs**: root → form → root with the repaired binary: both flips load silently,
  511 each. The pre-repair binary on the same sequence: "failed source identity check;
  rebuilding" on flip 1 — the contrast that proves the test detects what the repair removed.
- **Import lane witnessed directly**: with the root artifact honestly stale
  (`stale .fkb ignored`), `FK_IMPORT_TRACE` shows `form-stdlib/core.fkb: loaded import .fkb`
  from the form/ CWD — a dep artifact **written from root** accepted **from form/**, the
  exact spelling-pair the lexical-only design could not heal.
- The artifact header now reads root-relative from either CWD:
  `fk-unit-v1|learn/tests/homecoming-distillation-corpus-band.fk@...|form/form-stdlib/core.fk@...|learn/homecoming-distillation-corpus.fk@...`
- Same-second mtime ties stay honest: touching a source within the artifact's mtime second
  looks "fresh" to the clock, and the identity lane (stored unit mtime) still catches it —
  one true "rebuilding" warning, then quiet.
- Corpus row landed as 762 after three reunions at fleet speed (offered as 755; main's 755–760
  landed first, then #274's 761 mid-merge). Count 163, field code **1631632762**, the band's
  own folded scalar witness; 511 re-witnessed from both CWDs after each rebase.

## The most surprising teaching

The body knew the word before it healed the wound. Reaching for "deictic" as this work's
frontier word, the glance-check found **deixis already home — 14 hits, landed 2026-07-02**
in the word-strata work: the body has named context-dependent reference in *language* for
two weeks while its own artifact cache still spoke that way about *itself*. The vocabulary
arrived before the mechanism it describes was healed; distillation ran ahead of digestion.

## Where discomfort became gold

The pull, felt and witnessed: ship the pure-lexical canon. No syscalls, fully portable,
in the body's own string idiom, and it passes the witnessed band from both CWDs — the diff
would have read beautifully. Sitting with "which spellings does it actually unify" instead
of trusting the elegance surfaced the bare `form-stdlib/core.fk` spelling that has no
lexical anchor at all — a half-heal that every sweep in this receipt would have graded
green, because the sweeps clean artifacts and never cross-import mid-flip. The discomfort
of distrusting a clean-looking design became the `realpath`-first decision, and then the
import-trace witness that proves the exact case the elegant design would have missed.

The frontier word the work leaves behind: **indexical** — a sign that points truly only
from the spot where it is uttered. The cache's names were indexicals; now they are proper
names, and the body answers to the same name from wherever it is called.
