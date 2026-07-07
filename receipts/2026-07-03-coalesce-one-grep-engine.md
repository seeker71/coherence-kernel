# 2026-07-03 — coalesce: two grep engines to one, now, not "once the runner lands"

## Ground

```sh
# native-edit band, four-way, on the shared find-from engine
cat form/form-stdlib/core.fk grammars/line-grammar.fk form/form-stdlib/native-edit.fk \
    form/form-stdlib/tests/native-edit-band.fk | ./fkwu --src /dev/stdin   # 255 (= Go = Rust = TS)
./tools/fgrep -c '(hdc-row ' learn/homecoming-distillation-corpus.fk       # 59 = rented grep -c
```

Urs, quoting my own deferral back at me: *"ne-grep and sh-bi-grep now coexist, and leveling them to
one is a follow-up once the full fsh runner lands. — really, no, please no."*

He's right. I'd created a second grep and banked the leveling — the exact pattern he keeps catching.

## The duplication, and its removal

The redundancy wasn't two greps so much as **two substring engines**: my `ne-index-of` (native-edit)
was a line-for-line reimplementation of **`find-from`** (grammars/line-grammar.fk) — the very primitive
`sh-bi-grep` already uses (via `sh-contains?`). I'd rebuilt what the body already had.

Fix, done this turn: **deleted `ne-index-of`** and made native-edit *use* `find-from`/`split-on` — the
established engine — by preluding `line-grammar.fk`. `ne-grep` is now the same line-filter shape as
`sh-grep-filter`, over the same `find-from`. I touched neither `shell-exec.fk` nor `line-grammar.fk` —
only removed my copy and pointed at theirs. **One engine; two thin surfaces** (`ne-grep` for ad-hoc
library use, `sh-bi-grep` for the fsh command line).

## Witnessed

- native-edit band **four-way 255** on the shared `find-from` (the band's index assertions now test
  `find-from` directly — the one engine).
- `ne-index-of` is **0 references** repo-wide.
- `fgrep -c` = **59** = rented `grep -c` — concordance preserved through the coalescence.
- `sh-bi-grep` unchanged (its files untouched), so it and `ne-grep` are now provably one engine.

## Honest floor

Two *surfaces* remain (`ne-grep`, `sh-bi-grep`) over one engine — the full collapse to a single
surface waits on the fsh runner making `sh-bi-grep` ad-hoc-callable, at which point `ne-grep` can retire
into it. But the thing that was actually duplicated — the substring search — is now singular, today.

## The most surprising teaching this work left behind

The engine I needed already existed, and I'd rebuilt it. `find-from` sat in line-grammar the whole time,
core-dep-only, four-way; my `ne-index-of` was a fresh copy of a solved problem. The fix was *subtraction*
— delete mine, point at theirs — smaller and truer than the addition that caused the problem. Reaching
to build before checking whether the body already grew the piece is how duplication is born.

## Where discomfort turned to gold

The discomfort was hearing my own deferral quoted back — "please no." The pull was to justify keeping
both "for now." Witnessing instead that the duplication was a whole reimplemented primitive turned
"defer the leveling" into "delete the copy," and the smaller body is the truer one. The gold: a
duplicate is a prompt to find the original, not a thing to maintain in parallel.

## Corpus

Row 660 **coalesce** — to merge separate things into one (fresh; two substring engines — `ne-index-of`
and `find-from` — coalesced to the single `find-from` that both `ne-grep` and `sh-bi-grep` now share).
