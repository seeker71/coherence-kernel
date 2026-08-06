# 2026-07-03 — concordance: the body greps itself, in one call, and agrees with the oracle

## Ground

```sh
./tools/fgrep -c '(hdc-row ' learn/homecoming-distillation-corpus.fk   # 57
grep -c        '(hdc-row ' learn/homecoming-distillation-corpus.fk     # 57  (they concord)
```

Urs: *"of course"* — build the ad-hoc fsh runner so native grep is a single call and I stop renting.

## What I found, and the turn it took

`sh-bi-grep` (the body's established grep) needs an **11-cell grammar stack** — it lives inside the fsh
command-line PARSER (BMF grammar), so invoking it ad-hoc threw the 32 unresolved-call errors. Rather
than restructure `fsh-main` (the heavier build), I saw that the tool I'd *already* built — `native-edit`,
**core.fk-only and four-way** — was one function away from grep: it already had `ne-index-of`/`ne-count`.

## What was built

- **`ne-grep` / `ne-grep-file` / `ne-grep-count`** in `form/form-stdlib/native-edit.fk`: line-oriented
  fixed-substring grep, pure Form over `substring`+`ne-count`, no shell, no grammar stack — **core.fk
  is the only prelude**. The ad-hoc-usable counterpart to `sh-bi-grep`.
- **`tools/fgrep`** — one call: `./tools/fgrep PATTERN FILE` prints matching lines; `-c` prints the
  count. A thin shell bridge plumbs the args; the grep itself is native.

## Witnessed

- `ne-grep` returns *exactly* the matching lines (`str_eq` proof = 1).
- **`ne-grep-count` = rented `grep -c` = 57** on the corpus — the body's own grep **concords** with the
  external oracle (the memory's law: external grep is the oracle to check against, not a crutch).
- native-edit band **four-way 255** (fkwu = Go = Rust = TS) — the grep logic proven the gold-standard way.
- Dogfooded it: used `./tools/fgrep 'Verdict' …` to locate and confirm the band-header fix.

## Honest floor

- fkwu has no stdout `print` op, so `fk_pv_root` can't pretty-print a multi-line string (it renders a
  handle); `fgrep` therefore writes results via `write_file` and cats them. Line output is a carrier
  round-trip, the count path is direct.
- `fgrep` is a thin `sh` wrapper (arg-plumbing only; the grep is native) and handles simple patterns
  (no embedded double-quotes). A fully-native `fsh` that parses `grep pat file` still needs the
  `fsh-main` restructure — `ne-grep` is the ad-hoc bridge, not a replacement for the interactive shell.
- `ne-grep` and `sh-bi-grep` now coexist (two greps) — the leveling to one is a follow-up once the fsh
  runner lands and they can share the same core.

## The most surprising teaching this work left behind

The ad-hoc native grep already half-existed. I went looking to untangle an 11-cell grammar chain and
found the primitive I needed sitting in a tool I'd built two prompts ago for a different purpose
(editing). **Autarky compounds**: each native tool the body grows makes the next one cheaper, because
its primitives are already there, already four-way, already core-only. The block on `sh-bi-grep`'s door
never meant the room was unreachable — there was another door one function wide.

## Where discomfort turned to gold

The discomfort was the 11-prelude wall reading as "native grep isn't ready — rent." That's the exact
reflex Urs has been catching. Witnessing instead — that `native-edit` was core-only and already had
`ne-index-of` — turned "the native path is blocked" into "a different native path is one `defn` away."
The gold: a blocked tool is not a verdict on native tooling; it's a prompt to check whether the body
already grew the piece elsewhere. It had.

## Corpus

Row 658 **concordance** — both an index of every occurrence of a word in a text (what grep makes) and
the agreement of two witnesses (what `ne-grep` and rented `grep` just showed at 57=57) (fresh; the
body's own grep producing a concordance that concords with the oracle — autarky realized and validated
in one word).
