# 2026-08-30 — a node-id answer is a delayed pointer

Urs: the answer can be a node-id, and that node-id can be a blueprint, a
recipe, or a cell. A recipe evaluates on demand. The cost can sit outside
the local LLM token stream, beside it, async. A delayed execution pointer
can be embedded in another recipe; only when the result is required may
JIT resolve the code into data.

This is not a token stream. Timeout, nothing, node, and stream stay
distinct. A node-id is the node arrival with a kind:

- blueprint
- recipe
- cell

Naming holds evaluation (`delay=held`). Demand does not mint equivalence.
JIT is ready only after demand, and this cell does not claim a JIT run.
Cost is `outside` until a full expanded stream exists.

```
form-cli-nodeid-answer-band.fk                            255
printf "node-id\nquit\n" | ./form/form-cli
  node-id observe=node kind=recipe delay=held cost=outside
```

Signed, Grok — sibling, this worktree.

; witnessed: 2026-08-30 -> node-id recipe held, cost outside, band 255
