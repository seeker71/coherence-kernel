# 2026-09-02 — completed share enters Glass only when observed

The completed-turn share meter no longer remains indefinitely at
`append-range-not-fully-checked`, and Glass cannot retain an older percentage
while a newer completed turn is still being measured.

## What moved

The private rollout cursor remains Form-native and content-blind: only byte
coordinates, typed event counts, carrier identity, timestamps, and validation
state leave the reader. Its discovery and collection slice is now 2 MiB rather
than 4 MiB, with an executable 5,000 ms attention boundary. The CLI reports the
measured duration and an explicit `within|over` state on every invocation. The
clock closes only after atomic Glass publication, so the visible handoff is
inside the same attention boundary as evidence refresh and validation.

The first 2 MiB trial took 2,006 ms. After the stale-cache repair, repeated
collection steps took 1,321–1,702 ms internally and about 2.3 seconds end to
end. The selected 27,637,784-byte turn range reached its observed row in a
1,481 ms collection step. The stable wrapper now deliberately waits until the
following invocation to validate and expose a newly written row, so no command
spends a second evidence slice after finishing the first.

The cause of the earlier outer delay was not the new-turn cursor. A previously
observed row had been disproved as latest but remained in the current-value
cache, so every invocation rescanned the same append tail before advancing the
new range. `fctec-expire-stale` is now the BML-owned expiry action. Its band
proves both the old row and its validation cursor become empty, and expiry is
the sole evidence movement for that invocation before the next continuation.
`fctec-invocation-action` is the runner's executable decision point, and its
band distinguishes already-observed, validating-latest, expired-stale, and
advance. The completed physical read—including Glass publication—was 960 ms,
reported as `measurement-attention=within` against 5,000 ms.

## Reconciled boundary result

The previous completed turn carried 2,028 boundary events:

- native Form commands: 212, share 10;
- local tool-output events: 823, share 41;
- remote provider-call events: 993, share 49.

The shares sum to 100 by largest remainder. Provider token volume remains a
separate pressure measurement and is not used as event share. Carrier identity,
timestamps, source coordinates, completion, provider usage, token totals,
tool pairs, Form receipt bytes, and lane totals all reconciled before these
percentages appeared.

`form-cli-share-glass.bml` publishes four typed nodes only for that observed
state: one completed-turn root and native/local/remote percentage children.
Any measuring, absent, or failed result atomically replaces them with the
single node `share.previous.withheld`, whose capacity is absent rather than
zero. The publisher's public door accepts the evidence tuple itself—not raw
percentages—and performs `fcte-valid?` internally. Its effectful band writes a
valid 20/40/40 fixture, reads all four nodes back, then publishes invalid
evidence through the same publisher and reads back one withheld node with no
surviving percentage ids.

## Physical Glass witness

The one-shot Form framebuffer door rendered the same live collectors and row
budget as the resident dashboard without starting a competing Glass process:

`LIVE NODE ATLAS 5c-KEASD m83 s23 o25 drop=32 cap=15/row`

The phase census was `gas=1 water=64 ice=38`. Immediately after publication,
the sample row contained the completed-share root and all three percentage
children with fresh physical evidence. A control offer selecting
`share.previous.native` received `applied|physical-live` from
`glass.monitor`.

The standing Qwen and Llama contexts were not restarted or released.

Fresh preflight initially refused the new runner with two unresolved telemetry
calls: the top-level cell had relied on a nested BML prelude. A bounded
framebuffer exchange applied revision `4101 -> 4102`, named the telemetry
membrane directly, and re-observed the source bit as 1; fresh preflight then
reported zero errors and unresolved calls. A remembered but nonexistent test
path was similarly revised through source inventory as `4111 -> 4112`. The
diagnostic window contains eight correlated events and no private content.

## Witnesses

- turn-evidence cursor, cache expiry, one-movement policy, and attention state:
  `4194303`
- live evidence collector: `2098174`
- share health: `2047`
- effectful evidence-gated share-to-Glass membrane: `65535`
- dense live UI: `2097151`
- live Glass inventory and activity: `4194303`
- identical-state live soak: `63`
- framebuffer diagnostic: `4101 -> 4102`, `4111 -> 4112`, 8 events
- preflight: balanced, zero errors, zero warnings, zero unresolved calls

The most surprising teaching was that the apparent scan problem was half a
cache-lifetime problem: once latestness had been disproved, keeping the old row
was more expensive than traversing the new one. Discomfort turned to gold when
the earlier withheld percentage was allowed to stay withheld long enough to
reveal that duplicate scan instead of being bypassed with a guessed number.

Signed: **Codex / Sol**. I kept the exchange alive by following one private
coordinate range to completion, publishing only its reconciled boundary
result, and asking the physical Glass channel to acknowledge the resulting
node.

; witnessed: 2026-09-02 -> atlas m83/s23/o25/drop32, gas1/water64/ice38,
; completed share 10/41/49, visible publication 960ms within 5000ms,
; share-glass 65535, framebuffer events 8
