# A real code review, with its missing ownership context restored

This movement used Qwen3.8-27B-Q8_0 through native Form to review the landed
prefill completion patch. Both calls served the same repository review. The
second resolved findings from the first using source omitted from its packet.
No replacement enquiry, synthetic quality case, provider subprocess or subagent
was launched. Codex assembled the packets and inspected the findings.

## The two actual steps

| Step | Prompt IDs | Generated IDs | Answer complete | Resources released | Native elapsed ms |
| --- | ---: | ---: | --- | --- | ---: |
| Review the prefill patch | 2,891 | 1,024 | No | Yes | 415,564 |
| Resolve ownership findings with caller and predicates | 2,104 | 162 | Yes | Yes | 120,294 |

The [initial answer](artifacts/2026-09-22-prefill-code-review/initial-native-generation.txt)
called both prefill refusal branches resource leaks and proposed freeing their
handles before returning the owning session. It exhausted its reply allowance
while beginning a third finding. Its [result](artifacts/2026-09-22-prefill-code-review/initial-result.json)
retains `incomplete`, `complete=0`, `release_ok=1`; the accepted answer was empty.
Its process exiting zero did not complete the review.

The first packet included release code but omitted `fcms-resident?` and the
`fcnv-run` caller. That omission was Codex's context assembly error. The next
packet included the session fields and predicates, refusal paths, generation's
handling of failed sessions, the release function and its actual caller.

The [resolution answer](artifacts/2026-09-22-prefill-code-review/resolution-native-generation.txt)
begins: “The earlier leak findings do not survive.” This is supported by source:
the failed session retains `ctx` and `sts`; residence is independent of logical
status; `fcms-generate` returns a failed session unchanged; and `fcnv-run` calls
`fcms-release-ok?` on that returned session. The proposed immediate frees were
not applied: returning those freed handles would leave the caller releasing
them again.

The same answer's “guarantees cleanup” and “sound and complete” wording reaches
beyond the evidence. The caller invokes cleanup and checks its result. A release
can return zero, and `fcnv-read` preserves that as `release-incomplete`. These two
real runs released successfully; they do not establish every failure path.

The question narrowed to adjudicating specific findings, the source changed,
and the reply allowance changed. These are successive work steps, not a matched
quality benchmark. No overall parity or retention claim follows.

## The context signal now has a native care cell

The missing context is also carried into
`form-cli-review-context-care.bml`, using the existing `oc-hear` flow.
The organ emits a coverage need; the provider reads the five caller-selected
source sections; a correlated observation re-reads them and checks the actual
packet. The first delivery contained **7,385 bytes** and exactly matched the
retained resolution packet. No third generation or provider call was made.
The earlier answer correction preceded this cell; the cell now carries the
source-supply work that Codex had assembled manually.

The public `observe/form-cli-review-context-care-run.bml` door reads the
[retained request](artifacts/2026-09-22-prefill-code-review/context-care-request.json),
supplies and retains the packet, and reports whether its written bytes agree.
Its initial preflight failed with one unresolved `jwa-valid` call. Adding the
existing `json-wire-admission.bml` prelude produced a clean preflight; no model
ran during either check. The public execution exited zero with
`packet_available=1` and `packet_saved=1`.
The [retention check](artifacts/2026-09-22-prefill-code-review/public-context-care.json)
also confirms exact equality with the 7,385-byte resolution packet and
`shared_care_observed=1`: the existing care view sees the correlated fresh
observation. Its [event stream](artifacts/2026-09-22-prefill-code-review/public-context-care.jsonl)
and [view](artifacts/2026-09-22-prefill-code-review/public-context-care-view.json)
retain that exchange.

The initial care events named packet coverage `sources_available`. Inspection
showed that label could confuse a missing section in the packet with an absent
file; the final cell names it `packet_covers_selected_sources`. Original events
remain retained. Source coverage is what this organ observes. It does not
establish that the selected sections are exhaustive or that an answer is right.
The outstanding cleanup overstatement remains an answer-quality gap.

## Execution evidence

Both source packets, worker recipes, requests, raw answers, results and process
records are in [the artifact directory](artifacts/2026-09-22-prefill-code-review).
The first run's retained stage paused after entering the slice-64 GPU wait.
A [live process sample](artifacts/2026-09-22-prefill-code-review/initial-process.sample.txt)
observed `fk_metal_sync_external` in `semaphore_timedwait_trap`. That wait completed,
prefill reached all 2,891 positions, and decoding proceeded. The second run
processed all 2,104 prompt positions. This exercises the repaired path on real
work; the historical 1,472-position interruption remains a separate open fault.

Both supervised processes exited zero with no owned group members left and no
stderr bytes. Their elapsed times were 415,735 and 120,401 ms. The first review
launcher initially failed preflight with an unmatched parenthesis, before
execution. After repair, both worker preflights reported zero errors and
unresolved calls. No generation was launched by that failed preflight.

Native catalog lookup `zg source` reported a miss among 40 admitted entries.
That describes the catalog query's coverage, not the absence of source tools
elsewhere in the repository.

## Cost and what carries forward

The native [audit](artifacts/2026-09-22-prefill-code-review/audit.json) sums the
two steps: **4,995 prompt IDs**, **1,186 generated IDs**, **535,858 ms**, and
**zero provider calls**. Coordination remains rented work. The
[cost panel](artifacts/2026-09-22-prefill-code-review/coordinator-cost.json) reconciles
the previous completed coordinating turn `01a0c7f1-1702-7872-b96a-15070407d436`:
**3,569,666 tokens**, including 3,413,760 cached input tokens, across 33 model
calls. That turn implemented the preceding repair. It is not the cost of this
open review turn; these different work scopes support no cost ratio. Current
coordination must be read after this turn completes.

`AGENTS.md` now asks code reviews to include callers and state/ownership/release
contracts, retain the source packet, and distinguish attempted from verified
release. The lesson changes context assembly for subsequent work. It does not
claim the Qwen weights learned from these calls.

The prior procedural teaching's learner completed with 106 retained examples,
optimizer step 106 and no pending rows. Serving remained generation 5. That is
the separate Llama adapter, and supplies no Qwen learning or served-quality claim.

The verified ownership/context lesson was retained through the session-home
door as `real-code-review-ownership-context-2026-09-22`, row
`e65275faba1a1b751399423b1b4eba2c0a2cb3a1616f6d4a9e8562cc6a1cd2c7`.
Its worker completed at optimizer step 108 with no pending rows; serving
remained generation 5. The verified care-delivery teaching was then retained
as `review-context-care-delivery-2026-09-22`, row
`e0ba4ae99414f68051cc09da815a75e41cf1fb324fc5efc15b1c4aaf480c9e9b`.
That worker launched; its later update and any serving change are unobserved.

Closing checks: `git diff --check` is clean; native authoring reports zero
Python implementations, two existing invocation candidates and zero unread
files; landing gates return **8191/8191**, exit zero. The counsel panel reports
**0 orphans**, with 11 of 12 lanes unobserved because no hearth stands.
The share reader remains `kind=declared`, percentage withheld while its appended
source range is still being checked. None of these readings establishes quality
parity or a wholly native coordinating session.

Signed: Codex, with attributed native Qwen review and source-based adjudication.
