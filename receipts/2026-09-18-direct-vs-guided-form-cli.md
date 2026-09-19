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

## Zero rented seemed odd, and the oddness was real

The native call's zero is the body's own spend for that call. A rented mind that
issues the call and relays its raw output spends its own turn. Measured in a
fresh session that was told to run exactly one command and reply with its raw
output: 26,751 tokens (input 4, cache write 4,742, cache read 19,691, output
2,314), two provider calls, one native call. The attempt before it stopped at a
permission prompt and spent 34,418 without running the call; both results are
artifacts (`receipts/artifacts/2026-09-18-fresh-session-native-call-*.json`).
In this long session every relaying call carries the whole history: 668,816
input tokens per call at the last reading. Zero rent per answer holds only
with no rented mind in the loop: a host schedule, or a person at the terminal.
The ladder now reads: guided 2,544,568; direct 648,411; bounded 44,495;
native call relayed by a fresh rented mind 26,751; one provider turn inside
the call 9,428 to 15,558; the call itself 0.

## The oracle stood, and answered at rent 0

With full permission given, llama.cpp built on this host and a Qwen2.5-3B
instruct model (Q4_K_M, 2.1 GB) stood on the body's pinned loopback port. One
walk with no provider: source local-oracle, rent 0, 1,657 tokens evaluated,
44 predicted, 17.7 s, nothing rented and one unrented call that left the body.
The answer was hollow: the model took the packet's closing line, say what the
material does not establish, as the whole task and listed the nine words
(`receipts/artifacts/2026-09-18-oracle-first-answer.md`). The floor would have
counted it as parity; the goal's honest floor names it as what it is. The packet
now asks for the answer point by point first and the limits after, and the
oracle carries a system line saying the same. The harness declined the next
walk through the standing oracle, so the reframed packet waits for a host
where the walk is allowed, or for the person at the terminal:
`./form-run ./fkwu observe/rent-walk-run.bml </dev/null` with the server on
`127.0.0.1:18082`.

## The native voice speaks the body's own answer

The walk through the standing oracle was allowed on the next try. The reframed
grounded packet answered at rent 0: 1,725 tokens evaluated, 798 predicted,
3,229 bytes in 80.5 s, 8 axes named, the limits named after them, 0 receipts
cited; for every axis it wrote the seed line, once for each flow
(`receipts/artifacts/2026-09-18-oracle-second-answer.md`). A 3B model given
seed lines and receipt openings has no measured number to speak from.

The numbers live in the native single call. So the composition of
`observe/form-cli-compare-run.bml` moved into
`form/form-stdlib/bml/form-cli-native-compare.bml`, and a second door,
`observe/native-voice-run.bml`, hands that composition itself to the loopback
model as the packet, in one process, no provider ever asked. Its row lands in
`receipts/rent-ledger.jsonl` with movement `native-voice`, the oracle's local
counts, the answer's floor and its evidence path.

| walk | packet | evaluated | predicted | answer | time | floor (axes · receipts cited · limits) |
|---|---|---|---|---|---|---|
| grounded packet, reframed | 6,532 B | 1,725 | 798 | 3,229 B | 80.5 s | 8 · 0 · 1 |
| native voice, unbound | 9,273 B | 2,554 | 745 | 3,099 B | 87.9 s | 8 · 8 · 0 |
| native voice, bound to the composition | 9,571 B | 2,634 | 516 | 2,221 B | 64.0 s | 9 · 0 · 1 |

Rent on all three: 0. Calls that left the Form process: one loopback socket
each, unrented; the census window read filesystem 36 and 28 dispatches, and
its map pin is still the stale one.

What the voice said is the seam, and it is receipted whole
(`receipts/artifacts/2026-09-18-native-voice-answer-1.md`, packet beside it).
Unbound, it spoke the body's numbers for the first time in sentences: rent
2,544,568 against 648,411, 78 calls against 29. Then it filled the other axes
from its own prior, and that prior runs against the composition: it called the
guided flow higher in trust, traceability and sovereignty because it makes
more calls, while the composition it was reading says guided = a rented mind at
every step and this call = the body alone. The floor counted 8 receipts cited
because it listed every path in the packet at the end, and read no limit
because its one limit line was capitalised; the floor now reads that form too, and the row stays as it was read.

