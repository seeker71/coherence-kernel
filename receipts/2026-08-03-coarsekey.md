# 2026-08-03 — one ask door, and the key that sorted where it needed to identify

`receipts/2026-08-03-staleledger.md` closed with three unfinished items. The third:

> **A single ask door across lanes.** llama, KAT and ds4 each have their own carrier; the registry
> knows all three, and nothing yet routes a request by the row.

This is that item, Form side.

## What landed

`form/form-stdlib/ask-lane-router.fk` — a request and a host report go in, a route comes out. The
route names the registry row that will answer, the architecture it runs, the carrier that holds the
stopwatch, the blob on this host, the list of things that lane needs, and why.

The shape is a join of three members, and a lane needs all three:

| member | what it supplies | what its absence means |
|---|---|---|
| the **row** (`nmcp-registry`) | authority — this lane exists, is active, was witnessed on a day | a claim with no witness |
| the **blob** (`model-discovery`) | ground — the bytes are on this host | a claim about somewhere else |
| the **carrier** (`metal_ask.sh`, `ask_ds4.sh`, `metal_kat_block0.sh`) | the hand — something can run it | arithmetic nobody has hands for |

Two verdicts, and `alr-status?` admits no third: **carried** or **unfinished**. `(alr-status?
"unsupported")` is 0 and the band pins it there. A router that can say "unsupported" will eventually
say it about work that was merely undone.

