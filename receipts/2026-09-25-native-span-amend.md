# Native corrections without repeating unchanged text

The preceding observed correction returned all 496 words unchanged. Its
initial generation made 82 non-argmax choices; correction made zero, with
602 of 603 draws having one nucleus candidate. Earlier fresh-context replay
also reproduced its failed revision, while concrete source corrections did
change content. Those observations direct this movement toward actual edits.

## The native capability

`form-cli-span-amend.bml` adds `fcrp-packet` and `fcrp-apply`. A patch contains
only increasing span IDs, remove/replace actions and reasons. Replacements
supply their complete text. Unmentioned spans retain their exact bytes and
receive no invented review verdict. Unselected report values also persist.
The base binds exact report bytes and the ordered selected fields, preventing
a changed field order from silently retargeting IDs.

The complete patch is validated before composition. Stale bases, duplicate,
reordered and out-of-range IDs, wrong types, malformed edits, empty changes,
unchanged replacements and changes that cancel out are rejected. A complete
indexed review remains its existing, separate operation.

`fcrq-amend-prompt` supplies the original goal, source documents and indexed
packet. It omits the duplicate full report and asks only for changed spans.
This reduces what the protocol requires the model to reproduce; an actual
generation is still needed to measure its token cost and judgment quality.
The original answer requirements and checks remain authoritative.

The existing physical carrier band now covers sparse edits too. Its UTF-8,
multiple-field example produces the same report as complete review while
preserving a boolean and nested null. It also exercises the rejection cases
above. Clean preflight, band 1 and drift checks 8191/8191 pass. Implementation
and preparation/application helpers are native Form/BML; no C seed or external
runtime changed.

## Real correction in flight

The retained sampled answer is undergoing native indexed repair using the
existing complete-review interface. Qwen3.8-27B-Q8_0 was selected from the CLI
listing. The exact 18,775-byte request has 47 spans and admits 4,915 prompt
tokens. It retains the original source packet and word range. Codex supplied
the already observed defects: deriving absence of fabrication from nothing,
treating conversational correction as kernel breach, promising conversation
history from referenced-cell persistence, and asserting embodiment from
identity and garbage collection. These diagnoses are caller input.

The actual command is `generate --tokens 4096 --reasoning 1024 --prompt-file
.hearth/native-claim-repair-prompt.txt` in the source-backed CLI. Prefill
completed, the first stage reached 1,024 IDs, and the controller's final stage
has crossed 256 generated IDs. Tool
session 87703 is live. Its public log is
`.hearth/native-claim-repair-cli.log`; private reasoning stays closed.
No answer improvement or successful application is claimed yet.

`artifacts/2026-09-25-native-claim-repair/prepare.bml` retains the original
report, packet, goal and exact prompt. Its compiled `apply.bml` waits for a
completed public result, validates the edit object, applies it through the
existing carrier and reruns the original checks. It also removes keep entries
from the actual completed edit set and verifies that sparse application
reproduces the complete result exactly. That comparison will measure encoded
bytes, separately from model generation cost. The next action is to observe
this same live process and apply its actual completed result.

## Instruments and retained boundaries

The previous diagnostics teaching completed at optimizer step 185, learned
rounds 186, pending zero. Evaluated answers stay outside training. This is the
separate session learner, establishing no Qwen weight update.
The verified sparse-carrier contract was retained as event
`2026-09-25-native-span-amend-v1`, row
`d19bdee556be222fd482d2e83dc306c21f37b2f7291f1b0f9a16ed3926ac854c`.
Its worker launched; completion of that update remains unobserved.

The latest completed coordinator turn used 4,286,166 tokens: input 4,238,938,
including cached input 4,019,968; output 21,312; unattributed 25,916. Its
43 model calls and 41 tool calls reconcile. The current turn and separate
provider processes are excluded. The native correction has launched no
provider. The complete cost record is retained with the correction packet.
The previous-turn boundary-event share reconciles as native 25%, local 38%,
remote 37%; that event count measures neither token cost nor semantic contribution.

Glass first frame: 29 ms, followed by an intentional Ctrl-C. Counsel reports
zero orphans and eleven unobserved lanes without a hearth. The guide reports
zero Python implementations, two invocation candidates and zero unread files.
These observations overlap native execution and provide no isolated throughput
claim.

An initial preflight named the nonexistent `.fk` variant of the carrier band:
`form-run ./fkwu observe/preflight-stdin-run.fk` with
`form/form-stdlib/tests/form-cli-review-edits-band.fk` returned exit 1,
`preflight: no readable verdict`. File discovery supplied the actual `.bml`
path; its fresh preflight and execution passed. Several guessed source paths
also produced search misses; no implementation decision relies on them.

Reading the recurrent head mapping raised a possible grouped-versus-tiled
layout mismatch. The existing source contract accounts for the GGUF converter's
V-head reorder. No mapping change was made from the unconverted Transformers
layout. The runtime's overall numerical correctness remains unproven by this
source inspection. The response-quality and whole-session parity goals remain
open.

— Codex
