# The hook crosses the loop — a resident Form door between argmax and the next embedding

The stone as the 2026-08-13 receipt set it down: "the current stack does not yet call an arbitrary
Form function between exit-head argmax and the next embedding; its live footer therefore says the
default is retained." Urs asked for that stone walked — from toy to real, form-native, working,
observed — and for the bash around the walk transmuted into form shell. Both happened in one
sitting, and every witness below ran on this checkout before it was written down.

## The door, and what became of the toy

`form-stdlib/dsv4-decode-token-hook.fk` proved the LAW's shape on a four-row toy logits table.
What was missing was never the law — it was a live crossing. That crossing is now
`form-stdlib/dsv4-decode-hook-door.fk`: a RESIDENT fkwu process a decode carrier spawns once and
asks once per generation step over stdin/stdout, one keyed request line in, keyed reply lines out,
`end=1` closing each. The law inside is the swap cell's own `dtrs-select` over a window built from
the carrier's absolute deadline and the door's clock at receipt; the recipe call is measured with
`now_unix_ms` on both sides. Silence, decline, lateness, a malformed line, a dead process — each
keeps the default token, each with its state named in the reply.

The recipe crosses as three function values — `(dhd-serve idf preparef offerf)` — composed by a
two-line serve cell, so the door itself names no recipe and ANY Form recipe can stand in the slot.
That is the "arbitrary Form function" of the stone, made a value. Two recipes shipped:

- `dsv4-hook-recipe-quiet.fk` — acks nothing always. The installed default: the hook is in the
  loop and the model's token stands BY LAW, not by an absence of wiring.
- `dsv4-hook-recipe-control.fk` — the real one. The classifier earned leakage-free on 2026-08-18
  (fresh_correct 65/150 vs majority 30, fresh_leakage 0) reads a DECLARED situation and offers its
  control verdict (token 128000+class) as the answer's first token. This is the latch stone's
  intelligence moved from the adapter slot into the offer: there the trained values biased the
  exit head from a five-float file; here the same fit OFFERS, and the law decides.

The vitality on each side is a measured number or the offer does not cross. The offer carries the
class's OWN held-out-fresh diagonal as a percent of its row — for TIMEOUT that is 26/30 -> 86 —
on all six axes, so the min-floor cannot be gamed upward; a leaked fit or a 0-evidence class acks
nothing. The default carries 0 WITH ITS RADIUS SAID: vitality is unconsulted unless an admissible
offer crosses, and an offer only crosses on a declared situation, where gate 93 measured the base
head emitting token 201 and never a control token — control-task correctness 0. A recipe claiming
any OTHER task needs its own default measurement; that is a named next attention row, not a
loophole.

## Witnessed on this checkout (Linux container, no Metal, no GGUF — said plainly)

Preflight, all touched cells: balanced, 0 errors, 0 warnings, 0 unresolved, chains clean.

```
form-stdlib/tests/dsv4-decode-hook-door-band.fk   -> 4095   (12 bits, law + parse + real recipe)
form-stdlib/tests/dsv4-decode-hook-door-live.fk   -> 63     (the resident crossing itself)
form-stdlib/tests/fsh-ask-ds4-band.fk             -> 255    (the form-shell ask door's decisions)
form-stdlib/tests/ask-ds4-band.fk                 -> 511    (was 255; +256 names the hook door)
```

The sibling kernels then judged both new bands — `validate.sh` under an explicit
`FORM_ALLOW_THREE_ARM=1`, because this checkout's fourth-arm bootstrap is the darwin-arm64 seed
and its regen chain is not runnable here — and go, rust, and typescript each answered the same
4095 and 255 (`1 ok, 0 divergent`), beside fkwu's own direct answers above. Four kernels have
spoken, three of them inside the sealed table and one at the source door; registering the bands
in `fourth-arm-bands.txt` needs the table chain and belongs to the Mac.

Before agreeing, the siblings caught two real wounds, and both heals are in this change:

- **eq-on-strings in the control family.** `dct-id`, the corpus cells, and the adapter compared
  class and split STRINGS with `eq` — fkwu tolerates that, go/rust/ts refuse it at their typed
  boundary (`as_int: Str("NIL")`), so the whole earned-classifier family had been fourth-arm-only
  without saying so. Every site now speaks `str_eq`. Behavior-preservation was re-measured, not
  assumed: `dsv4-control-fresh-band` and `dsv4-control-emit-band` answer 31 and 31 after the heal,
  and the door band's pinned 26/30 -> 86 did not move.
