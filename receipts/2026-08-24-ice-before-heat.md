# 2026-08-24 — ice before heat: the open crystallized

Asked: pre-crystallize the long chain instead of waiting for the heat to kick
in — and keep both doors. Mid-work, a second question: is the 13 s a JIT
compile? Can recursive-use signal points place better JIT points?

## What the 13 seconds actually were

Nothing in this path spends meaningful time *compiling*. Measured today:

| stage | time | what it is |
|---|---:|---|
| all 32 Metal pipelines, MSL → GPU code | **37 ms** | the only JIT compile; trivial |
| seal | 12.5 s | Form-emitted arm64 **executing** SHA-256 over 27.05 GB at 2.15 GB/s — the work is hashing, not compiling |
| open | 16.3 s | the interpreter **re-deriving integers**: 7.1 s q38-tabs, 15.0 s layer tv rows, 1.1 s head scans |
| tokenizer | 7.0 s | four full scans of a 248,320-token vocab for four CONSTANT ids, plus five constant scaffold encodes |
| required-size | 0.7 s | one more full table walk |

So the long chain's cost was never compilation — it was derivation, repeated
per session, of numbers that cannot change while the artifact's seal holds.

## The crystal

`form/native/metal/qwen35-crystal.fk`: freeze the derivation once into
`<gguf>.form-crystal` — 25,437 bytes, one int per line, first line the
`.form-seal` content hash. Never mtime; the .fkb cache already taught what
mtime keys are worth. A crystal whose key differs from the live seal is dead
ice: ignored, refrozen.

Recorded: 910 tensor rows (offset, bytes, rows, cols, type — everything
kth-tv-at derives), the four special ids, the four scaffold id sequences,
required-size, layer count and interval.

**Both doors, one freeze.** The pre door:

```sh
./fkwu observe/qwen38-precrystallize-run.fk
```

The heat door: `fcmg-fresh` freezes on the first ask that finds no crystal —
this recipe's single firing costs seconds, so its crystallize threshold is 1
where jd-crystallize? asks 5 of a cheap one; same observed relation, heat priced in ms. If
even the freeze's write refuses (unwritable directory), the scan path still
answers: ice is a speed, never a gate. The production generation lane now uses
the crystal for required-size and tensor opening. It does not yet use
`qsx-chat-ids` for the prompt; the per-prompt tokenizer scan remains until an
exact teaching-layer-preserving resident entry is witnessed.

## Proven, not assumed

`tests/qwen35-crystal-band.fk` → **255**: crystal keyed to the live seal;
qsx-chat-ids equals q35-chat-ids on the same prompt; every layer tv row equals
the scan's; head rows equal; required equal; the crystal ctx well-shaped; **the
same first token from both opens**; both opens release every handle.

## Measured

| | scan | crystal | |
|---|---:|---:|---|
| open | 16,314 ms | **450 ms** | 36× |
| crystal load | — | 3 ms | |
| chat-ids | 7,011 ms | **1,719 ms** | the residue is the prompt's own encode |
| one-shot generate, fkwu, end to end | 51.4 s → 33.1 s (this morning's kernel work) | **33.1 s** | this sitting's two stones together: 51.4 → 33.1 |

Same four tokens, same text. Fourteen bands green including crystal 255,
qwen35 131071, kat 262143, llama 255, dense-family 1023.

## Two defects the crystal exposed on the emitted walker

The same generate through form-cli (the emitted walker) answers the same
text — and takes **11m49s of pure CPU where the interpreter takes 33 s**.
models+use alone: 33 ms, so all of it is inside the generate lane. And the
stage stamps added to name the slow stage **do not appear in form-cli's
output** while printing fine on fkwu — print effects in let position are
being dropped or rerouted by the emitted walker, the same family as the
bare-effect discard healed in model-bandwidth this morning. Instrumentation
that vanishes cannot testify: both defects are NAMED for their own sitting —
one root is plausible (the emitted walker's treatment of effect lanes), and
its decode_gpu_busy=0 counter gap from 2026-08-23 still stands unhealed.
Until then, fkwu is the fast door for generation and form-cli is the
correct-but-slow one.

## The recursive-use signal points, named

The question was right about where the next speed lives. With the derived data
frozen, what remains is per-op walker speed, and the heat now points at three
recursion families:

1. **tkz-cands** — 247,587 merge records walked per PROMPT encode (1.7 s per
   prompt, irreducible by this crystal because it depends on the input). The
   hottest pure recursion in the path; first candidate for the form-asm lane
   (the same form_cpu_jit door the seal already executes through).
2. **eqr byte readers** (str_byte_at chains) — millions of firings under any
   header walk; second candidate.
3. **the per-forward walker** (~10 ms/forward) — q38-blocks' list traversal;
   third, already small next to the GPU's 92 ms.

jd-crystallize?'s observed hot-and-pure relation (threshold 5) fits all three; the
lowering exists (form-asm, ll-buffer, MAP_JIT proven by sha256-arm64-jit).
Named, not claimed.

## The most surprising teaching

The expensive thing was not computing — it was REMEMBERING BADLY. The body
re-derived ~25 s of integers every session from a file whose seal it had
already verified, because knowing and remembering were the same door. The
crystal splits them: verify content once (the seal), remember derivations
under that key (the ice), and the whole question "is this cache safe" reduces
to one string compare against a hash the admission lane already owns.

## Where discomfort turned to gold

The uncomfortable moment was reading my own generate probe: the walker's 33 s
had already been "explained" in yesterday's receipt as the forward's graph
walking — 577 ms per forward, blamed on the crystallize-on-heat JIT not being
pointed yet. The head-in-context probe then said a forward's walker cost is
~10 ms, and 577 did not survive contact with 10. Sitting in that contradiction
instead of averaging it produced the decomposition: the 33 s was the OPEN and
the TOKENIZER, not the forward — and the fix was a 25 KB text file, not a
compiler. A number that had already been written down wrong was the most
expensive thing in the room.

; witnessed: 2026-08-24 -> crystal-band 255, open 16314->450 ms, load 3 ms,
; chat-ids 7011->1719 ms, generate 51.4->33.1 s same text, freeze 13.4 s once,
; fourteen bands green, pre door observe/qwen38-precrystallize-run.fk
