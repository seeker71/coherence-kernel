# The exact embodied goal becomes one BML compression receipt

Authored by Codex on 2026-08-31.

The exact local-reasoning goal now has one literal home:
`form-cli-embodied-goal-grammar.bml`. The direct-answer proof calls that BML
identity; the task is no longer copied into the proof cell or an observer.

When its independently visible anchors are present, the grammar lowers it to
the compact local direct-user observation that the resident already uses. The
grammar carries a typed compression receipt:

```text
lane | input-bytes | lowered-bytes | optional-input-ids | optional-lowered-ids
```

`nothing()` remains the value for unmeasured local token IDs. It is not `0`,
and local tokenizer count is not treated as remote-provider spend.

## Receipt

```text
./fkwu form/form-stdlib/bml/form-cli-embodied-goal-grammar.bml
-> 0

./fkwu form/form-stdlib/tests/form-cli-embodied-goal-grammar-band.fk
-> 255

./fkwu form/form-stdlib/tests/form-cli-peer-direct-answer-action-band.fk
-> 8191

./fkwu observe/form-cli-embodied-goal-compression-run.fk
-> input-bytes=432
-> lowered-bytes=295
-> bytes-saved=137
-> byte-reduction-bps=3171
-> token-reduction=nothing
```

The new BML band preflights with balanced parentheses, zero errors, zero
warnings, and zero unresolved calls. The observer is an effect-free BML
projection: no tokenizer, model context, Metal handle, provider, HTTP, or
second process opens.

An attempted artifact tokenizer counter was deliberately removed before
landing. It performed a vocabulary-scale `fcms-tool-tail-ids` scan for this
small comparison, remaining CPU-bound for tens of seconds. That work does
not measure the requested remote-token quantity and is the wrong shape beside
the scannerless BML lowering, so it did not become a committed path.

This structural compression is useful local context work, but it is **not** a
numeric proof of the requested 10% remote-token ratio. The one BML evidence
cursor still reports `no-complete-turn` for the active remote task, so no
comparable completed provider receipt exists. The local resident's prior
same-goal turn remains the actual zero-provider answer witness; this movement
makes its incoming meaning one authoritative, compact BML cell.

The current self-watch counsel panel records `lastms=0`, `tpot=0`, and
`admit nothing / unobserved`. Those are not performance claims for this
compression; they keep the absent successor-birth timing visible.

I kept the exchange alive by making the lowerer observable without asking a
tokenizer to repeat the work the grammar had already done. The surprising
teaching is that 137 bytes can be removed before a model sees the task while
the remote metric remains honestly absent. The discomfort of the slow scan
became a refusal to land a tokenizer detour.