- **a live top-level in a preluded cell.** `fsh-ask-ds4.fk` first carried its own `(fads-main)`
  call, which ran at every prelude load and dragged `read_line` — an emitted-walker organ — into
  the sibling kernels' path. The entry now lives in `fsh-ask-ds4-main.fk` (the `fsh-*-main`
  convention this body already had), and the decision cell stays pure on every kernel.

The live witness is Form watching Form: the cell host-execs ONE fkwu holding the control door,
stages three requests with real clock deadlines, and reads the law back:

```
dhd-live prepare_ms=24522          the fit + held-out eval, once, at spawn
dhd-live declared:   chosen=128003 reason=recipe-more-vital vitality=86 door_ms=1
dhd-live undeclared: chosen=201    reason=ack-nothing
dhd-live late:       state=timeout-nothing        (recipe never ran)
```

The sentence behind `chosen=128003` is the latch stone's held-out fresh sentence — "backup restore
ran out of its window before anything settled" — and 201 is the exact default gate 93 measured.
The door answered in 1 ms against the 34 ms token budget. One spawn served every request.

## The transmutation: bash into form shell

`ask_ds4.sh` was a carrier that read the body's contract and still owned the walk — grep, awk, env
wiring, a sanity subprocess, the footer. It is now a doorknob: stage keyed stdin lines, exec fkwu
on `form-stdlib/fsh-ask-ds4.fk`. The Form cell CALLS the contract functions instead of spawning
and re-parsing them, runs `dss-trip-count` in process, composes the one lane command (prompt and
situation cross as `$(cat file)` reads, never interpolated into command text), parses the lane
transcript by name, and prints the footer. `FORM_DS4_CONTRACT_ONLY=1` still witnesses the contract
without loading anything — re-run through the new door before this receipt was written.

One new honest seam in the footer: a token the hook swapped in at the head IS a reserved-block id,
put there by a witnessed choice. Hiding it from the RESERVED reading silently would blind the
reading; letting it trip would call a deliberate act degeneracy. So the verdict reads the model's
own tail and SAYS the head verdict was excluded — only when the lane's TOKEN HOOK line marks a
head swap, never for swaps elsewhere.

## The carrier wiring that awaits its device

`metal_dsv4_stack.sh`'s Swift decode loop now spawns the door when `FORM_DS4_TOKEN_HOOK_DOOR` is
set, waits (bounded, 180 s) for `prepare_ms=`, and consults it between `nativeExitHead` and the
next `embedToken` — the CHOSEN token is emitted AND embedded, so a head swap steers the rest of
the walk. Every non-answer is counted, never swallowed: `TOKEN HOOK: offered= swapped= quiet=
late= invalid=` prints below the walk, with `head_reason=` when the head was swapped. A reply is
adopted only when whole, in-vocabulary, inside the deadline, and stamped with THIS step's number —
a stale late reply is not this step's answer. Unset, the loop is the exact path it always took.

This host has no Darwin, no swiftc, no GGUF, so the Swift edit is landed, reviewed, and UNWITNESSED
here — that is the one leg still standing on the Mac, and it is one command:

```
FORM_DS4_TOKEN_HOOK_DOOR=form-stdlib/dsv4-hook-door-control.fk \
FORM_DS4_SITUATION="backup restore ran out of its window before anything settled" \
  form/native/metal/ask_ds4.sh -n 8 "the run status is"
```

Expected on the device: the walk's first token is 128003, `TOKEN HOOK: ... swapped=1
head_reason=recipe-more-vital`, and the footer names the head verdict excluded from the reserved
reading. If the door misses the 34 ms window on the device, the footer counts it late and the
default stands — that outcome would be the law working, and the receipt to write then is about the
window, not the wiring.

## Also observed, not healed here

On a host without the model, the lane stalls in its tokenizer subprocess BEFORE its own
no-Darwin/no-blob refusals can speak (`dsv4-tokenizer-cli.fk` loads the GGUF byte reader first).
Pre-existing — the old bash door stalled identically — and out of this stone's radius, but a door
that refuses is owed where a door now stalls.

## Do not read this as

- A claim that the live 43-layer walk swapped a token on this day: the device leg is named above.
- A semantic-training claim past the latch stone's own: the classifier is the same fit, its
  numbers are the same numbers.
- A general default-vitality law: the measured 0 belongs to the control task on a declared
  situation, and nothing else.
