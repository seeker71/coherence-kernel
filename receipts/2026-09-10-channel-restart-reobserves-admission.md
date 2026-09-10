# Restart carries the same admission check

A cell re-observed each live grammar amendment, but startup reconstructed the
grammar from every digest-matching `admit` in the shared log. Those were two
different meanings of trust. The pure counterexample rejected a redefinition
of `cc-ack` live, then replayed it anyway: `(cc-ack 2)` changed from 3 to 200.

`cc-grammar-current` and the Glass grammar-id calculation now replay the ordered
log through `cc-admit-agrees?`, against each accepted prefix. That common check
requires an `admit` from the field, a matching content digest, and agreement
with the existing grammar-admission check. A refused amendment does not enter
the reconstructed grammar; subsequent valid amendments still can.

The new band exercises redefinition, foreign sender, wrong kind, torn digest,
unknown inner symbol, forward dependency, accepted dependency, duplicate replay,
empty history, evaluation after replay, and the actual startup file reader:

```
./fkwu form/form-stdlib/tests/cell-channel-replay-band.bml  # 4095
./fkwu form/form-stdlib/tests/cell-channel-band.fk         # 4095
```

Both were freshly preflighted with zero diagnostics and exited 0. The new
reader fixture writes only two files under its unique scratch directory and
removes those files afterward. It starts no cell and publishes no live state.
The 13 drift gates passed, 8191/8191. Glass's bounded reading took 36 ms, showed
24/24 held findings, and still had 392 unread rows: not a whole-body health claim.

This does not authenticate an operating-system writer. It also does not repair
the symbol check's global rather than lexical binding collection, replace the
existing evaluation subprocess with native fuel, or prove whole-session local
equivalence. The replay now has the same admission policy as live operation;
the remaining limits of that policy stay explicit.

The surprising teaching was that restart could forget a refusal while keeping
the bytes. The mismatch became a shared native replay rule and a regression
fixture. That is the concrete movement carried forward here.

Signed, Codex.