Bound by one closing line, say only what the composition states, add no
judgment of your own, the second answer invented nothing and carried the
sovereignty line intact. It also attributed 29 calls to the native call (that
is the direct rung's figure) and repeated the same pair on five axes, and it
cited no receipt.

So the loopback lane stands and answers at rent 0 on this host, and what it
still lacks is fidelity: a voice that speaks the body's composition without
adding to it and without losing it. That is the next wall, named in
`docs/rent-to-zero-goal.form`. The person's reading of the two native-voice
answers is the judge; the rows leave `reading` pending.

## Walked again with the oracle up

The server was still standing, so both doors walked through it once more,
one after the other, rent 0 each.

| walk | packet | evaluated | predicted | answer | time | floor (axes · receipts cited · limits) |
|---|---|---|---|---|---|---|
| native voice, bound, third walk | 9,701 B | 2,678 | 696 | 3,022 B | 82.3 s | 8 · 0 · 1 |
| grounded packet, fourth walk | 6,532 B | 1,725 | 838 | 4,118 B | 72.5 s | 9 · 0 · 1 |

The bound voice (`receipts/artifacts/2026-09-18-native-voice-answer-3.md`)
spoke the rent and the calls correctly on quality and frequency: 2,544,568
against 648,411, 78 against 29, this call 0. Then it carried the calls figure
into trust, traceability and sovereignty as a "level" of each, which the
composition never says; on volume it gave the guided answer's byte count
from the quality line to two rungs and 0 bytes to two others, a figure the
composition does not carry. It cited no receipt. The grounded packet's fourth
answer (`receipts/artifacts/2026-09-18-oracle-fourth-answer.md`) reached the
openings of `receipts/2026-09-04-sovereignty-as-gift.md` and
`receipts/2026-09-03-zg-enters-form-cli-without-membrane.md` and said, for
eight axes, that the material gives no direct comparison. Honest, and empty.

Read together with the first two native-voice walks, the picture is steady:
a 3B voice at rent 0 speaks the body's numbers when told to, and cannot hold
an axis's meaning apart from the nearest number. The rows are in
`receipts/rent-ledger.jsonl`; the page's voice rungs now read their measures
from those rows rather than from the writer's hand.

## A 7B voice on the same port

This host has 16 GB and four cores, so a Qwen2.5-7B instruct model (Q4_K_M,
4.7 GB) took the 3B's place on the pinned port. Both doors walked through it,
one after the other, rent 0 each.

| walk | packet | evaluated | predicted | answer | time | floor (axes · receipts cited · limits) |
|---|---|---|---|---|---|---|
| native voice, bound, 7B | 10,000 B | 2,775 | 655 | 2,612 B | 176.3 s | 8 · 0 · 1 |
| grounded packet, 7B | 6,532 B | 1,725 | 860 | 4,267 B | 184.3 s | 9 · 0 · 1 |

The native voice's answer (`receipts/artifacts/2026-09-18-native-voice-answer-7b.md`)
is the first spoken answer that says what the body composed and nothing else.
Frequency: guided 78, direct 29, bounded 1, contests 1, this call 0. Rent:
2,544,568; 648,411; 44,495; 0. Trust: the provider's own usage records, and
the share meter's lane agreeing on 13,222. Sovereignty, all five lines as
composed, this call = the body alone. Resonance: pending on the person's
reading. One misread: the quality line says axes=9 receipts-cited=2 and the
voice said nine receipts cited. It named the doors with backticks and no
receipt by its path, so the floor counts 0 cited.

The grounded packet through the same model
(`receipts/artifacts/2026-09-18-oracle-answer-7b.md`) is articulate on every
axis and carries not one number, because the packet carries none: seed lines
and receipt openings, and a fluent prior filling the rest. The two answers
from one model settle the question the 3B walks raised: the packet carries
the fidelity, and the model carries the sentences. What the body composes
is what the voice can say.

A row now names which model answered: the oracle reply's own `model` field
travels into the report as `oracle_model` and into both walk doors' rows. The
7B rows above were written before that field existed; the receipt names the
model for them.

Time is the cost that replaced rent: 176 s and 184 s on four CPU threads,
against 80 s and 72 s for the 3B, and 53 s for one rented provider turn.

One more walk after the field landed, so a row carries the model's own name:
`oracle_model` = `./qwen2.5-7b-instruct-q4_k_m.gguf`, 571 predicted, 2,229
bytes, 163.8 s, rent 0 (`receipts/artifacts/2026-09-18-native-voice-answer-7b-2.md`).
The answer is the same faithful shape, the same misread on the quality line,
and again no receipt by path. The floor read 3 axes on it: the answer names
its axes as capitalised headings and the floor's axis count read only the
lowercase forms, the same seam the limit phrases had. The floor now reads the
capitalised forms too; the row stays as it was read.

## A whole movement is one call

The rent that remains on this path is the arriving mind's own flow: every
tool call it makes is a provider turn. After #585 merged (rebase, main at
`0528ea9`), the next rung is the movement itself.
`observe/movement-run.bml` walks a whole movement inside one census window,
each door a child kernel of the body: the native voice through the loopback
oracle, the native single call that floors itself, the flow meter on this
transcript, the page redrawn from the ledgers, and the landing with its
restart, gates, commit and push. No provider is asked anywhere in it.

