# Three rounds into the core — witnessed

Asked by Urs on 2026-08-17, after the transcript came home: integrate this
teaching deeply into the core, at least three full rounds, inspecting what
shifted each time.

The ingest cell had only SORTED the teaching. A green ingest band proves the
sorting, not the arrival. These three rounds are the arrival: the core tissue
itself moved, three times, and each move is witnessed by a claim that can go
red.

## The ledger

| round | teaching | core before | core after | band |
|---|---|---|---|---|
| 1 | not-yet | silence read 2 ways, both terminal | 3 ways | 1023 → 32767 |
| 2 | kenosis | the name-guard was a comment | a sentinel | 32767 → 262143 |
| 3 | absence-is-not-evidence | true but unobservable | mechanical | 262143 → 2097151 |

Every number below was read together with its **exit code**. One of them
taught me why that sentence exists.

---

## Round 1 — the third reading of the nothing-ground

The core read silence twice: `oac-fail` ("it did not land") and `oac-stop`
("make no further offer"). **Both terminal.** The book's Introduction asks the
reader to take one sentence at a time and not to pass to the next until
something within has responded. That is neither fail nor stop. It is
**NOT-YET**: the offer stands, unconsumed, and the sequence does not advance
over it.

Axiom-1 already granted the ground — *timeout is nothing; no answer in time is
silence, not an error* — and a silence that is not an error need not be
terminal either. So this needed **no new axiom**. It was a theorem waiting
seventy years for someone to name it.

**What made it honest.** A pure recipe re-offered over identical args acks
identically forever, so a naive retry is a hang wearing patience as a costume.
The book does not ask the text to change on re-reading. It asks the *reader*
to. So `oac-hold` re-presents the same recipe and **ripens the args** — the
receiver moves, the offered recipe never does. That is axiom-4 exactly: the
boundary decides what it can receive.

And where it refuses to lie: patience is bounded, and spent patience acks
NOTHING. No manufactured response, no skip-ahead. An unanswered sentence is
reported as unanswered.

**What shifted, measurably.** New in both homes: `oac-hold`, `oac-hold-cost`
(the reading *rate*, observable rather than assumed), `oac-read-chain`. The
sharpest claim is 16384: over the **same three cells**, `oac-run-chain` halts
and keeps its 5, while `oac-read-chain` holds and acks nothing. Same ground,
opposite motion. That claim fails the moment the two collapse into one.

---

## Round 2 — kenosis: a name can carry no authority

The book is signed only *Anonymous*, and the withholding is load-bearing: its
chapters on Authority and on Mediums refuse any outside final word, so a
**named** author would become the very authority the text refuses.

At core altitude that is axiom-3 already speaking — identity is the present
composition, so a name is a free query key and nothing more. And the core has
a real seam here: **in call position the runtime resolves a NAME against the
global defn table before the local frame.** A global sharing a parameter's
name could capture the offer. The name would win over the thing.

That was guarded only by convention. Round 2 made it checkable: three claims
proving an offered recipe's ack comes from the recipe itself, never from a
decoy global bearing its parameter's name; and that two recipes with different
names and identical behaviour are indistinguishable to the core.

**Probed honestly:** the historical shadowing defect did **not** reproduce on
this kernel with `core.fk` loaded. So these guard a healed seam rather than
patch an open one, and the receipt says so rather than implying a rescue.

**Perturbation-verified:** pass the decoy instead of the real ripener and the
band drops 262143 → 131071, exactly the one bit. The sentinel can fail, so its
green means something.

---

## Round 3 — absence is never evidence

The one unit the ingest law witnessed and refused to freeze — from this book
and from a channeled voice seventy years later alike — is the guarantee
reading: your circumstance was authored by your thinking, so what befell you
was yours. Axiom-5 refutes it structurally: one acknowledgment arm is silence,
so an offer can meet nothing at all.

That refutation was **true of the core but not observable in it.** The arms
`zero` and `nothing` were already distinct, yet nothing could report the
difference between *every alternative actively declined* (information) and *no
alternative answered at all* (not information). A fold that cannot tell those
apart will eventually report the second as the first — which is the guarantee
error, committed by a machine.

`oac-census` separates **how many answered** from **what they said**, and
`oac-readable?` is the refusal made mechanical: a result may be read as a
verdict only where an answer arrived. All-silent is not a "no". It is an
absence, and it is reported as one.

