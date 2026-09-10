# Native LoRA training

`native-lora-train.bml` trains the pinned Llama checkpoint through Form-owned Metal
kernels. `lora-voice.bml` and `learn/corpus-train-door.fk` use it directly through a
supervised Form worker. No Python, MLX executable or model server participates.

Build a request with `ntr-request(model, adapter, data, output, steps, batch,
learningRate, maskPrompt, layers)`. `nlw-launch(request, logDirectory)` starts it;
`nlw-run` also waits. `nlw-status(logDirectory)` reports the observed process state.
The worker reads one JSON request from stdin at `observe/native-lora-train-run.fk`.

The data directory contains `train.jsonl` and `valid.jsonl`. Rows carry `messages`,
`prompt` plus `completion`, or `text`. Prompt masking supervises the final assistant
response and its end marker. Tool-call payloads require their matching template
and are refused explicitly. Rows are processed whole; a row that cannot fit the
model's positions or current recommended device working set retains its line and
required bytes in the refusal. The trainer does not shorten it silently.

One admitted model and tokenizer serve the run. Sequences accumulate a batch
gradient weighted by their supervised token counts. Each completed round performs
Adam, with a full gradient norm and clipping measurement. A non-finite gradient
abstains before updating; a non-finite result withholds publication. The last
published generation remains available for the next process.

Every completed round writes `output/generations/<step>-<pid>-<time>/` containing
`adapters.safetensors`, `optimizer.safetensors`, `adapter_config.json` and
`complete.json`. `native-current` changes only after these files are complete.
Readers use `nag-resolve(output)`. Native continuation restores moments and step;
a historical adapter without optimizer state starts a disclosed new Adam state.
Generations are retained, so storage grows with completed rounds.

`training-events.jsonl` records row numbers, counts, per-stage elapsed/GPU time,
gradient norm, clipping, actual pair updates, checkpoint path, validation loss and
completion reason. It contains no training text. `train.log`, `train.err` and
`train.rc` retain the child outcome, including an early compile/runtime failure.
Creating `output/cancel` requests a stop between rounds; the last completed update
and generation remain published.

The model-free failed-child witness is
`form/form-stdlib/tests/native-lora-worker-band.fk`. The small-transformer Adam
continuation witness is `native-lora-resume-band.fk`. The actual 3B two-run witness
is `observe/native-lora-training-homecoming-run.fk`; its public fixture outputs
remain under `.hearth/native-training-homecoming`.
