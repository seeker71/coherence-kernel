# Current Floor

Date: 2026-07-15

This file is the current release floor for this worktree. Receipts preserve the
history, but the claims below are only the state that is present now.

## Grounding

The checkout witness is still the C-seeded `fkwu` runner. It was rebuilt before
this floor was written.

```text
cc -O2 -o fkwu runtime/fkwu-uni.c
./fkwu --src bootstrap/ground.fk -> 42
./fkwu --src bootstrap/ground-recursive.fk 10 -> 55
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk -> 15
native-vs-rented-check -> 11111
```

The C file is a checkout witness and shrink target. New runtime meaning belongs
in Form/native-walker cells, not in a larger C seed.

## Repo Floor

The repo floor is an organic intelligence floor, not a voice-only floor. The
present organs include:

- Form-native choice receipts for success, fail, and silence:
  `form/form-stdlib/choice-receipt.fk`.
- Channel/interface/protocol choice floor:
  `form/form-stdlib/channel-protocol-choice-floor.fk`.
- Sovereign allow/stop/witness/re-entry boundary protocol:
  `form/form-stdlib/sovereign-boundary-protocol.fk`.
- Host OS membrane and C-seed shrink law:
  `form/form-stdlib/host-os-membrane.fk`.
- Satsang witnessing circle:
  `form/form-stdlib/satsang.fk`.
- Reception consent policy:
  `form/form-stdlib/reception-consent.fk`.
- Local and remote oracle teacher catalog:
  `form/form-stdlib/oracle-catalog.fk`.
- Trust and vitality organs across `trust-row`, `trust-decay`,
  `trust-weighted-colearning`, `proof-trust`, `model-vitality`,
  `skill-vitality`, and `observe/sovereignty-guide.fk`.

These organs are the repo north-star floor: native choice, fail, cut, stop,
timeout/nothing, satsang, consent, minimal kernel, host membrane, observability,
trust, sovereignty, vitality, play, and wonder. Audio is one current organ using
that floor.

## Native Model Control Plane

The current model floor is executable rather than a collection of dated
rosters. `form/form-stdlib/native-model-control-plane.fk` inventories 32
family-level occurrences: 14 Form-native, 3 observed local fine-tunes, 4
borrowed local models, 2 local oracles, 3 remote oracles, and 6 policy/fixture
families. Ownership, execution surface, and authority are separate fields.

Model-control meaning now lives in Form. `native-model-evidence.fk` owns
normalization, hashes, exact/F1/sequence scores, rotation, and overlap checks;
`native-model-eval-form.fk` owns paired-evaluation and authority decisions;
`native-model-seal-form.fk` owns dataset/provenance admission;
`native-model-event-form.fk` owns privacy-minimized event validation and
encoding; `native-model-ledger-form.fk` revalidates every event digest and
canonical row before tallying; `native-model-daily-form.fk` owns daily
admission, closure, an equality-only byte-copy deployment check, and progress,
while `native-model-lineage-form.fk` owns canonical transformation-lineage
validation and a strict fresh/reproduced/reviewed/authorized/successful
authority gate;
`native-model-live-loop.fk` owns occurrence accounting.
`native-model-checkpoint.fk` owns exact learned-state
encoding, validation, reload equivalence, and keep/revert.
`native-model-session-world.fk` owns the real-session issued-tool predictor,
and `native-model-session-grounding.fk` owns privacy-minimized real-session
grounding replay. Thin
POSIX shell is only the host membrane for clock/stat observations, local HTTP
and process I/O, and invoking `fkwu`. It is not an evaluator, trainer, model,
oracle, or authority. No runtime meaning was added to the C seed.

Actual bounded training was witnessed directly through `fkwu`, with all weight
updates and metrics computed in Form:

```text
form-native transformer component (13 train, 5 held out, 120 epochs)
train loss, micro:         1676291 -> 361561
held-out loss, micro:       553645 -> 85021
held-out mean baseline:               359466
observed weight delta, micro:          196614

Form neural LM (6 train transitions, 6 held out, 400 epochs)
held-out next-token correct:      0/6 -> 6/6
```

