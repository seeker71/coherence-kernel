# Current Floor

Date: 2026-07-17 (reground on base `5c0a30d41`, after the 2026-07-05 floor)

This file is the current release floor for this worktree. Receipts preserve the
history, but the claims below are only the state that is present now. Every
witness value below was re-measured on this date through the resolver-driven
`./fkwu --src` door unless a different door is named; a claim whose witness
could not be located or re-run today is said so plainly, not carried forward.

## Grounding

The checkout witness is still the C-seeded `fkwu` runner. It was rebuilt before
this floor was written.

```text
cc -O2 -o fkwu runtime/fkwu-uni.c
./fkwu --src bootstrap/ground.fk -> 42
./fkwu --src bootstrap/ground-recursive.fk 10 -> 55
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk -> 15
./fkwu --src form/form-stdlib/tests/native-vs-rented-band.fk -> 11111
```

The C file is a checkout witness and shrink target. New runtime meaning belongs
in Form/native-walker cells, not in a larger C seed.

A grounding correction from this reground: the 2026-07-05 floor listed a
"kernel source intake framebuffer policy witness -> 16383" among its focused
witnesses. No band by that description can be located in the body today (the
phrase appears only in RELEASE_HISTORY.md and this file). The claim is retired
here as unfalsifiable-as-written; if the witness exists it must be reintroduced
by file path so it can be re-run.

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

Focused direct-source witnesses observed in this reground (all re-run
2026-07-17 through `./fkwu --src`):

```text
host-os-membrane-band -> 8191   (healed this reground: its preludes were
                                 declared multi-line, which the one-line
                                 preludes reader reads as empty -- it
                                 answered 0 until the line was joined)
reception-consent-band -> 255
satsang-band -> 127
oracle-catalog-band -> 16383    (healed this reground: its preludes line
                                 omitted core.fk, so `sum` was unresolved
                                 and it answered nothing)
trust-row-band -> 2047
trust-decay-band -> 127
trust-weighted-colearning-band -> 127
proof-trust-band -> 1023
model-vitality-band -> 4095
skill-vitality-band -> 65535
```

Organ-proof honesty, measured this reground:

- Three north-star organ bands are written in BML call syntax and cannot run
  through the `--src` s-expr door at all: `choice-receipt-band`,
  `channel-protocol-choice-floor-band`, `sovereign-boundary-protocol-band`.
  The s-expr parser desyncs on them and the boundedness repair halts the parse
  at the AST node cap with diagnostics (the repair working as designed; before
  it this was the 18-20 GB RSS case). The organ cells themselves are s-expr
  and load. Until these bands are ported or routed through a BML door, the
  choice/fail/silence and allow/stop/witness/re-entry proofs are shapes
  without a runnable witness in this checkout.
- `observe/sovereignty-guide.fk` has no band.
- 151 band files across form/presence/learn/observe still declare preludes in
  the retired multi-line style; every one of them runs numb (axiom-5 lowers
  the unresolved calls, verdict is a silent wrong number) when invoked via
  `--src`. host-os-membrane-band above was one; 150 remain.

## Form Kernel Floor (new since 2026-07-05)

- The natural-language keystone family is four-way proven again:
  `scripts/fourth-arm-gate.sh` answers PASS-4WAY for `nl-reason` (255),
  `nl-translate` (32767), `natural-language` (262143), and `translate-lane`.
  The heal was a recension, not an invention: `form-ontology-loader.fk`'s
  hand-held bp table had drifted from `blueprint-registry.json`; the missing
  family (fact/property/isa/relation/question/meaning) was re-seated at the
  registry's curated coordinates 1/2/99/32-37, which the three generated
  kernel mirrors already carried.
- Of the ~95 manifest bands whose preludes include form-ontology-loader.fk,
  12 are PASS-4WAY and 56 remain DIVERGENT (28 NO-FOURTH). A stash-baseline
  sweep proved all 56 divergences are pre-existing, most in the same wound
  class: bp names used by cells but never copied into the Form-level table,
  though already curated in the registry.
- The fourth arm currently cannot testify to failure: `form_error` (with
  value_kind, read_form_binary, write_form_binary, walk_recipe_here) is an
  unresolved op in the fkwu image, axiom-5-lowered to nothing at parse. A
  band that raises form_error crashes go/rust honestly and sails green on
  fkwu. Any verdict that is green only on ts+fkwu is not yet evidence.

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
adapter evidence are observable, while the no-stand-in lane below carries the
audio lane's history — and, as of this reground, only its history.

