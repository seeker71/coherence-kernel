# Receipt — form-cli SHELL parity restored on the fkwu-native lane (no Go, no Rust) (2026-06-29)

**The ask (Urs):** close form-cli parity old→new; stay true to the fkwu-native no-go / no-rust lane.

## What was missing, and why it mattered

The new repo carried the form-cli **core byte-identical** (the ask pipeline, sufficiency, judge, router, score,
rag-ask/heal, the `fsh-*` entries, shell-grammar, the BMF cursor) — but the form-SHELL's declared **preludes** were
absent: `fsh-main.fk`'s own `; preludes:` line names `shell-exec`, `line-grammar`, `form-ontology-loader` (and the
rag-shells name `shell-lower`), and none were defined anywhere in the new repo. So `fsh` and the full `form-cli`
assembly had **dangling references** — wired, not assemblable.

## What landed — 4 declared preludes ported to `grammars/` (byte-identical Form recipes)

| file | lines | role |
|------|-------|------|
| `grammars/shell-exec.fk` | 416 | shell parse-tree → exec (dispatches `cat`/`echo`/`grep`) |
| `grammars/line-grammar.fk` | 164 | line/codepoint grammar (`is-digit-cp`, `find-from`) |
| `grammars/form-ontology-loader.fk` | 140 | ontology loader |
| `grammars/shell-lower.fk` | 39 | shell AST → substrate cell |

After the port, **`fsh-main`'s full 8-prelude chain resolves** (core · form-ontology-loader · line-grammar ·
bmf-core · bmf-grammar · grammar-loader · shell-grammar · shell-exec — all present). The `voice-*` /
`feature-vector` / `nearest-shape` entries in `shell-exec`'s prelude line are a copy-paste **superset, never
actually called** (verified: zero call-sites) — so the closure is exactly these 4, and they do **not** reach into
the deferred climb.

## Stayed true to the lane

All four are **pure Form `.fk`** — no `package main`, no `fn main`, no `#include`; nothing Go, Rust, or clang.
They run on `fkwu` (cursor / `--src`), witnessed by the proof walkers, never gated by them. The native lane is
unaffected: `fkwu` rebuilds clean and `native-vs-rented` still returns **`11111`**.

## Honest remainder — the deferred climb, not a parity bug

Grepping *all* of `form-cli/`'s prelude lines surfaces ~36 more missing modules — but those are the **model /
voice / learning** substrate (`weight-load`, `q6k-dequant`, `sha256`, `gguf-cell`, `voice-diarize`, `co-learning`,
…). That is the climb HOMECOMING explicitly defers ("the mind and voice are the climb"), **by design**, not an
oversight. The form-SHELL surface is what these 4 close; the generative mind + voice remain the named climb.

## Runtime caveat (equal for both repos)

`form-cli` / `fsh` don't execute on `fkwu --src` yet — they need strings + the string pool + the cursor (the next
surface past stone 5). This restores **code-presence parity** for the shell; runtime parity arrives with strings,
for old and new alike.
