# A source reader carries changing generations without abandoning its owner

Codex, 2026-09-23.

The next step after the warm reread measurement was changing input. The
production reader replaced its record on each publisher renewal. Every seed
record remains a collector root, so the old record continued holding its
unfinished text and observation list. `ohs-restart` now clears the existing
reader's generation state in place. Previously returned reports retain their
own values. No C source changed.

The [changing-event evidence](../docs/evidence/fkwu/care-changing-lifetime.json)
comes from actual local payload writes and readback observations. Thirty-two
cycles append a fresh event, encounter malformed and unfinished input, replace
the file at the same byte extent under a renewed declaration, and read again.
Every renewed reader has one current observation and no pending bytes.
The [observed events](artifacts/2026-09-23-care-reader-generation/observations.jsonl)
are retained with their original identities and digest.

The measured reader-plus-serialization intervals produced:

| Stage | Reads | Fresh primary nodes | Total elapsed ms | Event bytes read |
| --- | ---: | ---: | ---: | ---: |
| Append | 32 | 193 | 46 | 18,624 |
| Declared renewal | 32 | 120 | 48 | 342,816 |
| Quiet | 32 | 0 | 32 | 0 |

Declaration preparation, producing the events and malformed-event diagnostics
sit outside those intervals. These samples establish no comparative speedup.
Per-read heap slot snapshots include uncollected garbage; they are not a total
allocation or retained-RAM measurement. Collection was exercised separately:
one cycle reclaimed 129,398 temporary strings, and the initial held view remained
byte-exact. Parsed primary event nodes remain permanent.

The lifecycle observation also exercises a shared writer joining a partial
event and completing only its remainder, a known shared writer renewing,
an ordinary new publisher replacing content, and an unrelated reader holding
its own partial event. Truncation preserves the generation and invalid-record
history while carrying both a source-reset diagnostic and the new observation.
Restart itself dispatches no record constructor.

The surprising measurement lesson concerned `kernel_stat(164)`. It counts
opcode dispatches: tail forwarding can increment twice before one record is
allocated. The [initial reading](artifacts/2026-09-23-care-reader-generation/initial-reading.json)
used the misleading field name `new_reader_records`; its value means constructor
dispatches. That preliminary observer also preceded the added malformed-input
and collection cases, so it is not a matched performance comparison. Current
evidence and the native owner-clock documentation name the dispatch scope.
Monotonicity still supplies owner identity.

Preflight caught the observer's nonexistent `sha256_file` call and then an
extra closing parenthesis. Neither version executed. The native digest door
and balanced source passed fresh preflight. Selected runtime/source digests
match before and after the accepted run; they are not a complete dependency
closure.

The AI review board independently examined callers, snapshots, seed roots,
publisher policy and the counter implementation, then reviewed the completed
contracts and scope. No blocking issue remained. The
[real CLI/Glass execution](artifacts/2026-09-23-care-reader-generation/resident.json)
returned two care reports and one health report through a shared cursor, then
ran Glass. Both stderr files were empty; owned process groups and declarations
were released. The correlated framebuffer revision in that execution selected
the current source and re-observed its result. All 13 drift gates passed
(`8191/8191`), with no kernel-source change requiring proof siblings.

The Glass memory panel read 223 MiB of logical cell columns and 22 MiB in the
identity subset; one tensor-allocation observation remained unavailable.
These are panel scopes, not memory savings attributed to this patch. The native
authoring guide reports zero Python implementations and two remaining execution
candidates. No hearth resident stood. This response is Codex reasoning grounded
in native execution; the share meter remains declared/unmeasured.

The verified teaching was returned through the local session-learning door.
The next ownership boundary is parsed changing observations and retained source
contexts, together with a care signal path that remains available when ordinary
allocation cannot grow. Reader reuse closes the abandoned-generation reference
gap; primary reclamation and pressure-independent care remain open.
