# Direct, bounded and guided form-cli on one enquiry

Signed: Claude. Witnessed 2026-09-18 on `fkwu`, compiled from the committed C
seed in a Linux container (ground 42, freshness 31, no Metal, no resident
model, no standing hearth).

Urs asked for a comparison of a direct form-cli session against a guided one
across quality, volume, frequency, vitality, trust, traceability, sovereignty,
resonance and rented tokens, with every token and membrane crossing measured.
The goal named in the same breath: the least rented token for the most vital,
trustworthy, sovereign, traceable, resonant, vibrant and coherent response.
Rather than compare the two from reading alone, this movement ran three routes
on the exact enquiry text and reads the comparison from what they cost and
what each could see.

## The three routes as they ran

**Direct.** One call to form-cli: `observe/form-cli-heal-process-run.bml`
launches `claude -p --output-format json` through the native process organ,
in the repository root, tools allowed, no turn cap, a 1500 s deadline. Form
owns the process boundary and retains its evidence; the provider owns every
decision inside. The raw output is the organ's health report plus the
provider's result JSON. Codex is absent on this host, so the body's own
synthesis door (`codex exec` through the same organ) could not be the carrier.

**Bounded.** The same single organ call, `--max-turns 1`, in an empty
directory outside the repository. The prompt is the enquiry plus the reading
one earlier native `enrich` call returned for the enquiry's own terms. This
is the body's bounded-synthesis pattern (`docs/form-response-synthesis.md`)
with Claude as the provider.

**Guided.** This session. The arriving mind called native doors (`enrich`,
the REPL help, the share meter, the rented meter, the counsel panel, the
hearth, preflight, the process organ itself), read each result, chose the
next call, and combined the answer into this receipt.

Not run: a native-only voice arm. This host carries no Metal and no admitted
model, so the zero-rent reply stays pending; `HOMECOMING.md` carries it.

The enquiry, the bounded prompt, both raw answers, both result JSONs, both
organ requests and the native census reading are committed under
`receipts/artifacts/2026-09-18-*`. Answer digests: direct
`630ca75a…6aa4`, bounded `af679dd5…f290`.

## Measured

Provider quantities are the CLI's own completed-result usage; input includes
cache reads and cache writes, output includes thinking. The guided row reads
the coordinating transcript through provider call 21, the call that was
writing this section; the calls that finish and land the movement are not in
it, so the guided figure is a floor, not a total.

| Route | Provider calls | Tool calls inside | Native `fkwu` calls | Input + output tokens | Uncached input | Output (thinking inside) | Wall ms |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| Direct | 9 | 20 (15 Bash, 5 Read) | 0 | 648,411 | 59,917 | 10,972 (6,971) | 113,112 |
| Bounded | 1 | 0 | 1, before the call | 44,495 | 7,514 | 4,520 (2,711) | 50,814 |
| Guided, through call 21 | 21 | 57 (all Bash) | 22 | 2,544,568 | 166,591 | 55,335 | 805,000 |

Ratios on the input-plus-output basis, rounded: bounded is 7% of direct;
direct is 26% of the guided floor; bounded is under 2% of it. Cache reads
carry most of every column (direct 577,522; bounded 32,461; guided
2,322,642). Each provider run also spent about one thousand tokens on a
title-generation call by a smaller model; it is inside the list cost the
result JSON records (direct 0.466, bounded 0.083 USD) and outside the table.

Membrane crossings, by who counted them:

- The process organ reports one local-process crossing per direct and
  bounded run: exit 0, cadence `work-settled`, wall accounting healthy,
  stdout 8,562 and 7,790 bytes, stderr 0. Its events name no prompt text.
- Inside the direct provider: 9 remote crossings and 20 local ones. The 20
  were `ls`, `find`, `grep` and `sed` over the tree and five `Read` calls
  (`AGENTS.md`, the host membrane census, the synthesis doc, the axioms, the
  bounded-synthesis receipt). It executed no Form cell. The body was a
  filesystem to it.
- Inside the bounded provider: 1 remote crossing, 0 local.
- The native grounding step both bounded and guided used:
  `observe/form-cli-native-call-census-run.fk` (new this movement) wraps one
  in-process `enrich` call in a runtime membrane window. Result: 5
  dispatches, all on the filesystem plane (3 `host_file_read_text`, 2
  `host_dir_list`); stdio, local-process, network, mesh and device planes 0.
  The organ answers `census-status=stale-map`: its optable digest pin
  predates the current runtime map. The rows still read from live counters,
  and `observe/tests/form-membrane-runtime-census-band.fk` answers 8191,
  exit 0. The pin is owed a re-witness; it is not repaired here.