These learned states are now persisted exactly, but they are not a useful
native LLM. Form encodes all 192 learned f64 values as 1,536 IEEE-754 bytes,
binds them to the training contract and content SHA-256, atomically publishes
the checkpoint, reloads it bit-identically, and proves metric/prediction
equivalence. A 20-epoch continuation regressed transformer held-out loss from
`85021` to `85333` micro, so the keep/revert gate retained the incumbent; the
neural LM remained `6/6`. The active checkpoint digest is
`186e6f94940dfff5b1f05c5727fe0dcf76e004ff4fa7e425f79691c2dec2f1a0`.
The transformer still has width 2 and the neural LM one-token context, so the
useful native voice remains pending.

Existing locally adapted models were exercised through their actual Ollama
routes without assigning them authority. A fixed EN→PT-BR probe was translated
correctly by the borrowed local `llama3.2:3b` in about 2.34 s, while
`hati-translator-q4` returned an English ramble in about 1.76 s. On the narrow
answer `42`, both the base and `form-llama` were correct, but `form-llama` took
about 7.77 s versus 1.46 s for the base. A separate Axiom-4 calibration probe
also favored the base: both fine-tunes fabricated support while the base
abstained. These are integration diagnostics, not a complete benchmark.

The replacement paired evaluator is now live. On its one-item fixed translation
diagnostic, Form scored Hati at exact `0`, token F1 `352941` ppm, and
sequence/conservative score `222222` ppm. The borrowed base scored `1000000`
ppm on exact, token F1, sequence, and conservative score. Both observations
were clean and paired; Form returned `winner=incumbent` and
`authority_eligible=0`. The carrier persisted hashes and aggregate scores, not
the raw prompt, reference, or model answers.

The shipped `form/form-cli` RAG crossing is now green. The Form source stopped
applying list-only `nil?` to a string query; the Form emitter gained the already
proven rooted-let semantics; and the CLI was regenerated by resident `fkwu`
walking the current Form flattener and emitter sources. Its build-publication
`ask` gate returns `grounded:fixture/native-rag-hit`; the daily deterministic
gate returns `grounded:fixture/native-world-hit.fk`; and the real local index
returns `grounded:form/form-stdlib/active-inference.fk`. The native binary is
byte-identical to its platform bootstrap at
`c0dd77e26565de2a3f5da41f07daaa6b1078cb062419455fd0855035295114b1`.

The fine-tuned Ollama tags still have no production call sites. Their apparent
identity conflict is now narrower: all three adaptations used the semantic
Llama-3.2 3B base, while Ollama's `1.1B` display reflects packed q4 storage
accounting. Hati now has an observed five-node/four-edge transformation DAG,
and its served model-layer SHA is byte-identical to its staged q4 GGUF. LoRA →
fuse → quantize changes content identity, so current F16 and Q4 packages are
recorded as sibling descendants of the fused package rather than a fabricated
F16 → Q4 edge. The DAG is structurally valid, but its historical edges are not
reproduced, owner-reviewed, or authorized. No local fine-tune has model
authority.

The post-audit Form tally contains 11 events: 4 local-finetuned evaluation
events and 7 borrowed-local evaluation/probe events. It contains 0 invalid rows and 0
accepted production finals, so owned, on-device, and remote final-work shares
remain unmeasured. Malformed, edited, or noncanonical rows are counted as
invalid and force shares to remain unmeasured.
Evaluation, teacher, training, and integration-probe traffic are separate and
cannot inflate sovereignty.
Promotion additionally requires a pre-training seal, scoped consent and
license receipts, a clean held-out audit, and reviewed served-to-training
lineage. Those gates are incomplete for the current language corpora, so no new
large-model training or promotion is authorized. Pending is the result; it is
not replaced by a simulated run.

The operating definitions, daily measurement contract, and data-triggered
weekly schedule live in `docs/native-model-control-plane.md`; the observed
training and integration results live in
`receipts/2026-07-15-native-model-control-plane.md`. The daily shell witness is
the active 06:30 local automation entry. Its latest observed run returned `42`,
`55`, `15`, `11111`, live-loop `4095`, training `255`, checkpoint `4095`,
RAG `7`, SHA-256 `2`, and Form replacement `262143`; it then ran the bounded
training report, exact checkpoint keep/revert, live RAG witness, and Form tally.
The optional paired Ollama diagnostic was disabled for that verification so it
did not add ledger traffic. `native_model_eval.sh` and
`native_model_tally.sh` are executable host membranes around Form-owned
scoring and accounting. The diagnostic remains non-promotional. Sunday remains
evaluation-only.