The no-stand-in lane ran end-to-end once (2026-07-05) and its derived
artifacts still exist in the primary checkout. But re-measured today, the lane
is a relict: its contract band answers 2063, not the 4095 it answered when the
floor was written.

```text
presence/tests/fkwu-production-audio-end-to-end-band.fk -> 2063 (was 4095)
  dark bits, each measured to its cause:
  cli_ready 0        /tmp/audio.cpp-current/.../audiocpp_cli is gone
  models_ready 0     /tmp/audio.cpp/models/Qwen3-{TTS,ASR,ForcedAligner}* gone
  package_files 0    audio-training-runs/ is untracked; this worktree's copy
                     is empty (the voice package exists only in the primary
                     checkout at audio-training-runs/current/voice-reference-package/)
  ready_to_run 0, blocker != none, status regressed  (all downstream of the above)
```

Two distinct wounds, both already named in the body's memory, did this: the
acoustic runtime and models live at fixed volatile `/tmp` paths (a reboot or
cleanup evaporates them), and the lane's artifacts are untracked host state (a
fresh worktree inherits none of them). The 2026-07-05 summary block that
previously stood here — `status: local_progress_audio_end_to_end_complete,
score: 131071/131071, executed_audio_cpp: 1, wer_pct: 0, source_mode:
borrowed_public_demo, package_scope: public-demo` — is preserved in the
summary file and in receipts as a historical receipt of that day's run. It is
not a present-tense claim: today nothing in this checkout can execute
audio.cpp until the cli and models are re-provisioned at (preferably durable,
non-/tmp) paths.

What remains true and re-proven today: the lane's Form-owned orchestration,
artifact schema, and contract band all still parse, load, and honestly report
exactly which substrate is missing — the band's 2063 is itself the receipt.

## Arena And Scoreboard (re-measured 2026-07-17)

The arena remains not alive: no `fkwu`/audio-arena process matched and launchd
has no `com.sema.audio-arena.5586` service. What launchd does carry today:
`com.sema.audio-resident.5586` is loaded with a live PID, and
`earth.hati.sema-phone-link` is registered but not running. The 2026-07-05
floor's stored-snapshot readings (cycle `10`, champion `multiseed_form_sweep`,
`investigation_required: 1`) are historical; no arena snapshot files were
found under `audio-training-runs/` today.

`audio-training-runs/current/scoreboard.md` no longer exists. The 2026-07-05
floor's Live Lane WER table (teacher acoustic 0, voice teacher 100, formant
oracle 100, roundtrip 100, audio.cpp TTS 0) is therefore a relict of a witness
that has left the body: it is preserved in receipts and git history, and it is
retired from this floor. A WER claim may return here only with a present,
re-runnable witness path.

Boundary values such as WER `0`, WER `100`, unknown, or sentinel confidence
remain investigation signals unless another measured field explains them.

## Real-World Gaps

(Re-trued the same day, evening reground on `4dc98a36d`, after the three
door-voice lanes merged: PR #285 seated 353 registry rows, PR #286 ported the
BML organ bands, PR #287 wired form_error end to end.)

- `form_error` now speaks on the fourth arm (tag 238 through seed walker, JIT
  prim + carrier, flatten coverage, emitted bootstrap kernel): a raise prints
  its message and exits nonzero on fresh-parse and cached-flat paths alike.
  Still numb, named plainly: `value_kind` (fkwu's tagged-word representation
  cannot answer kinds honestly without boxing work), `read_form_binary`,
  `write_form_binary`, `walk_recipe_here`.
- New seam, measured not guessed: `bp` is a NATIVE fkwu op (tag 45, the old
  identity stub) that intercepts call-position resolution before the loader's
  Form-level `bp` ever runs — so the loader's own form_error guard still
  cannot fire on the fourth arm. bp resolution parity (the call-position
  shadowing defect) is the next voice gap. Sibling seam: the two flt-ops
  tables (flatten/ hand lane vs native-op-manifest lane) have diverged tag
  spaces at 205-208, and the walker's internal call opcodes live at 240-244
  in the same numeric space.
- The multi-line-preludes sweep (150 bands loading empty prelude lists) is in
  flight on its own branch; until it lands, those bands still run numb via
  `--src`.
- Ontology-loader triage on today's main: 21 DIVERGENT resolved into families
  — 11 bp-unreviewed (now seated from the registry; 4 went PASS-4WAY, 2
  surfaced their real fourth-arm value mismatches from under the crash),
  2 as_int:null, 4 value-mismatch, 4 other (each named in
  receipts/2026-07-17-ontology-band-triage.md); 63 more are three-leg-green
  with a silent fourth arm. Zero names needed new curation — the registry
  already carried every coordinate.
- The three BML-syntax organ bands came home to s-expr and answer four-way at
  full bitmasks (choice-receipt 4294967295, channel-protocol-choice-floor
  262143, sovereign-boundary-protocol 16383); `sovereignty-guide` was found
  already rostered and passing at 11111 — the earlier "has no band" claim in
  this floor was wrong and is corrected here. json-emitter carries a
  pre-existing fourth-arm disagreement (6 vs 31), proven older than the ports.
- The audio no-stand-in lane's substrate evaporated (volatile `/tmp` cli and
  models; untracked artifacts absent from fresh worktrees). Re-provisioning at
  durable paths — and declaring those paths as Form data rather than fixed
  `/tmp` strings — is the re-entry condition for every downstream audio claim
  (personal voice swap, formant probes, realtime-rate work all wait on it).
