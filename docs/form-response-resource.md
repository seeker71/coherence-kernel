# A provider resource owned by Form

For native grounding followed directly by one explicitly offered provider
response, use the separate [synthesis path](form-response-synthesis.md).
The repair resource below retains its failed-local-report condition.

`observe/form-cli-response-resource-run.bml` repairs a retained, failing
read-only report through one optional provider CLI process. The caller owns
the question, source documents, assertions and provider permission. Ordinary
native `code` remains local-only. Response sessions also remain native-only by
default; a manifest can explicitly offer this resource with a small
session-wide process allowance as described in
[`form-response-comparison.md`](form-response-comparison.md).

Supply one JSON object on stdin:

- `id`: a nonempty identifier containing letters, digits or hyphens.
- `assessment`: an existing `code` request with `mode=review`, `evaluation=1`,
  read-only documents, source `checks` and `report_checks`.
- `local_reports`: paths to actual retained local reports, in chronological
  order. Form freezes their bytes and rechecks the latest report. The caller
  supplies their execution provenance; a file path alone does not prove which
  model generated it.
- `provider`: `{"allowed":true,"interface":"codex-exec","max_processes":1,
  "seconds":180}`. This permission belongs to the caller, not model output.

Form first runs the source and report assertions. A source failure needs repair
before a provider request. A passing local report requires no provider. Missing
permission leaves the request local. For an eligible failure, Form sends the
question, source documents, latest report and actual failed-check observation;
it does not send expected report-check values.

The provider runs through the existing native process organ in an owned empty
directory outside the repository. It is asked for one final JSON report. Form
retains its process evidence, answer and actual reported usage, then runs the
unchanged source/report assertions. Form requires observed process completion
and release before accepting the repair.
Responses retain the process duration, name their provider origin and leave
overall semantic quality unassessed. Assessment answers are not sent to
session learning.

Process release and successful completion are separate observations. A process
that exits nonzero can still have released all its owned resources; that release
does not make its answer an accepted repair. Missing completed provider usage
remains unknown, including after interruption.

Native callers that explicitly need a provider to operate a stateful Form tool
can use `frr-execute-tool-session(root, prompt, seconds)`. It adds that caller's
owned evidence directory to the isolated workspace's writable roots through
`codex exec --add-dir`. The caller supplies the tool command and its working
directory. Ordinary `frr-execute` calls retain their existing scope. This is an
internal execution option; it does not enable provider tools in the bounded
report-repair or synthesis prompts, or enable a provider for ordinary native
code sessions.

An atomic claim keyed by the complete request and retained report bytes admits
one provider process. Repeating the same request rechecks the retained answer
and reports zero new processes. Answer and usage bytes are frozen and their
hashes checked before reuse; changed evidence does not trigger another call.
Its original usage remains attached to the
same `usage_event`; do not add it again. A claim interrupted before process
completion remains unresolved and never silently launches another provider.
Inspect its owned evidence before deciding the next action.

The deadline bounds the process lifetime, not token usage. Usage is read from
the completed CLI transcript; absent usage is never treated as zero. Provider
CLI availability is needed only when the caller explicitly offers this resource.
This repair path is one part of the wider native/session comparison, not proof
of whole-session parity.
