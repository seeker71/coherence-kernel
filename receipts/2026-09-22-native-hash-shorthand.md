# Meet the native edit's observed identity

The live selector job's reply 33 supplied three edit arguments: the path,
the current document's SHA256 and replacement text. The tool treated the hash
as literal old text and returned `edit-not-found`. Native measurement confirmed
64 valid hexadecimal bytes, exact equality with the retained source's hash,
and zero source changes. The [actual reply](artifacts/2026-09-22-native-hash-shorthand/native-action.json)
is retained unchanged.

The three-argument edit now checks a missing literal against the complete
current document's identity. An exact match reaches the existing hash-guarded
replacement. One literal match still replaces that substring; multiple literal
matches still report ambiguity. Stale hashes, unchanged replacements, role
authority and writable paths keep their existing boundaries. The explicit
four-argument hash form remains available.

## Real action, separate outcomes

The [re-observation](artifacts/2026-09-22-native-hash-shorthand/observation.json)
uses the native reply's original 1551 bytes and original source, without a model
call or live checkpoint mutation. Tool delivery exits 0 and preserves the exact
proposed replacement. The unchanged caller checker then compiles that replacement
and exits 2: `unconsumed form.bml expression: while ...`.

The [before failure](artifacts/2026-09-22-native-hash-shorthand/before-failure.txt),
[before source](artifacts/2026-09-22-native-hash-shorthand/before-source.txt),
[after source](artifacts/2026-09-22-native-hash-shorthand/after-source.txt) and
[compiler result](artifacts/2026-09-22-native-hash-shorthand/after-check.json)
keep both boundaries visible. The delivery mismatch is repaired. The proposed
source remains invalid; none of its behavior cases ran. This does not establish
native task completion, higher answer quality or lower rented-token use.

The already-running job retains its loaded tool implementation. Its reply 34
diagnosed the edit-format mismatch and proposed recursion, while also incorrectly
saying the first `if` still lacked an `else`. That branch already had one in
the retained source. The diagnosis is native output, not a verified teaching.
The live checkpoint advanced to step 34 during this work.

## Verification

The policy band returns 65535 after clean preflight. It checks shorthand and
explicit replacement equivalence, stale identity, unchanged source, literal
precedence, ambiguity and read-only authority. The actual caller compiler still
rejects the invalid native proposal.
The existing tools band returns 65535, edge and wire bands each return 131071;
their preflights are clean.
Drift gates pass 8191/8191, refused=0, exit 0. The native authoring guide reads
0 Python implementations, 2 invocation candidates and 0 unread files. The
verified teaching is retained as event `native-hash-shorthand-2026-09-22` in
session `codex-native-arrival-bootstrap`; serving improvement remains unobserved.

Glass's first frame took 47 ms against its 5000 ms attention scale. The completed
turn meter reconciled 6429078 rented tokens, including 6301312 cached-input
tokens; it is the same earlier turn retained in the preceding receipt, not an
additional sample. Its event shares are native 28, local 39, remote 33 under
`observed-boundary-event-counts-v1`; semantic contribution is unmeasured.

My observation helper first failed preflight on an unmatched parenthesis and
then on the invented `fat-wire-strings` binding. I corrected both before reading
its result. Subsequent source changes caused existing cached helpers to emit
their rebuild signal and regenerate through the native compiler care path.

The useful distinction was exact identity versus a protocol marker: the native
reply already held the right identity. The receiving tool can recognize it
without accepting a guessed document or weakening the compiler check.

— Codex
