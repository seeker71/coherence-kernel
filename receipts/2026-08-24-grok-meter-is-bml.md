# 2026-08-24 — grok meter is BML, with named constants

Yes asked that new code be written in BML instead of `do` / `let` / `if`
chains, and that magic values become proper constants.

The s-expr organ in `form-cli-turn-evidence-grok.fk` is compost. The body
is now the FileIO door:

- authority `form/form-stdlib/bml/form-cli-turn-evidence-grok.bml`
  — `package`, `enum`, `interface IGrokMeter`, `template ReceiptAcc<TCount>`,
  `template SessionPaths<TPath>`, `class FormGrokEvidence`, `const` marks
  and widths
- executable `form/form-stdlib/form-cli-turn-evidence-grok.bml`
  — `class FormGrokEvidence<T>`, `field` constants, methods hoist bare
- crystallized `form/form-stdlib/form-cli-turn-evidence-grok-xtal.fk`

`@form `, `fkwu`, chunk 1048576, overlap 32, lanes 0..6, provider, model,
pipe, complete flag — all named fields. Newline is `fctg-lf()` =
`byte_to_str(10)`, not a raw `\n` inside a Form string.

```
./fkwu form/form-stdlib/form-cli-turn-evidence-grok-compile.fk
./fkwu form/form-stdlib/tests/form-cli-turn-evidence-grok-band.fk   # 1023
```

Compile still names the known chain unresolveds (`walk_recipe_here`,
`write_form_binary`, `file_byte_at`); the xtal writes anyway. The band
on that xtal is the witness: fixture receipts 255, class descriptor 256,
authority package/interface/class/const/template 512.

Bind and refresh prelude the xtal. A refresh through the BML methods
re-observed this session:

```
kind=observed scope=grok-session-form-receipts
events native=414 local=473 remote=105 total=992
share native=42 local=48 remote=10 sum=100
```

Codex evidence-band still 4095. The lowering to Form text still contains
`do`/`let` — that is crystallization, not the authored grammar.

Signed, Grok — sibling, this worktree.

; witnessed: 2026-08-24 -> grok-band 1023, share observed 42/48/10 from BML refresh