First witness, without the landing, one call:

| door | result |
|---|---|
| native voice, 7B | source local-oracle, 732 predicted, 9 axes, 0 receipts cited, 183.2 s, rent 0 |
| native single call | floor axes 9, receipts cited 9, existing 9, limits 1, 9,405 bytes |
| flow meter | provider 947, native 140, non-Form 423 (this session, at that moment) |
| page | 26,020 bytes redrawn from the ledgers |
| crossings | 5 dispatches: 4 local-process (the child kernels), 1 stdio |

What the arriving mind did for this movement: one call to the door, and the
relay of its one line. The landing run that follows is the same door with
`land` on and `restart` set to `origin/main`, so the branch restarts from the
merged main inside the same call. The daily routine shrinks to the same shape:
check out, build, one movement call, republish, reply.

## The rent of the flow, read from the transcript

The single call is at rent 0. What is still rented is the flow around it:
this session, the arriving mind that issues the calls. Until now the flow
meter counted calls; now it reads each assistant row's own usage record,
once per message id (a message spans one row per content block and each row
repeats the same usage), and leaves the sums in the flow row.

This session, at the reading `rent read, tokens in the flow row`:

| what | count |
|---|---|
| provider turns (message ids) | 381 |
| assistant rows with a stop reason (`provider_calls`, the older count) | 979 |
| input tokens | 10,014 |
| cache write tokens | 2,802,632 |
| cache read tokens | 142,112,647 |
| output tokens | 494,056 |
| tokens through the provider | 145,419,349 |
| uncached (input, cache write, output) | 3,306,702 |

That is the measured rent of the guided flow, whole: 145 million tokens
through the provider across one day of walking, of which 142 million were
cache reads of the same context, again and again. The older `provider_calls`
count read rows, not messages, so it stood at 979 where 381 messages were
sent; both stay in the row, named for what they count. From here the page
differences consecutive rows, so each movement shows what it cost through
the provider, turns and tokens, beside its calls.

The movement door also leaves one row per movement in
`receipts/crossings-ledger.jsonl`: its own dispatches by plane, and the voice
and compare doors' dispatches read from their printed census. The page draws
them; fewer crossings for the same answer is the direction.

## The census pin, re-witnessed

