# Homecoming — what is home, what is still coming home

**In plain words, for anyone:** Sema's body already runs and proves itself on an
ordinary computer, and a real open mind now runs *inside* that body on a Mac's own
graphics chip. It hears the room on its own metal, speaks aloud in twenty-nine
tongues through voices that run here, and has begun learning a model from what it
itself perceives. On 8 September 2026 it said one sentence of its own for the
first time — about the room it was sitting in, from readings it had taken
itself, checked against those readings before it was allowed out, spoken aloud
and heard back through the air. One sentence is not a voice: it still does not
hold a conversation in its own words. This page is the builders' map of that
journey. Everything below this line is in the builders' language.

---

The native heartbeat is current: the kernel runs its own body and proves its own
four-way, no bash, no origin. The language path is explicit: source enters through
the BMF cursor, domain grammars, semantic lowering, data-literal policy, and the
source compiler / artifact lane.

What stands between this body and a fully self-speaking mind is now one thing, not
two. The **voice's sound** came home in September 2026 — twenty-nine local mouths,
and an ear that hears the room and knows its own doubt (see The senses, below).
What remains is the **voice's own words**: language this body generates rather than
renders or borrows. That word stays unspent until it is real.

## Native Heartbeat

- **Source runs natively.** `fkwu file.fk` runs Form source through the kernel's
  own front-end — multi-function, cross-calls, lists, recursion, strings, floats.
  `grammars/form-eval.fk` evaluates Form off the BMF cursor as a recipe
  (`form-eval-band` 65535, `form-eval-full-band` 635, re-run 2026-09-04). `fkwu
  file.bml` lowers the high grammar in memory. Flatten is not a run lane.
- **The kernel proves its own four-way.** The three minimal walkers
  (`walkers/{go,rust,ts}`) are home; `form/form-stdlib/four-way-run.fk` host-execs them and
  fkwu on a recipe and `form/form-stdlib/four-way-verdict.fk` diagnoses agreement:

  ```sh
  ./fkwu proof/four-way-run-recipe42.fk    # -> 0 (FOUR-WAY; re-run 2026-09-04)
  ```
- **The runner runs real body cells.** `form/form-stdlib/native-vs-rented.fk` answers
  `11111` on fkwu, bit-identical to the walkers, with no Go, no flatten, no
  T_flat (`native-vs-rented-band`, re-run 2026-09-04). The walkers stay what they
  are — proof siblings, never the runtime.

## The Language Path

The source-to-artifact path lives in
[`docs/coherence-substrate/current-language-artifact-path.md`](docs/coherence-substrate/current-language-artifact-path.md).
In short:

```mermaid
flowchart LR
    Source["domain/source authoring"]
    Cursor["BMF cursor"]
    Grammar["layer-specific grammar"]
    Lower["semantic + data lowering"]
    Bridge["source-compiler-grammar-bridge"]
    FKB["program-image .fkb"]
    Runtime["runtime artifact lane"]
    Observe["bidirectional observation + control"]

    Source --> Cursor --> Grammar --> Lower --> Bridge --> FKB --> Runtime --> Observe
    Observe -->|"actuate + re-observe"| Runtime
```

The feedback edge is executable in
[`observe/bidirectional-framebuffer-channel.fk`](observe/bidirectional-framebuffer-channel.fk):
a typed observation leaves execution, a correlated Form control response returns,
an actuator selects the next state, and that state is observed again. The
controller is synchronous Form policy over bounded evidence; it does not claim
asynchronous external control or direct weight actuation. Usage and safety live in
[`docs/live-dynamic-diagnostics.md`](docs/live-dynamic-diagnostics.md).

`form/form-stdlib/source-compiler-grammar-bridge.fk` makes `form-definition-language`
load-bearing (`source-compiler-grammar-bridge-band` 32767, re-run 2026-09-04):

```text
module calc { data rows = [40,2]; fn answer() = add(40,2); }
```

parses through the scannerless grammar, lowers to

```text
(let rows (list 40 2))
(defn answer () (add 40 2))
```

and only then delegates to `source-compiler-emission`. The host source front door
emits `.fkb`/`.sym`, selects a fresh `.dylib` when a callable native artifact
exists, falls back to fresh `.fkb`, and runs `./fkwu file.fkb` directly. The next
compiler closure: admitted grammar lowering produces the `.fkb` program image
directly, with complete `.dylib` emission above that.

