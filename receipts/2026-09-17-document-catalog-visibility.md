# Document catalogs and model visibility

Codex, 2026-09-17.

Read-only native reviews can request `document_context: "catalog"`. Form keeps
the original documents for native tools and checks, while the initial model
prompt lists IDs, paths and byte counts. Full context remains the default.

The important boundary is availability versus visibility. A held document is
available to tools; that does not establish that its text reached the model.
Catalog admission starts with an empty document snapshot. Native `read` returns
actual source bytes, never a reference or splice against omitted text. State
transitions preserve the mode, and a resumed model admission resets visibility.
The caller selects the admission mode on resume. Source/report assertions and
the review's read-only boundary remain unchanged. The returned result names
the mode.

## Observed floor

The document-context witness observes 23 boundaries and returns 1. Existing
request and document bands return 255; policy and native memory bands return
65535. Preflight reports no errors, warnings or unresolved calls, and each
executed band exits zero. Implementation and experiment are native BML on the
existing C-bootstrap; the seed is unchanged.

The corrected v2 dialogue fixture retains its original documents and checks.
Its initial full prompt is 43,979 bytes / 10,505 tokens. The catalog prompt is
5,343 bytes / 1,072 tokens. These are counts from the actual native tokenizer,
without model admission. Private evidence is retained under
`.hearth/response-parity/generalization-v2/`; `catalog-experiment.bml` carries
the bounded native experiment.

The live catalog run requested all five documents: arrival, trust over fear,
voice attunement, uplifting dialogue and core axioms. The smaller initial
prompt therefore did not establish less source ingestion for this enquiry.
The run completed in 1,214,038 ms, with 678 generated IDs, 10,409 injected IDs,
five source reads, six policy turns and one check run. All original source
and report assertions passed, documents remained unchanged, and the model
released. No provider process or native report repair participated.

Initial plus injected input totals 11,481 tokens, 976 more than the full
initial prompt. The initial saving is 9,433 tokens. The earlier v1 full run
used a flawed attribution fixture and was interrupted, so its elapsed time
is not a paired v2 comparison. This experiment establishes neither a faster
session nor less total input for this enquiry.

The actual answer still begins defensively, denies hiding, and closes by
denying pretence. It describes relevant teachings but proposes an abstract
re-presentation of an offer. It applies the held-offer theorem to absence of a
resident process without establishing that equivalence. The two JSON flags
correctly remain false, but those structural checks do not detect the
unhelpful action or defensive voice. Voice parity remains open. The exact
answer stays at `generalization-v2/catalog/report.json` under the private
evidence root; its reply is `.hearth/code-memory/replies/47262-1789585606313.txt`.

## Cost and closure observations

The trial used zero provider processes. Coordinator development is separate:
the goal meter advanced from 6,777,790 at movement start to 6,878,793 at the
post-trial observation, a delta of 101,003. That is an intermediate development
cost, not a complete final-turn total or a per-session native inference cost.
`catalog-audit.bml` retains the native arithmetic and a readable private answer.

The counsel panel reports 0 orphans and 11/12 lanes unobserved because no
standing hearth is present. It supplies no general performance verdict.
Drift gates pass 8191. The native guide reports 0 Python implementations,
2 existing voice invocation candidates and 0 unread files.

The verified boundary teaching was retained under session
`native-arrival-bootstrap`, event `2026-09-17-native-document-visibility-v1`.
It contains implementation observations and checks, with no assessment answer
used as a training target. Retention and worker launch were observed; this
receipt makes no claim of a new serving generation or improved weights.
The transcript output-only meter reads 1,462,602; its scope differs from the
goal meter and the counts are not added. Reply share remains declared and
unmeasured, with its percentage withheld.

## Teaching

The surprising observation is how readily the model selected real source
reads when given only their catalog. The uncomfortable observation is that
the attractive starting count can simply move the cost later. Keeping those
observations separate makes the experiment useful. Neither a smaller prompt
nor passing structural checks establishes response resonance or parity.