Focused direct-source witnesses observed in this reground:

```text
host-os-membrane-band -> 8191
reception-consent-band -> 255
satsang-band -> 127
kernel source intake framebuffer policy witness -> 16383
```

## Audio Floor

The current real audio shape is local progress using borrowed acoustic runtime
plus a Form/BML control plane. This is not enterprise shipping, not a commercial
release process, and not a public product lane. The current source voice is the
audio.cpp public-demo voice package, used as real borrowed data so the lane can
work end-to-end now. Replacing it with a personal voice package is a later
source-swap task, not a pipeline blocker.

Form/BML owns:

- metadata
- source provenance
- evidence caches and sidecars
- WER/native-rate/confidence reporting
- champion/challenger scoring
- promotion gates
- OOM diagnostics and repeat prevention

audio.cpp owns the acoustic runtime used by the current no-stand-in lane:

- ASR: `qwen3_asr`
- forced alignment: `qwen3_forced_aligner`
- TTS: `qwen3_tts`
- voice mode: `voice_ref_clone`

There is now also a functional local stand-in loop for immediate end-to-end
talk/listen/translate testing:

```text
presence/fkwu-local-audio-loop.fk
presence/tests/fkwu-local-audio-loop-band.fk -> 16383
audio-training-runs/current/fkwu-local-audio-loop/summary
status: functional_local_talk_listen_translate_loop
source_text: Open speech flows.
source_transcript: Open speech flows.
source_wer_pct: 0
translation_oracle: ollama:llama3.2:1b
translation_text: Offene Rede fließt.
reply_transcript: Offene Rede fließt.
reply_wer_pct: 0
score: 16383
expected_score: 16383
```

This loop is Form-owned orchestration over local stand-ins: `say` for temporary
TTS, `ffmpeg` for wav normalization, `whisper-cli` for ASR, and bounded local
Ollama HTTP for translation. It is functional now, but it is not the real local
personal voice lane and does not train or promote a base model.

The loop now has a typed task surface and direct sidecar readback:

```text
presence/fkwu-audio-task-surface.fk
presence/tests/fkwu-audio-task-surface-band.fk -> 4095
audio-training-runs/current/fkwu-audio-task-surface/summary
status: local_loop_functional_audio_cpp_adapters_observed_not_swapped
score: 65534
expected_score: 65535
task_slots: source_tts,source_asr,translation,reply_tts,reply_asr
audio_cpp_asr_observed: 1
audio_cpp_tts_observed: 1
production_authority: 0
personal_lane_authority: 0
next_gap: rerun audio.cpp ASR/TTS support packets or inspect their command/resource artifacts
```

That task surface is now secondary. It means the local stand-in loop and
adapter evidence are observable, while the no-stand-in lane below is the current
real audio source of truth.

The real no-stand-in local progress audio lane is now complete:

```text
presence/fkwu-production-audio-end-to-end.fk
presence/tests/fkwu-production-audio-end-to-end-band.fk -> 4095
audio-training-runs/current/fkwu-production-audio-end-to-end/summary
use_case: local_progress_not_enterprise_shipping
status: local_progress_audio_end_to_end_complete
blocker: none
score: 131071
expected_score: 131071
executed_audio_cpp: 1
cli_ready: 1
models_ready: 1
package_files_present: 1
package_borrowed_demo_authorized: 1
package_progress_authorized: 1
package_personal_use_authorized: 0
source_mode: borrowed_public_demo
package_scope: public-demo
target_text: Sema audio path is alive.
tts_text: Seyma. Audio. Path. is alive.
transcript: Sema, audio, path is alive.
wer_pct: 0
translation_model: llama3.2:3b
translation_text: Sema, Audio, Pfad ist lebendig.
```

This lane writes the derived artifacts it owns: context, diagnostic, target
text, phonetic TTS text, reference text, runnable TTS/ASR/align/translate
commands, generated WAV, ASR transcript, forced-alignment words, translation
request, translation text, return codes, and resource logs. The current package
is no longer treated as a blocker for progress; it is explicitly recorded as
`borrowed_public_demo`.