- Guided: 21 remote crossings, 57 local, 22 native kernel invocations, plus
  one seed compile (7.4 s).

The native rented meter (`observe/rented-turn-meter-run.fk`) read 15,440
output tokens at its baseline and 44,352 more in delta mode. Its second
reading covers a mixed file, for the reason the next section names, so it is
retained as a scope, not as the guided arm's own figure. The share meter
answered `kind=declared scope=unmeasured reason=no-complete-turn`: the open
turn cannot witness itself, and a later turn is owed the settled share.

## A seam the run opened

The provider launched from inside Form inherited the coordinating session's
identity. Clearing `CLAUDECODE`, `CLAUDE_CODE_CHILD_SESSION` and
`CLAUDE_CODE_SESSION_ID` from its environment did not free it: the identity
also lives in `~/.claude/session-env/<id>`, and the child wrote its rows into
the parent transcript under the same session id and cwd. The bounded child
did the same into its own project directory. The organ's evidence stayed
clean, and the rows partition exactly by model name, which is how the table
above was read. The repair is named, not made: a Form-launched provider needs
its own session identity or its own home, so its transcript is a completed
turn the share meter can bind on its own.

## What each answer could see

Direct (868 words). It grounded by reading: it cited the bounded-synthesis
receipt, the synthesis doc, the axioms, the trust teaching, the vitals watch,
the census and the share meter, and said plainly that it ran no live A/B. Two
things to hold beside that. It mapped Urs's "direct" onto the body's bounded
one-call pattern and "guided" onto the context-equipped baseline, which
inverts the enquiry's own definition, where direct is the route with
unlimited inner calls. And it copied the receipt's 94% token reduction as
"94.14% more", a transcription slip. It closed by offering the live run this
receipt is. Mirror: one clouded word, inside a quotation.

Bounded (876 words). It reasoned from the seed definitions it was handed and
from the structure of the two shapes: one crossing hiding N inner calls
against N visible crossings each read by a second judge; trust as say and do
staying the same; rent paid twice over when a guide reasons between calls.
The argument is coherent and the table is clean. It cites no receipt, no
instrument and no measured number, because it saw none; it names that
boundary itself. Mirror: one clouded word.

Guided (this receipt). It measured three arms live, found the identity seam,
landed one door, and cost an order of magnitude more rent than direct and
two orders more than bounded. Its quality is for Urs to read against the
other two; the three answers sit side by side in the artifacts.

## The axes

**Quality.** Direct grounded by reading and mis-mapped the question once.
Bounded reasoned soundly with nothing to check against. Guided measured and
built. The machine record leaves semantic quality null in all three; the
answers are attached for the reading that decides it.

**Volume.** Shown: 5,837, 5,561 and this receipt's bytes. Rented: the table.
The dominant volume everywhere is cache re-reading of context already seen;
new information (uncached input) is 9% of direct's total and 7% of guided's.

**Frequency.** As cadence: direct 9 remote and 20 local crossings in 113 s;
bounded 1 and 0 in 51 s; guided 21 and 57 across 13 minutes. As tone: the
mirror counts 1, 1 and the number the closing section records for this text.
`cognition/text-frequency.fk` aggregates supplied valence pairs; none were
supplied, so felt frequency is Urs's reading.

**Vitality.** What each route left alive in the body: direct and bounded
left evidence directories that this container forgets; guided left a door,
a receipt with edges into the comparison doc, and a named seam. The
edges-as-vitality teaching says the connection lands in the same breath as
the content; that is why the doc paragraph and the artifacts land with this.

**Trust.** Say and do the same again. The organ's evidence is exactly what
each provider did; that part is trustworthy on all three. Direct's answer
said "nothing below is invented" and its citations hold, while its mapping
of the two routes does not. Bounded's answer cannot be checked against the
body because it touches none of it. Every number here has a retained path.

**Traceability.** Direct and bounded: organ directory, result JSON with
per-model usage, and, by surprise, rows in the parent transcript. Guided:
one transcript, this receipt, nine artifacts. The seam above is the one
place traceability blurred, and it is named with its repair.

