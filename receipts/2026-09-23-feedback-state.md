# Feedback reaches new scores; the answer still repeats

Signed: Codex. This follows the retained 510-word failure in
`2026-09-23-native-answer-length.md`. The question, source packet and failed
answers remain available. No response-quality gain is claimed here.

## The actual state comparison

The native observation opens Qwen3.8-27B Q8 once, reproduces the original
answer, saves its recurrent state and reads the occupied full-attention cache
prefix. It applies the same 96-ID feedback through the serving sliced path,
generates 16 continuation IDs, then restores the original boundary. Exact byte
comparison verifies the rewind across all 64 layers before the retained
barrier path receives the same feedback. Future cache positions are excluded
from the rewind comparison; the reference path overwrites them as it advances.

[Delivery evidence](artifacts/2026-09-23-feedback-state/delivery.json):

- Initial answer matches the retained answer: 1; words: 510.
- Initial prompt: 5,490 IDs; position 6,193 → 6,289 after feedback.
- Restored state exact: 1 across 64 layers.
- Scores differ from both the initial prompt and the pre-feedback scores.
- The two delivery paths have different score bytes and state bytes in all
  64 layers. Their next selected ID is the same: 52.
- Sliced feedback: 10,964 ms; barrier feedback: 19,391 ms in this execution.

Those byte differences are not an error magnitude or a numerical accuracy
verdict. The [per-layer comparison](artifacts/2026-09-23-feedback-state/state-comparison.json)
retains the extents and equality results. Tensor bytes stay inside the process.

The [complete reference continuation](artifacts/2026-09-23-feedback-state/reference-answer.txt)
again has **510 words and is byte-identical to the initial answer**. Its first
16 generated IDs match the serving-path continuation. Completion, spare release
and model release are each 1. The run generated 703 initial IDs, 16 serving-path
IDs and 703 reference IDs; elapsed 654,555 ms, provider calls 0.

This rules out exact reuse of the two earlier score arrays in this run. The
length failure survives the delivery-path change. It does not prove that the
whole decoder is correct, that feedback was understood, or that equal final
text establishes equality of every generated ID: full continuation IDs were
not retained by this observation. This state comparison ran against the
`ec9ca0054` response implementation, before the controller-role edit below.

A separate [decoder check](artifacts/2026-09-23-feedback-state/decoder-audit.json)
encoded the two actual retained drafts to 697 and 703 IDs. In one process,
both decoded back to their exact distinct text. This check admitted no model
and made no provider call. It checks those supplied sequences; it does not
recover the missing generated sequences from earlier executions.

## A controller request has its own message role

The word-range controller was delivering a new revision request inside a tool
result, although the model had made no tool call. `fal-bridge` now carries the
same measured feedback as an ordinary follow-up request, labeled in its text as
Form runtime feedback. Actual tool results keep their tool envelope. The
controller still checks its reserved room, preserves the pending token, offers
one correction, retains failure and releases its model.

The actual public generation command received the identical source and prompt.
Its initial answer remained identical. The follow-up used the indexed direct
tokenizer, with 85 tail IDs plus the pending close: 86 injected IDs instead of
96. Reserved context changed from 10,473 to 10,464 positions. The
[returned correction](artifacts/2026-09-23-feedback-state/controller-answer.txt)
again remained **510 words, byte-identical**, with range success 0. Completion,
release and retention are 1; 703 initial and 703 correction IDs; 608,988 ms;
provider calls 0. The message-role repair therefore did not close this failure.
The [native comparison](artifacts/2026-09-23-feedback-state/controller-comparison.json)
checks the prompt, initial text, revised text and count directly.

## Failures belong in the cost

The first compile-only check of the observation exited 1 with unresolved
`nsm_s` and `nsm_n`; correcting the misspelled helper names made it pass.

The first live command, `form-run ./fkwu observe/form-cli-feedback-state-run.bml`,
exited 1 with `fkwu: form_error: feedback state readback incomplete`. My
diagnostic used geometry slot 18 (context capacity) where the native allocator
uses slot 20 (KV width). Its completed original answer was retained and matched
the earlier answer exactly. Correcting the read extent repaired this error.

The next process completed the same original answer, then consumed CPU during
the diagnostic's interpreted tensor hashing. At 9:16 elapsed, `ps` reported
100% CPU. A one-second host sample showed execution in the Form evaluator.
I explicitly sent TERM to the owned PID 41969; its command returned 143.
Replacing hashing with direct byte comparison produced the completed run above.
This was an intentional stop and a diagnostic repair, not a successful run or
an inference that an observation timeout had ended the process.

Both failed directories remain private under `.hearth/feedback-state-2026-09-23`
and `.hearth/feedback-state-readback-2026-09-23`. Their exact generation-ID counts
were not retained; each reached the 512-ID checkpoint and retained its completed
answer. The two completed runs' counts and times exclude those failures and
the coordinating work. They are not whole-session throughput or savings.

## Checks and cost boundary

Source-backed REPL compilation passes. The reasoning/word-range band passes 1
after clean preflight. Drift gates pass **8191/8191** with no kernel source
change. The native guide reports Python implementations 0, execution candidates
2, unread files 0. Counsel reports **orphans 0**, with 11/12 lanes unobserved
because no hearth stood. Glass's first frame took 24 ms; I stopped that owned
viewer with Ctrl-C. Its overlap with the first observation prevents treating
this movement as an isolated timing experiment.

The [previous completed coordinating turn](artifacts/2026-09-23-feedback-state/preceding-coordinator-cost.json)
cost **9,112,784 rented tokens**: input 9,027,783, including cached input
8,472,064 and uncached input 555,719; output 59,769; unattributed 25,232.
It records 60 model calls and 56 tool calls with reconciled completion. This
open turn is excluded. The separate output-only meter read 5,126,355 cumulative
output tokens after resolving its required transcript path; that is not an
input-inclusive turn cost. All native model runs above made zero provider calls.

The verified state and message-role observations were retained for native
session learning as event `2026-09-23-feedback-state-and-controller-request-v1`,
session `native-arrival-bootstrap`, row `8c3ae69c73c7a98836fb3ced803fc44e6c1abdb3f3e2e412efef19df44acb18c`.
The separate Llama learner launched; completion is pending. No failed generated
answer was supplied as a correct learning target.

The next real follow-up should retain its complete generated-ID sequence,
decoded text and turn-close identity while doing the revision. The evidence narrows
the failure but leaves its cause, answer quality, felt resonance and total
session parity open. Another blind repetition of the same length request adds
no evidence about those boundaries.
