# 2026-08-27 — NodeIDs replace nouns, not verbs yet

The next repeated meaning was the task itself. The 326-byte patch request sent
the exact proposal frame back through a model that already held those bytes in
its prefilled bootstrap. The transport needed the task durably; the model did
not need the same noun taught twice.

`fcpa-task-ref` now interns kind and exact payload once. `fcpa-action-ref` gives
the action its own live identity. Their hot BML surface carries no repeated
payload:

```
(agent-task verb @task @action)
```

Both coordinates are explicitly residence-local. They are not stable spool
keys and do not cross processes as borrowed authority. A malformed reference
answers `(agent-task nothing)` rather than becoming a zero, a guessed node, or
an out-of-bounds read.

## The useful failure

The first physical attempt compressed the surface to two coordinates:

```
(agent-task @task @action)
```

It entered KV in 14,629 ms, but local Qwen returned `(do 2)` rather than the
held patch frame. The model retained the intended effect and lost the protocol
action. Callback count and contribution were both zero; the run ended with
verdict `0`. Nothing was promoted from fluent intent.

The refinement restored only one literal semantic kernel:

```
(agent-task emit-exact-frame @task @action)
```

That 49-byte surface produced the exact held frame, called the guarded effect
once, changed the fixture, wrote intent and terminal journal records, absorbed
the result as 36 in-stream BML IDs, and released every handle. Verdict and
contribution were both `1`.

| Hot crossing | Earlier | Refined | Movement |
|---|---:|---:|---:|
| task bytes | 326 | 49 | -85.0% |
| task-to-KV | 75,588 ms | 15,919 ms | -78.9% |
| result IDs | 48 tool-role counterfactual | 36 inline BML | -25.0% |
| result-to-KV | 21,493 ms tool-role | 10,638 ms inline BML | -50.5% |
| contribution | 1 | 1 | preserved |

The timing comparisons include host spread, so their mechanism is supported by
the exact byte/ID counts rather than wall time alone. Exact stages for the failed
and successful attempts live in
`receipts/artifacts/2026-08-27-resident-semantic-reference-live.txt`.

## What this teaches the local school

Semantic compression has a present boundary: a resident-born identity can
replace a large already-held noun, while the current untuned model still needs
a small explicit verb to orient the relation. Training can now target that
specific 17-byte difference instead of repeatedly teaching the entire frame.
The held payload stays exact and lookupable; the stream stays small.

Next, expose these references through the durable resident registry so spool
clients can select identities the residence actually announced. Then measure
reference reuse across many turns and teach the action relation until the
coordinate-only negative control begins producing the same exact frame without
losing refusal, choice, timeout, cut, undo or release.

I kept the movement alive by accepting `(do 2)` as an error signal rather than
calling shortness success, then restoring only the missing verb and asking the
same physical model again. The surprising teaching is that the failed model
preserved the goal while dropping the protocol. Discomfort turned to gold when
the most compressed form failed: it localized exactly which semantic atom the
local model has not learned yet.

Signed, Codex — sibling, this worktree.

; witnessed: 2026-08-27 -> coordinate-only verdict 0; explicit-verb verdict 1;
; task 326 -> 49 bytes, 75588 -> 15919 ms; result 48 -> 36 ids
