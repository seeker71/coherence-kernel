# The actual adapter runs and learns through Form

Signed: Codex, 2026-09-10.

The local 1,807,496,278-byte Llama 3.2 3B checkpoint and the committed voice
adapter now have a Form-owned Metal execution path. The adapter contributes all
56 A/B pairs across all seven projections in its last eight layers. Base
weights remain mapped; mutable adapter factors have independent GPU owners.
No Python or MLX executable ran during this work.

The complete live evidence is in
[live.txt](evidence/2026-09-10-mlx-homecoming/live.txt). It records the exact
model and adapter paths, token counts, stage durations, GPU time, optimizer
choice, losses, serialized sizes and ownership after cleanup. On that run:

- Model/tokenizer opening: **15,303 ms**.
- Native voice: **“Hello.”**, EOS after **2 tokens**, **1,411 ms**.
- Public four-token training fixture: loss **8.0065746307 → 7.5891280174**.
- Forward: **152 ms**, device busy **124,274 μs**. Backward: **479 ms**, device
  busy **455,817 μs**. These are observed batch spans, not per-kernel timestamps.
- The update covered **56 pairs**. Measured gradient norm **93.14399856**;
  the selected norm limit of 1 applied a multiplier of **0.01073606475**.
- Adapter checkpoint: **13,905,800 bytes / 112 tensors**. Adam checkpoint:
  **27,812,024 bytes / 224 tensors**. Every payload byte was compared with its
  GPU owner before publication. Both saves took **765 ms** together.
- **Zero Metal buffers retained** after closing the model, context and optimizer.

The stage event rows remain at `.hearth/native-llama-homecoming/events.jsonl`;
the observer can reproduce them with `./fkwu observe/native-llama-homecoming-run.fk`.
The probe checkpoint is separate from the active voice adapter. A fixture loss
decrease is evidence of a real parameter update, not evidence that the full
voice corpus has been retrained or that held-out quality improved.

The surprising lesson was alignment. The real adapter's payload begins at byte
11,994, two bytes off a four-byte boundary. My first typed GPU reads reported
13,543 non-finite parameters. Byte-addressed reads of the same unchanged file
reported zero. The defect was in my reader, not in the checkpoint. The bounded
diagnostic selected abstention, changed the read, then re-observed every pair.
That discomfort became an unaligned-file regression with an independent Form
decode of the packed weights and half-precision scales/biases.

Focused witnesses: native affine arithmetic **127**, unaligned file reads **15**,
full LoRA finite differences **7**, checkpoint reload **3**, tokenizer **31**,
existing safetensors header **8191**. The gradient witness checks both factors
of all seven projections in two layers: **28/28** finite differences. Its
Adam step reduced loss **4.1426067352 → 4.1364123821**. Preflight was clean
after repairing my initial test syntax and list-identity comparison mistakes.

The panel reading was concrete: the glass first frame arrived in **50 ms**;
lane counsel showed **0 orphans** and **11/12 lanes unobserved** because this
checkout has no standing hearth. No resident-performance or hardware-floor
claim follows from those absent lanes. The native authoring guide still reads
**26 Python implementations / 325 execution candidates / 0 unread files**:
the remaining callers have not yet been replaced by this landing.

[Drift gates](evidence/2026-09-10-mlx-homecoming/drift-gates.txt) passed
**8191/8191**, exit 0. The execution map and API boundaries live in
[native-llama-metal.md](../docs/native-llama-metal.md).

The exchange stayed alive by moving the existing model and adapter, checking
the full gradient, and preserving failures before claiming a result. The next
work is already concrete: connect the corpus and voice callers, preserve
prompt masking and optimizer continuity, replace fusion and large-Whisper
execution, and reuse the tokenizer instead of rebuilding its tables per open.
Those external crossings remain; this receipt does not call them removed.
