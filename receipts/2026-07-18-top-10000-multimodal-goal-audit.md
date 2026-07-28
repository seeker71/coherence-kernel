# Top-10,000 multimodal goal — strict current-state audit

> Superseded for post-#330 counts by
> `receipts/2026-07-18-non-toy-native-multimodal-expansion.md`. This file
> remains the attributed audit of its earlier commit and must not be read as
> the current completion snapshot.

Date: 2026-07-18
Audited commit: `33d54967c` (`origin/main`)
Verdict: **not complete**

The objective audited without narrowing was:

> top 10,000 concepts represented in the Form-native model, able to detect,
> sense, and generate in audio, video, and text through the top 13 NL and PL
> lenses.

The repository now has several exhaustive 10,000 × 13 audits. They do not all
measure the same capability. The text table and program-source lanes cover the
full matrix. The full audio/video audits cover lazy addressability, plans, and
identity envelopes; semantic content has only much smaller observed samples.
Those claims must not be added together as if they proved multimodal semantic
parity.

## Fresh native ground

The checkout witness was rebuilt from the committed temporary C seed before
this audit. No source file was changed by the rebuild.

```text
bootstrap/ground.fk                                      42
bootstrap/ground-recursive.fk 10                         55
form/form-stdlib/tests/binary-freshness-band.fk          15
bootstrap/ground-numeric-list.fk                         [1, 2.5, [3, 4]]
observe/native-vs-rented.fk                              11111
```

Direct current-state gates:

```text
model/tests/concept-10000-band.fk                        127
model/tests/concept-semantics-10000-live-band.fk         1023
cognition/tests/concept-nl-semantic-13-live-band.fk      8191
cognition/tests/concept-text-detection-13-live-band.fk   262143
presence/tests/concept-pl-10000-13-live-band.fk          32767
model/tests/concept-video-generation-10000-13-band.fk    8191
model/tests/concept-audio-scale-13-band.fk               32767
model/tests/concept-audio-real-life-13-band.fk            31
model/tests/concept-video-open-label-band.fk             511
presence/tests/concept-video-open-label-live-band.fk     1023
presence/tests/concept-video-open-label-world-live-band.fk 255
```

The open-label gates reran the actual Apple Vision carrier over the committed
photographs. The local `whisper-cli`, `ffmpeg`, and `swiftc` commands were
present, but the default pinned Whisper model file was not present in this
checkout's `.cache`; the acoustic live gate therefore still requires an
externally supplied `SEMA_WHISPER_MODEL` with the pinned hash.

## Requirement and evidence matrix

