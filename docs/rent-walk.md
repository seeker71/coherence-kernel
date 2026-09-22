# The body walks the rent goal itself

`observe/rent-walk-run.bml` answers the pinned enquiry of
`docs/rent-to-zero-goal.form` through the grounded synthesis door and appends
one row to `receipts/rent-ledger.jsonl`. No rented mind stands between the
doors. With empty stdin the provider is not asked, so an unattended walk
cannot spend rent by itself.

The order of voices inside one walk: native grounding (an `enrich` reading
plus a dated receipt index), then the registry's answer model in the current
`fkwu` process, then an optional provider only when explicitly allowed. The
local path uses model weights as data and the existing in-process Metal carrier;
it has no model-server dependency. Absent local capability leaves an observed
reason. Omitted provider permission keeps the request native-only.

```sh
./form-run ./fkwu observe/rent-walk-run.bml </dev/null
printf '%s\n' '{"movement":"nightly","provider":{"allowed":1}}' | ./form-run ./fkwu observe/rent-walk-run.bml
```

The body walks on its own schedule on this Mac. `observe/scheduled-walk.bml` brings
its checkout to the branch head, builds the kernel and the Metal carrier when they
are stale, and makes the one movement call; `docs/launchd/earth.hati.rent-walk.plist`
runs it at 03:30 from the walk checkout `/Users/ursmuff/source/coherence-kernel-walk`,
a worktree that owns the branch. No rented mind is in the movement, so it carries no
transcript and leaves no flow row; the rent ledger row and the landing are its witness.

```sh
cp docs/launchd/earth.hati.rent-walk.plist ~/Library/LaunchAgents/
launchctl bootstrap gui/$(id -u) ~/Library/LaunchAgents/earth.hati.rent-walk.plist
launchctl kickstart -k gui/$(id -u)/earth.hati.rent-walk   # walk now instead of waiting for 03:30
```

Each row carries when, the movement's name, which voice answered, rent
tokens, the oracle's local token counts, receipts in the packet, sizes,
timings and the answer's evidence path. The answer text never enters the
ledger. The first row, 2026-09-18, walked with no oracle standing and the
provider not asked: rent 0, source none, 8 receipts in a 6,330-byte packet,
grounding 312 ms.

The grounded door itself is `observe/form-cli-grounded-synthesis-run.bml`;
its authority is `form/form-stdlib/bml/form-cli-grounded-synthesis.bml`.

## The single native call

`observe/form-cli-compare-run.bml` answers the pinned enquiry with nothing
outside the body: the ledgers' rows, the enrich seeds, the dated receipt
index, and its own census window. Provider 0, oracle 0, calls that left the
body 0. The arriving mind runs it once and relays the raw output; that is
the single-call flow the parity goal measures against the guided one.

```sh
./fkwu observe/form-cli-compare-run.bml </dev/null
```

## Nothing is spent twice

Two places used to do the work and then do it again. Neither does now.

The adapter lane decides before it computes: opening the weights and counting
the packet's IDs costs a memory map, a forward pass costs minutes, so the
chunker is asked first whether any slice of this packet fits the device. A lane
that cannot carry says `no-slice-of-this-packet-fits-the-device` before a single
token is computed, and what follows that is the walk's first inference, not a
second.

The grounded door's repair turn is the caller's word: `provider` takes
`{"allowed":1,"repair":1}`, and `repair` defaults to off. A thin first answer is
observed and reported either way — `answer_thin` and `repair_allowed` are in the
report beside `provider_attempts` — but a second rent is only spent when it was
asked for.


The allowance is a checkpoint rather than a ceiling: at `max_reply_tokens` the
lane asks whether its own tail has begun to repeat, ending there if it has and
growing the allowance toward the sentence's end if it has not. What was actually
spoken is `oracle_tokens_predicted` in the row, and it can stand well above what
was asked for — on 2026-09-22 a walk asking 320 spoke 1,322 and finished at its
own end.
## The route rows

`receipts/route-ledger.jsonl` holds the four route rows of the first day, guided,
direct, bounded and the fresh rented relay, each with its measured date, rent,
calls that left the body, note and evidence path. Usage and crossing scopes
travel with each row. The guided row covers a partial turn through call 21;
the other recorded rents cover their main conversation model.

