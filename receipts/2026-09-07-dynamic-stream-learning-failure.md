# Dynamic supervision: observed progress and a failed learning completion

Work continues according to observed progress. Quick work completed in 22 ms;
a progressing child completed in 1,878 ms, beyond the observation interval;
explicit cancellation signalled and reaped its child in 275 ms. The dynamic
policy band returned 63. These are supervisor tests, not LLM quality scores.

The real native Q4 stream emitted two tokens (two bytes), matching the ordinary
forward path byte for byte. The context-capacity boundary and resource release
were observed; verdict 31. This narrow probe does not establish unlimited
context or end-to-end repair quality. Its evidence is in
`.form-heal/flow-live-jffc7dv4/`.

Completed verification: 12 repair integration groups, 6 timeout/ownership groups,
5 incremental token-transport groups, and 4 dynamic-process groups. Provider
routing fixtures use simulated models; the Q4 stream and MLX update below use
real local weights.

## Real LoRA update; supervisor completion failed

The local Llama 3B learner performed one optimizer step on 55 supervised tokens.
Its purpose was distillation of the current, actually observed contract-validation
result. Training loss was 3.901. Validation loss printed 1.295 before and after;
three-decimal equality does not establish zero regression. Comparison of the
saved tensor bytes found 112 changed tensors. The base weight file hashes still
match the pre-training hashes.

The supervisor returned:

```text
refused:[Errno 1] Operation not permitted
```

The completion carrier then returned:

```text
refused:invalid literal for int() with base 10: 'refused:[Errno 1] Operation not permitted'
```

The exact invoked command was:

```sh
env HF_HUB_OFFLINE=1 TRANSFORMERS_OFFLINE=1 TOKENIZERS_PARALLELISM=false /Users/ursmuff/.mlx-venv/bin/python -u -m mlx_lm lora --model /Users/ursmuff/.cache/huggingface/hub/models--mlx-community--Llama-3.2-3B-Instruct-4bit/snapshots/7f0dc925e0d0afb0322d96f9255cfddf2ba5636e --config form/form-stdlib/adapters/llama-3.2-3b-voice/adapter_config.json --train --data /Users/ursmuff/.codex/worktrees/bc7d/coherence-kernel/.form-heal/learning/round-jxkaju3d/data --adapter-path /Users/ursmuff/.codex/worktrees/bc7d/coherence-kernel/.form-heal/learning/round-jxkaju3d/adapter --resume-adapter-file /Users/ursmuff/.codex/worktrees/bc7d/coherence-kernel/form/form-stdlib/adapters/llama-3.2-3b-voice/adapters.safetensors --iters 1 --batch-size 1 --learning-rate 0.00001 --optimizer sgd --mask-prompt --max-seq-length 2048 --steps-per-report 1 --steps-per-eval 1 --val-batches -1 --save-every 1 --test --test-batches -1
```

The failed supervisor left its metadata at `running` and did not retain the
original exit status. A subsequent OS observation found PID 40990 absent and
its process group empty. The saved candidate remains in its round directory;
it was not published to the candidate pointer or activated. Automatic learning
has not been enabled. The JSON companion retains the before/after hashes,
base-file checks, exact command, process trace, and observed limitations.

The implementation stopped at this failed live check under the user-provided
repository instruction: “If setup, checks, or local gates fail, stop and report
the exact failing command and output.” No commit or push was made.

Closing counsel: orphans 0; 11 of 12 lanes unobserved because no hearth stands.
Share kind is declared, with no percentage. The useful discovery was that real
weight movement and accepted process completion are separate facts. The
incomplete completion record exposed the next transport repair instead of
being presented as a successful learning round.

Signed: Codex.
