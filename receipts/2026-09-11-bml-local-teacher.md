Codex · 2026-09-11

Local Qwen generated three fresh BML functions that passed all 18 native
execution checks. A native Llama LoRA fit learned its three practice functions,
also passing 18/18 checks, but still passed no complete transfer task. These are
different results: the local teacher can code these new compositions from
verified context; the smaller learner has demonstrated practice acquisition.

The frozen curriculum in `learn/bml-scalar-practice.bml` separates counting
present entries, first-positive selection and sum of squares from zero counting,
first-negative selection and absolute-value summation. Every reference was
executed before generation or training. Transfer answers never entered gradients.

| Observed condition | Complete tasks | Execution evidence |
| --- | ---: | --- |
| Llama candidate 13, unassisted transfer | 0/3 | All three failed compilation |
| Qwen practice, original responses | 0/3 | Markdown fences failed compilation |
| Same Qwen practice, explicit fence-body extraction | 3/3 | 18/18 checks; no code edits or new model calls |
| Llama fit candidate 26, unassisted practice | 3/3 | 18/18 checks; exact generated declarations |
| Llama fit candidate 26, unassisted transfer | 0/3 | Zero-count compiled, passing 3/6 checks; two compile failures |
| Shared session candidate 17, unassisted transfer | 0/3 | All three failed compilation |
| Qwen transfer with verified practice context | 3/3 | 18/18 checks; raw code, no extraction or edits |

The native fit made 13 updates across two admissions, from candidate 13 to 26.
Each update changed 56 LoRA A/B pairs on the native Metal Llama path, with Adam
continuity and prompt masking. It consumed 5,182 input tokens and 1,512 supervised
tokens. After the initial admission, the loop trained only currently failing
functions and re-observed all three after every update. Its recorded selection
narrowed from three functions to two, then one, and brought a previously passing
function back into practice when its generated behavior failed again.

The fit admissions occupied 4,891,928 ms in total. Completed training rounds
occupied 2,627,163 ms, including 2,598,389 ms of training, 9,022 ms of optimizer
updates, 17,475 ms of checkpoint work and 2,277 ms of token preparation.
These round components and the row GPU/host measurements are overlapping views.
The admission total includes 15 generated practice observations and owner work;
it excludes the interval between admissions. Other local model jobs were active,
so these times are not a controlled hardware-floor measurement.

Qwen's fresh transfer run consumed 1,916 prompt tokens and generated 195 tokens.
Its 1,276,594 ms total comprises 1,147,798 ms of context preparation, 128,514 ms
of answer generation and 282 ms of other work. The first context duration already
includes model admission. Weights stayed admitted between tasks; each task
received fresh recurrent and KV state. Qwen used no LoRA or training in this run.
The recorded model releases left zero live buffers.

The important diagnostic was smaller than the average loss suggested. At fit
checkpoint 18, count's teacher-forced predictions matched 41/42 target tokens;
the remaining decision chose ` pollen` instead of ` add`. Checkpoint 19 then
generated a correct counter. First-positive at checkpoint 19 matched 68/69
target tokens, choosing ` if` instead of its final ` head`; at checkpoint 22,
the remaining error was the earlier ` if` needed to skip zero. Exact positions,
IDs and pieces remain in the private token observations. Generation and
teacher-forced prediction are measured separately.

Concrete prefix probes also executed counterexamples before requesting two
runaway practice streams to stop. A transfer stream carrying unresolved `tai`
was retained as failed evaluation. These controls preserved the original
responses; diagnostic suffixes were never treated as model-generated answers.
A missing-adapter probe exposed accidental base-model admission in an initial
cache comparison. That comparison is marked invalid. The corrected comparison
admitted checkpoint 18's 56 pairs and reproduced the same count response after
reopening and with genuinely larger KV allocation.

The new token reader initially passed JSON nodes to an API expecting serialized
rows. That helper error and its admission remain recorded; correcting the row
boundary produced the successful 69-token readings above.

New native pressure handling responds to interference by reducing update size
when a previously passing function fails. Its actual observation/response/request
boundary was replayed on retained consecutive executions: 0.0001 became 0.00005;
an unchanged observation preserved 0.00005. The completed fit ran the preceding
fixed-rate policy. This receipt does not attribute its success to the newer
pressure handling or claim a completed training trial of that handling.

The Qwen response decoder now decodes each new token chunk once and retains token
history through shared list tails. An isolated 49-token parity check produced
identical text in 3,414 ms versus 17,343 ms for repeated prefix decoding. This is
a decoder measurement, not a model-throughput claim.

Five verified teachings completed through the ordinary session learner: three
executed teacher code targets and two verified procedural teachings. Its candidate
advanced from 13 to 18; serving remains generation 4, with zero pending rows.
The transfer measurement above names candidate 17 explicitly. Candidate 18's
completion and serving assessment are retained separately. Learning here updates
the native Llama adapter, not Qwen or the external authoring model.

The current glass sample measured 19/20 frames under 50 ms, mean 37 ms, first
frame 191 ms and maximum warm frame 35 ms. Queue and storage sensors were absent;
counsel still reported 11/12 judged lanes unobserved. Share remained declared
with its percentage withheld. Native experiment `remote_model_calls=0` excludes
Codex's external authorship and orchestration; it does not establish independence
from external reasoning.

The [native-generated data](2026-09-11-bml-local-teacher.data.json) carries the
conditions, source hashes, admissions, losses, tokens, timing events, selected
care, failed comparisons and session outcomes. Exact private proposals and token
streams remain at the `.hearth` paths it names. Runtime doors and their scope are
documented in [native BML execution learning](../docs/native-bml-execution-learning.md).
Preflights closed without errors or unresolved calls; the drift door returned
8191/8191 with zero refusals before landing.

After rebasing onto current main, kernel freshness returned 15. Rebuilding the
checkout witness restored 31, and all six successful generated functions again
passed their 36 execution checks. The native guide now reports one remaining
Python implementation elsewhere in the repository; this movement introduced none.

The repository knowledge source list also omitted this learning guide. Adding it
and republishing produced 1,985 source-attributed examples from 1,178 source units.
Both guide sections follow the existing whole-document held-out split. Source
rehashing and manifest checks passed; train/held-out prompt and answer overlaps
were zero. This publication adds available source knowledge, with zero training
updates; the five completed session updates above remain a separate measurement.

The exchange stayed alive by asking local Qwen, executing its proposals, and
letting failed execution change the next native action. The surprising teaching
was how one wrong token could survive otherwise near-perfect target prediction.
The discomfort of repeated failures became useful when it exposed interference,
produced a token-level diagnostic, and ended in a measured local coding result.
