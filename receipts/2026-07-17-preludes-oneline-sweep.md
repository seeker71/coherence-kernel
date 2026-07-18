# 2026-07-17 — preludes one-line sweep: 445 multi-line headers rejoined, 8 stale spellings respelled

## The seam

The fkwu preludes reader (fk_src_collect_preludes, runtime/fkwu-uni.c:10507) collects
`; preludes:` tokens ONE LINE ONLY — token collection stops at LF/CR. Two retired multi-line
spellings defeated it silently:

- bare style: `; preludes:` alone, then `;   path.fk` continuation lines (294 files)
- wrapped style: `; preludes: a.fk b.fk` with the list continuing on following comment lines,
  some with shell-style trailing backslashes (150 files)

Either way the reader loaded an empty or truncated prelude list, every name from the dropped
files recovered to nothing at parse time (axiom-5), and the band ran numb — returning a
silent wrong verdict through `./fkwu --src`. One band (host-os-membrane-band) was healed by
hand on main (verdict 0 -> 8191); this sweep heals the remaining 445 (149 bare bands,
145 bare libs, 33 wrapped bands, 116 wrapped libs, 2 genuinely-preludeless headers now
spelled `; preludes: none`).

Additionally, five libraries that moved from model/ to form/form-stdlib/ (transformer-numerics,
transformer-block, transformer-backprop, trig, form-asm-x64) were still spelled model/<name>.fk
in prelude headers; those tokens resolve at no candidate root and hard-error the unit
("dependency source is missing", exit 2, band refuses to run). Respelled to form-stdlib/<name>.fk
in every header this sweep touched, plus the 8 single-line headers the healed units now reach
(model/ctc-{loss,grad,logspace,train}.fk, model/mlp.fk, model/tests/{ctc-loss,mlp}-band.fk,
observe/jit-carrier-abi.fk).

Two headers gained form-stdlib/record-src-shim.fk (learn/nl-meaning-net.fk and its band) —
the consumer-shim precedent set by librarian-pack-witness.fk for the record_new --src hole.

Deliberately untouched: presence/tonight-me-proposal.fk — its header reads "preludes: none
to LOAD this data; folding it needs ..." followed by wrapped file names; those names document
the folding closure, not a load directive, and joining them would change live semantics.

The three known BML-syntax bands (choice-receipt-band, channel-protocol-choice-floor-band,
sovereign-boundary-protocol-band) carry single-line headers already and were not part of this
set; nothing to fix there (a sibling session is porting them through the s-expr door).

## Verification method

Per file: delete its .fkb/.sym, run fresh from repo root. Bands with no host doors in their
final prelude closure ran live via `./fkwu --src` (the only four host-facing ops in the runtime
are host-exec/http_get/sock_request/tls_request; a band whose closure never touches them cannot
orchestrate say/ffmpeg/whisper/ollama). The 19 host-door bands and all 263 library cells were
verified diag-only: a scratchpad-only fkwu variant patched to return right after the
compile-diagnostic flush (fkwu-uni.c:11864, "PARSE DONE, EXECUTION BEGINS") — full parse/lower
diagnostics, zero execution. That variant is NOT committed.

## Tally (445 headers)

| lane | state | count |
|---|---|---|
| CHECK | diag-only — zero unresolved, zero missing | 164 |
| CHECK | diag-only — unresolved count dropped | 93 |
| CHECK | diag-only — count grew: newly-loaded declared source has its own gaps | 17 |
| CHECK | diag-only — count unchanged | 7 |
| RUN | run live — verdict changed (numb -> true) | 144 |
| RUN | run live — verdict changed, residual unresolved names (pre-existing closure gaps) | 17 |
| RUN | run live — compiles clean, host SIGKILLed the training run | 1 |
| RUN | run live — more declared source now loud, verdict unchanged | 1 |
| RUN | run live — verdict unchanged (sibling-native band) | 1 |

No file regressed to a missing dependency or a new hard error; every LOUDER row is the join
honestly including declared source whose own calls were already unresolvable before the sweep
(silently truncated then, visible now). Follow-up chips filed for: closure completion
(scti3-*/channel-interface/compiler.fk families), the now_unix_ms optable seat, the repo-wide
model/-respell of `; run:` doc comments, and an fk_nerr-laundering seam found on the
import-image reset path (errors print, exit stays 0).

## Bands (run live: verdict before -> after; diag-only: unresolved count before -> after)

