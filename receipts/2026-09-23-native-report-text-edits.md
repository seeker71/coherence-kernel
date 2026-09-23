# Express the correction without repeating the answer

Signed: Codex, 2026-09-23.

The next completed reply from the same native owner amended **521 → 517 words**.
It spent 512 initial-stage and 772 final-stage generated IDs, **1,284** total,
to emit a 3,463-byte action removing only ` of attending to evidence`.
The [actual model action](artifacts/2026-09-23-native-report-text-edits/public-reply-5.json)
and [answer](artifacts/2026-09-23-native-report-text-edits/answer.txt) retain that
limited movement. The original length check still fails by 67 words, the
unsupported frequency-to-response claim remains, and the old findings still
claim the answer fits. No quality parity follows.

The report amendment door required replacing an entire JSON value. It now
also accepts `text_edits`: an existing string path, exact unique `old` text,
and `new` text, with empty replacement text permitting deletion. The original
report's byte identity, nonoverlapping paths, typed values, atomic refusal,
source preservation and complete report checks still apply. Overlapping
literal occurrences are ambiguous too. The native body uses its existing
string search and replacement primitives; no C seed or external dependency
changed. The repair observation advertises this smaller action.

Codex encoded the model's already-observed deletion using the new door.
The [native re-observation](artifacts/2026-09-23-native-report-text-edits/reobservation.json)
establishes an **identical resulting report**, with a **170-byte** action
instead of 3,463 bytes: **3,293 fewer payload bytes**. Original source checks
pass and the original report check still fails at 517 words. This is a
native replay of the same edit, not a newly generated model action. Future
model adoption, generated-token savings and improved answer quality remain
unobserved. The [replay cell](artifacts/2026-09-23-native-report-text-edits/reobserve.bml)
uses committed inputs and takes only an output directory on stdin.

The first preflight of `form/form-stdlib/tests/form-cli-native-review-repair-band.fk`
failed with one error and no readable verdict (exit 1). The direct check
reported `source-compile: unconsumed form.bml string suffix` (exit 2): the
new test's nested quoted string had one missing escape. After repairing that
test input, clean preflight and execution returned **34359738367**, exit zero.
The existing band covers literal deletion, UTF-8 and JSON escaping, nested
array paths, stale bases, ambiguous matches, atomic failure and unchanged
source/report checks. Request and policy bands returned **255** and **65535**,
respectively, after clean preflight, exit zero.

Drift gates returned **8191**, exit zero. Counsel reports **zero orphans**,
with 11/12 lanes unobserved and no standing hearth. The native authoring guide
reports zero Python implementations, two existing invocation candidates and
zero unread files. The verified API and replay teaching was retained under
session `codex-native-arrival-bootstrap`, event
`2026-09-23-native-report-literal-edit`, example
`f64917827df903377bb8bd3c54bd5cfa08773056d45903daa6c5c7451103e5e6`.
Retention is observed; a serving-weight update is not established. No model
answer from this movement was submitted as a correct training target.

The continuing review remains owned by PID **57766**, exec **7861**, with its
already-loaded controller. It has not been restarted for this change. The
new operation is available to subsequent native contexts; no claim is made
that this running context has acquired it. Its eventual result remains due.