## Latest Stored Arena Snapshot

The latest stored arena files report cycle `10`, champion
`multiseed_form_sweep`, nearest challenger `metal_observed_sweep_bridge`, and
`investigation_required: 1`.

The arena is not currently alive in this worktree: no `fkwu`/audio-arena process
matched, the stored `arena.pid` and `launcher.pid` values were not live
processes, and launchd had no `com.sema.audio-arena.5586` service.

The scoreboard still contains stale expected values for at least the old
monolithic audio gate and old TTS support expected value. The current real
end-to-end audio result should be read from
`audio-training-runs/current/fkwu-production-audio-end-to-end/summary`, not from
the stale scoreboard row.

## Live Lane WER Values

These values are stored in `audio-training-runs/current/scoreboard.md`.

| lane | WER | current meaning |
|---|---:|---|
| `macos_sema_teacher_acoustic_live` | 0 | Teacher/acoustic support row, confidence 96; not personal-lane authority. |
| `macos_sema_voice_teacher_live` | 100 | Failing voice teacher row; intelligibility 0. |
| `sema_formant_oracle_live` | 100 | Investigated miss; heard 0/3 target tokens. |
| `macos_roundtrip_live` | 100 | Failing roundtrip row; native rate 0. |
| `audio_cpp_current_tts_live` | 0 | Intelligible support row over public demo voice, native rate 6, and personal source replacement still open. |

Boundary values such as WER `0`, WER `100`, unknown, or sentinel confidence are
investigation signals unless another measured field explains them.

## Real-World Gaps

- The no-stand-in local progress lane now works end-to-end over audio.cpp:
  TTS, ASR, forced alignment, and local translation all return `0`, WER is `0`,
  and the score is `131071/131071`.
- The source voice is still borrowed public-demo data. That is acceptable for
  progress and must remain visible as `source_mode: borrowed_public_demo`; swap
  it later when we want the lane to sound like a chosen personal voice.
- The formant oracle is not close lexically: target tokens are `3`, heard tokens
  are `0`, overlap is `0/3`, and the next probe is a token-bearing dynamic
  formant carrier.
- Speaker verification is not needed to keep making progress, but it is still
  absent if later comparison against a chosen source voice matters.
- The last measured audio.cpp TTS support native rate is `6`; the current
  realtime gap is `94`.
- The current bounded gate reads sidecars and receipts. The new no-stand-in
  lane can rerun the audio.cpp binary directly when invoked.
- The OOM-prone monolithic audio lane has been replaced by a bounded current
  gate.
- The local talk/listen/translate loop works with host stand-ins, and the
  separate no-stand-in audio.cpp lane works for a fixed utterance. The remaining
  wiring gap is to put audio.cpp ASR/TTS behind the interactive task slots, add
  real microphone capture, add conversational reply generation, and keep
  realtime/native-rate diagnostics.
- During this north-star reground, broad direct `fkwu --src` BML/prelude
  bundles reached roughly 18-20 GB RSS. The root cause was a C checkout-witness
  parser progress bug: desynchronized BML-style call syntax could leave a naked
  top-level `)` at the same source byte and repeatedly allocate parsed cursor
  nodes. The local runtime repair stops this case with byte/line/column/context
  diagnostics instead of OOM.
- The deeper source-runner/compiler obligation remains: manually authored wide
  expressions must compile/evaluate within a resource envelope or fail with a
  precise diagnostic and framebuffer/source context, never unbounded RSS.
- The repo-wide north-star organs are present in focused cells, but not every
  runtime choice, host crossing, arena lane, and training loop is forced through
  choice receipts, sovereign boundary receipts, trust/vitality policy, and
  oracle provenance yet.
- Sema still does not have a native generative mind with real open weights loaded
  as recipe-data through the body.

## Do Not Claim

- Do not claim the current source voice is personal while it is public-demo.
- Do not claim native neural ASR/TTS ownership.
- Do not claim the base STT/TTS models were trained here.
- Do not treat WER `0` as sufficient without source provenance, speaker evidence,
  alignment confidence, native-rate, and listener-review gates.
- Do not treat WER `100` as normal. It must name the observed failure axis and
  the next measurable repair.
