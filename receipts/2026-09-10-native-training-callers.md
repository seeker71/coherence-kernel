# Native training callers, observed

Codex · 2026-09-10 · continuation of the full 3B Metal checkpoint work.

The voice and corpus training doors now launch a Form supervisor and Form trainer.
The child exit code and stderr survive a failed compile or refused training request.
The school compares the same 3B base with and without the adapter, sharing one
tokenizer across its two model owners. Corpus observation also keeps its model
resident across probes. An incomplete answer prevents a grade.

Training reads chat, prompt/completion and text JSONL. Prompt masks select the final
assistant response; sequence microbatches accumulate a gradient weighted by the
number of supervised tokens. Each completed round updates every loaded LoRA pair
and publishes parameters, configuration and Adam moments through one generation
pointer. Whole rows are admitted against current device memory; none are silently
truncated. The legacy adapter's missing optimizer is explicitly a new Adam state.
Native continuation restores both moments and their step from the same generation.

The live door is `observe/native-lora-training-homecoming-run.fk`. It learns only
two public greeting fixtures and validates on a separately phrased greeting.
The active voice checkpoint is unchanged. Raw output and stderr are in
`receipts/evidence/2026-09-10-native-training/`.

| Measurement | First run | Resumed run |
| --- | ---: | ---: |
| Input / supervised tokens | 81 / 7 | 81 / 7 |
| LoRA pairs updated | 56 | 56 |
| Adam step after update | 1 | 2 |
| Training loss | 0.877447 | 0.532529 |
| Held-out loss before | 2.011228 | 1.518110 |
| Held-out loss after | 1.518110 | 1.102139 |
| Admission and tokenizer, ms | 15,467 | 16,159 |
| Forward/backward total, ms | 12,779 | 11,874 |
| Update, ms | 255 | 301 |
| Checkpoint, ms | 719 | 780 |
| Entire process, ms | 31,953 | 31,862 |
| Retained Metal buffers | 0 | 0 |

These are fixture losses, not a claim of corpus-wide quality or hardware-floor
performance. Stage events include the actual model, checkpoint generation, token
counts, timings, GPU busy time, dispatches and selected execution path. They contain
no prompt or answer text. Natural-language generation also records its final EOS,
memory, cancellation or explicit token-limit reason.

Independent checks: native Adam save/reload continuation matches uninterrupted
training byte for byte for all parameters and moments (7); dataset boundaries and
JSONL line numbers (31); a real failed child retains exit 1 and diagnostic text (7);
the corpus request preserves prompt masking, 16 layers, batch two and rate 2e-5
(511); native asking band (4095). Drift gates read 8191/8191 with no refusal.

Own panel: the bounded glass first frame measured **33 ms**. Counsel measured zero
orphans and honestly left 11/12 resident lanes unobserved because no hearth stood.
The transcript meter read 996,225 cumulative session output tokens; that is a
session counter, not a current-turn contribution percentage.

The surprising lesson was at the supervisor boundary: a compact BML body with
multiple statements on one line had lowered only its first statement. A clean
compile alone did not establish a working launcher. Multiline authoring and the
real failed-child band now witness the returned pid, exit and diagnostic together.
The discomfort of a false launch label became an observed process contract.

The next crossings still owed in this movement are native standalone weight fusion
and the large Whisper checkpoint. This receipt does not relabel them as completed.

The exchange stayed alive by following a failed launch through the actual process,
then teaching the same failure boundary to the native worker.