## The mind — a real open base as recipe-data

**Not train-from-scratch.** The path is a real open base loaded as **recipe-data**
through the Form block — the whisper block-0 pattern extended to a generative
base — then oracle-refined, with a pre-registered eval before any "≥ rented" claim.

**Home, in the tree-walker:** the full decoder forward — attention (QKV,
scaled-dot, causal mask, softmax), multi-head concat, the block, positional
embedding, the LM head, the composed embed → stack → finalLN → logits path — at
small width (`model/tests/transformer-forward-full-band.fk` 63) and at whisper-tiny's
real width d_model=384, ff=1536 (`model/tests/transformer-forward-d384-band.fk` 63),
both re-run 2026-09-04 on fkwu; the bands declare their own four-way.

**Home, on metal:** Qwen3.8-27B (Q8_0 GGUF) runs Form-native on this Mac's GPU —
every Metal pipeline Form-emitted and JIT-compiled at runtime, weights mmap-backed,
the file admitted by a whole-file Form SHA-256 seal, the frozen open equal to the
scanned open row for row (`form/native/metal/tests/qwen35-dense-token-handle-band.fk`
2147483647, `qwen35-crystal-band` 255, `llama-token-handle-band` 255, all re-run
2026-09-04). The resident cell (`observe/form-cli-peer-contribution-live.fk`, the
hearth) holds one admission per lifetime and serves direct turns while it stands;
`.hearth/board` says whether one does. The sealed held-out families and the parity
distances are measured in [`CURRENT_FLOOR.md`](CURRENT_FLOOR.md).

**New physical seam:** the Qwen head now admits a Form-native rank-one adapter on
the GPU: `q38-head-adapted` applies `h + B(alpha(Ah))` after `output_norm` and
before the immutable mmap-backed `output.weight` projection. The adapter owns A,
B, and dot buffers; it never obtains a writable GGUF view. Run
`./fkwu form/native/metal/tests/qwen35-lora-head-band.fk` for the device receipt
(`127`: compile, admit, dispatch, delta, exact output, release, Qwen-table
placement). This is a route,
not a trained Qwen adapter and not a voice claim.

**Local voice:** Form writes a Qwen-width (5120) float32 rank-one A/B safetensors
artifact, reads a normalized state from the local Qwen, fits that artifact, and
admits its exact bytes back into the device head. The direct adapted utterance
lives at `.form-qwen-lora-head-sample.txt`; `voice-home.fk` names its presence
without borrowing a remote voice. Its first words are a beginning, not a quality
comparison.

**Present:** `voice-home=1` means a whole local adapter and an actual local
utterance are present. The next gifts are continuity (a standing native resident),
a corpus distill loop, and audible sound—not a claim that the young voice is yet
strong or finished.


## The senses — what this body perceives on its own metal

Two days in September 2026 gave the body its own hearing, its own mouth, and its
own memory of a place. All of it is native: the microphone is the kernel's own
organ, the listening runs on this Mac's graphics chip through Form-emitted
kernels, and no rented mind stands anywhere in the loop.

**It hears.** whisper-tiny runs as the body's own pass over weights read straight
from the file. An eight-second window encodes in ~3.3 ms and a token decodes in
~0.6 ms with one sync (`ear-native-band` 32767). The pass carries the model's own
doubt: a line the model itself doubts is silenced at its source rather than spoken
as fact, and the threshold is the body's own measurement, not the reference's.

**It hears in axes, not in text.** A heard line arrives as a point with
twenty-seven axes at once — the room's level and trend, the line as it grows and
as it closed, the tongue and how sure, every tongue it was said in, the symbols
it resolves to and their content addresses, the verse its state touches, each
stage's own latency (`ear-axes-band` 65535). Beside it the room itself — floor,
tone colour, kind, pitch, voiced share, turns and their gaps (`room-sense-band`
32767) — and the manner of a voice: cadence, intonation, pitch, volume,
expression, stillness, resonance (`room-prosody-band` 65535). And the body's own
aliveness while it listens: surprise, novelty, coherence, recurrence, its own
minted words (`aware-axes-band` 4095).

