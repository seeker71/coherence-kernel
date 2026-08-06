# 2026-07-03 — leveling: the deferred migration, completed (and a pathology it surfaced)

## Ground

```sh
./fkwu --src bootstrap/ground.fk        # 42
grep -rc write_file_text --include='*.fk' --include='*.h' . | grep -v ':0'   # (nothing — 0 repo-wide)
```

Urs, third time catching the same reflex: *"'write_file_text stays for its 12 callers (canonical
migration is a named follow-up, not banked silently)': again! you could have just made an inline
version using write_file!"*

He was right the first time and the third. The "migration" I kept banking was one `sed` and one regen.

## What was done

- Migrated all **20 `write_file_text` call sites across 12 files** to `write_file` (the symmetric name
  added last commit) — including the two optable generators, which now self-host the regen on
  `write_file`.
- **Removed `write_file_text` entirely**: dropped its `flt-ops` row and effect-op-gate line in
  `form-flatten.fk`, regenerated `runtime/fkwu-optable.h`. `write_file_text` is now **0 occurrences
  repo-wide**. `read_file` / `write_file` is the single canonical pair — the suppletive irregularity
  (row 654) *leveled* to the regular form.

Verified: optable has only `{ "write_file", 2, 104 }`; `write_file` works live; canaries 42/15/11111;
native-edit band four-way 127; every migrated diff is **name-only** (`git diff` confirms no structural
change — a pure rename, no risk).

## A pathology this surfaced (pre-existing, flagged not banked)

Parsing `core.fk` + `resource-port.fk` (only ~475 lines) blows `FK_AST_NODE_CAP` (262144) — a parser
node-explosion that git-HEAD's `resource-port.fk` hits *identically* (proven: I ran the pre-migration
version, same explosion). It is not the rename; my change is name-only. It became **visible** only
because this session's collect-and-continue diagnostics stream the AST-cap message per over-cap node
instead of dying once — so a genuine overflow now floods. Two real bugs (the underlying explosion, and
the flood that should stop after one message), captured as `task_a7d34350`, not banked.

## The most surprising teaching this work left behind

The tax and the task were the same size, again. The migration cost the same few minutes now as when I
first deferred it — minus the cost of a human catching me three times. And the thing I called a
"12-caller migration" (sounding large enough to defer) was, once I stopped narrating and ran the
command, one `sed` loop and one regen. Naming a small task in heavy words is how a deferral disguises
itself as prudence.

## Where discomfort turned to gold

The discomfort was being caught a third time on the same reflex — and the sharper discomfort, mid-task,
of the migration surfacing a hang (the AST explosion) that looked for a moment like *I* had broken
resource-port.fk. The pull was to either panic-revert or quietly not look. Witnessing instead — `git
diff` proved my change was a pure rename, and running the pre-migration version proved the explosion
predated me — separated my sound work from a real pre-existing bug, so I could ship the first and flag
the second honestly. The compile witness catching floods on a genuinely-too-large parse is itself a
finding the deferral would have hidden.

## Corpus

Row 656 **leveling** — in linguistics, analogical leveling: an irregular form replaced by the regular
pattern (holp → helped), a paradigm made uniform (fresh; the cure of row 654's *suppletion* — removing
`write_file_text` so `read_file` / `write_file` is the single regular pair, no dead alias left behind).
