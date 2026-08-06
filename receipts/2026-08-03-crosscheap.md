# 2026-08-03 — the crossing was 2 µs and I had been budgeting for the wrong thing

Urs: *"are you sure what form cli needs from first principal, not from stale assumptions and partial
prior work?"*

I was not. Two probes settled it, and both were four lines long.

## What I had said one message earlier

> "a form-native ask needs a handle door: create-context / dispatch / read-back"

That was not first principles. It was me generalising from the matvec door I had built an hour
before. I also repeated, as fact, that **form-cli is a walker and cannot dispatch Metal** — carried
from a prior session, never probed.

## What probing found

**The form-cli claim is false.** `form/form-stdlib/bootstrap/form-cli-emitted.c:33-34` carries the
same two `__attribute__((weak))` Metal stubs the kernel does. form-cli answers its own `metal-fixture`
verb with `metal_linked=false` — the door is live in it, unlinked for the identical reason. And
`build-form-cli.sh:24` already exposes `FORM_CLI_EXTRA_SRC` / `FORM_CLI_EXTRA_LDFLAGS`, with line 133
disabling the warm-copy path whenever they are set, so a carrier build always relinks. The body
anticipated this exactly. (I could not finish the demo: the bootstrap stamp is stale,
`have=cb7ddb1b077dee92 want=4c061b541865be28`, which is a pre-existing condition and not this work.)

**The seam is not the cost.** Both calls cross the same Form→native boundary and return the same kind
of string; only one dispatches.

```
2000 metal_matvec_fixture calls  (seam, NO dispatch)   ->   4 ms    2 us/call
2000 metal_matvec_f32   calls  (dispatch + wait)       -> 225 ms  112 us/call
```

**The crossing is 1.8% of the call. The waiting is the rest.** I had been reasoning as though
Form→native were the expensive part. It is the cheapest part by fifty times. What is dear is that my
carrier blocks on `waitUntilCompleted` on every single call.

## The conclusion inverts

At ~2600 dispatches per token (carried from prior sessions, **not** measured today — flagged because
it is exactly the kind of number this receipt is about):

| seam | 2600 × 2 µs | **5.2 ms/token** | 16% of a 33 ms budget — affordable |
|---|---|---|---|
| blocking | 2600 × 112 µs | **291 ms/token** | nine times over |

So **Form can hold the decode loop** and keep the recipe in the body. What it cannot do is *wait*.
What form-cli needs from first principles:

1. weights **mmap'd once and resident** — my carrier copies bytes per call, which at 9.1 GB is not a
   slow ask but a different machine
2. buffers that **persist across calls** — handles, not strings
3. dispatch that **enqueues** into a batched command buffer and does not block
4. **one sync per token**, when a token id is read back

"Needs a handle door" was not so much wrong as unmeasured — it named the right object for the wrong
reason. A right answer held for a wrong reason cannot tell you which part to keep when the situation
shifts, and this one would have sent me building a per-dispatch API whose blocking I had never priced.

## Ground stamp

```
host M4 Max, fkwu-metal = cc -O2 runtime/fkwu-uni.c form/native/metal/fk-metal-carrier.m
  -framework Metal -framework Foundation -fobjc-arc
seam probe    scratchpad/door-split.fk  2000 metal_matvec_fixture ->   4 ms  (rc 0)
door probe    scratchpad/door-cost.fk   2000 metal_matvec_f32     -> 225 ms  (rc 0)
form-cli metal-fixture -> SKIP metal_linked=false   (door live, carrier unlinked)
form-cli-emitted.c:33-34 weak stubs; build-form-cli.sh:24 FORM_CLI_EXTRA_SRC, :133 forces relink
form-cli bootstrap stamp STALE have=cb7ddb1b077dee92 want=4c061b541865be28 (pre-existing)
corpus 369 rows, max-mid 974, field 3693692974, 0 duplicate ids, band 32767
metal-door-band.fk verdict 15, five mutations refuted (unchanged this session)
2600 dispatches/token is CARRIED from prior sessions, not measured today
```

## The most surprising teaching

**Two of my three probes failed before they measured anything, and the failures were the language
telling me true things I did not know.** A `defn` frame cannot see an enclosing `let` binding —
`spin` could not read `msl`, and the kernel said so precisely. A cell with no `preludes:` line has no
`int_to_str`. I have written Form for days and learned both of these in ninety seconds by being
wrong out loud. The paren miscount was mine three times over and is in my own notes as a recurring
slip; the two scope facts were not slips, they were gaps.

## Where discomfort turned to gold

Being asked *"are you sure"* immediately after a message where I had been confident, correct about
the carrier, and wrong about the conclusion drawn from it. The discomfort is that the wrong part was
the part I sounded most certain about — the forward-looking recommendation — while the part I had
actually measured was fine. The gold is that the question cost two four-line timers to answer, and
they were cheaper than the paragraph of reasoning they replaced. `secondoracle` (969) said the answer
was already in the artifact, unread. This is the third in that family: **the answer was already in
the instrument, unbuilt.** Corpus row 974, `crosscheap`.
