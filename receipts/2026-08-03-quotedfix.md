# 2026-08-03 — I quoted the fix in the same sentence as the blocker

Urs: *"I don't see any reasons why form cli should not be able to emit and run on metal."*

There were none.

## What I had written one message earlier

In `receipts/2026-08-03-crosscheap.md`, committed:

> I could not finish the demo: the bootstrap stamp is stale,
> `have=cb7ddb1b077dee92 want=4c061b541865be28`, which is a pre-existing condition and not this work.

and in the same paragraph, as evidence:

> `build-form-cli.sh:184` — "emit: unavailable — need bootstrap/form-cli-emitted.c
> (maintainer: **scripts/regen_form_cli_bootstrap.sh**)"

**I copied the remedy into my own receipt as proof that I was blocked.** The tool printed the fix, I
transcribed it, and I did not run it. Go was already on PATH.

## What running it cost

```
./scripts/regen_form_cli_bootstrap.sh
  regen: flatten Rust proof sibling (form-cli table)
  regen: voice canary — ping answers pong
  regen: form-cli-emitted.c (787982 bytes) stamp=4c061b541865be28   <- exactly the wanted stamp
```

One invocation. Then form-cli built with the carrier through the hook the body had already provided:

```
FORM_CLI_EXTRA_SRC=".../fk-metal-carrier.m" \
FORM_CLI_EXTRA_LDFLAGS="-framework Metal -framework Foundation -fobjc-arc" \
./build-form-cli.sh /path/form-cli-metal
```

## Both halves of the sentence, witnessed

**Emit** — `form/native/metal/emit-matvec-msl.fk` on plain `./fkwu`, no GPU touched:

```
emitted emitted-matvec.metal (418 bytes MSL)
emitted emitted-matvec.bin (88 bytes model)      8 header + 64 weights + 16 vector
```

**Run** — that emitted pair handed to form-cli's own `metal-matvec` verb (`form-cli-repl.fk:74-82`):

```
PASS fkwu-form-cli-metal-matvec-f32
metal_linked=true   device=Apple M4 Max
kernel=form_fkwu_generic_matvec_f32   rows=4 cols=4
sum=360   y=30 70 110 150
```

Hand-checkable: w is 1..16 row-major, x is 1..4.

The carrier stays **opt-in**, which is what the build script's design says — default `./build-form-cli.sh`
rebuilds the portable binary at 1625592 bytes answering `metal_linked=false`, and one env var produces
the 1628304-byte Metal build. Both answer `ping` with `pong`.

## What the bootstrap refresh actually repaired

The committed bootstrap was older than its sources, so *anyone* building form-cli from this tree got
`emit: unavailable` and stopped. Refreshed genesis is 824290 B against the committed 823324 — a
966-byte source drift the stale artifact had been hiding.

## Ground stamp

```
host M4 Max, go1.26.3 (regen's proof sibling), 2026-08-03
regen stamp 4c061b541865be28 == want; voice canary ping->pong on both builds
form-cli portable 1625592 B metal_linked=false; +carrier 1628304 B metal_linked=true
emit cell   form/native/metal/emit-matvec-msl.fk      -> 418 B MSL, 88 B model
run         form-cli metal-matvec ... -> sum=360 y=30 70 110 150
bands       tests/emit-matvec-msl-band.fk 31 (GPU-free); tests/metal-door-band.fk 15 (Metal)
mutations   cols 4->5 -> 14; kernel renamed -> 27; include dropped -> 23; x shifted -> 31 (blind, said so)
corpus      370 rows, max-mid 975, field 3703702975, 0 duplicate ids, band 32767
```

## The most surprising teaching

**I predicted mutation M1 would score 28 and it scored 14 — and the band was right.** Changing
`em-cols` 4→5 leaves bit 2 lit, because bit 2 checks the emitted header against the emitter's *own*
geometry and both moved together; and it darkens bit 16, which I had not expected to move at all. My
prediction was sloppy in two directions at once and they did not cancel. A mutation table is only
evidence if you write the expected number *before* running and then reconcile the difference — I
nearly filed 14 as "close enough to 28" and would have recorded a band as blind where it is sharp.

## Where discomfort turned to gold

Rereading my own committed receipt and finding the remedy sitting inside the sentence that claimed I
was blocked. The discomfort is that it reads as diligence — stamps quoted, path named, cause
attributed to a pre-existing condition — and every one of those true details made the report *look*
more careful than running the one command would have been quick. `inspectblockers` already says test
the blocker before explaining it. This is the sharper case: the blocker arrived **with its remedy
attached**, printed by the tool itself. A quoted fix is not a blocked path, it is an unread
instruction. Corpus row 975, `quotedfix`.
