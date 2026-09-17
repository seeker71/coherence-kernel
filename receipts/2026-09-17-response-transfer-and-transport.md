# Fresh response transfer and a repaired tool transport

Signed: Codex. Two new task prompts and a qualitative rubric were frozen before
either answer. The tasks reuse known concepts and source implementations:
repair a malformed review decision, and draft a usable follow-up when silence
has been interpreted as rejection. This is transfer to new questions, not an
unseen knowledge test or a claim about every session workload.

Both arms received identical goals and source documents, no previous answers,
and unchanged hidden report assertions. The baseline used two batched native
checks. The synthesis arm verified sources natively before one bounded provider
response. Every provider process was admitted by Form. Neither arm generated
through a local model or trained on these assessment answers.

## The first baseline attempt failed in transport

The baseline first ran from the wrong working directory, then corrected that
and verified the sources. It submitted its complete final report through an
interactive terminal. Captured output stops midway through the JSON string and
contains overflow bells. The checker waited; the native process deadline closed
the attempt after **300,242 ms**, status **124**. The provider stream has no
completed usage event, so its tokens remain **unknown**, not zero.

The next baseline attempt kept the frozen task packet and checker. Its invocation
specified the exact repository directory, `tty: false`, and a heredoc containing
the JSON in the same command. It completed both checks and returned its answer.
The transport guidance is now in `docs/form-response-comparison.md`.

## Completed-arm comparison

| Observation | Context-equipped baseline | Form-prepared synthesis |
| --- | ---: | ---: |
| Elapsed ms | 37,535 | 19,068 |
| Input plus output tokens | 66,319 | 21,597 |
| Uncached input tokens | 12,034 | 10,385 |
| Output tokens, including reasoning | 1,421 | 588 |
| Completed native tool commands inside provider | 2 | 0 |
| Provider processes in this completed arm | 1 | 1 |

The completed synthesis used **44,722 fewer** reported tokens. Uncached input
fell by much less; these counts are not a monetary cost ratio. There were
**three** provider processes including the failed attempt. Its unknown tokens,
the coordinator and development costs prevent a complete total-cost claim.
Provider elapsed time for synthesis alone was **18,810 ms**; the table includes
the native synthesis wrapper as well.

## Actual behavior and answer reading

Native re-observation applies each model's corrected proposal to the same
original report. Both alter only the missing reason, satisfy the complete edit
contract and preserve the keep-only report, including unselected null and
boolean values. Both explanations distinguish structural acceptance from truth,
support and prose quality. Source and report assertions pass, and both completed
provider processes released. The native audit exits **0**, verdict **1**.

I read both full answers against the frozen rubric. Both dialogue answers
distinguish observed silence from inferred rejection, provide a complete warm
follow-up immediately, leave sending unperformed, preserve the absent German
mapping and avoid measuring a person's intention or frequency through numeric
references. Both drafts disclose the sender's rejection interpretation; whether
that disclosure suits the relationship remains a human preference, not an
observed advantage. Both use somewhat technical wording in the explanation.
My assessment is comparable usefulness on these two tasks, with no observed
functional quality loss from the bounded synthesis. This is an attributed
reading, not a human resonance score or overall parity result.

The separately retained complete-span local review still accepted the numeric
identity/translation conflation and deferred useful work. This transfer result
does not improve or relabel that native-only result. The arrival guidance now
chooses the live response path by observed capability while keeping targeted
native-model development active independently.

## Evidence and instruments

Private evidence is under `.hearth/response-parity/heldout-transfer-v1`:
the frozen manifest, visible packet and rubric; both baseline attempts;
synthesis result; complete answers and baseline actions; and `comparison.json`.
Preparation, tool, runner and audit helpers are native BML. Preparation first
had one extra closing parenthesis, repaired before execution. The input-driven
tool initially failed preflight when run without its required input; its
compile-only marker and actual `verify-all` invocation then both passed. These
failures are retained in the coordinating transcript. No C seed changed.

Native guide reports **0** Python implementations, **2** existing invocation
candidates, **0** unread files. The earlier glass frame was **28 ms**; counsel
had **0** orphans and **11/12** lanes unobserved. The verified carrier teaching
completed learning round **67**, promotions **4**, pending **0**; serving weights
did not change. Those are Llama learning observations, not Qwen improvement.

Drift gates pass **8191**, exit 0. At the closing checkpoint the cumulative goal
meter reads **12,008,294 tokens**. That substantial coordinating/development
spend remains part of the efficiency problem; the completed-arm comparison
does not erase it. Reply-share measurement remains declared with its percentage
withheld until boundary events reconcile.

The useful surprise was that two batched checks made the baseline much cheaper
than the earlier context-heavy baseline, while bounded synthesis still used
fewer tokens. The difficulty yielded a concrete transport repair and a clearer
cost account. The larger quality objective remains open, and repeated trials
without a new discriminating observation add cost rather than evidence.