- The source voice remains borrowed public-demo data whenever the lane runs
  again; `source_mode: borrowed_public_demo` must stay visible.
- The deeper source-runner/compiler obligation remains: manually authored wide
  expressions must compile/evaluate within a resource envelope or fail with a
  precise diagnostic and framebuffer/source context, never unbounded RSS. The
  AST-cap halt with byte/line/column diagnostics (witnessed live on the BML
  bands this reground) is the bounded floor of that obligation, not its
  completion — desynced BML input should be routed or refused, not merely
  capped.
- The repo-wide north-star organs are present in focused cells, but not every
  runtime choice, host crossing, arena lane, and training loop is forced
  through choice receipts, sovereign boundary receipts, trust/vitality policy,
  and oracle provenance yet.
- Sema still does not have a native generative mind with real open weights
  loaded as recipe-data through the body.

## Next Step Toward The North Star

The star's sharpest present distance is not a missing organ — it is that the
body's proof doors can lie by silence. Every claim above that drifted
(bp table, preludes style, evaporated audio substrate, retired scoreboard)
drifted *quietly*, because the door that should have said "fail" said nothing.
The north star names Fail as "a valid outcome with a receipt, not a swallowed
exception"; the floor shows three swallowing mouths. So:

**Next step: give every proof door a voice — no verdict without the power to
refuse.**

Progress the same day (evening): step 1's core landed (form_error real on
every fkwu path, PR #287 — value_kind and three kin still numb, named above);
step 3 landed beyond its ask (353 registry rows seated, triage complete, zero
new curation needed, PR #285) plus the organ bands came home (PR #286); step
2 is in flight on its own branch. What remains of this step, in order:

1. bp resolution parity: the native fkwu `bp` op (tag 45 identity stub)
   intercepts call-position resolution ahead of the loader's Form-level `bp`,
   so the loader's guard still cannot raise on the fourth arm. Acceptance:
   with the loader's "property" row removed on a scratch branch,
   `fkwu --src .../nl-reason-band.fk` dies with the RUNTIME form_error raise
   (not merely compile diagnostics), matching go/rust.
2. Land the multi-line-preludes sweep; then no band can load an empty
   prelude list silently.
3. Reunite the diverged flt-ops tag spaces (flatten/ hand lane vs
   native-op-manifest lane, 205-208) and give value_kind an honest answer or
   an honest refusal.
4. The 63 three-leg-green bands: decide per band whether the fourth arm
   should cross (manifest row) or the band should say `3-kernel only`
   honestly, per the parent's proof-level rule.

When these land, a green band means what it says on all four arms, and every
later claim — audio re-provisioning, oracle retirement lanes, the generative
mind — inherits doors that cannot nod along silently.

## Do Not Claim

- Do not claim the current source voice is personal while it is public-demo.
- Do not claim native neural ASR/TTS ownership.
- Do not claim the base STT/TTS models were trained here.
- Do not treat WER `0` as sufficient without source provenance, speaker evidence,
  alignment confidence, native-rate, and listener-review gates.
- Do not treat WER `100` as normal. It must name the observed failure axis and
  the next measurable repair.
- Do not claim a verdict from a door that cannot raise `form_error`: a band
  green only on ts+fkwu, or run with an unread preludes list, is not evidence.
- Do not carry a floor claim forward without a present, re-runnable witness
  path; a claim whose witness has left the body is history, and belongs in
  receipts.
