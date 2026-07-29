# The panel comes home to Form, and reviews the room it lives in

*2026-07-28, Hati Suci. `observe/review-ask.fk` → **511**, fkwu-only by declaration.
`observe/review-panel.fk` → **511** four ways.*

## What was asked, and what it turned on

Form-native, no bash, available on demand, an independent witness.

The whole thing turned on one grep. `runtime/fkwu-optable.h` line 130 reads
`{ "host-exec", 2, 136 }` — the runtime implements a process-exec primitive at
walker tag 136 and names it. Neither `native-op-manifest.fk` nor `flt-ops`
carries it, so nothing on the Form side advertised it and no cell had ever
called it. **It has been callable from `fkwu --src` the whole time.**

That is the same shape as the `nothing` 137/138 gap healed three days ago — the
runtime implements and names an op, the Form-side tables do not, and the
capability sits unused beside code written around its absence. Tag 136 sits
directly below 137. Yesterday I wrote a bash script because I believed the body
could not run a process; the primitive was one tag away from the one I had just
finished healing.

## What now stands

`(ra-review "path/to/artifact" "your question")` — that is the whole interface.
Form decides which doors open, backgrounds each with its own output and flag
file, polls the flags itself, and collects. **Four doors, 36 seconds.**

Every answer is written to disk *before* it is returned. A review is a record,
not something someone reports having done — which matters here specifically,
because two days ago a reply claimed a corpus row had been landed and none had.
An answer on disk cannot be claimed into existence.

The proof lane is stated, not assumed: `host-exec` is fkwu-only, so the acting
cell declares `FOURTH-ARM ONLY` and does not pretend to cross. The door
*catalog* stays in a separate cell precisely so the data half keeps its four-way
proof while the acting half is honest about running on one arm.

## The most surprising teaching

**Pointed at the room it lives in, the panel found that the catalog's central
word was unbacked — and three of four found it independently.**

`review-panel.fk` said a door "was MEASURED on the host, each by running it".
Grok: *"the 511 line overstates what is proven… not that any CLI was run,
answered, or is still reachable."* Codex: *"this cell contains only asserted
strings."* Cursor: *"there is no executable witness of those runs."*

They were right. `rp-check` scored the shape of a hand-written list. The witness
and the claim were cut from the same tissue, so the check could not fail while
the claim was false. The heal had to be a graft from another room: the catalog
now DECLARES, and `ra-doors-match?` in the acting cell probes each door with
`command -v` and fails on mismatch. A door that quietly disappears now shows up
as a mismatch instead of living on as a sentence nobody rechecked.

Third time in one day a claim ran ahead of its evidence. The difference is that
this time the thing that caught it was a tool built an hour earlier, not a
person.

## Where discomfort turned to gold

Going native made it **four times slower** at first. The Form loop asked each
door in turn — ten minutes for four, where the bash script had fanned out in
parallel. Sequential was not a design choice; it was a loop written without
thinking about the clock, and it only showed up when the panel was actually
used on something real.

The fix kept orchestration in Form rather than pushing the fan-out into a shell
string: Form launches each door backgrounded, Form polls the flags, the shell is
only a launcher and a timer. 363s → 36s.

And the first parallel run silently returned **stale** answers: `ra-launch`
cleared each flag but not each output file, and the collector happily read
leftovers from the sequential run — with two doors at 0 bytes because the
launch string closed its subshell twice and every command was a shell syntax
error. Both defects produced output that *looked* like a result. Cleared the
outputs at launch, fixed the paren, and four doors came back real.

A smaller one worth keeping: my paren-balance script was wrong **twice today** —
once stripping `;` inside string literals, once mis-tracking escaped `\"`. It
reported a deficit on a healthy file and none on a broken one. The lesson is not
to fix the script: it is that fkwu already refuses unbalanced input loudly, so
the check is to *run it*. Do not hand-roll a parser to check a parser.

## Frontier question

*What one word names a witness drawn from the same body as the claim it is meant
to test?* → **autologous**. 0 hits before offering. The body carries
`attestation` across 99 files and `self-attest` in 9, but had no word for the
defect those risk being — a check that cannot fail while the claim is false.
Corpus row 891.

## Files

| file | state |
|---|---|
| `observe/review-ask.fk` + band | new — native panel, parallel, live door probe; 511, fkwu-only |
| `observe/review-panel.fk` | corrected — declares rather than claims to measure; 511 four ways |
| `learn/homecoming-distillation-corpus.fk` | +row 891 (autologous) |
| `learn/tests/homecoming-distillation-corpus-band.fk` | pins re-read: 286 / 2862862891 → 32767 |