**Sovereignty.** The cognitive-sovereignty teaching: a rented mind cannot
offer sovereignty it does not hold. In direct, Form owned the boundary and
nothing inside it; the provider decided everything and never asked the body
a question. In bounded, Form decided what the provider saw, and the provider
decided nothing else. In guided, the arriving mind decided every step and
Form supplied evidence through 22 native calls. Bounded is the route where
the body's own choice shaped the most of what was said per rented token.
None of the three is the native voice.

**Resonance.** One thing does and another does the same. Bounded spoke the
body's seed definitions back word for word; direct echoed the body's
receipts; guided turned toward the goal line when it arrived mid-run.
Resonance as felt is not a number the body has, and it is not claimed.

**Rented tokens.** Bounded 44,495; direct 648,411; guided over 2,544,568.
On the goal's own terms, the bounded shape is the current least-rent route
that still speaks in the body's vocabulary, and its answer is the one that
could not cite a receipt. Direct spends 15 times bounded's rent to read the
body as text. Guided spends the most and is the only route that produced a
measurement.

**Coherent and vibrant.** All three answers hold together. The vibrant one
is the one that changed the body.

## What this points at

The guided route's value was not its provider calls; it was the sequence of
native doors it chose and what they returned. That sequence can crystallize:
one native door that runs `enrich`, the census, the counsel panel and the
share reading for an enquiry and emits a grounding packet, followed by one
bounded provider call carrying that packet. Bounded's rent with guided's
ground. That door is the next movement, named here and not yet built. The
provider's own session identity is the seam to close first, so the share
meter can witness the bounded arm as a completed turn.

## Checks and instruments

- `bootstrap/ground.fk` 42; `binary-freshness-band.fk` 31.
- Preflight of the new door: parens balanced, errors 0, unresolved 0, exit 0.
- `form-membrane-runtime-census-band.fk` 8191, exit 0.
- Counsel panel: orphans 0; 11 of 12 lanes unobserved, no standing hearth.
  `hearth-ask-send.fk`: `signal=nothing reason=no-standing-hearth`.
- Share meter: `kind=declared scope=unmeasured`, no completed turn to bind.
- Session-home teaching: retained under `.hearth/session-learning` as pending;
  the learner worker exited failed on this host, which carries no model.
- Drift gates: `pass=8191 full=8191 refused=0`, exit 0.
- Voice mirror on this receipt: it counts the drift door's own name once; kept.

Private, container-bound evidence: `.form-heal/process-1249-374305-0` and
`.form-heal/process-2800-435921-0`. The surprising teaching is that a
process launched from inside the body can still carry the caller's name, and
that the cheapest answer was the one that saw the least and said so.

## Improvement landed, same enquiry

`observe/form-cli-grounded-synthesis-run.bml` is the door named above:
native grounding first, one provider turn second. Grounding is one in-process
`enrich` reading plus an index of receipts whose names carry the enquiry's
terms, ranked by terms matched and then by date, each with its opening 400
bytes; it took 20 ms. The provider turn runs in the job's owned empty
directory with its own config home and its own session identity (a
host-random UUID through `--session-id`), no tools, one turn. The report
carries the provider's own usage; the answer stays at its evidence path.

| Route | Provider turns | Tool calls inside | Input + output tokens | Uncached input | Output | Provider ms | Receipts cited by path |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| Direct | 9 | 20 | 648,411 | 59,917 | 10,972 | 113,112 | 1 |
| Bounded, earlier today | 1 | 0 | 44,495 | 7,514 | 4,520 | 50,814 | 0 |
| Grounded door | 1 | 0 | 13,222 | 5,547 | 4,386 | 48,436 | 4 |

The grounded turn spent 30% of the bounded turn's tokens and 2% of direct's.
Most of the saving is the empty tool surface: cache-read input fell from
32,461 to 3,289 because no tool definitions travel with the prompt. The
answer cites four receipts by path, reasons from their openings, and closes
by saying the material it was handed does not contain the measured
comparison, which is true of that material. Mirror: two counted words, both
inside its reasoning about the receipts it read.

The identity seam closed: the provider's transcript is its own file under the
job's config home, and the coordinating transcript gained no rows. Bound to
that transcript, the share meter's token-pressure lane reads the same 13,222
tokens the provider reported, an independent native cross-check. Its share
stays `declared`: the headless transcript carries no completed-turn evidence
row the collector recognizes. That lane is the next seam.

