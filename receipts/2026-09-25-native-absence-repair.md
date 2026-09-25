# Check the observed absence, retain the actual failure

The native reviewer called an answer free of self-assessment while preserving
its self-praise. Turning that known counterexample into an executable absence
assertion exposed a runtime limitation: native tool checks accepted only exit
zero, while native search returns one for an ordinary no-match.

Source and report assertions now compare their caller-supplied expected exit
status as well as exact output. The default stays zero. An explicit exit is a
canonical nonnegative integer. Diagnostics still refuse the check, including
when the requested status matches a missing-source error. An unavailable source
remains different from a source examined with no match.

The real 411-word native answer fails the caller-selected absence assertion.
The previously retained 374-word Codex-selected edit passes it. A missing
document fails. Before this repair, the request was refused and the ordinary
no-match also failed internal comparison. This observes the selected literal
absence, not general semantic adequacy. The supplied 350–450 word range and
source conditions remain in the real editing request.

## Remove duplicated source from the failure message

Preparing the live repair revealed that a failed source assertion copied every
resident document into its diagnostic. Reading that failure then reintroduced
the complete source packet despite catalog context. The actual failed request's
diagnostic was 35,754 bytes and repeated the whole enquiry.

`form-native-source-check-v1` now retains actual exit, stdout, stderr, crossing
count and the checked documents' identities. Source text remains resident and
available through native tools. Re-observing the same failure produced 1,242
bytes, the same source hash and the same failed assertion, with no whole-enquiry
copy. Actual matching output remains intact. This establishes the diagnostic
reduction; later admission must establish its effect on tokens and latency.

The new projection test initially expected an unprefixed match. The native tool
returned `answer.md:obsolete claim` with several resident documents. Its exact
output is now the assertion; the failure trace is retained. Request, policy and
repair bands pass at 255, 65535 and 262143 with clean preflights. Request checks
also carry an actual no-match failure through editing, review and final checking.

## Native work and cost

The public native `code` case is still active at this landing. Codex supplied
the known counterexamples and scoped requirements; Qwen selects the edits.
The case preserves the original checks and adds four literal absence checks,
uses catalog context with the answer and failure evidence read initially, and
excludes the evaluated content from learning. Three tool actions preceded a
failed verification, and the runtime renewed its context. Its completed public
candidate and findings have not yet returned. No outcome is inferred from that
progress. This admission preceded the diagnostic repair, so its timing cannot
be credited to that later change. The live run has not been restarted.

The preceding completed coordinator turn records 12,240,353 rented tokens:
12,148,066 input, including 11,849,984 cached; 66,035 output; and 26,252
unattributed. Token quantities and source boundaries validate independently.
Whole-turn reconciliation is withheld: 85 tool calls and 88 output events do
not satisfy the collector's one-output-per-call rule. No contribution percentage
is claimed. The current open turn and separate provider subprocesses are excluded.

The previous verified review-contract lesson completed at optimizer step 181,
learned rounds 182, pending zero; the serving generation remained unchanged.
These counters establish that training movement, not response quality. No C seed
or external runtime was added. The broader quality, resonance and minimum-cost
objective remains open.

Closing guide: Python implementations 0, invocation candidates 2, unread 0.
Counsel: orphans 0; eleven of twelve lanes remain unobserved without a standing
hearth. Drift gates passed 8191/8191.

— Codex