| Exact requirement | Authoritative current evidence | Status | What is still missing |
|---|---|---|---|
| One ranked universe of 10,000 concepts in Form | `model/concept-10000.fk`: `c10-count`, `c10-concept-at`, `c10-detect-text`; fixed tables are exactly 300,000 and 260,000 bytes | **Partial** | Lines 7–10 name these as frequency-ranked surface lexemes, not disambiguated concepts. OpenSubtitles frequency is the ranking law, not a general concept-importance ranking. |
| Semantic representation for every anchor | `model/concept-semantics-10000.fk`: `cs10-at`, `cs10-relations`, `cs10-audit`; full index is 350,000 bytes | **Incomplete** | 7,371 anchors have WordNet semantics; 2,629 are explicit method-0 misses with no synset, gloss, sense, or relation. |
| Thirteen top NL lenses | `cognition/concept-nl-semantic-13.fk`: `cnl13-locale-codes`, `cnl13-lookup-sourced`, `cnl13-generate`; 10,001-line, 1,404,435-byte table plus 130,000 provenance bytes | **Partial** | “Top” currently means the first 13 non-pending repository locale seats; population-ranking parity is explicitly zero. No human-reviewed cells exist. |
| NL representation/generation over all 10,000 × 13 cells | Full source counts are F=10,000, W=34,941, D=16,621, C=68, G=68,370, absent=0; live band `8191` | **Structurally complete, semantically partial** | `cnl13-generate` returns one lexical label. It is not fluent sentence or discourse generation. 68,370 cells are machine-translated-unreviewed; all mapped sources remain unreviewed. |
| NL text detection/sensing across all 13 lenses | `cognition/concept-text-detection-13-runtime.fk`: `ctd13-runtime-detect-all`, `ctd13-runtime-detect-sentence`, `ctd13-runtime-analyze-senses`; live `262143` | **Partial, strongest semantic lane** | The live gate proves 13 sentences, two context rankings, one 17-candidate ambiguity, and one general scan—not every cell or open discourse. Sense state is deliberately `context-ranked-not-resolved`; ASCII-only casefold and exact lexical/substring evidence remain. |
| Thirteen PL lenses represented and generative | `presence/concept-pl-10000-13.fk`: `cp10-languages`, `cp10-generate-in`, `cp10-recover-source-in`, `cp10-audit-in`; `130,000/130,000` generated, nonempty, and recovered; live `32767` | **Structurally complete, semantically narrow** | Every program belongs to one six-signal selective-retention task family. This is not general concept-to-program generation. |
| PL detection/sensing | `cp10-recover-source-in` rejects altered sources and recovers source identity | **Partial** | Detection depends on an `FKC10` marker plus exact regeneration. It does not infer the meaning of arbitrary code or provide parser/AST/typechecking parity. |
| PL execution through all 13 lenses | `presence/concept-pl-10000-13-live.fk`: `cp10x-run-one`; 4 concepts × 12 carriers = 48 exact executions | **Incomplete** | Python is intentionally not executed under the user policy. The other 12 carriers execute four concepts, not all 10,000. Full parsing, typechecking, compilation/execution, and semantic equivalence remain absent. |
| Audio generation for 10,000 × 13 NL addresses | `model/concept-audio-scale-13.fk`: `ca13-generate`; lazy speech generation; pure address band `32767` | **Addressable, not semantically complete** | The 130,000 audit proves address arithmetic and a tone envelope, not 130,000 generated or intelligible utterances. Host `say` and `ffmpeg` carry synthesis; Kiswahili uses an English voice fallback. |
| Audio semantic detection/sensing without address leakage | `model/concept-audio-asr-13.fk`: `casr13-observe`; `presence/concept-audio-text-10000-live.fk` sends clean transcripts into `ctd13-runtime-detect-sentence`; every semantic row states no envelope | **Incomplete** | Evidence is 13 distinct daily-life concepts across 13 locales, plus an earlier 20-row/3-concept set—not 130,000 cells. Inputs are controlled synthesized speech, not noisy held-out human recordings. Whisper weights are host-rented, not Form-native, and the model is not committed. |
| Video generation for 10,000 × 13 NL addresses | `model/concept-video-generation-10000-13.fk`: `cvg13-plan-surface-in`, `cvg13-audit-addresses`; exhaustive identity audit `8191` | **Plans/address complete, semantic media incomplete** | Only three concepts have content-matched real footage. Other concepts receive captions, glosses, and deterministic semantic-hash animation with an 18-bit identity band. This is not learned/open-vocabulary video generation. |
| Video semantic detection/sensing without address leakage | `model/concept-video-open-label.fk`: `cvol-observations`; Apple Vision top-20 labels enter the complete English 10k text scan; live `1023`; world join `255` | **Incomplete** | Observed scope is 10 photographs with nine target hits and one guitar miss, plus three trajectory classes elsewhere. No 10k visual parity, 13-NL visual classification, boxes, masks, temporal tracks, relations, or Form-native visual weights. |
| One integrated operational runtime | `presence/concept-10000-13-runtime.fk`: `c1013r-address-in`; courthouse runtime score `255` | **Partial and address-based** | Its audio identity is recovered from the tone envelope; video is a plan/backdrop selection; PL is exact source recovery. The semantic audio and visual organs remain separate and cover small sets. The score cannot serve as unified multimodal semantic parity. |
| Integration with the native world model | `presence/concept-video-open-label-world-live.fk`; `presence/concept-lingbot-spatial-world-*.fk` | **Partial** | Nine photo concepts and three geometric video concepts are persisted, not 10,000. Photo positions are collection slots; trajectory concept positions are camera viewpoints, not semantic object centers. |
| Form-native model and inference | Tables, joins, scans, evidence composition, decisions, and world persistence run through `fkwu` | **Partial** | TTS, Whisper, Apple Vision, CoreText, ffmpeg, and PL compilers are host carriers. Acoustic and visual learned weights are not native Form. `AGENTS.md` separately names native generative voice as pending. |
| Full multimodal cross-product completion | No current gate | **Missing** | Nothing executes and scores semantic representation, held-out detection, sensing, and generation across 10,000 concepts × 13 NL × 13 PL and both audio/video content paths. The earlier multimodal receipt explicitly records completion parity `0`. |

## Address integrity is not semantic content

