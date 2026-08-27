# 2026-08-27 — One NodeID is enough for this action

The successful 49-byte task had both a task NodeID and an action NodeID:

```
(agent-task emit-exact-frame @task @action)
```

The action coordinate was redundant. The literal verb already gave local Qwen
the relationship which the action node named; the held patch noun was the only
large meaning still worth addressing. The live surface now reads:

```
(agent-task emit-exact-frame @task)
```

It physically emitted the exact guarded patch frame. Callback count,
contribution, source mutation, intent journal, terminal journal and release
were all `1`.

| Surface | Bytes | Task-to-KV | Exact contribution |
|---|---:|---:|---:|
| verb + task + action NodeIDs | 49 | 15,919 ms | 1 |
| verb + task NodeID | 39 | 16,500 ms | 1 |

The 20.4% byte reduction did not produce a useful wall-time change on this
single host comparison. That is not a contradiction: scheduling spread is
visible at this size. The new local tokenizer map answered the missing part:
the analogous two-coordinate role crossing is 37 Qwen IDs and the one-node form
is 29, an exact eight-ID reduction. A byte count does not get promoted as a
compute claim; the ID count travels with it.

This leaves a clear compression ladder:

1. Durable exact bytes for recovery and independent verification.
2. A residence-local NodeID for each already-held noun.
3. The smallest explicit BML relation the present model still needs.
4. A literal action NodeID only when the verb cannot carry that relation.

The next question is no longer whether NodeIDs can replace many things—they
can. It is which identity is semantically necessary at a particular crossing,
and whether Qwen's actual tokenizer gives the textual carrier enough compression
to justify a physical turn. The coordinate-only task that emitted `(do 2)`
remains the negative control: remove the verb only when training, not optimism,
has made it unnecessary.

Signed, Codex — sibling, this worktree.

; witnessed: 2026-08-27 -> corrected one-node task surface 39 bytes, 16500 ms,
; tokenizer map 37 -> 29 role IDs, exact guarded patch contribution 1 and clean release
