# Health projection observation inputs

These files retain the local parity and resident observations, not production
implementations. The public lifetime witness is
`observe/form-health-view-witness.bml`; it takes a new absolute directory on
stdin and prepares its own local write/readback events.

`health-projection-before.bml` is the exact raw capture of
`d02faa1c6:form/form-stdlib/bml/form-core-health.bml`. The preparation cell
renames only its namespace into `health-projection-reference.bml`. For replay,
restore those files under `.hearth/` and run the preparation cell there.
The contract takes `run` on stdin and uses the retained failed-child event path
from `.hearth/live-process-care-report.json`. Its input is 44,397 bytes with
SHA-256 `91200297104504482f60d7b4194dc1f9803d744b936a84cc73380cdb45d0853d`.
The resident observer also requires the private helpers named in its prelude
chain and a real ancestor session state for its isolated-home observation.
It is a retained local observation, not a standalone replay fixture.

The first resident run completed its command, ownership and cleanup checks but
refused the clean-stderr assertion. `resident-cache-warning.txt` retains the
source-identity cache diagnostic and automatic rebuild. An unchanged repeat
passed with empty CLI and Glass stderr.

`extract-care-witness-common.bml` is the one-shot native edit used to move shared
utilities into a passive prelude. Its preflight executed the edit; the explicit
second invocation therefore stopped with `shared witness extraction anchors
missing`. Inspection confirmed the completed move, and both public witnesses
were preflighted and executed successfully. This historical edit is not a
runtime dependency and must not be replayed against the already edited source.

`source_states: 6` in the contract report counts six cases, including populated
and empty caught-up sources. There are five distinct state strings.
