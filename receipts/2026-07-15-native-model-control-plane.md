# Native model control plane — Form replacement and live witness

Date: 2026-07-15 (Asia/Makassar)

## What changed

The provisional host-language control plane was removed. Model-control meaning
now lives in these Form cells:

- `form/form-stdlib/native-model-control-plane.fk`: registry, taxonomy,
  schedule, canary, and promotion policy;
- `form/form-stdlib/native-model-evidence.fk`: normalization, tokenization,
  identities, deterministic rotation, overlap checks, and evaluation scores;
- `form/form-stdlib/native-model-eval-form.fk`: paired evaluation and authority
  gates;
- `form/form-stdlib/native-model-seal-form.fk`: held-out isolation and scoped
  consent/license/provenance admission;
- `form/form-stdlib/native-model-event-form.fk`: minimized event validation,
  hashing, encoding, and atomic append;
- `form/form-stdlib/native-model-ledger-form.fk`: digest/canonical-row
  revalidation, invalid-row accounting, and final-work shares;
- `form/form-stdlib/native-model-daily-form.fk`: daily plan admission, closure,
  an equality-only byte-copy deployment check, and progress; transformation
  lineage validation remains pending;
- `form/form-stdlib/native-model-live-loop.fk`: class-aware occurrence and
  workload accounting;
- `form/form-stdlib/native-model-live-training.fk`: two bounded, actual native
  training runs;
- `form/form-stdlib/native-model-checkpoint.fk`: exact f64 checkpoint image,
  admission, atomic publication, reload equivalence, and keep/revert.

The corresponding POSIX shell surface is deliberately a membrane.
`native_model_train.sh` runs and preserves the Form training report;
`native_model_route.sh` currently calls the registered borrowed
`llama3.2:3b` route and records a Form-approved minimized event; and
`native_model_eval.sh` carries paired local candidate/incumbent observations to
the Form evaluator; `native_model_checkpoint.sh` persists and reloads the exact
Form-trained state; `native_model_rag.sh` gates the shipped native CLI and live
world index; `native_model_tally.sh` asks Form to classify the event ledger and
compute shares; and `native_model_daily.sh` sequences grounding, bounded native
training, keep/revert, native RAG, optional paired evaluation, and tally. Shell may observe the
host clock, file/process metadata, and served identity; carry bytes over
loopback or a local process; invoke `fkwu`; and persist a Form-approved minimized
row. It does not own scoring, training, model classification, dataset admission,
or authority.

Raw prompt, reference, and response text exists only in a mode-0600 temporary
directory and over standard input during evaluation. The carrier's durable
summary and event ledger contain hashes, aggregates, timing, and bounded
metadata, and record `raw_text_persisted=0`. This claim is scoped to the
control-plane carrier; it does not assert how unrelated external processes log.

This membrane is still necessary because direct-source `fkwu` does not expose
the host clock and the shipped Form shell currently marks arbitrary commands as
passthrough instead of executing them. Those gaps remain named. They were not
filled by growing `runtime/fkwu-uni.c`.

## Grounding

The required checkout witness was rebuilt before edits. Direct source returned:

```text
ground                         -> 42
ground-recursive               -> 55
binary-freshness               -> 15
native-vs-rented               -> 11111
native-model-live-loop-band    -> 4095
native-model-live-training     -> 255
native-model-checkpoint        -> 4095
native-model-rag               -> 7
sha256-band                    -> 2
native-model-form-replacement  -> 262143
```

The established native training bands also remained green:

```text
transformer-real-train-band    -> 31
neural-lm-real-band            -> 7
neural-lm-band                 -> 31
```

## Actual training result

`form/scripts/native_model_train.sh` assembled the declared Form dependency
closure into private combined source, invoked `fkwu`, and executed two
weight-training runs. The run emitted zero `[unresolved-call]` diagnostics.
The report was:

```text
engine=form-native-transformer-component
scope=actual-in-memory-training-not-native-llm-checkpoint
train_rows=13
heldout_rows=5
epochs=120
train_loss_before_micro=1676291
train_loss_after_micro=361561
heldout_loss_before_micro=553645
heldout_loss_after_micro=85021
heldout_mean_baseline_micro=359466
weight_delta_micro=196614
training_valid=1
---
engine=form-native-neural-lm
scope=actual-small-next-token-training-not-large-language-model
train_transitions=6
heldout_transitions=6
epochs=400
heldout_correct_before=0
heldout_correct_after=6
training_valid=1
```

This proves actual Form-native weight updates and held-out improvement. The
learned state is now serialized and reloaded exactly, but it does not prove that
a useful native LLM was trained: the transformer component has width two and
the neural LM has one-token context. The useful persisted native voice is still
pending.