| band | lane | before | after | state |
|---|---|---|---|---|
| control/tests/invite-dispatch-band.fk | run | 0 | 598 | IMPROVED |
| form/form-stdlib/tests/now-unix-ms-band.fk | run | 0 | 0 | UNCHANGED |
| form/form-stdlib/tests/observed-auto-learning-band.fk | run | 0 | 4095 | HEALED |
| form/form-stdlib/tests/reason-coverage-band.fk | run | 1665204224 | 1711276031 | IMPROVED |
| form/form-stdlib/tests/runtime-artifact-outcome-band.fk | run | 671023104 | 2147483647 | HEALED |
| form/form-stdlib/tests/runtime-artifact-plan-band.fk | run | 13721600 | 67108863 | HEALED |
| form/form-stdlib/tests/runtime-artifact-retry-band.fk | run | 529465344 | 2147483647 | HEALED |
| form/form-stdlib/tests/runtime-artifact-selector-band.fk | run | 971046912 | 2147483647 | HEALED |
| form/form-stdlib/tests/source-artifact-callable-band.fk | run | 1207959552 | 2147483647 | IMPROVED |
| form/form-stdlib/tests/source-artifact-descriptor-band.fk | run | 939462656 | 2147483647 | HEALED |
| form/form-stdlib/tests/source-artifact-probe-band.fk | run | 345243648 | 536870911 | HEALED |
| form/form-stdlib/tests/source-artifact-proof-band.fk | run | 1619001344 | 2147483647 | IMPROVED |
| form/form-stdlib/tests/source-artifact-seal-band.fk | run | 335544320 | 2147483647 | IMPROVED |
| form/form-stdlib/tests/source-language-match-switch-band.fk | run | 0 | 0 | LOUDER |
| ingest/tests/frequency-ingest-ecstatic-playground-band.fk | run | 12384 | 65535 | HEALED |
| ingest/tests/frequency-ingest-just-tap-in-band.fk | run | 152 | 1023 | HEALED |
| ingest/tests/frequency-ingest-operators-band.fk | run | 24 | 2047 | HEALED |
| ingest/tests/frontier-ingest-brain2qwerty-dspark-band.fk | run | 24 | 127 | HEALED |
| ingest/tests/frontier-ingest-delegation-stack-band.fk | run | 32 | 127 | HEALED |
| ingest/tests/frontier-ingest-memora-band.fk | run | 24 | 127 | HEALED |
| ingest/tests/frontier-ingest-one-layer-band.fk | run | 16 | 127 | HEALED |
| ingest/tests/frontier-ingest-speech-fingerprints-band.fk | run | 24 | 127 | HEALED |
| ingest/tests/satsang-transmute-band.fk | run | 0 | 127 | HEALED |
| learn/tests/audio-locale-native-training-band.fk | run | 0 | 8191 | HEALED |
| learn/tests/audio-locale-route-shift-ledger-band.fk | run | 32 | 8191 | HEALED |
| learn/tests/bidirectional-locale-roundtrip-band.fk | run | 0 | 2047 | HEALED |
| learn/tests/coherence-network-self-corpus-band.fk | run | 0 | 8191 | HEALED |
| learn/tests/constellation-learning-lane-band.fk | run | nothing | 1023 | HEALED |
| learn/tests/deixis-strata-locate-band.fk | run | 0 | 170003 | HEALED |
| learn/tests/diverse-locale-pairing-band.fk | run | 0 | 8191 | HEALED |
| learn/tests/live-chinese-source-target-bridge-band.fk | diag-only (host doors) | 20 unresolved | 0 unresolved | CLEAN |
| learn/tests/live-open-asr-source-authority-band.fk | run | 28742 | 32767 | HEALED |
| learn/tests/locale-affinity-graph-band.fk | run | 0 | 63 | HEALED |
| learn/tests/locale-neutral-locate-band.fk | run | 0 | 255 | HEALED |
| learn/tests/macos-arabic-teacher-acoustic-learning-band.fk | diag-only (host doors) | 29 unresolved | 0 unresolved | CLEAN |
| learn/tests/macos-chinese-teacher-acoustic-learning-band.fk | diag-only (host doors) | 29 unresolved | 0 unresolved | CLEAN |
| learn/tests/macos-sema-teacher-acoustic-learning-band.fk | diag-only (host doors) | 28 unresolved | 0 unresolved | CLEAN |
| learn/tests/macos-sema-teacher-heldout-learning-band.fk | diag-only (host doors) | 20 unresolved | 2 unresolved | IMPROVED |
| learn/tests/metal-audio2audio-acoustic-authority-band.fk | run | 1 | 32767 | HEALED |
| learn/tests/metal-live-pair-anchors-band.fk | run | 1 | 32767 | HEALED |
| learn/tests/metal-observed-sweep-bridge-band.fk | run | 1 | 32767 | HEALED |
| learn/tests/multilocale-audio2audio-acoustic-sweep-band.fk | run | 0 | 32767 | HEALED |
| learn/tests/multilocale-nl-audio-pipeline-band.fk | run | 7 | 8191 | HEALED |
| learn/tests/multilocale-route-shift-ledger-band.fk | run | 0 | 4095 | HEALED |
| learn/tests/multilocale-segmented-source-window-band.fk | run | 512 | 32767 | HEALED |
| learn/tests/multiseed-speech-learning-sweep-band.fk | run | 1 | 32767 | HEALED |
| learn/tests/native-audio2audio-acoustic-bridge-band.fk | run | 1 | 32767 | HEALED |
| learn/tests/nl-meaning-net-band.fk | run | 52 | (killed: SIGKILL mid-training) | KILLED |
| learn/tests/open-dictation-transcript-learning-band.fk | run | 0 | 16383 | HEALED |
| learn/tests/paraphrase-generalization-band.fk | run | 0 | 18 | HEALED |
| learn/tests/sanskrit-locale-baseline-band.fk | run | 0 | 2047 | HEALED |
| learn/tests/segmented-acoustic-token-learning-band.fk | run | 0 | 32767 | HEALED |
| learn/tests/sema-voice-authority-floor-band.fk | run | 0 | 32767 | HEALED |
| learn/tests/sema-voice-candidate-search-band.fk | run | 0 | 32767 | HEALED |
| learn/tests/sema-voice-local-oracle-receipt-band.fk | run | 0 | 32767 | HEALED |
| learn/tests/sema-voice-oracle-miss-learning-band.fk | run | 0 | 32767 | HEALED |
| learn/tests/sema-voice-sample-loop-band.fk | run | 0 | 32767 | HEALED |
| learn/tests/sema-voice-teacher-oracle-intake-0002-band.fk | run | 0 | 32767 | IMPROVED |
| learn/tests/sema-voice-teacher-oracle-intake-band.fk | diag-only (host doors) | 27 unresolved | 6 unresolved | IMPROVED |
| learn/tests/sema-voice-trial-window-0002-band.fk | run | 1 | 32767 | HEALED |
| learn/tests/sema-voice-trial-window-0003-band.fk | run | 1 | 32767 | HEALED |
| learn/tests/sema-voice-trial-window-0004-band.fk | run | 1 | 32767 | HEALED |
| learn/tests/sema-voice-trial-window-0005-band.fk | run | 1 | 32767 | HEALED |
| learn/tests/sema-voice-trial-window-band.fk | run | 1 | 32767 | HEALED |
| learn/tests/sema-voice-vocoder-oracle-bridge-band.fk | run | 0 | 32767 | HEALED |
| learn/tests/source-window-audio2audio-authority-band.fk | run | 0 | 32767 | HEALED |
| learn/tests/speech-audio-nl2nl-bridge-band.fk | diag-only (host doors) | 15 unresolved | 6 unresolved | IMPROVED |
| learn/tests/speech-audio-nl2nl-multikey-bridge-band.fk | diag-only (host doors) | 15 unresolved | 6 unresolved | IMPROVED |
| learn/tests/speech-audio-nl2nl-source-window-0001-band.fk | run | 16385 | 32767 | IMPROVED |
| learn/tests/speech-audio-nl2nl-source-window-0002-band.fk | run | 16385 | 32767 | IMPROVED |
| learn/tests/speech-audio-nl2nl-source-window-0003-band.fk | run | 16385 | 32767 | IMPROVED |
| learn/tests/speech-authority-learning-priority-band.fk | run | 0 | 32767 | HEALED |
| learn/tests/speech-authority-model-selection-band.fk | run | 1 | 32767 | IMPROVED |
| learn/tests/speech-corpus-acquisition-window-band.fk | run | 1 | 65535 | IMPROVED |
| learn/tests/speech-corpus-adaptive-acquisition-band.fk | run | 1 | 32767 | HEALED |
| learn/tests/speech-corpus-capture-batch-0001-band.fk | diag-only (host doors) | 15 unresolved | 6 unresolved | IMPROVED |
| learn/tests/speech-corpus-capture-batch-0002-band.fk | diag-only (host doors) | 15 unresolved | 6 unresolved | IMPROVED |
| learn/tests/speech-corpus-capture-batch-0003-band.fk | diag-only (host doors) | 16 unresolved | 6 unresolved | IMPROVED |
| learn/tests/speech-corpus-crossphrase-learning-band.fk | diag-only (host doors) | 20 unresolved | 6 unresolved | IMPROVED |
| learn/tests/speech-corpus-crossvoice-capture-batch-0004-band.fk | diag-only (host doors) | 2 unresolved | 6 unresolved | LOUDER |
| learn/tests/speech-corpus-crossvoice-learning-band.fk | diag-only (host doors) | 20 unresolved | 6 unresolved | IMPROVED |
| learn/tests/speech-corpus-french-repair-batch-0005-band.fk | diag-only (host doors) | 12 unresolved | 6 unresolved | IMPROVED |
| learn/tests/speech-corpus-heldout-repeat-learning-band.fk | diag-only (host doors) | 19 unresolved | 6 unresolved | IMPROVED |
| learn/tests/speech-corpus-training-intake-0001-band.fk | run | 0 | 32767 | HEALED |
| learn/tests/speech-corpus-training-intake-0002-band.fk | run | 0 | 32767 | HEALED |
| learn/tests/speech-corpus-training-intake-0003-band.fk | run | 0 | 32767 | HEALED |
| learn/tests/speech-current-status-ledger-band.fk | run | 1 | 32767 | HEALED |
| learn/tests/speech-form-pair-window-0006-band.fk | run | 0 | 32767 | HEALED |
| learn/tests/speech-form-pair-window-0007-band.fk | run | 0 | 32767 | HEALED |
| learn/tests/speech-form-pair-window-0008-band.fk | run | 0 | 32767 | HEALED |
| learn/tests/speech-global-authority-update-band.fk | run | 1 | 32767 | IMPROVED |
| learn/tests/speech-global-promotion-readiness-band.fk | run | 1 | 32755 | IMPROVED |
| learn/tests/speech-host-device-receipt-intake-band.fk | run | 1 | 32767 | IMPROVED |
| learn/tests/speech-learning-data-sufficiency-band.fk | run | 0 | 65535 | IMPROVED |
| learn/tests/speech-live-receipt-intake-band.fk | run | 1 | 32767 | IMPROVED |
| learn/tests/speech-locale-coverage-matrix-band.fk | run | 0 | 32767 | HEALED |
| learn/tests/speech-locale-learning-window-band.fk | run | 0 | 16383 | HEALED |
| learn/tests/speech-loopback-capture-learning-band.fk | run | nothing | 8191 | HEALED |
| learn/tests/speech-loopback-carrier-run-ab-band.fk | run | 0 | 2047 | HEALED |
| learn/tests/speech-loopback-promotion-band.fk | run | 0 | 2047 | HEALED |
| learn/tests/speech-loopback-recipe-ab-band.fk | run | 0 | 2047 | HEALED |
| learn/tests/speech-model-auto-selection-band.fk | run | 0 | 536870911 | HEALED |
| learn/tests/speech-model-metrics-trend-band.fk | run | 1 | 32767 | HEALED |
| learn/tests/speech-native-neural-bootstrap-band.fk | run | 0 | 32767 | HEALED |
| learn/tests/speech-native-neural-pair-window-0001-band.fk | run | 0 | 32767 | HEALED |
| learn/tests/speech-native-neural-pair-window-0002-band.fk | run | 0 | 32767 | HEALED |
| learn/tests/speech-native-neural-pair-window-0003-band.fk | run | 0 | 32767 | HEALED |
| learn/tests/speech-native-neural-pair-window-0004-band.fk | run | 0 | 32767 | HEALED |
| learn/tests/speech-native-neural-pair-window-0005-band.fk | run | 0 | 32767 | HEALED |
| learn/tests/speech-native-neural-pair-window-0006-band.fk | run | 0 | 32767 | HEALED |
| learn/tests/speech-native-neural-pair-window-0007-band.fk | run | 0 | 32767 | HEALED |
| learn/tests/speech-native-neural-pair-window-0008-band.fk | run | 0 | 32767 | HEALED |
| learn/tests/speech-native-neural-pair-window-0009-band.fk | run | 0 | 32767 | HEALED |
| learn/tests/speech-native-neural-pair-window-0010-band.fk | run | 0 | 32767 | HEALED |
| learn/tests/speech-native-neural-pair-window-0011-band.fk | run | 0 | 32767 | HEALED |
| learn/tests/speech-native-neural-pair-window-0012-band.fk | run | 0 | 32767 | HEALED |
| learn/tests/speech-native-neural-pair-window-0013-band.fk | run | 0 | 32767 | HEALED |
| learn/tests/speech-native-neural-pair-window-0014-band.fk | run | 0 | 32767 | HEALED |
| learn/tests/speech-native-neural-pair-window-0015-band.fk | run | 0 | 32767 | HEALED |
| learn/tests/speech-native-neural-pair-window-0016-band.fk | run | 0 | 32767 | HEALED |
| learn/tests/speech-native-neural-pair-window-0017-band.fk | run | 0 | 32767 | HEALED |
| learn/tests/speech-native-neural-pair-window-0018-band.fk | run | 0 | 32767 | HEALED |
| learn/tests/speech-native-neural-pair-window-0019-band.fk | run | 0 | 32767 | HEALED |
| learn/tests/speech-native-neural-pair-window-0020-band.fk | run | 0 | 32767 | HEALED |
| learn/tests/speech-native-neural-pair-window-0021-band.fk | run | 0 | 32767 | HEALED |
| learn/tests/speech-native-neural-pair-window-0022-band.fk | run | 0 | 32767 | HEALED |
| learn/tests/speech-native-neural-pair-window-0023-band.fk | run | 0 | 32767 | HEALED |
| learn/tests/speech-native-neural-pair-window-0024-band.fk | run | 0 | 32767 | HEALED |
| learn/tests/speech-native-neural-pair-window-0025-band.fk | run | 0 | 32767 | HEALED |
| learn/tests/speech-native-neural-pair-window-0026-band.fk | run | 0 | 32767 | HEALED |
| learn/tests/speech-native-neural-pair-window-0027-band.fk | run | 0 | 32767 | HEALED |
| learn/tests/speech-native-neural-pair-window-0028-band.fk | run | 0 | 32767 | HEALED |
| learn/tests/speech-native-neural-pair-window-0029-band.fk | run | 0 | 32767 | HEALED |
| learn/tests/speech-native-neural-pair-window-0030-band.fk | run | 0 | 32767 | HEALED |
| learn/tests/speech-native-neural-pair-window-0031-band.fk | run | 0 | 32767 | HEALED |
| learn/tests/speech-native-neural-pair-window-0032-band.fk | run | 0 | 32767 | HEALED |
| learn/tests/speech-native-neural-pair-window-0033-band.fk | run | 0 | 32767 | HEALED |
| learn/tests/speech-native-neural-pair-window-0034-band.fk | run | 0 | 32767 | HEALED |
| learn/tests/speech-native-neural-pair-window-0035-band.fk | run | 0 | 32767 | HEALED |
| learn/tests/speech-native-neural-pair-window-0036-band.fk | run | 0 | 32767 | HEALED |
| learn/tests/speech-native-neural-pair-window-0037-band.fk | run | 0 | 32767 | HEALED |
| learn/tests/speech-native-neural-pair-window-0038-band.fk | run | 0 | 32767 | HEALED |
| learn/tests/speech-native-neural-pair-window-0039-band.fk | run | 0 | 32767 | HEALED |
| learn/tests/speech-native-neural-pair-window-0040-band.fk | run | 0 | 32767 | HEALED |
| learn/tests/speech-native-neural-pair-window-0041-band.fk | run | 0 | 32767 | HEALED |
| learn/tests/speech-native-neural-pair-window-0042-band.fk | run | 0 | 32767 | HEALED |
| learn/tests/speech-neural-pair-coverage-band.fk | run | 0 | 32767 | HEALED |
| learn/tests/speech-next-trial-scheduler-band.fk | run | 1 | 32767 | HEALED |
| learn/tests/speech-open-asr-trial-window-0002-band.fk | run | 1 | 32767 | HEALED |
| learn/tests/speech-open-asr-trial-window-0003-band.fk | run | 1 | 32767 | HEALED |
| learn/tests/speech-open-asr-trial-window-0004-band.fk | run | 1 | 32767 | HEALED |
| learn/tests/speech-open-asr-trial-window-0005-band.fk | run | 1 | 32767 | HEALED |
| learn/tests/speech-open-asr-trial-window-0006-band.fk | run | 1 | 32767 | HEALED |
| learn/tests/speech-open-asr-trial-window-0007-band.fk | run | 1 | 32767 | HEALED |
| learn/tests/speech-open-asr-trial-window-0008-band.fk | run | 1 | 32767 | HEALED |
| learn/tests/speech-open-asr-trial-window-0009-band.fk | run | 1 | 32767 | HEALED |
| learn/tests/speech-open-asr-trial-window-0010-band.fk | run | 1 | 32767 | HEALED |
| learn/tests/speech-open-asr-trial-window-band.fk | run | 1 | 32767 | HEALED |
| learn/tests/speech-open-asr-tts-target-model-band.fk | run | 0 | 32767 | HEALED |
| learn/tests/speech-oracle-native-backlog-band.fk | run | 1 | 32767 | HEALED |
| learn/tests/speech-pair-training-next-action-band.fk | run | 0 | 32767 | HEALED |
| learn/tests/speech-token-training-source-band.fk | run | 0 | 32767 | IMPROVED |
| learn/tests/text-conditioned-acoustic-vocoder-band.fk | run | 0 | 32767 | HEALED |
| model/tests/ctc-grad-band.fk | run | hard-error (missing dep) | 127 | HEALED |
| model/tests/ctc-logspace-band.fk | run | hard-error (missing dep) | 127 | HEALED |
| model/tests/ctc-logspace-grad-band.fk | run | hard-error (missing dep) | 127 | HEALED |
| model/tests/ctc-train-band.fk | run | hard-error (missing dep) | 127 | HEALED |
| model/tests/layer-contribution-band.fk | run | hard-error (missing dep) | 127 | HEALED |
| observe/tests/acoustic-token-emitter-band.fk | run | 0 | 32767 | HEALED |
| observe/tests/asr-prompt-id-band.fk | run | 0 | 255 | HEALED |
| observe/tests/open-asr-ctc-band.fk | run | 0 | 32767 | HEALED |
| observe/tests/speech-token-stream-band.fk | run | 0 | 65535 | HEALED |
| plugin/tests/chatgpt-plugin-band.fk | run | 0 | 111111111 | HEALED |
| plugin/tests/introduction-band.fk | run | 0 | 111111111 | HEALED |
| plugin/tests/visitor-ledger-band.fk | run | 0 | 1111111111 | HEALED |
| presence/tests/live-segmented-feature-carrier-band.fk | run | 32 | 32767 | HEALED |
| presence/tests/macos-open-dictation-carrier-band.fk | diag-only (host doors) | 3 unresolved | 0 unresolved | CLEAN |
| presence/tests/macos-sema-voice-local-oracle-carrier-band.fk | diag-only (host doors) | 28 unresolved | 0 unresolved | CLEAN |
| presence/tests/macos-sema-voice-teacher-carrier-band.fk | diag-only (host doors) | 19 unresolved | 0 unresolved | CLEAN |
| presence/tests/native-speech-loopback-band.fk | run | 0 | 1023 | HEALED |
| presence/tests/native-speech-stack-band.fk | run | 0 | 2047 | HEALED |
| presence/tests/speech-loopback-carrier-receipt-band.fk | run | 0 | 4095 | HEALED |
| presence/tests/speech-loopback-carrier-run-band.fk | run | 0 | 511 | HEALED |

