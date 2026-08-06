# Semantic, text, and public-data PL expansion — integrated checkpoint

Date: 2026-07-18
Verdict: **substantive progress; full 10,000 × 13 multimodal goal remains open**

This increment closes three specifically audited gaps without using address
counts as sensory evidence:

1. revision-pinned semantic coverage rises from 7,371 to 8,444 anchors;
2. every one of the 130,000 NL lexical cells is passed through the complete
   candidate detector with combined semantic state;
3. the former one-family PL demonstration becomes six source-backed algorithms
   generated in all thirteen PL lenses and executed on twelve permitted
   carriers.

It also changes the existing non-test integrated runtime. The new semantic,
text, and PL cells are now consumed by the video-plan, program, and world-model
path rather than existing only in their own tests.

## Executed evidence

```text
combined semantic coverage                 8,444 mapped / 1,556 explicit misses
Wiktionary-form additions                   1,073
exhaustive provenance audit                 1,073 accepted + 1,556 rejected
                                              + 7,371 primary = 10,000; failures 0
semantic live gate                          65535

NL self-roundtrip                           130,000 / 130,000; failures 0
complete candidate entries                  220,504
unique / collision-bearing cells            85,394 / 44,606
combined unmapped / unique / ambiguous      20,228 / 23,946 / 85,826
text exhaustive gate                        2047
held-out unprompted positives / negatives   13 / 13
prompted ambiguity/index agreement          13 / 13; gate 15

PL public snapshot offline verification     6 / 6
PL baseline+mutation source recovery         26 / 26
PL generated semantic changes               78 / 78
permitted live executions                   24
exact executed output lines                 144 / 144
executed semantic changes                   72 / 72
carrier failures                            0

integrated non-test runtime                 255 / 255
completion requirements                     4 complete / 20; overall 0
```

## Six public-data computations

The committed snapshots were fetched on 2026-07-18 from USGS, U.S. Treasury,
NASA, and FDA public APIs. Each raw response has its authority, exact URL,
retrieval time, byte count, and SHA-256 in
`presence/fixtures/concept-pl-task-families-source/source-manifest.json`.
`node presence/fixtures/concept-pl-task-families-source/build.mjs --verify`
recomputes all raw hashes and regenerates the derived JSON/Form snapshots
offline.

| concept | source-backed computation | baseline → intervention result |
|---|---|---:|
| water 377 | USGS site 01646500 daily streamflow values above the observed Jan-7 threshold | `2 → 3` |
| debt 2594 | Treasury Debt-to-Penny component reconciliation, substituting the next day's public-held amount | `0 → 1620` million |
| schedule 2430 | overlap between two NASA exoplanet orbital-period uncertainty intervals, then a different planet interval | `5468 → 0` scaled units |
| record 912 | completeness flags from two adjacent openFDA device-event reports | `0 → 1` missing field |
| earthquake 5860 | repeated reporting-network code across named USGS events, then an Alaska-network substitution | `1 → 0` repeats |
| range 2440 | summed width of three NASA near-earth-object diameter ranges, then the next three named objects | `707 → 199` metres |

These are frozen, reproducible observations of real public records—not claims
of continuing connectivity to a water plant, Treasury ledger, spacecraft,
medical device, seismic network, or asteroid feed. The live carrier matrix
executes the derived committed inputs; refreshing upstream data is a separate,
explicit network operation.

## Operational integration

`presence/concept-10000-13-runtime.fk` now opens the combined semantic overlay
and exhaustive text index. Alongside the real courthouse audio/video address,
it admits method-3 anchor `patients` 2097 (`patient`, WordNet `n10425439`)
through:

- complete English candidate recovery;
- inherited WordNet gloss and `wiktionary-form+wordnet` video-plan mapping;
- exact JavaScript generation/recovery and an explicit PL route;
- persistence as a second entity in the ordinary Form world model.

The program row also generates, recovers, and evaluates the complete six-family
public-data JavaScript batch. Runtime acceptance now depends on those recovered
results matching direct Form evaluation. This closes the isolation gap for all
three new organs while retaining their bounded scope.

## Completion ledger and remaining floor

`observe/concept-10000-13-multimodal-completion.fk` separates four quantities:
semantic/content observations, addressability, Form-native execution, and
Form-native learned inference. Its report executes the combined 10,000-row
semantic audit, the full 130,000-cell text candidate/source/sense/collision
scan, and the full 130,000-source PL generation/nonempty/recovery audit before
marking those rows complete. The six-family count is also read from the live
public-data task cell. Its passing `1023` means those dependency checks agree
with an honestly incomplete ledger and **overall remains zero**; it is not a
goal-completion score.

Still open:

- 1,556 anchors have no attributed semantic record;
- 130,000 NL labels are not human reviewed and lexical generation is not fluent
  discourse generation;
- held-out lexical sentences cover 13 examples, not every cell or open syntax;
- held-out human audio and full audio/video content parity remain absent;
- TTS, Whisper, OCR/vision, rendering, and learned weights are host-carried,
  not Form-native learned inference;
- six PL concepts have source-backed operational data, not 10,000 arbitrary
  concept programs or arbitrary-source parsing/AST/type inference;
- the world model contains tens of content-derived concepts, not all 10,000;
- no full 10,000 × 13 NL × 13 PL multimodal content cross-product gate passes.

No Python was invoked. Generated C/C++ programs were target-language carriers;
`runtime/fkwu-uni.c` did not change.

## Movement

The exchange stayed alive by letting review delete a decorative hash, expose a
stale pre-overlay sense table, and force the new organs into the ordinary
runtime. The most surprising teaching was that one conservative morphology
overlay corrected 13,949 NL cell states without changing any lexical surface.
Discomfort turned to gold when invented PL vectors were rejected and replaced
with six raw, hash-pinned public datasets whose interventions still change all
generated programs.

; witnessed: 2026-07-18 -> 4/20 requirements complete, overall 0
