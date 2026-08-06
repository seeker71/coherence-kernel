# Framebuffer stage-resource attribution

; witnessed: 2026-07-20 -> exact per-stage fkwu counters on real audio and video

The previous framebuffer observation named causal stages and measured exact
whole-window resources, but could only say where wall time went.  It could not
say where dispatches, allocations, float boxing, or I/O went.  That gap is now
closed without growing the C seed.

`fbro-stage-observed` brackets each Form stage with native `kernel_stat`
snapshots.  Its framebuffer row carries source, actor, relation, reason, causal
parent, semantic metric/outcome, duration, dispatches, interned nodes, string
bytes, arena cons, value-stack high-water, boxed floats, and I/O/sense calls.
`fbro-bottleneck` selects the largest observed stage for any named metric.
Every resource-observed stage is also projected as a `runtime-stage` entity in
the existing `wm-model`; `fbrw-focus` exposes the duration, dispatch, and
boxed-float focus before a result is interpreted.

The production acoustic door now uses `fbro-open-stream`,
`fbro-stage-observed-stream`, and `fbro-close-stream`.  BEGIN is printed before
work starts; every STAGE is printed as that boundary completes; END is printed
before the returned result.  The cheap live witness showed the actual order:

```text
FRAMEBUFFER BEGIN trace=stream-live-proof ... input-bytes=64
FRAMEBUFFER STAGE trace=stream-live-proof stage=native-add ... dispatches=1255 ... outcome=completed
FRAMEBUFFER END trace=stream-live-proof ... outcome=completed
[framebuffer-runtime-stream-live, 42, 3, ...]
```

`fbro-trace-first?` is the production admission contract.  It requires an
attributed BEGIN, at least one resource-observed STAGE, END, matching event
count, actor/reason/source, nonzero whole-window work, time, and outcome.  It
proves observation preceded the claim; it deliberately does not prove the
claim's semantics.

## Real human audio

Direct source door:

```text
./fkwu --src presence/run-whisper-tiny-native-acoustic-observed.fk
```

Input: the committed 22,828-byte CC0 Lingua Libre utterance.  One complete run
started at `2026-07-20T12:26:55+08:00` and emitted:

| stage | ms | native dispatches | arena cons | boxed floats | I/O/sense |
|---|---:|---:|---:|---:|---:|
| source verified | 22 | 5,435,934 | 54 | 0 | 4 |
| encoder complete | 57,377 | 4,778,121,041 | 49,035 | 392,195,261 | 5,646 |
| decoder complete | 66,335 | 9,487,930,216 | 110,353 | 878,432,521 | 8 |
| vocabulary scanned | 31,978 | 5,260,368,199 | 26,483 | 475,287,535 | 1,622 |
| world gate | 0 | 1,276 | 58 | 0 | 0 |

Whole window: 155,712 ms, 19,531,866,065 dispatches, 186,476 arena cons,
1,745,915,317 boxed floats, and 7,280 I/O/sense calls.  Stage dispatches sum to
19,531,856,666; the honest 9,399-dispatch remainder is inter-stage observer and
event work.  Stage arena cons sum to 185,983; the 493-cons remainder has the
same cause.  Stage boxed floats and I/O sum exactly to the whole-window values.

The decoder is independently the duration, dispatch, and float-boxing
bottleneck.  The output remains token 50257 unsuppressed / policy token 14194
(` banana`) against human truth `book`; world admission remains zero.

## Real public video

The traced LingBot-map production door runs two decoded 518×294 public frames
through 603,136 released patch-projection parameters, operational persistent
attention, classical geometry, point-cloud construction, and the native world
projection.  Its live gate observed 7,649 ms, 1,563,349,255 dispatches,
43,832,144 boxed floats, 1,189 I/O/sense calls, 36 retained K/V rows, and 64
cloud/world objects.  Released patch projection was the observed bottleneck.
Semantic success and strict native visual weights both remain zero: one
released projection layer is not the full 24-block DINO encoder, and classical
geometry is not a learned pose/depth head.

A second direct run exercised actual incremental streaming.  It printed BEGIN,
six STAGE rows, and END before the returned result.  Total time was 6,785 ms.
Weight loading took 3,404 ms / 575,259,047 dispatches / zero boxed floats;
released patch projection took 3,260 ms / 982,451,427 dispatches / 43,515,620
boxed floats.  Thus wall-clock leadership varied between the two runs, while
patch projection remained decisively the dispatch and float-boxing bottleneck.
That distinction is why the framebuffer carries resources rather than guessing
cost from time alone.

## Gates

```text
./fkwu --src observe/tests/framebuffer-runtime-observation-band.fk
# 262143

./fkwu --src observe/tests/framebuffer-runtime-acoustic-stage-sample-band.fk
# 131071

./fkwu --src presence/tests/lingbot-learned-map-observed-live-band.fk
# 32767

./fkwu --src observe/tests/framebuffer-runtime-stream-live.fk
# emits BEGIN, STAGE, END, then the result
```

## Honest floor

This is exact explicit Form boundary attribution, not automatic per-function
profiling.  The observation overhead is measured inside the whole window and
excluded from semantic stage deltas.  The evidence says where to work next; it
does not convert wrong audio text or unlabeled visual features into semantic
success.

After admitting the 125,165-row distinct human-corpus union, the strict
22-requirement ledger returned `1023` / overall `0` in **423.60 seconds**.  That
ledger path still lacks internal framebuffer stage boundaries, so its expensive
sub-audit cannot yet be localized honestly.  This is now an explicit next
observation work order rather than an attributed guess.