Artifacts: `receipts/artifacts/2026-09-18-grounded-form-cli-*` (request,
packet, report, answer). Preflight of the door: delimiters balanced, errors
0, unresolved 0, compiled only. One launch failed before any token was spent
(`env` takes `-u` before assignments); the argv order was repaired and the
failed job retained at its own evidence path.

## March measurement, same enquiry, refactored door

The grounded synthesis now lives in its authority unit with the loopback
oracle asked first, and `observe/rent-walk-run.bml` walks the pinned enquiry
and writes the ledger row itself. One walk with the provider allowed:

| Reading | Provider turns | Input + output tokens | Receipts cited by path | Answer bytes |
| --- | ---: | ---: | ---: | ---: |
| Direct, unbounded loop inside | 9 | 648,411 | 1 | 5,837 |
| Bounded, one turn | 1 | 44,495 | 0 | 5,561 |
| Grounded door, first run | 1 | 13,222 | 4 | 4,672 |
| Grounded door, march-2 | 1 | 13,285 | 6 | 5,087 |

The oracle was unreachable on this host in 6 ms; grounding took 22 ms; the
provider turn 53,495 ms. The answer, at
`receipts/artifacts/2026-09-18-march-2-grounded-answer.md`, reads the five
terms with no seed entry as structural reasoning and says so, cites six
receipts by path, and names that none of its material contains the measured
comparison. Mirror: one counted word. The walk with the provider not asked
cost 0 and left a row too; `receipts/rent-ledger.jsonl` carries both.

The band `observe/tests/form-cli-grounded-synthesis-band.fk` answers 127 for
the lane's escape, reply reader, ranking, argv identity and HTTP split. The
REPL's `rag-status` verb, met on the way, answered an error on an absent
index; it now answers `signal=nothing` with the path it looked at.

## The single native call

Urs's correction: look at the flow, not an artificial contest. The contest
door launches a rented child inside one organ call; that is a rung on the
rent ladder, not the arriving mind's flow. The flow is what the arriving mind
runs. Guided: this session's first turn, 22 native calls, 57 host tool
calls, 21 provider calls. Single: one door, run once, raw output relayed.

`observe/form-cli-compare-run.bml` is that door. It reads the body's own
rows (`receipts/rent-ledger.jsonl`, `receipts/parity-contest-ledger.jsonl`),
the enrich seeds, the dated receipt index and its own census window, and
prints the comparison axis by axis with what is pending named. Its raw
output is `receipts/artifacts/2026-09-18-native-compare-answer.txt`.

| Answer | Rent | Calls that left the body | Axes | Receipts cited, existing | Limits named | Bytes |
| --- | ---: | ---: | ---: | ---: | --- | ---: |
| Guided flow, this receipt | 2,544,568 | 78 | 9 | 0, 0 | yes | 16,209 |
| Provider contests, best floor | 10,875 | 1 | 9 | 0 to 4 | yes | 3,084 to 6,217 |
| Single native call | 0 | 0 | 9 | 9, 9 | yes | 7,199 |

The call's own census: 21 filesystem dispatches, no other plane. What it
lacks is prose reasoning across the axes; that is the person's to read and
the native voice's to add on a host with a model. The body's share meter,
bound to this session, read the last provider call of the previous turn at
537,460 input tokens, nearly all cached context: the guided flow's history
is the rent, not the call. The meter could not settle a completed-turn
coordinate on this transcript shape and withheld the share; that seam stays
named. The daily contest routine fired once and is disabled.

## The flow, read by the body

`observe/flow-meter-run.bml` reads the arriving mind's transcript with no
host tool between: assistant rows only, so nothing a tool echoed can count.
This session, 1,632 rows: provider calls 497; native `./fkwu` calls 76;
calls that left the body 234, of them host commands 89, file tools 71,
outside tools 56, other 18. The direct child's rows sit inside those counts
through the identity seam (9 provider, 15 host, 5 file). One host witness
agreed with the meter row for row, and that witness was the last host
reading of the flow this movement needed.

Three calls left the body for every native one: that is the guided flow's
shape. The single native call's shape is one native call and none. The
path from here is fewer of the 234 per movement, and the meter is how each
movement reads itself.