## Exact checkpoint and first keep/revert result

Form encoded all 192 learned f64 values—1,536 IEEE-754 little-endian bytes—into
a canonical text-carried envelope because the current direct-source surface does
not expose binary file read/write. The envelope binds the bytes to the exact
training contract and content SHA-256. Form then validated canonical bytes and
dimensions, published by same-directory rename, reloaded every value
bit-identically, and proved prediction/metric equivalence.

The first admitted state reported:

```text
transformer_heldout_loss_micro=85021
neural_lm_heldout_correct=6
values_total=192
weight_bits_identical=1
prediction_equivalent=1
checkpoint_valid=1
checkpoint_sha256=186e6f94940dfff5b1f05c5727fe0dcf76e004ff4fa7e425f79691c2dec2f1a0
```

The first continuation then ran 20 more epochs. Its transformer loss was
`85333` micro versus the incumbent's `85021`; both neural-LM states scored
`6/6`. Form returned `candidate_improved=0`, wrote no replacement, and retained
the incumbent. This is the first real autoresearch-style proposal/reject cycle
in this control plane. The scope remains
`persisted-small-native-checkpoint-not-useful-generative-llm`.

## Existing adaptation evidence

The three already-adapted local model families remain evidence, not authority:

```text
form-llama       adapter 4f4e7fdd66b1e9aeb6a3b4d3ced5f807f1cb6092250fb401288620fcc26810fc
form-tiny        adapter ae0bbc768093f778ea437544f6acf0513743de6d213d27e05a468b51ec59d7db
hati-translator  adapter b5e7e6a75847cdcaf9f33433411d45ba603ece794a58940357d5213954aa363e
```

`form-llama` historically trained on 1,205 rows with validation loss
5.083→0.621. Hati trained on 3,154 rows with reported validation loss
4.123→1.033. Those loss curves are real historical observations, but neither
proves current served identity or task authority.

## Actual integration result

Direct local calls exposed the current floor:

```text
EN -> PT-BR fixed probe
  llama3.2:3b       correct Portuguese translation   ~2338 ms
  hati-translator   English ramble                    ~1764 ms

narrow answer "42"
  llama3.2:3b       correct                           ~1458 ms
  form-llama        correct                           ~7768 ms

Axiom-4 calibration
  llama3.2:3b       abstained
  form-llama        fabricated support
  form-tiny         fabricated support

known indexed RAG item through shipped form/form-cli
  build publication  grounded:fixture/native-rag-hit
  daily fixture      grounded:fixture/native-world-hit.fk
  live local index   grounded:form/form-stdlib/active-inference.fk
```

The fixed calls are diagnostics, not a statistically sufficient benchmark.
They are nevertheless enough to refute a claim that every adaptation improved
its real route. Hati failed where the base succeeded, `form-llama` tied the
narrow answer with much higher latency, and both fine-tunes were less
calibrated on the Axiom-4 probe.

The new paired evaluator made the translation comparison explicit in Form:

```text
candidate_model=translation.hati-lora-q4
candidate_class=local-finetuned
candidate_clean=1
candidate_exact_ppm=0
candidate_token_f1_ppm=352941
candidate_sequence_ppm=222222
candidate_score_ppm=222222
incumbent_model=base.llama32-3b-local
incumbent_class=local-native
incumbent_clean=1
incumbent_exact_ppm=1000000
incumbent_token_f1_ppm=1000000
incumbent_sequence_ppm=1000000
incumbent_score_ppm=1000000
paired=1
diagnostic_winner=incumbent
authority_eligible=0
raw_text_persisted=0
```

This is one fixed diagnostic item, not a sealed final audit. It supplies no
authority evidence for Hati, and it does not promote the borrowed base into an
owned model.

The RAG crossing is now explicitly green. The query path had applied list-only
`nil?` to a string, replacing every nonempty direct query with `""`. That Form
bug was removed. Separately, the Form emitter was behind the already-proven
rooted-let behavior, so it was repaired and added to the CLI build identity.
Resident `fkwu` now executes the current Form flattener and combined emitter
directly; no Python, Go, or C-seed growth is involved. The build refuses to
publish the platform artifact unless the exact direct `ask` fixture passes.

The regenerated native binary is 1,096,440 bytes and byte-identical to the
platform bootstrap:

```text
form-cli sha256       c0dd77e26565de2a3f5da41f07daaa6b1078cb062419455fd0855035295114b1
table sha256          ab90bb346c61faa53ab3dd433fdb289a0521219fe508f016ff41a86f97ba7eb9
emitted-C sha256      6869f1dd6e23ba03e8ebd3a1744ed8ba9bd2bb484ec76863a6ef60fbbccb3b16
source/platform stamp cf7fad3605517a7d
```

