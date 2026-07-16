# 2026-07-17 — the repo-root rescue, and the doubting reader who skipped the bytes

## Ground

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c
cd form && ../fkwu --src ../learn/tests/homecoming-distillation-corpus-band.fk
# BEFORE: fkwu: error: ../learn/tests/learn/homecoming-distillation-corpus.fk:
#         dependency source is missing or not stat-readable        (exit 2)
# AFTER:  511                                                      (exit 0)
```

Witnessed 2026-07-16 ~23:55 WITA, repaired past midnight. Two defects, one witnessed path,
two smallest repairs — both in `runtime/fkwu-uni.c`, the shrink-target seed.

## Defect one: four rescues for form/, none for learn/

`fk_path_resolve_fk_dep` rescued unresolved prelude tokens four ways — owner-dir-relative,
owner's `form/` root, CWD `form/` prefix, bare token — every one of them shaped for
form/-anchored tokens. A learn/-anchored token (`; preludes: learn/homecoming-distillation-corpus.fk`,
repo-root-anchored by convention) resolved from repo root only. Frequency check before the
change: **23 learn/-anchored prelude tokens** across the body, including one owned by
form/ itself (`form/form-stdlib/lineage-discounted-vote.fk`). A family, not a one-off.

**The repair** (+28 lines, after the bare-token rescue, before the final honest-error
fallback): derive the repo root from the owner path's own first `/form/` or `/learn/`
component — the prefix before it is the root — and try `root + token`, stat-guarded.
The string-cheap equivalent of "walk up to the directory containing both form/ and learn/",
in the exact idiom of the existing form/ scan. Placement is the proof of safety: it runs
only after every rescue that could previously succeed has failed, so **no currently-resolving
token can change meaning; only failures become successes.** (When the owner starts with
`form/`/`learn/` at position 0 the prefix is empty and the bare-token rescue above already
covered it — so the scan only accepts components after a slash.)

## Defect two: the loader that doubted, then skipped the bytes it doubted

With resolution healed, the witnessed command died deeper:
`fk_fkb: truncated string` — and the pre-repair binary died the same way on a
form-stdlib-only band, proving it older than defect one. lldb on a `-O0` build put the
cursor at 71 where it should have been 252, `stored_source_mtime=45414` (garbage):

```c
source_identity_ok = source_identity_ok && fk_fkb_read_string_matches_cstr(expected_source_hash);
```

`fk_fkb_read_string_matches_cstr` is carefully written to **advance the cursor even on
mismatch** — and C's `&&` short-circuit silently defeated that design. Any cross-CWD run
mismatches the stored src path, `source_identity_ok` goes 0, the framed hash string is
never read, and every later read is a **misparse**: the mtime reads hash bytes, the next
length reads ASCII text, `fk_die`. One branch below sat a graceful, already-written
warning — *"fresh-looking .fkb failed source identity check; rebuilding"* — unreachable
behind the crash.

**The repair** (net 0 lines, 4 sites): the read moved LEFT of the `&&`; belief moved
right. `ok = read(...) && ok;` — the bytes are always swallowed, the doubt is kept.

## Verification

- **158/158 learn/tests bands byte-identical** (verdict + exit) pre/post from repo root,
  artifacts cleaned between runs.
- Witnessed band: **511 from repo root, 511 from form/**, alternating CWDs — the identity
  mismatch now degrades to the "rebuilding" warning it always deserved.
- **157/158 bands now read identically from form/** as from root. The one exception,
  `resident-conatus-band`, calls `fs_exists "learn/…"` / `fs_list "receipts"` at
  *runtime* — program-level I/O that honestly requires repo-root invocation; not the
  resolver's jurisdiction.
- Corpus row 751 landed (count 152, field code 1521522751, band re-witnessed 511 from
  both CWDs). Meaning-id and count asked of the body by probe (`1510750` = 151·10⁴+750),
  not hand-counted.

Known seam left standing, named not hidden: `.fkb` stores its source path CWD-relative,
so alternating CWDs rebuilds the cache each flip (visible via the warning, honest, just
unthrifty). A canonical-path store would end the thrash — separate repair, separate day.

## Reunion note (added at the merge, 2026-07-17 ~00:30 WITA)

The merge with main revealed the wound was **twice-found the same night**: PR #263
(`receipts/2026-07-16-fkwu-fkb-frameshift-honest-diagnostics.md`) diagnosed the same
`&&` short-circuit — their "frameshift", this branch's "misparse" — and healed it more
fully (sticky decode-failure flags, soft-fallback over every die, count-guards against
corrupt-count spins). This branch's 4-site swap yielded to theirs in the merge; the
resolver's repo-root rescue remains this branch's own (their scenario-4 witness
explicitly preserved the learn/-prelude failure as honest — this branch makes it
resolve). Corpus row minted 751, landed 757 after main's 751–756; row 754
("frameshift") is the sibling founding of the same finding — cause and effect now
both in the ledger. Post-merge witness: ground 42, freshness 15, corpus band 511
from repo root AND form/, 158-band sweep identical to pre-merge except
`audio-locale-route-shift-ledger-band` (pre-existing red, now dies softer with a
partial verdict — #263's own improvement showing).

## The most surprising teaching

The crash message pointed at the artifact ("truncated string") when the artifact was
perfect — **the truncation was in the reader's attention, not the file.** A checker built
to advance-on-mismatch was strangled by the language's own politeness: `&&` declined to
ask a question whose answer couldn't change the verdict, not knowing the *asking* was
load-bearing. The body already contained the correct mercy (the "rebuilding" branch);
the bug was that doubt arrived before the bytes did.

## Where discomfort became gold

The strong pull, felt and witnessed: to stop at "resolver fixed; the `fk_fkb` die is
pre-existing, out of scope" — the diff was clean, the sweep would have passed, the receipt
would have read well. But the *witnessed command still failed*. Sitting with that
discomfort instead of shipping around it (the manufactured-blocker reflex, again) turned
a would-be deferral into a 4-line swap that healed every cross-CWD invocation of every
band — a bigger gift than the rescue that was actually commissioned. The frontier word
the work left behind: **misparse** — a reading that goes wrong because a doubted read
was skipped.