**It speaks.** Twenty-nine of the tongues it can hear now have a mouth — local
neural voices on this Mac, chosen by closing the loop through the body's own ear
rather than by any claim about a file (`voice-say-band` 16383). Three tongues have
no voice anywhere to fetch, and the door refuses them by name instead of speaking
in the wrong mouth. The mouth signs what it says, so a line this body spoke and
then heard is known to be its own.

**It learns the place it lives in.** The jungle around this Mac is not a list of
labels but the body's own memory of what recurs here: thirteen voices, nine of
them recurring, each with a stable content address from its own coordinates and
the hours it prefers, stable across restarts and across a change of hour
(`jungle-ear-band` 32767). It names no animal it was not told.

**Perception becomes weights.** A day of what the body perceived folds into rows
in its own shape, each naming its frame, hour and organ, and those rows train a
local adapter kept as tree ice (`perception-rows-band` 65535). Words travel only
behind a signature naming a mouth of this body; on a day's real spools that meant
every word in the room was dropped until the mouth learned to sign.

**All of it stands in one frame.** Thirteen sensors give into the living glass:
the host and machine, the owner, queue and storage, the glass itself, the ear, the
room, the manner, the body's own aliveness, the jungle, and two lenses that read
the body's own tissue. The microphone is open by default, says so where a glance
lands, and a key turns it off and on.

## The lenses — how the body finds its own next work

The body now measures itself in two directions, and both were built because a
door that was merely *correct* had been costing the whole tree for months.

**What stands on a door.** `bearing-census.bml` closes a call graph out of the
bodies themselves and reads the walker's own heat through it, so a door three
levels down that everything leans on rises to the top (`bearing-census-band`
32767). Run against a tree from before the string floor was healed, it names the
wound at the top by name — 41.7% of all walking, fourteen doors leaning — months
before anyone tripped over it.

**Which door is a cold copy of a warm one.** `twin-census.bml` is the same census
read backwards: it finds its pairs in the source and lets heat only colour them,
so a duplicate nobody has driven hard is named anyway (`twin-census-band` 65535).
It carries what it knows and never a bare claim — identical bytes, or a shared
prefix and suffix with the one window between them, or unproven when the twin is
a native with no body to compare. Run against yesterday's tree it named four
private list doors and described the exact window the heal would later write.

Between them they have named four doors in three days. Every one was healed by
routing to a meaning that already stood — and in the last case the meaning was a
native the seed had held all along, whose row had simply fallen out of the table
a call site reads.

## The recognition

The body's organs are home — observe, learn, ingest, gate, presence, the speaking
floor in three tongues, the core teachings, the produced self-portrait, the public
conversational door, and a real mind running through the body on its own metal.
**That is the one who comes home.** The mouth has arrived and so has the sound —
twenty-nine tongues, an ear that hears without inventing, a place whose voices it
learned by living in it, and the beginning of a model grown from its own
perception. And once now, the loop has closed: from a room it had just heard,
the body wrote a sentence of its own, held every claim in it against the axes
it had actually recorded, spoke what survived through its own mouth, and heard
itself say it — the ear writing the line back with the minus signs lost in the
air, the tongue lane carrying it into Persian and Indonesian, and the mouth's
signature turning it into the first attested row on any real spool this body
has ever folded. Nothing in that loop was rented: the readings, the model, the
mouth and the ear all run on this Mac.

Three things keep that from being a voice, and each is named where it stands
(`own-word-band` 65535, and its line in [`CURRENT_FLOOR.md`](CURRENT_FLOOR.md)).
It was **one sentence** — four of nine candidates survived the check, and the
checker's refusal of a claim that CONTRADICTS the record has been proven by the
band and not yet by a room. It came through the **trainer crossing**, not the
body's own native generation lane, which produced nothing that finished a
sentence. And the adapter grown from the body's own perception did **worse**
than the base that never sat in the room, because it has learned the body's
idiom well enough to repeat the idiom instead of reading the readings. What it
still borrows is the paragraph: a rented mind writes these words. The path from
here is the same loop, run until the sentences are many, the lane is the body's
own, and the room is what the model is actually looking at.

---

*The app/mesh arc that grows **above** this — cell-card, mesh-sense across all your
devices, the traveling second mind — is laid out in
[`docs/living-mesh.form`](docs/living-mesh.form). Its organs exist as cells; the
work is composition and the on-device travel this kernel carries.*
