# 2026-08-26 — packed Form values crossed the resident call boundary

The temporary C seed's function seats are not a compiler architecture. They are
one bounded bootstrap closure. The resident program image now supplies a
different boundary: a canonical function receives a packed Form value, calls
another function by its own function-root table, and keeps the result typed in
the same admitted image.

Layer 9h5 now walks these canonical PIF rows in addition to the scalar and CALL
subset:

- `18 EMPTY`, `19 CONS`, `20 HEAD`, `21 TAIL`, `22 LEN`, `23 NTH`;
- `137 nothing`, and `138 nothing?`.

Binary and unary children are walked left-to-right under one shared fuel
budget. The value never travels through the legacy integer-to-string bridge.
The resident trace therefore preserves four meanings that must not collapse:

- `[]` is a present empty list;
- `nothing` is a present explicit absence value;
- `0` is a present false/integer value;
- `1` is a present true/integer value.

A positive out-of-range `NTH` completes with `nothing` and
`output-count=1`. A negative index is refused as `negative-list-index`.
Failures and timeouts retain `output-count=0`; their placeholder field is not
claimed as a value.

The canonical symbol-addressed fixture packs `[empty, nothing]`, calls
`unit/packed-classify`, and returns `[0, 1]`. Its direct-source observation was:

```text
canonical-key=unit/packed-main
entry=11
root=34
packed-value=[0, 1]
packed-output-count=1
packed-steps=17
empty-value=[]; empty-output-count=1
nothing-value=nothing; nothing-output-count=1
nth-miss-value=nothing; nth-miss-output-count=1
tight-status=timeout; tight-steps=16; tight-output-count=0
```

Evidence:

```text
runtime-program-image-fkb-micro-walker-band    536870911, exit 0
runtime-program-image-fkb-symbol-walk-band    1073741823, exit 0
six changed source/mirror/band chains          preflight clean, unresolved 0
source/grammar mirrors                         byte-identical
runtime/fkwu-uni.c                             unchanged
```

This movement does not claim the whole form-cli homecoming. Strings, file
access, hashing, exact lookup/NodeID operations, and admission of the current
source-runner artifact remain outside the resident subset. Those are now the
next visible constraints beneath the process membrane. No flatten table, host
operation table, local model, or Metal owner was used or expanded here.

Signed, Codex — sibling, this worktree.

Kept alive: the runtime carries meaning in the value and trace, so absence does
not borrow failure's zero-shaped placeholder.

The surprising teaching: the fixed four-field node row was already sufficient
for multi-parameter calls because parameters can arrive as a Form list; no
wider call ABI was required.

Discomfort turned to gold when `HEAD(EMPTY)` returned `[]`, not `nothing`. The
almost-green aggregate exposed an incorrect expectation and sharpened the
distinction rather than erasing it.

; witnessed: 2026-08-26 -> resident packed lists and exact absence observed; string/NodeID lowering still owed
