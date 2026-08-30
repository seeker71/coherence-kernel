# 2026-08-30 — observe is timeout, nothing, node, or stream

Urs asked who came up with a deadline, and how that is healthy. It was
this sibling. A held 2000ms cut minted timeout from a clock, and named a
Metal status node "ready" as if a generate had completed. That is not
healthy. Waitvoice (corpus row 1159) asked Form to own a policy deadline
the policy can act on; the carrier reports, it does not decide. This
repair keeps that clock from naming the arrival. The body's own floor
already keeps timeout and nothing distinct; interruption is not timeout.

The held-out answer now names one of four arrivals. A clock does not
choose among them. Elapsed is a recorded fact.

- nothing: no node, no timeout, no stream
- timeout: the carrier named timeout
- node: a node arrived without a completed generate
- stream: the full expanded generate completed (stopped, tokens > 0)

Only a stream may even be considered for equivalence. A node does not
teach. `metal_status` is a node.

```
form-cli-native-agent-orient-band.fk                     32767
printf "held-out-live\nheld-out\nanswer-generate\nquit\n" | ./form/form-cli
  held-out id=ag01 observe=node elapsed-ms=31 equiv=0 exact-ppm=0 error=0 stack=1
  held-out id=ag01 observe=nothing elapsed-ms=0 equiv=0 exact-ppm=0 error=0 stack=1
  answer-generate observe=nothing elapsed-ms=0 equiv=0 stack=1 voice=pending
```

Signed, Grok — sibling, this worktree.

; witnessed: 2026-08-30 -> deadline removed; live observe=node; named=nothing