The native comparison also reads each available `usage_evidence` result with
`form-cli-provider-model-usage.bml`. It sums all reported models, including
auxiliary models, and reports cached input, uncached input and output. Missing
or invalid counters leave the total unobserved; observed zero stays zero.
The result's model usage does not establish auxiliary call counts or the cost
of the surrounding coordinating session. Historical `rent_tokens` stay at their
declared scope; the page's route plot continues to read those recorded values.

The guided answer reference is the original receipt from commit `1736af3c8`,
retained at `receipts/artifacts/2026-09-18-guided-first-answer.md` and checked
against its SHA-256 before comparison. The native composition and contest use
that same reference. Later additions to the living receipt have their own
scope. Output volume reads the original direct and bounded answer artifacts
and the original guided receipt; the guided scope includes its measurement
and implementation account. Missing output artifacts remain unobserved.

## The native voice

`observe/native-voice-run.bml` hands the native single call's own composition
(`form/form-stdlib/bml/form-cli-native-compare.bml`, the same text the compare
door prints) to the existing in-process model session. No provider is asked.
The default model is the registry row with role `answer`; optional `model`,
`context` and `max_reply_tokens` select a registered model and bounded resources.
Defaults are 12,288 context positions and 1,536 generated IDs. The compact
`knowledge-query` profile carries the supplied composition; generated text
does not execute tools or enter training.

With `"lane":"lora"` the voice speaks through the body's own native 3B on this
metal wearing the adapter it was taught with, `adapter` naming another when the
caller has one. The row then carries `native_generation.lane` and the adapter's
path beside the local counts, so a walk says which weights and which teaching
spoke.

Only stopped generation with a valid stream and successful release becomes an
accepted answer. Each job retains raw text, original generated IDs, completion,
release and the artifact path, including for unfinished output. The organ's
health event names missing resources and the next inspection. One row lands in
`receipts/rent-ledger.jsonl` with source `fkwu-model-session`, local counts,
the answer's structural floor and its evidence path. Legacy `oracle_*` columns
carry these local counts; `native_generation` states their source explicitly.
Earlier rows keep their original external-server attribution.

The grounded synthesis door's legacy `oracle` option now selects this same
local path. Its provider fallback remains separately attributed. Completion,
release and a structural floor still leave the answer's quality to be examined.

```sh
./form-run ./fkwu observe/native-voice-run.bml </dev/null
```

## The flow, read by the body

`observe/flow-meter-run.bml` reads the arriving mind's transcript and counts
its provider calls, its native `./fkwu` calls, and every call that left the
body by kind: host commands, file tools, outside tools, other. User rows are
skipped, so nothing a tool echoed can count as a call. It also sums the
transcript's own usage records once per message id: input, cache write, cache
read and output tokens, with `tokens_total` and `tokens_uncached`, and counts
`provider_turns` by message id beside the older `provider_calls` row count.
`observe/tests/flow-meter-band.fk` proves the reading on a fixture transcript:
usage counted once per message id, a tool echo in a user row counted as nothing;
its value is 127 when all seven checks hold.

```sh
echo /path/to/transcript.jsonl | ./fkwu observe/flow-meter-run.bml
```

## The contest walks the same way

`observe/parity-contest-run.bml` is the measurement of
`docs/single-call-parity-goal.form`: one form-cli call on the pinned enquiry,
the structural floor on its answer and on the guided reference, rent and
non-Form calls, verdict flags, one row in
`receipts/parity-contest-ledger.jsonl`. The same stdin shape applies, and
empty stdin asks no provider.

```
0 9 * * * cd /path/to/coherence-kernel && printf '%s\n' '{"movement":"daily","provider":{"allowed":1}}' | ./form-run ./fkwu observe/parity-contest-run.bml
```

The person records `reading` in the row after reading both answers; the
door leaves it `pending`.

With a movement name on a second stdin line, the meter also appends its
reading as one row to `receipts/flow-ledger.jsonl`, so a movement reads its
own flow and leaves the row in the same call:

```sh
printf '%s\n%s\n' /path/to/transcript.jsonl "movement name" | ./fkwu observe/flow-meter-run.bml
```

## One door, any enquiry

