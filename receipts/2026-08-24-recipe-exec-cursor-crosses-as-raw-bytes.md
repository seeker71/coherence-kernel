# The recipe request crossed as raw bytes

The gap was concrete: `form-recipe-exec-token.fk` held a strict NodeID request
and typed lifecycle result, but no model-neutral cursor carried its open and
close marks across arbitrary decoded-token boundaries. Nothing joined that
request to a future resident model session without a tokenizer pre-step.

The attempt is now
`form/form-stdlib/form-cli-recipe-exec-cursor.fk`. It accepts decoded byte
chunks, holds only a bounded recognition window, distinguishes
`none | held | complete | over`, parses one complete frame through `frex-parse`,
and offers the resulting request exactly once through a generic callback. A
validated `frex-result` returns separately as one bounded
`<|form:recipe-observation|>` prefill span. The model-byte delta is never mixed
with the injected-byte delta.

The pure join reads are explicit:

- `frxc-completed-request-surface`
- `frxc-prefill-observation`
- `frxc-callback-calls`
- `frxc-byte-ledger`

The witness at
`form/form-stdlib/tests/form-cli-recipe-exec-cursor-band.fk` tries every
two-chunk split point, then streams the same frame one byte at a time. It also
tries incomplete, nested, malformed, over-budget, repeated, trailing, and
control-shaped inputs; callback mismatch and oversized/control-shaped
observations; separate nothing/failure/timeout/value-0/value-1 results; and the
whole choice/cut/undo/refine/crystallize/dissolve/release trace.

Observed on `fkwu`:

```text
binary-freshness-band.fk                         -> 31       exit 0
native-vs-rented-check                           -> 11111    exit 0
preflight form-cli-recipe-exec-cursor.fk         -> clean    exit 0
preflight form-cli-recipe-exec-cursor-band.fk    -> clean    exit 0
form-cli-recipe-exec-cursor.fk                   -> 0        exit 0
form-cli-recipe-exec-cursor-band.fk              -> 33554431 exit 0
git diff --check (the new cells)                 -> clean    exit 0
```

The seam remains named. The callback in the band only composes result data.
No model forward pass, recipe evaluation, Metal/MLX/CPU work, emitted native
code, or model-session integration ran here. The cursor returns the exact pure
values that such a session can join next; it does not claim that join already
exists.

The surprising teaching was that a content-addressed result can still carry a
control-shaped reason. Identity validation alone was not enough at the prefill
boundary, so the outer observation is re-read as strict bytes and declined if
it nests or smuggles recipe control marks.

Discomfort turned to gold twice. The first source draft was two forms open, and
preflight refused to let balance become inference. The first balanced band then
placed one closer early: the total depth was zero but the reader still found a
stray value-position close. Moving that closer kept the witness list intact and
made both preflight and the direct band agree.

— Codex, `recipe_exec_cursor` sibling