The following evidence is valid but cannot be used as proof of concept meaning:

- `ca13-sense-file` recovers concept/lens from four tone windows appended after
  speech. It proves address integrity.
- `cvg13-sense-frame-file` recovers 18 visible concept/lens bits from a bottom
  pixel band. It proves address integrity.
- `cvg13-audit-addresses` exhausts all 130,000 bit codes. It does not render or
  visually recognize all concepts.
- `cp10-recover-source-in` proves that generated source is exactly the Form
  source associated with its marker. It does not understand arbitrary source.

The content-bearing paths are narrower:

- speech-only PCM → Whisper transcript → full Form lexical scan;
- raw photograph/frame pixels → Apple Vision labels → full Form English scan;
- lexical surface/context → complete candidate recovery and transparent sense
  ranking.

Those paths are substantive, but their observed inputs are tens, not 130,000.

## Concrete completion gates still owed

Completion should not be inferred from another aggregate score. It requires
new gates whose rows retain failures and distinguish unavailable from wrong:

1. **Concept-semantic completeness**

   ```text
   proposed: model/tests/concept-semantics-10000-complete-live-band.fk
   required: 10,000 represented concepts; 0 method-0 semantic holes; explicit
             senses/relations or an attributed non-WordNet semantic record for
             every row; collision and ambiguity accounting retained
   ```

2. **Reviewed NL surface and semantic text gate**

   ```text
   proposed: cognition/tests/concept-text-10000-13-heldout-live-band.fk
   required: all 130,000 cells source-attributed; reviewed/machine states kept;
             per-cell round-trip candidate recovery; held-out sentence suites
             per locale; ambiguity scored without forced first-match claims
   ```

3. **Audio content gate—no envelope permitted**

   ```text
   proposed: presence/tests/concept-audio-content-10000-13-live-band.fk
   required per row: speech-only waveform hash, independent transcript or
                     acoustic concept observation, full-10k candidate set,
                     expected-id presence/absence, locale, speaker/noise class,
                     provenance, and address-envelope=0
   completion condition: all 10,000 concepts × 13 lenses observed on held-out
                         content with declared accuracy/abstention thresholds
   ```

4. **Video generation gate—content intervention, not identity pixels**

   ```text
   proposed: presence/tests/concept-video-content-generation-10000-13-live-band.fk
   required per row: decoded video, content-derived observation set, temporal
                     change, caption/envelope ablations, expected concept score,
                     source/weight provenance, and retained negatives
   completion condition: all 130,000 requested cells render concept-bearing
                         video whose identity survives removal of metadata,
                         captions, filenames, and address bands
   ```

5. **Video sensing gate—open 10k parity**

   ```text
   proposed: presence/tests/concept-video-sense-10000-heldout-live-band.fk
   required: held-out real videos for every concept; content-only inference;
             complete candidate sets; accuracy/coverage/abstention matrix;
             temporal tracks or an explicit not-applicable state
   ```

6. **PL semantic gate**

   ```text
   proposed: presence/tests/concept-pl-10000-13-semantic-live-band.fk
   required: more than one task family; parser/AST/typecheck states per lens;
             execution or explicit policy absence; semantic mutation tests;
             arbitrary-source detection separated from generated-source receipt
   ```

7. **Unified content-derived runtime gate**

   ```text
   proposed: presence/tests/concept-10000-13-content-runtime-live-band.fk
   required: same concept identity independently recovered from text, speech
             content, and video content; then persisted into the world model;
             no tone/pixel address signal may contribute to semantic acceptance
   ```

8. **Final cross-product audit**

   ```text
   proposed: observe/concept-10000-13-multimodal-completion.fk
   required: requirement-level counts and hashes for semantic concepts, NL text,
             PL, audio generation/sensing, video generation/sensing, and world
             integration; every component parity=1 and no weaker address score
             accepted in place of content evidence
   ```

Until those gates exist and pass at their stated scope, the exact goal remains
active.

## Movement

What kept this audit alive was refusing to let equal-looking `130000` totals
erase what each total measured. The most surprising teaching was that the
strongest full-scale result is program-source regeneration, while the most
important sensory results are deliberately much smaller. Discomfort became
gold when the integrated `255` was read bit by bit: it is a useful operational
door, but its audio and video acceptance still rests on address evidence, so it
cannot honestly close the semantic goal.

; witnessed: 2026-07-18 -> objective incomplete; current native and host gates above rerun