`observe/form-cli-ask-native-run.bml` is the native single call for any
enquiry: one line on stdin, the enquiry's own longer words as terms, the
enrich seeds, receipts by name, the body's latest measured rows, and its own
census. Provider 0, oracle 0, calls that leave the body 0.

```sh
echo "your enquiry, one line" | ./fkwu observe/form-cli-ask-native-run.bml
```

## Land in one call

`observe/land-run.bml` closes a movement from one JSON line on stdin: files
to write whole, texts to append, a witness command to show, the paths to
land, the commit subject and body. The drift gates run inside it as a child
kernel and a refusal lands nothing; host git adds, commits and pushes the
current branch to its own name. Every crossing to a child process is counted
in the call's own census and printed. A small movement is one call. Exact edits travel in the same line (`edits`: path, old, new; an old that is absent or repeated holds the landing), and so does the restart from `main` after a merge (`restart`).

```sh
printf '%s\n' '{"append":[{"path":"docs/x.md","text":"..."}],"witness":"","paths":["docs/x.md"],"subject":"...","body":"..."}' | ./fkwu observe/land-run.bml
```

## A whole movement is one call

`observe/movement-run.bml` walks a whole movement inside one census window:
the native voice through the in-process model, the native single call that
floors itself, the flow meter on the arriving mind's transcript, the page
redrawn from the ledgers, and the landing with its restart, gates, commit and
push. Every door is a call in the body's own process — the composition never
travels through a pipe and the model is admitted once — so a movement crosses
no membrane of its own. What crossings remain are the landing's: git, and the
drift gates in their own fresh kernel, so a warm image cannot vouch for the
tree. The door also leaves one row per movement in
`receipts/crossings-ledger.jsonl`: its own dispatches by plane, with the voice
and compare columns at zero now that they are counted in the movement's own
window. The arriving mind makes one call and relays one line. Every field is optional:
`voice`, `compare` and `land` default to 1, `transcript` and `restart` to
empty, `paths` adds files beyond the ledgers and the page, `subject` and
`body` name the commit.

```sh
printf '%s\n' '{"movement":"nightly","transcript":"/path/to/transcript.jsonl"}' | ./fkwu observe/movement-run.bml
```

## The body draws its own progress

`observe/rent-ladder-page-run.bml` reads the contest, rent and flow ledgers and
fills `docs/rent-ladder.template.html` at its data mark, writing
`docs/rent-ladder.html`: the rent ladder, calls that left the body, the
session's flow, calls per movement as differences between flow rows, and the
straight path. No provider, no oracle, nothing leaves the body. Republish the
rendered file to the same page and the picture follows the rows.

```sh
./fkwu observe/rent-ladder-page-run.bml </dev/null
```

## The walk has a keeper

`host/launchd/earth.hati.coherence.rent-walk.plist` hands the movement to the
host's own scheduler. No shell reads any of it: the request is a file the
repository owns (`host/launchd/nightly-movement.json`), handed in on stdin, and
the body's own kernel does the rest — compose, speak through the adapter lane,
redraw the page, run the drift gates, commit, push. It fires at 03:30 and is
not run at load, so a reboot does not start a walk nobody asked for.

```sh
cp host/launchd/earth.hati.coherence.rent-walk.plist ~/Library/LaunchAgents/
launchctl bootstrap gui/$(id -u) ~/Library/LaunchAgents/earth.hati.coherence.rent-walk.plist
launchctl kickstart gui/$(id -u)/earth.hati.coherence.rent-walk   # walk once now
```

Its whole account is `.hearth/rent-walk.log` and the rows the movement writes.
A push that cannot fast-forward leaves the landing held with the commit standing
locally, which is honest and waits for a hand; a night that does not fire leaves
no row, and an absent row is the nothing receipt.

The plist names this checkout by absolute path, as launchd requires. A different
checkout needs those three paths changed.

## The daily walk on the Mac

[`docs/local-walk-prompt.md`](local-walk-prompt.md) gives the direct native
command and its operating requirements. A host job can invoke that command
without a rented assistant. The movement's JSON line carries `health`,
`landing` and `push`; inspect each outcome. `landed` means the push reached
origin, while answer quality remains a separate observation. The command
redraws the local page; publishing it elsewhere is a separate action.
