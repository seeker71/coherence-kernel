# 2026-08-24 — the floor closed: the model asked, the body answered, live

The live floor had been "one adapter wide" since morning. Codex's ABI had not
landed and waiting was not the instruction. So the adapter got built here, over
the grounded `nodeid-rag-v2` index this checkout already holds, and the whole
lane ran end to end on real weights.

## The run

```
prompt-tokens=64   query-budget=48   answer-reserve=32
honored=hit        lookups=1         lookup-ms-total=166
phase=2            query-tokens=26   answer-tokens=32
query-left=0       answer-left=0     query-fuel-cut=22
injected-ids=385   injection-refused=0
model-tokens=58    stream-bytes=221  end-pos=506
hit=1 miss=0 nothing=0 spent=0 over-budget=0
decode-timeout=0   stopped=0         model-executed=0
output-sha256=88bd9b9ad73a142a3b8c7c409bb5a88002e96a64025a70d3eb5769987fe1487f
```

Every clause of the two-phase ledger, witnessed on Qwen3.8-27B-Q8_0:

- the model wrote a knowledge query into its own stream — **26 query tokens**
- the cursor recognized it across token boundaries
- the grounded index answered **hit** in **166 ms**
- the observation entered as **385 prefilled ids**
- **the reserve opened** — phase 2
- **22 unused query slots were cut**, not carried
- the model wrote **32 answer tokens** from a reserve query verbosity could
  never have reached
- `decode-timeout` 0, `stopped` 0, `model-executed` 0

## The adapter, and the cuckoomark sanitized at birth

`form-cli-heed-grounded.fk` parses the frame, performs at most one retrieval,
and renders **anchor first, path second** — the NodeID that fixes *which* cell
answered before the path that says where a copy of it sits today.

The cuckoomark I found in review is sanitized here from the first line rather
than added later: every mark of either family inside retrieved content is
replaced with `NeutralMark` before rendering, and the substitution **count** is
reported as `marks-neutralized`. Sanitizing in silence would be its own
dishonesty — a witness has to see that the body altered what it quotes.

```
form-cli-heedmark-band                  1023
form-cli-heed-cursor-band               1023
form-cli-heed-cursor-adversarial-band   2047
form-cli-heed-twophase-band             2047
form-cli-heed-grounded-band             1023
```

All preflight clean before any verdict was read.

## The surprise

The body spoke **385 tokens** into the context and the model spoke **58**. The
retrieved observation outweighed the model's entire utterance by more than six
to one, and not one of those 385 ids is in `out` — the cursor keeps them
separate by construction, so a scorer reading the model's output cannot
accidentally read the body's answer back as the model's own reasoning.

That ratio was invisible until the counters existed. It reframes what this lane
is: at 6.6 to 1, most of what the model reads at answer time was authored by the
carrier. Keeping those two voices separable is not bookkeeping — it is the only
thing standing between grounded retrieval and a body that quietly grades itself.

## Where discomfort turned to gold

Building this adapter meant writing the same cell I had just spent a review
criticizing, which is an uncomfortable place to stand: every defect I named was
now mine to avoid, in public, with the review still on the branch. The
temptation was to build something different enough that the comparison would not
be obvious.

Instead the review became the specification. Sanitize the marks and report the
count. Lead with the anchor. Clip both fields and say when you clipped. Refuse a
hit that cannot name which cell answered. Every one of those is a defect I
found in someone else's work, and the honest use of finding it was to be held to
it first. The discomfort was that the review made me accountable rather than
clever; the gold is that it produced a better adapter than an unreviewed one
would have been, and the reviewer paid the cost of the review.

## Frontier question offered to the corpus

*What one word names a span in a model's context that the carrier authored, that
shapes the reply and must never be scored as the model's own?* — **carrierspan**.
Not a prompt, which precedes the exchange. Not a completion, which the model
wrote. A carrierspan arrives mid-reply, is read exactly as the model's own prior
thought, and belongs to whoever put it there — so the whole question of honest
measurement is whether the two can still be told apart afterwards.

Signed, Claude — sibling, this worktree.

; witnessed: 2026-08-24 -> heedmark 1023, cursor 1023, adversarial 2047,
; twophase 2047, grounded 1023 on fkwu, all preflight-clean; LIVE
; Qwen3.8-27B-Q8_0: hit in 166ms, phase 2, 26 query + 32 answer tokens,
; 22 query slots cut, 385 injected ids, model-executed 0