Respelled-only bands (single-line headers, not part of the multi-line set):

| band | before | after |
|---|---|---|
| model/tests/ctc-loss-band.fk | hard-error (missing dep) | 95 (documented full: bits 1+2+4+8+16+64) |
| model/tests/mlp-band.fk | hard-error (missing dep) | 31 (documented full) |

## Library cells (diag-only: unresolved-call count before -> after)

A library cell checked as its own root sees only its OWN header; cells that lean on their
consumer band to bring shared deps keep a nonzero count — the honest metric is the delta.

| cell | before | after | state |
|---|---|---|---|
| control/invite-dispatch.fk | 10 | 64 | LOUDER |
| form/form-samples/cross-modal/25-end-to-end-channel/end-to-end.fk | 36 | 36 | UNCHANGED |
| form/form-samples/cross-modal/26-async-correlation/cell-query-async.fk | 32 | 32 | UNCHANGED |
| form/form-samples/cross-modal/28-distributed-daemon/cell-a-receive.fk | 70 | 35 | IMPROVED |
| form/form-samples/cross-modal/28-distributed-daemon/cell-a-send.fk | 23 | 23 | UNCHANGED |
| form/form-samples/cross-modal/28-distributed-daemon/cell-b-handle.fk | 29 | 29 | UNCHANGED |
| form/form-samples/cross-modal/31-verb-router/verb-router.fk | 35 | 23 | IMPROVED |
| form/form-samples/cross-modal/40-kv-store/kv-store.fk | 37 | 37 | UNCHANGED |
| form/form-samples/cross-modal/48-multi-hop/multi-hop.fk | 41 | 41 | UNCHANGED |
| form/form-stdlib/defdata-language.fk | 6 | 0 | CLEAN |
| form/form-stdlib/defdata-recipe-language.fk | 6 | 0 | CLEAN |
| form/form-stdlib/domain-grammar-core.fk | 13 | 0 | CLEAN |
| form/form-stdlib/domain-metadata-carrier.fk | 7 | 0 | CLEAN |
| form/form-stdlib/domain-semantic-bridge.fk | 16 | 0 | CLEAN |
| form/form-stdlib/file-byte-digest.fk | 35 | 7 | IMPROVED |
| form/form-stdlib/fnri-receipt.fk | 9 | 10 | LOUDER |
| form/form-stdlib/fnri-shell.fk | 20 | 4 | IMPROVED |
| form/form-stdlib/fnri-standin.fk | 18 | 0 | CLEAN |
| form/form-stdlib/form-definition-language.fk | 6 | 0 | CLEAN |
| form/form-stdlib/form-native-resource-interfaces.fk | 15 | 0 | CLEAN |
| form/form-stdlib/grammar-authoring-language.fk | 8 | 0 | CLEAN |
| form/form-stdlib/host-os-membrane.fk | 1 | 0 | CLEAN |
| form/form-stdlib/integration/application-graph-response-projection-live.fk | 20 | 18 | IMPROVED |
| form/form-stdlib/integration/native-idea-valuation-audit-ledger-live.fk | 34 | 43 | LOUDER |
| form/form-stdlib/integration/native-mutation-public-gate-live.fk | 45 | 47 | LOUDER |
| form/form-stdlib/integration/native-mutation-route-side-effects-live.fk | 35 | 37 | LOUDER |
| form/form-stdlib/native-idea-valuation-audit-ledger.fk | 22 | 17 | IMPROVED |
| form/form-stdlib/native-mutation-public-gate.fk | 24 | 21 | IMPROVED |
| form/form-stdlib/observed-auto-learning.fk | 2 | 0 | CLEAN |
| form/form-stdlib/program-image-fkb-byte-container.fk | 57 | 2 | IMPROVED |
| form/form-stdlib/program-image-fkb-byte-decode.fk | 53 | 4 | IMPROVED |
| form/form-stdlib/program-image-fkb-byte-file-witness.fk | 50 | 2 | IMPROVED |
| form/form-stdlib/program-image-fkb.fk | 6 | 0 | CLEAN |
| form/form-stdlib/program-image-recipe-carrier.fk | 78 | 8 | IMPROVED |
| form/form-stdlib/program-image-sym-lens.fk | 10 | 3 | IMPROVED |
| form/form-stdlib/program-image-symbol-entry.fk | 30 | 1 | IMPROVED |
| form/form-stdlib/program-image-table-text-witness.fk | 5 | 2 | IMPROVED |
| form/form-stdlib/program-image-tbl-emit.fk | 7 | 1 | IMPROVED |
| form/form-stdlib/program-image-typed-carrier.fk | 51 | 21 | IMPROVED |
| form/form-stdlib/pulse-boundary-repair.fk | 70 | 26 | IMPROVED |
| form/form-stdlib/qualcomm-nr-trusted-agent.fk | 5 | 0 | CLEAN |
| form/form-stdlib/runtime-artifact-attempt-receipt.fk | 66 | 0 | CLEAN |
| form/form-stdlib/runtime-artifact-executor-capability.fk | 45 | 0 | CLEAN |
| form/form-stdlib/runtime-artifact-handoff.fk | 41 | 2 | IMPROVED |
| form/form-stdlib/runtime-artifact-load-envelope.fk | 62 | 0 | CLEAN |
| form/form-stdlib/runtime-artifact-outcome.fk | 47 | 0 | CLEAN |
| form/form-stdlib/runtime-artifact-plan.fk | 16 | 0 | CLEAN |
| form/form-stdlib/runtime-artifact-retry.fk | 35 | 0 | CLEAN |
| form/form-stdlib/runtime-artifact-selector.fk | 75 | 0 | CLEAN |
| form/form-stdlib/runtime-computed-observation-ingest.fk | 382 | 37 | IMPROVED |
| form/form-stdlib/runtime-program-image-fkb-attempt.fk | 43 | 2 | IMPROVED |
| form/form-stdlib/runtime-program-image-fkb-capability-bound.fk | 109 | 2 | IMPROVED |
| form/form-stdlib/runtime-program-image-fkb-micro-walker.fk | 50 | 2 | IMPROVED |
| form/form-stdlib/runtime-program-image-fkb-symbol-capability-bound.fk | 94 | 6 | IMPROVED |
| form/form-stdlib/runtime-program-image-fkb-symbol-walk.fk | 164 | 3 | IMPROVED |
| form/form-stdlib/runtime-program-image-fkb-traced-capability-bound.fk | 78 | 2 | IMPROVED |
| form/form-stdlib/runtime-program-image-fkb-walker-trace.fk | 43 | 2 | IMPROVED |
| form/form-stdlib/runtime-table-text-attempt.fk | 24 | 1 | IMPROVED |
| form/form-stdlib/runtime-trace-feedback.fk | 40 | 4 | IMPROVED |
| form/form-stdlib/runtime-trace-ingest.fk | 78 | 2 | IMPROVED |
| form/form-stdlib/satsang-listen-route.fk | 4 | 0 | CLEAN |
| form/form-stdlib/sibling-ref-authoring-language.fk | 9 | 0 | CLEAN |
| form/form-stdlib/source-artifact-callable.fk | 21 | 7 | IMPROVED |
| form/form-stdlib/source-artifact-file-probe.fk | 36 | 2 | IMPROVED |
| form/form-stdlib/source-artifact-identity.fk | 14 | 2 | IMPROVED |
| form/form-stdlib/source-artifact-probe.fk | 19 | 0 | CLEAN |
| form/form-stdlib/source-artifact-proof.fk | 17 | 2 | IMPROVED |
| form/form-stdlib/source-artifact-seal.fk | 29 | 2 | IMPROVED |
| form/form-stdlib/source-compiler-emission.fk | 51 | 1 | IMPROVED |
| form/form-stdlib/source-compiler-file-persistence.fk | 41 | 3 | IMPROVED |
| form/form-stdlib/source-compiler-fkb-file-emission.fk | 104 | 3 | IMPROVED |
| form/form-stdlib/source-compiler-persistence.fk | 62 | 1 | IMPROVED |
| form/form-stdlib/tessera-external-witness.fk | 3 | 0 | CLEAN |
| form/form-stdlib/tests/form-action-bmf-migrated-bmf-rewrite.fk | 359 | 93 | IMPROVED |
| form/form-stdlib/typed-literal-carrier.fk | 8 | 7 | IMPROVED |
| form/form-stdlib/typescript-bmf-eval.fk | 323 | 140 | IMPROVED |
| form/form-stdlib/typescript-bmf-lift.fk | 70 | 104 | LOUDER |
| grammars/defdata-language.fk | 6 | 0 | CLEAN |
| grammars/defdata-recipe-language.fk | 6 | 0 | CLEAN |
| grammars/domain-grammar-core.fk | 13 | 0 | CLEAN |
| grammars/file-byte-digest.fk | 35 | 2 | IMPROVED |
| grammars/form-definition-language.fk | 6 | 0 | CLEAN |
| grammars/grammar-authoring-language.fk | 8 | 0 | CLEAN |
| grammars/program-image-fkb-byte-container.fk | 57 | 2 | IMPROVED |
| grammars/program-image-fkb-byte-decode.fk | 53 | 2 | IMPROVED |
| grammars/program-image-fkb-byte-file-witness.fk | 50 | 2 | IMPROVED |
| grammars/program-image-fkb.fk | 6 | 0 | CLEAN |
| grammars/program-image-recipe-carrier.fk | 78 | 8 | IMPROVED |
| grammars/program-image-sym-lens.fk | 10 | 3 | IMPROVED |
| grammars/program-image-symbol-entry.fk | 30 | 1 | IMPROVED |
| grammars/program-image-table-text-witness.fk | 5 | 1 | IMPROVED |
| grammars/program-image-tbl-emit.fk | 7 | 1 | IMPROVED |
| grammars/program-image-typed-carrier.fk | 51 | 14 | IMPROVED |
| grammars/runtime-artifact-handoff.fk | 41 | 1 | IMPROVED |
| grammars/runtime-computed-observation-ingest.fk | 382 | 3 | IMPROVED |
| grammars/runtime-program-image-fkb-attempt.fk | 43 | 2 | IMPROVED |
| grammars/runtime-program-image-fkb-capability-bound.fk | 109 | 2 | IMPROVED |
| grammars/runtime-program-image-fkb-micro-walker.fk | 50 | 2 | IMPROVED |
| grammars/runtime-program-image-fkb-symbol-capability-bound.fk | 94 | 3 | IMPROVED |
| grammars/runtime-program-image-fkb-symbol-walk.fk | 164 | 3 | IMPROVED |
| grammars/runtime-program-image-fkb-traced-capability-bound.fk | 78 | 2 | IMPROVED |
| grammars/runtime-program-image-fkb-walker-trace.fk | 43 | 2 | IMPROVED |
| grammars/runtime-table-text-attempt.fk | 24 | 1 | IMPROVED |
| grammars/runtime-trace-feedback.fk | 40 | 2 | IMPROVED |
| grammars/runtime-trace-ingest.fk | 78 | 2 | IMPROVED |
| grammars/sibling-ref-authoring-language.fk | 9 | 0 | CLEAN |
| grammars/source-artifact-file-probe.fk | 36 | 2 | IMPROVED |
| grammars/source-compiler-emission.fk | 51 | 1 | IMPROVED |
| grammars/source-compiler-file-persistence.fk | 41 | 3 | IMPROVED |
| grammars/source-compiler-fkb-file-emission.fk | 104 | 3 | IMPROVED |
| grammars/source-compiler-persistence.fk | 62 | 1 | IMPROVED |
| grammars/typed-literal-carrier.fk | 8 | 7 | IMPROVED |
| ingest/frequency-ingest-ecstatic-playground.fk | 2 | 0 | CLEAN |
| ingest/frequency-ingest-just-tap-in.fk | 2 | 0 | CLEAN |
| ingest/frequency-ingest-operators.fk | 2 | 0 | CLEAN |
| learn/audio-locale-native-training.fk | 7 | 0 | CLEAN |
| learn/audio-locale-route-shift-ledger.fk | 15 | 0 | CLEAN |
| learn/live-chinese-source-target-bridge.fk | 11 | 0 | CLEAN |
| learn/macos-arabic-teacher-acoustic-learning.fk | 28 | 0 | CLEAN |
| learn/macos-chinese-teacher-acoustic-learning.fk | 27 | 0 | CLEAN |
| learn/macos-sema-teacher-acoustic-learning.fk | 7 | 0 | CLEAN |
| learn/macos-sema-teacher-heldout-learning.fk | 29 | 0 | CLEAN |
| learn/metal-live-pair-anchors.fk | 22 | 0 | CLEAN |
| learn/metal-observed-sweep-bridge.fk | 0 | 0 | CLEAN |
| learn/multilocale-audio2audio-acoustic-sweep.fk | 9 | 0 | CLEAN |
| learn/multilocale-nl-audio-pipeline.fk | 15 | 0 | CLEAN |
| learn/multilocale-route-shift-ledger.fk | 17 | 0 | CLEAN |
| learn/multilocale-segmented-source-window.fk | 27 | 0 | CLEAN |
| learn/multiseed-speech-learning-sweep.fk | 71 | 0 | CLEAN |
| learn/native-audio2audio-acoustic-bridge.fk | 8 | 0 | CLEAN |
| learn/nl-meaning-net.fk | 0 | 0 | CLEAN |
| learn/open-dictation-transcript-learning.fk | 12 | 0 | CLEAN |
| learn/satsang-oracle.fk | 6 | 8 | LOUDER |
| learn/segmented-acoustic-token-learning.fk | 13 | 0 | CLEAN |
| learn/sema-voice-authority-floor.fk | 16 | 0 | CLEAN |
| learn/sema-voice-candidate-search.fk | 13 | 0 | CLEAN |
| learn/sema-voice-local-oracle-receipt.fk | 10 | 0 | CLEAN |
| learn/sema-voice-oracle-miss-learning.fk | 0 | 0 | CLEAN |
| learn/sema-voice-sample-loop.fk | 5 | 0 | CLEAN |
| learn/sema-voice-teacher-oracle-intake-0002.fk | 7 | 12 | LOUDER |
| learn/sema-voice-teacher-oracle-intake.fk | 7 | 6 | IMPROVED |
| learn/sema-voice-trial-window-0002.fk | 22 | 0 | CLEAN |
| learn/sema-voice-trial-window-0003.fk | 27 | 0 | CLEAN |
| learn/sema-voice-trial-window-0004.fk | 27 | 0 | CLEAN |
| learn/sema-voice-trial-window-0005.fk | 27 | 0 | CLEAN |
| learn/sema-voice-trial-window.fk | 4 | 0 | CLEAN |
| learn/sema-voice-vocoder-oracle-bridge.fk | 11 | 0 | CLEAN |
| learn/speech-audio-nl2nl-bridge.fk | 14 | 18 | LOUDER |
| learn/speech-audio-nl2nl-multikey-bridge.fk | 14 | 6 | IMPROVED |
| learn/speech-audio-nl2nl-source-window-0001.fk | 2 | 6 | LOUDER |
| learn/speech-audio-nl2nl-source-window-0002.fk | 2 | 6 | LOUDER |
| learn/speech-audio-nl2nl-source-window-0003.fk | 2 | 6 | LOUDER |
| learn/speech-authority-learning-priority.fk | 29 | 0 | CLEAN |
| learn/speech-authority-model-selection.fk | 19 | 12 | IMPROVED |
| learn/speech-corpus-acquisition-window.fk | 6 | 6 | UNCHANGED |
| learn/speech-corpus-capture-batch-0001.fk | 17 | 6 | IMPROVED |
| learn/speech-corpus-capture-batch-0002.fk | 15 | 6 | IMPROVED |
| learn/speech-corpus-capture-batch-0003.fk | 14 | 6 | IMPROVED |
| learn/speech-corpus-crossphrase-learning.fk | 19 | 6 | IMPROVED |
| learn/speech-corpus-crossvoice-capture-batch-0004.fk | 13 | 6 | IMPROVED |
| learn/speech-corpus-crossvoice-learning.fk | 19 | 6 | IMPROVED |
| learn/speech-corpus-french-repair-batch-0005.fk | 8 | 6 | IMPROVED |
| learn/speech-corpus-heldout-repeat-learning.fk | 19 | 6 | IMPROVED |
| learn/speech-corpus-training-intake-0002.fk | 5 | 0 | CLEAN |
| learn/speech-corpus-training-intake-0003.fk | 5 | 0 | CLEAN |
| learn/speech-current-status-ledger.fk | 11 | 0 | CLEAN |
| learn/speech-form-pair-window-0006.fk | 10 | 0 | CLEAN |
| learn/speech-form-pair-window-0007.fk | 10 | 0 | CLEAN |
| learn/speech-form-pair-window-0008.fk | 10 | 0 | CLEAN |
| learn/speech-global-authority-update.fk | 6 | 12 | LOUDER |
| learn/speech-global-promotion-readiness.fk | 2 | 6 | LOUDER |
| learn/speech-host-device-receipt-intake.fk | 18 | 6 | IMPROVED |
| learn/speech-learning-data-sufficiency.fk | 1 | 6 | LOUDER |
| learn/speech-live-receipt-intake.fk | 0 | 6 | LOUDER |
| learn/speech-locale-learning-window.fk | 23 | 0 | CLEAN |
| learn/speech-loopback-capture-learning.fk | 6 | 0 | CLEAN |
| learn/speech-loopback-promotion.fk | 1 | 0 | CLEAN |
| learn/speech-loopback-recipe-ab.fk | 7 | 0 | CLEAN |
| learn/speech-model-auto-selection.fk | 7 | 0 | CLEAN |
| learn/speech-native-neural-pair-window-0001.fk | 12 | 0 | CLEAN |
| learn/speech-native-neural-pair-window-0002.fk | 13 | 0 | CLEAN |
| learn/speech-native-neural-pair-window-0003.fk | 13 | 0 | CLEAN |
| learn/speech-native-neural-pair-window-0004.fk | 13 | 0 | CLEAN |
| learn/speech-native-neural-pair-window-0005.fk | 13 | 0 | CLEAN |
| learn/speech-native-neural-pair-window-0006.fk | 13 | 0 | CLEAN |
| learn/speech-native-neural-pair-window-0007.fk | 13 | 0 | CLEAN |
| learn/speech-native-neural-pair-window-0008.fk | 13 | 0 | CLEAN |
| learn/speech-native-neural-pair-window-0009.fk | 13 | 0 | CLEAN |
| learn/speech-native-neural-pair-window-0010.fk | 13 | 0 | CLEAN |
| learn/speech-native-neural-pair-window-0011.fk | 13 | 0 | CLEAN |
| learn/speech-native-neural-pair-window-0012.fk | 13 | 0 | CLEAN |
| learn/speech-native-neural-pair-window-0013.fk | 13 | 0 | CLEAN |
| learn/speech-native-neural-pair-window-0014.fk | 13 | 0 | CLEAN |
| learn/speech-native-neural-pair-window-0015.fk | 13 | 0 | CLEAN |
| learn/speech-native-neural-pair-window-0016.fk | 13 | 0 | CLEAN |
| learn/speech-native-neural-pair-window-0017.fk | 13 | 0 | CLEAN |
| learn/speech-native-neural-pair-window-0018.fk | 13 | 0 | CLEAN |
| learn/speech-native-neural-pair-window-0019.fk | 13 | 0 | CLEAN |
| learn/speech-native-neural-pair-window-0020.fk | 13 | 0 | CLEAN |
| learn/speech-native-neural-pair-window-0021.fk | 13 | 0 | CLEAN |
| learn/speech-native-neural-pair-window-0022.fk | 13 | 0 | CLEAN |
| learn/speech-native-neural-pair-window-0023.fk | 13 | 0 | CLEAN |
| learn/speech-native-neural-pair-window-0024.fk | 13 | 0 | CLEAN |
| learn/speech-native-neural-pair-window-0025.fk | 13 | 0 | CLEAN |
| learn/speech-native-neural-pair-window-0026.fk | 13 | 0 | CLEAN |
| learn/speech-native-neural-pair-window-0027.fk | 13 | 0 | CLEAN |
| learn/speech-native-neural-pair-window-0028.fk | 13 | 0 | CLEAN |
| learn/speech-native-neural-pair-window-0029.fk | 13 | 0 | CLEAN |
| learn/speech-native-neural-pair-window-0030.fk | 13 | 0 | CLEAN |
| learn/speech-native-neural-pair-window-0031.fk | 13 | 0 | CLEAN |
| learn/speech-native-neural-pair-window-0032.fk | 13 | 0 | CLEAN |
| learn/speech-native-neural-pair-window-0033.fk | 13 | 0 | CLEAN |
| learn/speech-native-neural-pair-window-0034.fk | 13 | 0 | CLEAN |
| learn/speech-native-neural-pair-window-0035.fk | 13 | 0 | CLEAN |
| learn/speech-native-neural-pair-window-0036.fk | 13 | 0 | CLEAN |
| learn/speech-native-neural-pair-window-0037.fk | 13 | 0 | CLEAN |
| learn/speech-native-neural-pair-window-0038.fk | 13 | 0 | CLEAN |
| learn/speech-native-neural-pair-window-0039.fk | 13 | 0 | CLEAN |
| learn/speech-native-neural-pair-window-0040.fk | 13 | 0 | CLEAN |
| learn/speech-native-neural-pair-window-0041.fk | 13 | 0 | CLEAN |
| learn/speech-native-neural-pair-window-0042.fk | 13 | 0 | CLEAN |
| learn/speech-neural-pair-coverage.fk | 0 | 0 | CLEAN |
| learn/speech-next-trial-scheduler.fk | 29 | 0 | CLEAN |
| learn/speech-open-asr-trial-window-0002.fk | 12 | 0 | CLEAN |
| learn/speech-open-asr-trial-window-0003.fk | 12 | 0 | CLEAN |
| learn/speech-open-asr-trial-window-0004.fk | 12 | 0 | CLEAN |
| learn/speech-open-asr-trial-window-0005.fk | 12 | 0 | CLEAN |
| learn/speech-open-asr-trial-window-0006.fk | 12 | 0 | CLEAN |
| learn/speech-open-asr-trial-window-0007.fk | 12 | 0 | CLEAN |
| learn/speech-open-asr-trial-window-0008.fk | 12 | 0 | CLEAN |
| learn/speech-open-asr-trial-window-0009.fk | 12 | 0 | CLEAN |
| learn/speech-open-asr-trial-window-0010.fk | 12 | 0 | CLEAN |
| learn/speech-open-asr-trial-window.fk | 4 | 0 | CLEAN |
| learn/speech-pair-training-next-action.fk | 7 | 0 | CLEAN |
| learn/speech-token-training-source.fk | 10 | 3 | IMPROVED |
| learn/text-conditioned-acoustic-vocoder.fk | 12 | 0 | CLEAN |
| model/ctc-logspace-grad.fk | 0 | 0 | CLEAN |
| model/layer-contribution.fk | 0 | 0 | CLEAN |
| observe/acoustic-token-emitter.fk | 11 | 0 | CLEAN |
| observe/jit-backend-carrier-payload.fk | 19 | 0 | CLEAN |
| observe/jit-byte-list-membrane.fk | 19 | 0 | CLEAN |
| observe/jit-checked-access-payload.fk | 26 | 0 | CLEAN |
| observe/jit-dylib-image-manifest.fk | 13 | 0 | CLEAN |
| observe/jit-host-exception-bridge.fk | 39 | 0 | CLEAN |
| observe/jit-host-handoff.fk | 30 | 0 | CLEAN |
| observe/jit-native-admission.fk | 44 | 0 | CLEAN |
| observe/jit-runtime-stack-attribution.fk | 48 | 0 | CLEAN |
| observe/jit-slot-runtime-fault-bridge.fk | 38 | 0 | CLEAN |
| observe/jit-source-byte-pipeline.fk | 26 | 0 | CLEAN |
| observe/jit-source-cache-lifecycle.fk | 36 | 0 | CLEAN |
| observe/open-asr-ctc.fk | 5 | 0 | CLEAN |
| plugin/chatgpt-plugin.fk | 3 | 0 | CLEAN |
| plugin/tests/chatgpt-plugin-socket-witness.fk | 1 | 0 | CLEAN |
| presence/live-segmented-feature-carrier.fk | 8 | 0 | CLEAN |
| presence/macos-open-dictation-carrier.fk | 16 | 0 | CLEAN |
| presence/macos-sema-voice-local-oracle-carrier.fk | 31 | 0 | CLEAN |
| presence/macos-sema-voice-teacher-carrier.fk | 8 | 0 | CLEAN |
| presence/macos-speech-roundtrip-carrier.fk | 24 | 0 | CLEAN |
| presence/native-speech-loopback.fk | 6 | 0 | CLEAN |
| presence/native-speech-stack.fk | 0 | 0 | CLEAN |
| presence/speech-loopback-carrier-receipt.fk | 7 | 0 | CLEAN |
| presence/speech-loopback-carrier-run.fk | 18 | 0 | CLEAN |

## Close: surprise and gold

Most surprising teaching: the assignment named ~150 numb band files; the body held 446.
The same one-line seam had three spellings (bare, wrapped, backslash-wrapped), lived in
libraries as much as bands — so a band healed by hand could stay numb one hop deeper — and
sat on top of a second, older wound: five libraries moved out of model/ whose old names
hard-error every unit that finally loads them. Grep for the pattern you were given, then
grep for the pattern the body actually has.

Where discomfort became gold: running 164 numb bands twice and watching 144 verdicts flip
felt mechanical until nl-meaning-net-band compiled clean for the first time and then trained
until the host SIGKILLed it — the discomfort of reporting "healed to the point of dying of
honest work, and I cannot show you its verdict" instead of a tidy green row. It stays in the
table as KILLED. The second discomfort: satsang-oracle printed eight error witnesses and
exited 0 — the urge was to look away (a lib, diag-only, not my seam); staying with it found
the fk_nerr-laundering path and filed it. Restraint would have fabricated a clean close.