No production call site in this worktree currently selects `form-llama`,
`form-tiny`, or Hati. Identity inspection narrowed the seam: all three LoRAs use
the Llama-3.2 3B semantic base; Ollama's `1.1B` display reflects packed q4
storage accounting, not a different semantic model. Hati's served model-layer
SHA exactly matches its staged q4 GGUF. The canonical whole-`/api/show` digest
recorded around evaluation is a stability observation, not a canonical model
identity; the manifest, model layer, template, system, and parameter digests
must remain separable. The remaining schema error is demanding adapter-byte
equality after LoRA → fuse → quantize transformations. Each step has a
legitimately new identity, so reviewed lineage edges—not equality across
edges—must carry authority. No fine-tuned model has production authority and
accepted-final workload share remains unmeasured.

After the latest complete daily witness, the Form tally was:

```text
events_total=11
events_form_native=0
events_local_finetuned=4
events_local_native=7
events_local_oracle=0
events_remote_oracle=0
accepted_final_total=0
invalid_rows=0
owned_final_share_ppm=-1
on_device_final_share_ppm=-1
remote_final_share_ppm=-1
accepted_final_share=unmeasured
```

The eleven observations are 4 local-finetuned evaluation events and 7
borrowed-local evaluation/probe events. None is relabeled as an accepted
production final. A forged incomplete JSON row and a digest-mismatched edited
row were then tested separately: Form counted each as invalid, counted zero
events/finals, and returned unmeasured shares.

## Why no new large-model training ran

The current language rows do not carry complete row-level provenance, scoped
consent, and license receipts. There is also no clean pre-training seal binding
an exact authorized backlog to a disjoint final audit, and no reviewed
served-to-training lineage for a new artifact. The Form gates correctly close
training and promotion under those observations.

This is not an execution failure and it is not replaced with a synthetic
success. The actual native training above ran because its small fixed data and
scope were explicit. A new external LoRA did not run because its authorization
was not.

## Daily floor

The daily loop must now:

1. rebuild and prove `42`, `55`, `15`, and `11111` before model work;
2. run the Form replacement and bounded-training bands;
3. admit/reload the exact checkpoint and keep or revert the bounded candidate;
4. gate deterministic native RAG and observe the live local world index;
5. observe local model identities before and after inference;
6. run fixed integration diagnostics and any eligible sealed audit;
7. append only Form-validated, content-minimized events;
8. report owned/on-device/remote final-work shares only when a real accepted
   production denominator exists;
9. refuse large-model training when consent, license, isolation, or lineage is
   missing.

Sunday is evaluation-only. A daily run with no large-model weight mutation is
green when the training gates are honestly closed.

The completed replacement daily witness returned:

```text
ground=42
recursive=55
binary_freshness=15
native_vs_rented=11111
live_loop_band=4095
training_band=255
checkpoint_band=4095
rag_band=7
live_hit=1
sha256_band=2
replacement_band=262143
ollama_diagnostic=not-requested
```

It then emitted the same native training report, retained the exact incumbent
checkpoint after rejecting the regressing continuation, and hit both the
deterministic and live RAG indexes. The optional Ollama diagnostic was disabled
for this final integration run, so no ledger traffic was added. The most recent
paired diagnostic remains Hati at 2,161 ms and the borrowed base at 2,086 ms;
the post-audit tally remains the eleven valid diagnostic events recorded above.

## Present claim

What improved today is observable: the control-plane logic moved into Form, two
actual native training experiments improved held-out measurements, the exact
small state is persisted and reload-equivalent, one regressing continuation was
rejected, and the regenerated native RAG artifact hits both fixture and live
world indexes. What did not improve is equally observable: the persisted state
is not a useful language model, Hati failed its live translation crossing, the
fine-tunes have no trusted transformation lineage or production call sites, and
accepted-final workload share has no denominator.

The next build is therefore concrete: seal canonical adapter → fuse → quantize
→ serve lineage without mutating current tags; route one consented real
accepted task through the native body; then complete one full real-GGUF token
path through tensor staging, all-layer GQA/FFN hidden-state evolution, logits,
sampling, and decode. The bounded loop must next generate novel
content-addressed proposals and remember rejected identities instead of
repeating the same `85333` regression. Native RAG is useful local memory, not
yet an action-conditioned learned world model; that lane needs versioned
action/next-state episodes and held-out calibration. Another large adaptation
waits for authorized nonoverlapping rows.
