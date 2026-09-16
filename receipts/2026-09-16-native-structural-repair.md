# Native search repairs the repeatedly failed proposal

Signed: Codex, 2026-09-16.

## Change and actual result

`form-cli-review-search.bml` searches a specific one-edit family: replace an
existing call with a compatible native binding, forwarding the function's
original parameters. The caller supplies result roles, bindings and unchanged
behavioral checks. Every generated source is reparsed before checking. The
budget counts actual checker invocations, including the unchanged original;
exhaustion or refusal returns that original source.

This strategy was designed after examining the repeated failure. It is targeted
repair, not an unseen-task evaluation or unrestricted synthesis. The search
itself receives the failed proposal, existing bindings and their result roles,
and the five original behavior cases. No reference patch is supplied. The
caller declares `fcac-finish` and `fcac-loop` as returning the same continuation
result shape; their actual controlled implementations remain the checker inputs.

The native search finds a changed proposal in **12 ms**, with **three checker
invocations**, zero model calls and zero provider calls. That timing surrounds
the search only; process startup, compilation, implementation work and later
verification are outside it. The result routes the exhausted-budget branch
through the existing loop without injecting feedback. Existing checkpoint and
model-liveness outcomes therefore remain observable.

The generated proposal hash is
`591f150cb8b21448e2ac75db7362d8db9aced40dc1ed68c874a08df1fc361cde`.
The independent compiled witness hash is
`eecaa2527d1262bab27366c841a4174905f7c0252145757cec0c42ca9bf2ead0`.

## Re-observation and limits

The generated source compiles and runs through fkwu against the frozen original
loop and controlled dependencies. All five original cases pass. An additional
compiled cross-product covers completion, checkpoint success/failure, live/offline
model, three used-turn counts, two turn limits and two context positions:
**96 cases, zero failures**. It compares complete result values and expected
feedback-injection counts, with varied retained candidate values. These cases
were not search inputs. They remain controlled branch tests, not live disk or
GPU fault injection or a claim about arbitrary inputs.

The public search band uses a separate two-argument task. It checks additional
input values, budget exhaustion, unchanged source, absent roles, malformed
checkers, failed candidates, invalid admission and escaped/unicode rendering:
**511**, exit 0. The evaluator band remains **65,535**, exit 0. Preflights are
clean. A correlated branch action 2 re-observes the original five cases on
current source and retains the 96-case compiled evidence.
The landing drift door passes **8,191/8,191**, exit 0, with zero refusals;
`git diff --check` is clean. The share reader reports declared/unmeasured and
withholds a percentage.

The first private search probe lacked an effect marker, so preflight executed
it and retained its result. The marker is now present; that result was inspected
and independently checked rather than overwritten by an accidental duplicate
run. Private inputs, generated proposal and per-case evidence remain under
`.hearth/response-parity/structural-*`.

The reusable native function and its documented embedding interface are the
delivered capability. This does not automatically enable search for every JSON
CLI request. It repairs this proposal; the earlier model-written explanation
has not been regenerated or upgraded by this result. The production controller
already contains the correct budget behavior and is not replaced by the probe.

## Cost and embodiment

At the intermediate snapshot, the goal counter moves from 4,132,728 to 4,166,049:
**33,321 coordinator tokens**, subtracted in Form. These implementation tokens
remain rented; zero model calls applies to the native search, not this entire
movement. This is neither final-turn usage nor an API input/output breakdown.
The separate transcript meter reports 952,759 cumulative output tokens, decided
through byte 47,002,506 of 47,002,538; these scopes are not added.

The panel reports **11/12 lanes unobserved**, with no standing hearth. The native
guide reports zero Python implementations and two existing invocation candidates
in `voice-say.bml`, outside this movement.

The verified method is retained as session `native-structural-search-v1`, event
`verified-search-contract-and-independent-observation-v1`. At observation the
worker initially has one pending example at completed round 24. Before landing,
candidate round 25 is written, pending is zero, and the teaching door exits 0;
four promotions and serving generation 5 remain unchanged. The teaching excludes assessment proposals and
reference patches. The learner trains Llama 3B, not the Qwen used in earlier
trials; no learned response gain is claimed.

The useful surprise is that three native checks resolve a proposal that repeated
model dialogue did not repair. The failure became a concrete capability when
the native body generated and tested a structural alternative. Connecting that
capability into broader autonomous sessions, improving the resulting explanation,
and proving overall quality, resonance, throughput and rental efficiency remain
active work.