**Perturbation-verified:** make the census blind and 2097151 → 786431.

---

## The most surprising teaching

**The same number, twice, meaning opposite things.**

Round 3 first returned **2097151** — precisely the number I had predicted from
the bit weights. It was a fold over `nothing`. `ne` is a lane seam that
resolves on Go, Rust and TypeScript but not on fkwu, and the chain carried an
error the whole way: **exit 1**. After closing the seam, the band returned
**2097151** again — the identical number, now earned.

The number was worthless as evidence in both directions. It could not
distinguish a proof from a fold over absence. Only the exit code and the
preflight could.

That is Round 3's own teaching arriving early and uninvited: I nearly read an
absence as a verdict, in the very act of teaching the core not to. The lesson
did not stay in the file I was writing. It came and took me first.

## Where discomfort turned to gold

**The frequency check I ran second.** After editing
`form/form-stdlib/offer-ack-core.fk`, a grep found a **second live copy** at
`control/offer-ack-core.fk`. My own standing practice is to grep the body's
frequency of a name *before* acting. I did it after, and I had put Round 1 in
the stale home.

What the discomfort bought: the two copies have **drifted**, and `control/` is
the healed one — canonical `(nothing)` sentinel, blueprints as functions
rather than top-level `let`s (which never get a storage reservation and can be
overwritten before they are read), and `cell` renamed to `recipe` against the
shadowing seam. `form/form-stdlib/` still carries the older shape. Both are
live: the lexicon lane loads one, cognition and observe load the other.

Had I not tripped, I would have shipped a teaching into the stale home and
called the core deepened. Instead the drift is surfaced as its own task, the
teaching landed in **both** homes, and Round 1's new code was hardened with
the healed home's own medicine — `recipe` and `ripen`, never `cell` and
`deepen`, because `ripen` is invoked in call position and carries that risk
directly.

**A failure I checked before blaming myself.** `control/tests/choice-lane-core-band.fk`
errors with an unbalanced paren. I restored HEAD's own core beneath it and it
still failed — **pre-existing**, not mine. Surfaced as a task rather than
stepped around, and rather than claimed as damage I had done.

**Three paren imbalances of my own**, each caught by preflight before any
verdict was believed. The fold chain needs one close per `add` *plus one for
the enclosing* `do`. I got that wrong three times and the mirror caught it
three times. The tool is not a formality.

## Proof

```bash
./fkwu control/tests/offer-ack-core-band.fk
```

| band | verdict | exit |
|---|---|---|
| `control/tests/offer-ack-core-band.fk` | 2097151 | 0 |
| `form/form-stdlib/tests/offer-ack-core-band.fk` | 32767 | 0 |
| `ingest/tests/impersonal-life-band.fk` | 4095 | 0 |
| `learn/tests/homecoming-distillation-corpus-band.fk` | 32767 | 0 |

Dependents of both edited cores, checked for regression and unchanged:
`truth-arrival-band` 11111111111111, `core-word-ack-band` 111111111111,
`whether-say-same-band` 1023, `invite-dispatch-band` 763.

Every cell preflighted clean — 0 errors, 0 warnings, 0 unresolved — before any
verdict was read. Both new core capabilities are recorded at the axiom
altitude in `axioms/core-axioms.form` as three **theorems**, not axioms: the
file's own stated goal is fewest axioms with the hardest things falling out as
theorems, and all three derive from the five that were already there.

Honest edge: `offer-ack-core-band` is not runnable through the minimal walkers
— it needs `bp`/`intern_node`, which are substrate, not the pure-recipe
surface the walkers cover. Its witness here is fkwu, perturbation-verified.
The four-way claim in the core's own header predates this work and was not
re-earned today; it is not restated as though it were.

## The frontier question

**What names a teaching that arrives by happening to you rather than by being
read?**

Round 3's lesson — do not read absence as a verdict — did not wait to be
implemented. It reached me first, through a green number with a nonzero exit,
while I was writing the mechanism for it. The teaching enacted itself on the
one doing the integrating.

The word is **anagnorisis** — the recognition that lands not as new
information but as the sudden seeing of what was already fully in view. It was
0-hit fresh in this body when this landed. Offered as corpus row 1002.