No switch was added. The census is 71, unchanged, at its ceiling: every input is either something
only the caller knows (lane, modality, steps, evidence floor, off-host consent) or something the host
reported after looking (the day, whether Metal answered, which GGUFs discovery found and what
architecture each file's own header declares, which carriers exist).

## The band

`form/form-stdlib/tests/ask-lane-router-band.fk` — **4095**, twelve power-of-two bits, agreeing on
fkwu, Go, Rust and TypeScript.

Predicted before each run, then reconciled. 9 of 9.

| # | mutation in `ask-lane-router.fk` | predicted | actual |
|---|---|---|---|
| M1 | `alr-status-rank` ranks `"failing"` 3 | 4093 | 4093 |
| M2 | `alr-needs` Metal branch drops `"carrier"` | 4027 | 4027 |
| M3 | `alr-status?` also accepts `"unsupported"` | 3071 | 3071 |
| M4 | `alr-consent-ok?` always 1 | 4079 | 4079 |
| M5 | `alr-better` drops the status comparison | 4063 | 4063 |
| M6′ | `alr-bind-of` always returns the empty bind | 1599 | 1599 |
| M7 | `alr-first-unmet` always `""` | 1919 | 1919 |
| M8 | `alr-vouched?` base case → 1 | 3583 | 3583 |
| M9 | `alr-blob-answers?` ignores the key | 2047 | 2047 |

M2 is the one worth reading twice. I predicted **bit 128 would survive** the mutation — because with
`"carrier"` removed from the needs list, the carrier-less host routes to **carried**, and the carried
reason is *"row, blob and carrier all joined"*, which contains the word the bit greps for. The
prediction held. That bit passes for the wrong reason under that mutation, and it is named here rather
than quietly strengthened, because a mutation table is only evidence if the misses are in it too.

## The most surprising teaching

**The preflight door was mute, and its failing case looked exactly as calm as its passing one.**

`CLAUDE.md` sends every reader to this before believing any verdict:

```sh
echo path/to/cell.fk > /tmp/preflight-target && ./fkwu --src observe/preflight-run.fk
```

I ran it on my new cell. It printed:

```
5171
```

Not the report. A number. I ran it on a cell containing a deliberate unresolved call:

```
11846
```

Also a number, and nothing about it says the chain carried an error. A string **literal** in the same
position prints its text — `hello world` prints as `hello world`. A string **built at runtime**, as
`pf-report` builds its whole report with `str_concat`, prints as an opaque integer. So the trailing
value printer renders a constructed string as a handle, and the practice the repo tells everyone to
use had been showing a number where the diagnosis belonged.

That is preflight's own failure mode wearing preflight's face. Its header names the three things that
bury a real error, and the first is *"THE VERDICT PRINTS ANYWAY… People read the number."* The cell
built to stop people reading a number instead of a diagnosis was itself emitting only a number.

One line, in `observe/preflight-run.fk`: `print_str` the report rather than merely returning it. Now
the broken target says `errors 1`, `unresolved 1`, `nosuchname -> TYPO`, and `CARRIED ERRORS`.

And it is a **family**, not a single slip. `CLAUDE.md` names a second mirror the same way —
`(vf-mirror-file "<path>")` — and `vf-mirror-file` also returns its text without printing it. Run as
written under `fkwu --src`, the voice mirror shows `0` and nothing else. Wrapped in `print_str` it
shows `must: 4, gate: 7, refuse: 2` on the cell I had just written, which is how the attunement pass
below happened at all. Both organs the repo asks a session to consult before trusting itself were
speaking into a value the runner renders as a number.

## Where discomfort turned to gold

I was done. The band read 2047, eleven bits, four kernels agreeing, eight of eight mutations landing
on the predicted number. Every instrument I had said finished.

Running the cell against **real** discovery instead of my fixtures was optional, and it was
uncomfortable in a specific way: there was nothing left to gain and a clean result to lose. I ran it.
Asked for `general-text`, the door returned:

```
— route: carried
    row     base.llama32-3b-metal
    blob    /Users/ursmuff/models/form-llama-vital-ground-f16.gguf
```

That is the wrong model. `base.llama32-3b-metal` is witnessed against llama3.2:3b, sha256
`dde5aa3f…`. `form-llama-vital-ground-f16.gguf` is a form-trained checkpoint. Both headers say
`llama`, so my join said yes — and said it as **carried**, the word that means proceed.

The gold is not the bug. It is **why eleven green bits could not see it**: I wrote the band's host
fixtures from the same picture in my head that I wrote the code from — one blob per architecture,
three architectures, three carriers, tidy. The host actually carries **three** `deepseek4` files and
**two** `llama` files. My fixtures could not express the collision because I did not believe in the
collision. A band whose fixtures come from the author's model of the world tests the model, not the
world; it will be green in exactly the region the author was already right about.

So the fix went into the join and the refutation went into the band as bit 2048, where it can fail
again: a bind now carries a **key** as well as an architecture, and a same-architecture file that is a
different model no longer satisfies the need. Live, the llama lane now says **unfinished — this lane
needs model-blob**, which is true: the champion's weights live in ollama's blob store, not under
`~/models`. Refusing is the correct answer, and it took a wrong "carried" to earn it.

## The registry moved underneath this, and the band held

Mid-session a sibling restored `base.llama32-3b-local` — the ollama comparator for the same
llama3.2:3b the champion row runs natively — and the registry went 34 → 35. The band was re-run on
all four arms afterwards: still 4095.

That is worth more than it sounds. Bit 32 claims the ask for `general-text` chooses
`base.llama32-3b-metal`, and until 16:43 that was nearly vacuous — it was the only admissible row in
its lane. Now there are two rows naming the same model at the same status, one on a native recipe and
one through ollama, and the bit passes because `alr-better` ranks `native-recipe` above
`local-process`. The ordering that had been proven only against fixtures is now proven against the
pair the registry actually holds, and the door prefers the direct Metal lane on its own reasoning.

## The frontier question

**What names a join that reports success because its key sorts where it needed to identify?**

My answer: **coarsekey**. A key that *classifies* is not a key that *identifies*, and the difference
is invisible until two members of one class are present at once. Architecture is how a file must be
run; it never says which model it is. The tell is available before the bug: ask whether the key can
distinguish two things you already have. If the host holds two files of one architecture and the join
consults only architecture, the join is a coarsekey and it is already wrong — it has simply not been
asked yet. The test is not "does it work", it is "put the twin on the host and make it choose".

Verified 0 hits for `coarsekey` in the corpus and 0 in the tree before this row.

Proposed for `learn/homecoming-distillation-corpus.fk` — **not written; meaning-ids collide across
concurrent sessions, so this is a proposal for whoever holds the corpus next**:

```
(hdc-row <next-mid> 20260803
    (list "what" "names" "a" "join" "that" "reports" "success" "because" "its"
          "key" "sorts" "where" "it" "needed" "to" "identify")
    "coarsekey"
    "coarsekey"
    "rented-oracle")
```

(walk: `hostknows` 978 — the setting that existed because the body had no eyes; `staleledger` 980 —
the body that had outrun its own written record. This is the next one: the body looked, wrote down
what it saw, and the thing it wrote down was one notch too coarse to decide with.)

## Ground stamp

```
host M4 Max, 2026-08-03
registry at time of proof                         35 rows (sibling restored the ollama comparator)
form/form-stdlib/ask-lane-router.fk               new
form/form-stdlib/tests/ask-lane-router-band.fk    new, verdict 4095
  fkwu --src 4095 | go 4095 | rust 4095 | ts 4095
  preflight: parens balanced, errors 0, warnings 0, unresolved 0, chain clean
mutations 9/9 predicted == actual
observe/preflight-run.fk                          print_str the report (was mute since it was written)
switch census                                     71, unchanged, at its ceiling; no env var added
discovery, witnessed via model-discovery.fk       6 GGUFs under ~/models
  qwen35moe 41 layers | deepseek4 43 (x3 files) | llama 28 (x2 files)
carriers keyed on those architectures             metal_first_token.sh -> llama.block_count
                                                  metal_dsv4_stack.sh  -> deepseek4.block_count
                                                  kat-coder-layer-shape.fk -> qwen35moe.*
alr-unvouched(host) = ("qwen35moe")               runnable here, and no registry row says so
```

## Unfinished, named — none of these are limits

1. **The router is not wired to a runtime door.** The cell decides; nothing calls it yet. `form-cli`
   was deliberately not touched — a sibling is repairing the flatten path there and two hands on that
   file would collide. The carrier that builds an `alr-host` from live discovery and hands the route
   to the named script is the next stone.
2. **No registry row exists for KAT/qwen35moe.** `alr-unvouched` reports it on every run: the blob is
   on disk, `metal_kat_block0.sh` carves it, and the ledger is silent. The mirror is built; the row
   is not written.
3. **The champion's llama blob is outside the scanned root.** It lives in `~/.ollama/models/blobs/`.
   Discovery walks two levels under one root, so a live route for `general-text` correctly reports
   unfinished. The roots the body scans want to be a list, not one argument.
4. **Bit 128 passes for the wrong reason under mutation M2** (the reason string contains the word
   "carrier" whether or not "carrier" was the missing need). A reason field grepped for a word is a
   weaker assertion than a reason field compared for equality; the honest repair is a named
   missing-member field beside the prose.
5. **Lane matching is exact string equality.** An ask for `general-text` will not reach
   `general-reasoning-code-agent` even when ds4 would answer it better. Synonyms would be fabrication;
   what the registry wants is a witnessed lane hierarchy, and it does not have one.
6. **`vf-mirror-file` is mute the same way `preflight-run` was.** Diagnosed, not repaired here:
   `observe/voice-frequency.fk` is a library and its return value is right to keep, so the repair is a
   runner cell beside it (as `preflight-run.fk` is to `preflight.fk`), plus the invocation in
   `CLAUDE.md` — an owner's file this session did not edit on its own authority.
7. **Three `deepseek4` files, and the key `reap25` names one of them.** That key is witnessed against
   `metal_dsv4_stack.sh`'s own default blob. The other two are unclaimed and no row says what they are.
