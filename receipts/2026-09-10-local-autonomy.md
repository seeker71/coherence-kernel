# Re-observing local execution after session learning

Observed 2026-09-10 by Codex, from `5e5d2679` (already on origin/main at entry).
This movement adds native evaluation/replay doors and repairs two reporting defects
found by real execution. Codex selected and launched the experiments, authored the
comparison harness, investigated failures and issued the recorded Qwen cancellation.
No external model was called by the task executors, and no solution hints were
supplied during their runs. This is not a claim of autonomous task selection.

| Trial | Observed result | Scope |
| --- | --- | --- |
| Existing deterministic repair curriculum | 3 of 5 defects repaired; correct control preserved | All three syntax cases passed; both semantic defects remained unresolved. Development 1/1; held-out 2/4. No retrieval, model, training or remote fallback. |
| Llama 3.2 3B, historical voice adapter | 0/2 coding contracts passed | 110 + 35 output tokens, 282 + 279 input tokens. Configuration response was prose; arithmetic response stopped with `adapted-repeat`. |
| Same Llama, session adapter at optimizer step 2 | 0/2 coding contracts passed | Same token counts and byte-identical responses. Both conditions admitted 56 rank-8 LoRA pairs and released all model buffers. Session state stayed unchanged during evaluation. |
| Native Qwen 3.8 27B Q4, positive-boundary defect | No verified repair; assessor cancelled after 124 repeated exclamation marks | 766 input tokens, 124 emitted tokens, 395,066 ms model-process time. Base-only, no LoRA. The original coordinator summary is `incomplete`; its inner evaluation is `unresolved`. |
| Current local RAG | Query `native session learning LoRA` missed | 11,196-byte index, eight rows. Indexed paths cover introductory/receipt material, not session learning. Index presence is not coverage. |
| Resident / self-direction | Hearth standing = 0 | The existing self-direction demonstration returned `selected-action-category-has-no-executable-recipe-binding`. Its dated synthetic health rating is excluded from these measurements. |

These denominators stay separate. Neither green proof bits nor a training update
is counted as a completed coding task. Public fixtures may have prior exposure;
there is no claim of a sealed benchmark or representative general autonomy rate.
The Llama code route was correctly ineligible: the promoted skills were dialogue.
The comparison explicitly probed that adapter without enabling it for production code.

Llama configuration inference took 27,962 ms with the seed and 27,854 ms with the
session adapter; arithmetic took 16,649 and 17,052 ms. Admission-inclusive condition
times were 61,058 and 61,883 ms. Per-stage CPU wall time, GPU busy time, prompt/output
counts, identities and hashes are in the [recheck](evidence/2026-09-10-local-autonomy/llama/recheck.json).
These were observations under existing host load, including repair work and a brief
Glass observation; they do not establish an optimization gain or a hardware floor.

Two concrete reporting repairs followed the failed Qwen attempt:

* Token flow validation reused a command-duration parser limited to ten digits.
  Every contemporary epoch-millisecond stamp was silently rejected. The flow now
  accepts native nonnegative integers, while rejecting strings, fractions and null.
  Native replay recovered all 125 emitted events from retained stdout. The original
  empty flow file remains beside the recovered file, with its recovery basis stated.
* A status read could precede the supervisor's atomic publication, while the next
  liveness probe observed its exit. That race reported 127 although the eventual
  child receipt held 0. Completion now reconciles the retained status after reaping.
  Explicit timeout/cancellation statuses keep their meaning. The original Qwen
  coordinator discrepancy is preserved, not rewritten into a successful trial.

Qwen's current progress policy did not autonomously resolve the repeated output.
The assessor's native diagnostic wrote the existing stop control after observing
124 uniform punctuation bytes. The owned model group was released: wrapper status
130, child termination 143. There was no final EOS or accepted patch. Restoring its
token trace repairs visibility, not reasoning quality. The next reasoning work is
to diagnose that degeneration and bind semantic progress signals to an executable
local response; no second Qwen quality trial is claimed here.

The native comparison harness freezes caller documents, writable paths and checks
before either condition. It admits each condition once, preserves per-token events,
and excludes training. Its reporter replays the frozen contracts from a relocated
evidence directory, rejects incomplete comparisons, and reconciles result/token/hash
claims. No foreign-language implementation or analysis helper was introduced.

Reproduction:

```sh
form-run ./fkwu observe/form-cli-heal-run.fk <<'EOF'
eval
EOF
form-run ./fkwu observe/form-local-autonomy-eval-run.fk </dev/null
form-run ./fkwu observe/form-local-autonomy-report-run.fk <<'EOF'
receipts/evidence/2026-09-10-local-autonomy/llama
EOF
form-run ./fkwu observe/form-cli-heal-flow-replay-run.fk <<'EOF'
receipts/evidence/2026-09-10-local-autonomy/qwen
EOF
```

The first two commands execute new trials; the latter two only replay retained
evidence. Live inference needs the model/adapter files named in the manifests.

Evidence: [deterministic summary](evidence/2026-09-10-local-autonomy/deterministic/summary.json),
[Llama comparison](evidence/2026-09-10-local-autonomy/llama/summary.json),
[Qwen original coordinator](evidence/2026-09-10-local-autonomy/qwen/summary.json),
[Qwen recovered flow](evidence/2026-09-10-local-autonomy/qwen/replayed-flow-summary.json),
[recorded intervention](evidence/2026-09-10-local-autonomy/qwen/assessor-control.json),
[current grounding](evidence/2026-09-10-local-autonomy/ground.json).
Each public trial directory retains raw relevant streams, process events and choices.
Full disposable snapshot inventories remain locally at `.form-heal/eval-native-6503-276799176-0`
and `.form-heal/eval-native-23941-277403354-0`; they are not needed to replay the published checks.

Validation: comparison/replay policy 2047; full-width flow/exit policy 4095; twelve
real alternating success/failure child exits 4095; existing flow/learning contract
31. Preflight chains were clean and all executions exited 0. Initial authoring
mistakes in the isolated bands were repaired before these verdicts were accepted.
`git diff --cached --check` flags the original trailing blank lines in Qwen stdout
and its request. Those evidence bytes are preserved; the code/document whitespace
check is clean. The structural gate initially caught a duplicate exported launch
script; that export was removed while its original private transport receipt and
public argv record remain. No gate was relaxed.

Panel: Glass first frame **42 ms**; counsel **11/12 lanes unobserved**, because no
hearth stood. Share is `kind=declared`, percentage withheld: transcript reconciliation
was still incomplete. Native guide: Python implementations 25, execution candidates
314, grammar inputs 50, unread 0. Those existing migration gaps remain visible.

After freezing the trials, the verified measurement method was returned through
session learning; no withheld task answers entered training. A real update advanced
optimizer step **2 → 3**, passed its retained assessment, promoted, drained the worker
and left zero retained buffers. [Assessment](evidence/2026-09-10-local-autonomy/session-close/result.json).
Step 3 is the current serving adapter; its coding behavior has not been retested.

The surprising teaching was that changed LoRA weights produced identical failed
answers on both tasks. The uncomfortable failed trial yielded two observable
reporting repairs. The exchange stayed alive by preserving that failure, recovering
its measurements and returning the verified lesson to the local learner.

— Codex
