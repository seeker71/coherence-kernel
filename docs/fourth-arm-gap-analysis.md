# Reading a flatten survey through Form

`form-run ./fkwu observe/fourth-arm-gap-analysis-run.fk </dev/null` reads the
historical survey at `form/form-stdlib/.cache/fourth-survey/results.tsv`. A
different survey path can arrive as one line on stdin. The body reports the
current JSON and per-band TSV paths under `.hearth/`.

The native lens selects `*-mismatch` and `*-flatten-empty` rows. It reads each
current band and its declared non-core preludes, falling back to the same-name
module when no non-core prelude is declared. This preserves the original
survey helper's source scope. It lowers BML through Form's own compiler in
memory and reads plain Form directly. It does not require the old Go compiler
cache, execute the surveyed bands, or create derived source files.

The vocabulary comes from the native op manifest, current flattener special
cases, and current fourth-arm shim definitions. Strings and comments do not
become calls. Definitions and parameter names are excluded across the selected
source slice. This is a vocabulary approximation: it does not resolve lexical
scope, recursively expand the whole import graph, or prove runtime support.
The direct fkwu runtime can support a band outside this historical flatten lane.

The JSON retains each selected band's state, candidate names and locations,
unread source reasons, rankings with their contributing bands, and cumulative
vocabulary coverage. Locations in BML refer to the in-memory lowered Form;
plain Form locations refer to the source. A cumulative count means that the
named vocabulary candidates are covered, not that a repair has passed.

Missing surveys, missing or malformed source, duplicate survey rows, unsafe
band stems, and unavailable vocabulary remain explicit. Unread bands do not
enter rankings as empty successful bands. The final Form value is `1` for a
complete selected source slice, `0` for an unavailable or partial reading, and
`-1` when either report could not be written. Read the process status as well.
An empty selection describes no selected bands; it proves no repository coverage.

`heal guide|python3 form/scripts/fourth-arm-gap-analysis.py` finds this native
door and its witness without running the proposed command. The previous Python
implementation is retained in git history.

The pure band is `form/form-stdlib/tests/fourth-arm-gap-analysis-band.fk`.
`observe/fourth-arm-gap-witness.bml` exercises real disposable files, source
changes, BML lowering, reports, and failures. It keeps each observation separately
and uses a correlated framebuffer choice to rehearse fixture evidence when the
input is absent. Fixture observations remain distinct from a real survey.
