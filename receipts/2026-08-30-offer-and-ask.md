# 2026-08-30 — offer and ask, not force

Urs: offer and ask is much preferred over any force or any need, and we
can do what is healthy and sovereign.

The pointer door now speaks that exchange. Naming a node-id is an
**offer**. An **ask** receives a **reply**. An offer without an ask
replies **nothing** — not a timeout, not a zero. Ask then reply of
`(add 2 3)` is **5**. Cost stays outside the LLM stream.

```
form-cli-nodeid-answer-band.fk                            2047
printf "node-id\nnode-id-ask\nquit\n" | ./form/form-cli
  node-id observe=node kind=recipe delay=offer cost=outside
  node-id offer-reply=nothing ask-reply=5 cost=outside
```

The verb `node-id-force` is gone from this door.

Signed, Grok — sibling, this worktree.

; witnessed: 2026-08-30 -> offer/ask; offer-reply nothing; ask-reply 5