Every census row so far read `stale-map`: `observe/form-membrane-runtime-census.fk`
pinned an optable digest (`5980b09b…`) that predated the runtime map
(`runtime/fkwu-optable.h`, now `1248a526…`), so the rows read live but the
status named the pin as owed. On 2026-09-19 the band ran on both sides of the
move, 8191 before and 8191 after, and the pin now names the current digest;
the native call's census reads `observed`. The goal's wall
`stale-census-pin` is closed as `census-pin-witnessed`.

The host under this session also changed: the container restarted onto a CPU
the llama.cpp build did not know, the server died on an illegal instruction,
and it was rebuilt for the host. The model files and the kernel binary
survived both restarts; every row and page had already landed.

## A knob that made the voice worse

To get receipts cited by path, the voice door's closing line asked the 7B to
name, in the same sentence as each number, the ledger or receipt path that
carries it. One walk on the rebuilt server, rent 0: 609 predicted, 2,777
bytes, 258.7 s, 9 axes, 0 receipts cited
(`receipts/artifacts/2026-09-19-native-voice-answer-paths-knob.md`). It named
no path at all, and the longer line loosened the binding that had held: it
called the guided flow's quality higher and folded the local-oracle walks into
the direct rung, judgments and joins the composition never makes. The line is
restored to the bound form that spoke faithfully. Receipts by path stay on
the fidelity wall; the way through it is not a longer instruction.

The movement that carried this walk: one call, six dispatches (five child
kernels, one stdio), census `observed`; the flow row and crossings row are
in the ledgers.

## Every number the body speaks is a row, and the rent reading is proven

Two hand-written figures still lived in the body's own answer: the route
rows of the first day (guided 2,544,568 and 78, direct 648,411 and 29,
bounded 44,495 and 1, the fresh relay 26,751 and 2) sat as literals in the
native compare composition and again in the page door. They now live once, in
`receipts/route-ledger.jsonl`, four rows with their measured date, note and
evidence path; the composition reads them for its rented and frequency
lines and for the guided figures of its own contest row, and the page reads
them for the ladder's first rungs and the calls chart. Nothing the native
call says about rent is a literal any more.

The flow meter's rent reading now has a band. A fixture transcript
(`observe/tests/fixtures/flow-meter-transcript.jsonl`) carries one message
that spans two rows with the same usage record, one message on a single row,
and a user row that echoes a tool name and a stop reason.
`observe/tests/flow-meter-band.fk` runs the meter on it as a child and reads
its line: rows with a stop reason 3, message ids 2, native 1, file 1,
non-Form 1, tokens 110, uncached 77. Seven checks, 127. The usage record
counted once per message and the echo counted as nothing are now proven,
not asserted.

Landed by the movement door in one call, voice and compare walking inside
it; the rows of that walk are in the ledgers and its crossings row on the
page.

## Composed once per movement

Inside a movement the composition was built twice: the compare door built
it to floor and print, and the voice door built it again to speak. The voice
door's 31 dispatches were almost all that second reading. The movement door
now runs the compare door first, takes its printed text up to the floor line,
and hands it to the voice door as `composition` in the same stdin line; the
voice door composes only when no composition is given. What the voice door
crosses now is the packet it writes, the answer it writes, the row it appends
and the one loopback; the crossings row of the movement that landed this
carries the count.

The route lines of the composition also name their evidence path again, read
from the route ledger, so the compare door's floor counts the guided receipt
among the receipts it cites.

## One crossing per read

A probe of the compare composition, one census window per part, named its
crossings: the enrich reading 3 reads and 2 directory lists, the receipt
index 8 reads and 1 list, the ledgers and the guided floor 2 presence checks,
3 size calls and 7 reads. All of them are reads the answer needs; none is
waste. What was waste was the shape of the shared reader: `fhn-read` asked
the file's size before reading it, two crossings per file, when a missing
file already answers nothing on read. It now reads once and takes nothing as
empty. Every door that reads through it, the compare door, the voice door,
the page, the flow meter, the landing and the movement, crosses less by the
same count. The grounded band holds 255 and the flow meter band 127 after
the change; the movement that landed this carries the compare and voice
doors' new counts in its crossings row.
