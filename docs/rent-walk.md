# The body walks the rent goal itself

`observe/rent-walk-run.bml` answers the pinned enquiry of
`docs/rent-to-zero-goal.form` through the grounded synthesis door and appends
one row to `receipts/rent-ledger.jsonl`. No rented mind stands between the
doors. With empty stdin the provider is not asked, so an unattended walk
cannot spend rent by itself.

The order of voices inside one walk: native grounding (an `enrich` reading
plus a dated receipt index), then the loopback oracle the body already speaks
to on `127.0.0.1:18082` (llama.cpp `/completion`, ChatML envelope) at rent 0,
then one provider turn only when the request allows it.

```sh
./form-run ./fkwu observe/rent-walk-run.bml </dev/null
printf '%s\n' '{"movement":"nightly","provider":{"allowed":1}}' | ./form-run ./fkwu observe/rent-walk-run.bml
```

A host schedule is one line; the body's landing cadence carries the row to
origin:

```
*/30 * * * * cd /path/to/coherence-kernel && ./form-run ./fkwu observe/rent-walk-run.bml </dev/null
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

## The native voice

`observe/native-voice-run.bml` hands the native single call's own composition
(`form/form-stdlib/bml/form-cli-native-compare.bml`, the same text the compare
door prints) to the loopback oracle as the packet, in one process, and no
provider is ever asked. The voice speaks from the body's measured rows rather
than from seed lines. One row lands in `receipts/rent-ledger.jsonl` with
movement `native-voice`, the oracle's local counts, the answer's floor and its
evidence path. The row names the model that answered (`oracle_model`, the
server's own name for it), so a port that carries different models over time
stays traceable row by row.

```sh
./form-run ./fkwu observe/native-voice-run.bml </dev/null
```

## The flow, read by the body

`observe/flow-meter-run.bml` reads the arriving mind's transcript and counts
its provider calls, its native `./fkwu` calls, and every call that left the
body by kind: host commands, file tools, outside tools, other. User rows are
skipped, so nothing a tool echoed can count as a call.

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
