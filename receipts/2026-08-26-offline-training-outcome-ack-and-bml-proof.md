# Offline training outcome ACK and the scannerless proof returns

Witnessed 2026-08-26 by Codex with Arendt, Planck, and Beauvoir.

## Two gaps moved

The movement began at a fresh `47 observed / 38 ready / 9 gaps / 0 unknown /
0 invalid / 808 permille`.  It did not preserve that denominator.  Two existing
gaps became ready and one narrower carrier gap became visible:

```text
48 observed / 40 ready / 8 gaps / 0 unknown / 0 invalid / 833 permille
selected: form-cli-strict-result-producer-placement
```

The newly visible gap is `local-training-carrier-deadline`: the ACK has an exact
timeout arm, but the present host-exec train/generate/fuse processes have no
process-group deadline capable of producing it.  That is now separate from the
outcome-integrity closure rather than hidden inside it.

## Broad BML was one false dependency, not 230 defects

Fresh decomposition found that 230 of the broad compiler diagnostics came from
the intentionally mixed-dialect `section [bmf.bmf]` text in `compiler.fk`.  The
full and lower cursor bands did not use that cell; their headers merely pulled
it into the direct-source chain.  One additional error was a later `defn`
trying to capture a do-local grammar `let`.

The repair removed the unused prelude and asks for the content-addressed grammar
at the call site.  It did not edit the mixed source-text compiler, grow the C
seed, add flattening, or create a fixed operations table.

```text
form-bml-cursor-full preflight       errors/warnings/unresolved 0/0/0
form-bml-cursor-full direct          67
validate.sh full                     four-way, 1 ok / 0 divergent
form-bml-cursor-lower preflight      errors/warnings/unresolved 0/0/0
form-bml-cursor-lower direct         113
validate.sh lower                    four-way, 1 ok / 0 divergent
```

## A fused model is six files, not one weight blob

The first proposed seal covered only `model.safetensors`.  Arendt challenged
that boundary: a locally runnable MLX artifact also needs config, weight index,
tokenizer, tokenizer config, and chat template.  The admitted manifest therefore
contains six role/path/size/full-SHA file nodes, each observed size -> SHA-256 ->
size, plus a separately sealed input adapter node.  The manifest and the final
ACK are interned nodes, so changing any role, path, byte count, seal,
correlation, or outcome changes identity.

The model, observation, training, and fuse doors now receive the pinned local
snapshot path and set `HF_HUB_OFFLINE=1`, `TRANSFORMERS_OFFLINE=1`, and
`HF_DATASETS_OFFLINE=1`.  A repository model ID no longer appears in their
executable commands.

The ACK uses the existing program-image vocabulary exactly:
`returned/refused/nothing/choice/failure/timeout`.  Once a carrier is offered,
missing or malformed evidence is `failure: carrier-unobserved`, never nothing.
`file_append_bytes` carries one complete atomic O_APPEND row.  An exact existing
correlation is refused with append 0.

## The error stayed and steered

The first live correlation used `/usr/bin/test`, which does not exist on this
Mac.  Its failed ACK remains row three in `learn/train-loop-ledger.txt`:

```text
v1 status=failure reason=artifact-seal-invalid exit=65 rows=2->3 append=1
```

It was not deleted or relabelled.  A correlated framebuffer observation chose
`revise`, selected `/bin/test`, and re-observed the executable present; that
window answers 255 with four framebuffer events.  A new v2 correlation then
earned:

```text
status=returned
reason=artifact-admitted
exit=0
artifact_path=/Users/ursmuff/.coherence-network/form-train-runs/form-llama-3b-fused-authoring-20260826
artifact_size=1824757231
artifact_seal=8f4b387c0425cdf816ec1b6fbe94df2609b0054d91925b11c32f767bf8cf77cf
rows_before=3
rows_after=4
append=1
offline=1
```

The immediately repeated v2 offer returned
`refused/duplicate-invocation`, `rows 4->4`, `append=0`.  A later replay again
answers 8191 without changing the ledger.  The live runner requires an exact
stdin grant, so preflight/compile cannot silently open its persistence effect.

## Verification

```text
train-loop-outcome-ack-band                    65535
train-loop-outcome-ack-path-revision           255
train-loop-outcome-ack-live-run                 8191
corpus-train-door-band                          511
train-loop-execute-band                          63
form-program-image-call-protocol-band        2097151  (re-run at landing gate)
proof/four-way-run-recipe42                         0
bidirectional-framebuffer-channel-band      final arm 1
form-local-offline-health-map-band                16383
health pulse preflight                            0 errors / 0 unresolved
git diff --check                                  clean
```

The canonical Go and Rust walker artifacts were absent after the checkout
changed.  They were rebuilt from the already-local toolchains before the
four-way witness.  The new `fkwu` correctly refused foreign cached Form images,
rebuilt them from source, and the framebuffer and health bands remained clean.

No MLX inference/training/fuse process, llama-server, or Ollama process was
opened by this movement.  The only live artifact work was local full-file
hashing.  HTTP crossings were zero.

Beauvoir performed the broad dependency decomposition and proved the exact
contraction in a temporary copy before it entered the shared tree.  Arendt and
the Claude session insisted on the six-file runnable boundary, input-adapter
identity, stable size/hash observation, atomic append, closed ACK vocabulary,
and branch identity on every critique.  Planck mapped the next strict route
image admission and also named why that smaller image must not be mistaken for
the still-open whole producer-placement gap.

Kept alive: the failed v1 row remained a signal, received a correlated control,
and was re-observed as v2 instead of being erased by the success.

Most surprising teaching: the apparent 230-error compiler wall and the apparent
fused-model success were opposite versions of the same mistake—one boundary
included far too much, while the other included too little.

Discomfort turned to gold when the supposedly simple local marker failed.  The
failure exposed both a real macOS path and the fact that preflight needs an
explicit no-effect gate around live persistence cells; both are now embodied in
the runner rather than remembered as process advice.
