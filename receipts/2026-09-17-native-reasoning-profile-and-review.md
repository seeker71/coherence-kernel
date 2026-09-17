# Preserve reasoning across tokenizer routes; observe a plain local review

Signed: Codex. The preceding movement landed as `9ce9e3467`, repairing source
selection and retaining exact declared-JSON response boundaries. It made
progress. The local learner has now completed round **71**, pending **0**,
promotions **4**; its serving adapter is unchanged. This is the Llama session
learner, not evidence that Qwen learned the assessment.

## The next quality observation

The compact review corrected the speaker and removed the placeholder, but
endorsed an unsupported transfer from computational timeout to human silence.
It generated with an explicitly closed reasoning channel. Earlier reasoning
experiments used the coding bootstrap. This experiment uses the exact caller
prompt, sources and retained draft of `dialogue-json-review-v2`, with open
reasoning and no coding bootstrap.

A correlated native control selected the observation. The model is Qwen3.8
27B Q8, through `fcms-open-with-profile` with the existing
`knowledge-query-reasoning` profile, 12288 positions and a 4096-token ceiling
covering reasoning and final answer. The previous compact run had 1536-token
direct/query and post-lookup allowances. This intervention changes reasoning,
the output allowance and the session carrier. The session carrier exposes
actual generated IDs; it does not perform the ordinary generator's automatic
knowledge lookups. The prior compact review used zero such lookups. This is
not an isolated causal test of one factor.

The review takes no replacement answer or list of the particular errors.
Its final answer is separated at the actual closing token using `fcrt-answer`.
Reasoning remains private, and all assessment material stays outside learning.
Original source and report checks remain unchanged. A final judgment must
come from reading the returned answer, separately from those checks.

Evidence lives under `.hearth/response-parity/dialogue-reasoning-review-v1/`.
The run completed its allowance and released its model, with no final-channel
boundary and no answer:

| Measurement | Observed |
| --- | ---: |
| Prompt tokens | 4168 |
| Generated tokens | 4096 |
| Final-channel tokens | 0 |
| Stop observed | 0 |
| Release complete | 1 |
| Original report checks | 0 |
| Original source recheck | 1 |
| Elapsed ms | 1088672 |
| Provider processes | 0 |

The final-channel file is empty and the decoded report is null. No text from
private deliberation was promoted to an answer. The process exits **0** because
the measurement and release completed; the receipt records failed answer
completion. It took substantially longer than the prior compact run's
**372536 ms**, which returned a flawed but complete answer.

A bounded inspection of the private generation shows repeated drafting and
manual word-by-word counting, with conceptual applicability still under
discussion at the cut. This was continuing generation, not a dead process.
It does not establish that more allowance would resolve the error. The next
discriminating intervention is to give deliberation and final response their
own explicit allowances and let native code carry mechanical length checks.
Any controller-driven transition must remain distinct from a model-generated
closing token and keep its injected bytes and cost visible. This run supplies
no quality improvement and no basis for making open reasoning the default.

## Profile consistency at the fallback boundary

The live cursor knew the reasoning profile. The indexed/reference generation
fallback did not; session admission also selected the full scaffold regardless
of its requested profile when the cursor was absent. A missing cache could
therefore change the model's instructions and close its reasoning channel.

`bml/form-cli-reasoning-template.bml` now rebuilds the same scaffold from the
existing tokenizer and materializes those same segments. General generation
uses it for reasoning through the indexed/reference routes. Session fallback
uses the requested profile; the existing stream-renewal reference path shares
that delegate. The ordinary profiles keep their existing token composition.
The change adds no C runtime code, external dependency or model server.

The first edit missed the semicolon after an expression-bodied block; native
lowering refused with `unconsumed form.bml do suffix`, exit **2**. After that
repair, the pure empty-text witness exposed an unnecessary reference-tokenizer
call on its deliberately absent source: `str_byte_at: only a string has bytes`,
exit **1**. Empty text now contributes no IDs, as it does in the live cursor.
Fresh preflight and execution then pass **16383**, exit **0**.

The actual Qwen tokenizer replay checks three inputs through the live cursor,
indexed materialization and reference materialization:

| Profile | Prompt bytes | Token count | All three equal |
| --- | ---: | ---: | ---: |
| Open reasoning, empty prompt | 0 | 10 | 1 |
| Open reasoning, literal marker in prompt | 30 | 18 | 1 |
| Compact closed reasoning, same prompt | 30 | 20 | 1 |

Replay takes **18514 ms**, returns **1**, exit **0**, with no model admission
or provider processes. It overlapped the live review's prompt admission.
Its data is `.hearth/response-parity/reasoning-profile-token-replay.json`.
The live review loaded its implementation before this fallback repair; the
repair does not explain its result.

Fresh preflight also passes on the session carrier, generation-report band
and declared-JSON band. Their executed bands return **16777215** and **1**,
exit **0**. `git diff --check` is clean.

## Instruments and scope

Freshness is **31**. Native guide reports Python implementations **0**,
invocation candidates **2**, unread files **0**. No standing hearth answered.
Its stale image was rebuilt by the native compiler's observed/applied care.
No provider subprocess has been used in this movement; the coordinating
agent still incurs provider cost. Overall quality parity remains unproven.

Counsel observes orphans **0**, with **11/12** lanes unobserved because no
hearth stands. The owned glass viewer ran during part of generation and was
closed with exit **130**. Its terminal stream was larger than the bounded
reader output; no renderer-speed conclusion is drawn from that reading.
Native guide remains **0** implementations, **2** invocation candidates,
**0** unread files. Drift gates pass **8191**, refused **0**, exit **0**.

The spendglass checkpoint is **2653198 cumulative transcript output tokens**;
the separate goal counter checkpoint is **13207798 cumulative goal tokens**.
These are neither local-model counts nor the cost of this single comparison.
The share reading is **declared** and withholds a percentage because no
completed evidence row is available. No semantic contribution score is inferred.

The useful surprise is that cache availability had changed the requested
reasoning profile. That repair is observed across actual tokenizer routes.
The difficult result is also retained: more private deliberation yielded no
usable answer. Keeping those facts separate directs the next attempt toward
response completion and applicability, while preserving the verified repair.

After Qwen released, the verified procedural teaching
`reasoning-profile-fallback-verified-v1` was retained under
`codex-native-response-parity-2026-09-17`. The local learner launched; no
serving improvement from this teaching has yet been observed.
