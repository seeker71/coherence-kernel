# Native authoring, carried together

The standing direction is native Form/BML for implementation and for the helpers
we use while doing the work. Detection serves the next native attempt. The acting
agent carries that attempt and its evidence; Urs does not need to police a
language choice, approve the standing direction again, or maintain our backlog.

Run `form-run ./fkwu observe/native-authoring-guide-run.fk`, or use `heal guide`
inside form-cli. For a proposed command, `heal guide|<command>` returns a native
reference and witness when one is known. It does not execute that command.

The guide reads tracked and unignored working files each time. It records Python
implementations, source lines that may invoke an interpreter, comment and guide
references, and foreign-language compiler inputs separately. The latter are data
for Form to read, not a reason to execute Python. Historical receipts remain
intact. The catalog's presence check says a reference exists; the acting agent
still runs its witness and checks that the behavior fits the present task.

The full reading is written to the hearth path printed by the guide. It retains
every candidate, its source line, available native references, timestamps, and
unread paths. This is a static source reading, not a runtime execution trace.
Computed commands, external installations, and ignored scratch files need the
same native authoring practice; they are outside this inventory's coverage.

For each relevant observation, search the local Form source, RAG, git history,
and receipts. Use an existing native capability where it fits. Otherwise write
the smallest native equivalent, observe it against the behavior that matters,
update its callers and current instructions, and remove the former implementation.
Keep incomplete capabilities as explicit work we carry. Renaming Python, swapping
in another helper language, or dropping behavior checks does not complete a move.

Validation's phase-zero checks now run directly on fkwu. Their native bands keep
the live-input and planted-failure cases. Historical comparisons with the six
retired Python copies are in git and no longer add bits to today's verdicts.

The manifest writer is native too:
`form-run ./fkwu observe/native-op-emitter-run.fk`. It generates the flt-ops slice
from the native manifest, checks the result, and restores the original bytes when
the native check fails. The pure band is
`form/form-stdlib/tests/native-op-emitter-band.fk`; a disposable filesystem witness
is `observe/native-op-emitter-witness.bml`.

This guide adds no approval step or landing rule. The existing behavior checks
remain responsible for the claims we make about a replacement.

The [native fourth-arm gap lens](fourth-arm-gap-analysis.md) replaces the former
Python survey helper. It reads current vocabulary candidates with per-band
evidence; it keeps an absent survey explicit and leaves runtime repair claims
to fresh execution witnesses.
