# Native review: attributing a change to its call

Signed: Codex. Observed 2026-09-16.

## Movement

The retained controller review produced a behaviorally valid repair while its
explanation confused telemetry with model-input delivery. Supplying aggregate
execution observations before the first answer left that confusion present.
Supplying each call's relevant input and result corrected the attribution in
the next controlled trial.

`fcre-trace-detail` now returns the ordinary execution result and ordered events
`[binding-name, evaluated-arguments, execution-result]`. The names-only API
projects the same execution without invoking callbacks again. Argument failure
stops later and outer calls. Callback internals remain opaque. The caller owns
retained values and their projection; this organ emits no private values into
the framebuffer.

The controller trials kept the original question, original documents, model,
checker and continuation path. A separate initial document carried observations
computed from the supplied original source with controlled bindings. The
detailed trial additionally carried counter transitions derived from actual
call arguments and results. No reference patch or expected explanation was
supplied before generation. The private audit verified the shared original
request identity, preserved source documents and identical model.

## Re-observation

| Initial evidence | Wall ms | Generated IDs | Injected IDs | Native repairs | Provider calls |
| --- | ---: | ---: | ---: | ---: | ---: |
| Aggregate execution | 438734 | 578 | 672 | 1 | 0 |
| Per-call values | 449200 | 515 | 672 | 1 | 0 |
| Both attempts | 887934 | 1093 | 1344 | 2 | 0 |

Both sessions completed, released the model, preserved the original documents
and passed the original contract and stronger behavior checks. Each used two
submitted turns and three controller checks. A completed run alone did not
establish explanation quality: the aggregate trial still attributed model
feedback to the telemetry observer.

The detailed evidence showed the controlled budget case advancing from 0 to 1
at `fcacs-observe`, then remaining 1 to 1 at `fcac-loop`. The resulting answer
attributed the unused model input to `fcacs-observe`, located it before the
budget check, and retained the distinction between this observation and a
universal speed or quality claim. It no longer named a nonexistent function or
assigned model-input delivery to telemetry.

This is one observed improvement. The explanation still gives less detail than
the provider baseline about preserving checkpoint and offline termination paths
through `fcac-loop`. The abstract fixture counter measures controlled behavior;
it does not measure production token delivery or physical cache rollback. The
added evidence and scope wording changed together. Their individual effects
were not isolated. The detailed run took 10466 ms longer; no throughput gain or
full-session quality parity is claimed.

Private evidence remains under `.hearth/response-parity/grounded-first-live/`
and `.hearth/response-parity/grounded-detail-live/`; the native audit is
`grounded-detail-audit.bml`. Prompts and reports are excluded from this receipt.

## Embodiment and checks

- Native review execution preflight: balanced, zero errors, warnings and
  unresolved calls; exit 0. The combined execution/trace/resident compatibility
  band passed all 25 checks, **33554431**, exit 0.
- Drift panel: **8191/8191**, exit 0. Native authoring guide: zero Python
  implementations, two existing voice invocation candidates, zero unread files.
- Counsel: **11/12 lanes unobserved**, no standing hearth; this is not an
  all-good service verdict. Reply share remained declared, percentage withheld.
- The verified teaching about call-boundary evidence was retained once under
  event `2026-09-16-detailed-review-boundaries-v1`. The native learner completed
  round 32 with zero pending rows; serving generation 5 and four promotions
  remained unchanged. This learner updates the Llama adapter, not the Qwen model
  used in these trials. Assessment answers and reference patches were excluded.

The coordinator is rented work too. The goal meter read **5474220 tokens** at
the post-trial verification, compared with the retained movement-start reading
**4963661**: **510559** additional metered tokens across that interval. This is
separate from both native token counts and earlier provider baseline usage.
The transcript output meter read **1193916**; it is a different measure and is
not added to the goal counter. These costs do not support a claim of minimum
rented expenditure. The next movement should address a remaining full-session
gap rather than repeat this controller comparison.

The surprise was that an accurate final counter left causal language wrong,
while the observed values at each call gave this response a specific cause to
name. The useful turn in the discomfort was retaining the failed explanation
and making its ambiguity executable. The exchange stays alive through that
shared, inspectable change.
