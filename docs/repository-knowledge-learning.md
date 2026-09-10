# Repository knowledge enters native training

`form-run ./fkwu observe/repo-knowledge-corpus-run.fk` builds the local dataset.
Form reads an explicit technical-document inventory, the primitive registry's
literal declarations, and admissible historical homecoming teachings. Registry
callbacks are data, never imported or executed. No model invents the targets.

Each JSONL pair carries its source path, SHA-256, source unit and evidence kind.
Documentation and registry declarations are attributed source claims, not a new
execution proof. Historical rented teachings retain that attribution. Known
edge-contract omissions are listed in the manifest. Exact duplicate pairs count
once. Source errors refuse publication instead of quietly deleting evidence.

`train.jsonl` contains training rows. Whole documents stay on one side of the
split; historical teachings retain the existing answer-word split. The larger
`heldout.jsonl` pool is separate from training. `valid.jsonl` contains the two
existing public session sentinels, whose prompts cannot enter the training set.
These sentinels do not measure general knowledge or whole-session autonomy.

The content-addressed dataset lives below the private session home's
`repo-knowledge/versions/`. Existing version bytes cannot be replaced with
different content. `current.json` names the publication and actual row counts.
`session status` distinguishes this repository corpus from retained session
examples and distinct sessions. LoRA A/B matrix pairs are model parameters,
not examples, sessions or knowledge coverage.

Historical term examples cover both directions: description → coined term and
coined term → the exact recorded description. No generated explanation is
substituted for the source. Both examples retain the same source unit and split;
the publishing organ checks every reverse association byte-for-byte. `source_units`
counts their shared source material once, while `examples` counts both learning
directions. `historical_teachings` and `historical_description_examples` make
that distinction explicit. More directions are not more independent lessons,
and historical descriptions can be questions rather than current instructions.

`form-run ./fkwu observe/repo-knowledge-train-run.bml` reads one JSON stdin line,
for example `{"steps":2,"batch":2}`. Empty stdin uses that bounded request.
The native trainer checks dataset hashes, restores the local Llama adapter's
Adam state, and performs full-row updates in one model residence. Explicit
requests can cover the entire dataset; every completed round saves a generation.
Rows are never silently shortened to fit. Position or memory refusal names the
row, preserving the dataset for attention. The worker's exit and stderr persist.

`repo-knowledge/training-current.json` names the run. Its `adapter/training-events.jsonl`
records admitted examples, each consumed row, tokens, losses, updated matrix
pairs and actual checkpoints. Re-running resumes the same run's optimizer and
cyclic row cursor. Available rows are not a count of rows already trained.
Corpus and training health enter the existing session diagnostic flow.
The publishing organ checks exported hashes, counts, source freshness and
train/held-out overlap before publishing. Its read-only observation door is
`form-run ./fkwu observe/repo-knowledge-corpus-witness.bml`; it calls those same
live checks rather than maintaining a separate fixture score. Training records
row-start and forward/backward completion, so a long row names the active stage.
Retries preserve the preceding attempt's streams and process evidence.

This door retains a research candidate without changing the serving selection.
It trains the registered native Llama model, not Qwen or Whisper. More examples
and completed updates are observable progress, not a `voice-home=1` or 95%
autonomy verdict. Held-out generation and representative task checks still own
those claims.
