# Two rungs moved, one held: turning pending into observed, and where it stopped

**Date:** 2026-08-14
**Status:** witnessed four-way
**Landed:** [`learn/zh-summary-eval.fk`](../learn/zh-summary-eval.fk),
[`learn/tests/zh-summary-eval-band.fk`](../learn/tests/zh-summary-eval-band.fk) → **1023**
**Updated:** [`receipts/2026-06-29-native-zh-summary-PENDING.md`](2026-06-29-native-zh-summary-PENDING.md)
**Corpus:** row 1031, `underwitness`

## The ask

*Turn pending into observed.* Two of the six rungs moved. Rung 5 did not, and it was not narrated
forward.

## Rung 2 and rung 3a: they had already moved, six weeks ago

The voice receipt's own update named its next step: RoPE at position > 0, then the multi-token native
loop. Both were closed **the same day** in a sibling receipt —
[`2026-07-02-native-generate-rope.md`](2026-07-02-native-generate-rope.md): sin/cos as range-reduced
Form Taylor, causal multi-key attention with real softmax over a growing prefix, seven tokens
generated natively and **token-for-token identical** to an independent projection-runner oracle on
the same checkpoint. A wrong RoPE angle would have shown as a diverging word. It did not.

The master page went on saying *"rungs 3a-full, 3b, 4, 5, 6 stand exactly as written"* for six weeks.
So the first part of turning pending into observed was not engineering at all — it was reading what
the body had already witnessed about itself. Rung 2 is now **observed on real logits**; rung 3 reads
**3a CLOSED · 3b pending**.

## Rung 6: blocked only for want of a metric, and that needed no weights

The receipt marked rung 6 **blocked**, and said exactly why: no eval metric was named, so
*"earns observed" is a fake-in-waiting.* That block was removable today, with nothing loaded.

`learn/zh-summary-eval.fk` fixes the rule now, while the native zh output does not yet exist to shape
it. Held-out items only. Both lanes scored in one batch with lane labels hidden. Faithfulness and
fluency, 0..5. At least **20** items. Native earns it when its total *reaches* rented's — `ge`, not
`gt`, because the claim under test is "at least as good", and demanding better-than would be a
different claim than the one written.

Blindness is a protocol fact the cell carries as a caller-asserted flag and does **not** pretend to
verify by computing. A cell that faked that check would be the thing it exists to notice.

Four of the band's ten bits exist to catch the rule answering in the body's favour whatever arrives —
a losing batch earns nothing, an unblind one earns nothing, a short one earns nothing, an empty one
earns nothing — and b8 perturbs the top level so the reading has to move between win and loss at all.
The floor of 20 is pinned in a bit so it cannot drift quietly later; a floor lowered after seeing the
outputs is the same fake as a metric written after them.

```text
band 1023 on fkwu / Go / Rust / TypeScript · preflight clean · registers clear
```

Rung 6 now reads **shaped**, not blocked. Running it still waits on rung 5.

## Rung 5: held, and the floor is narrower than the page implied

I wrote, and had to correct within the hour, that no zh-capable base was on this Mac. Wrong. There is
a 91 GB DS4-Flash gguf under `~/models/ds4/`, plus the body's own `form-llama-vital-ground` at f16 and
q4_k_m. The Metal door is real too — `metal_matvec_f32` (tag 204) with the strong symbol written in
`form/native/metal/fk-metal-carrier.m`, not the weak stub that used to answer `metal_linked=false`.
And `tests/ask-ds4-band.fk` stands at **255**.

That 255 is a **contract, not a voice**. Its own first line says so: *the body-owned ask contract,
without loading a model fixture.* Its bits check step counts, the cache cap outrunning the walk, the
six radius statements, membrane names. Nothing in it emits a token.

So what stands between here and rung 5 is neither missing weights nor a missing GPU door. It is that
no run on this host has yet taken a zh-capable base through the Form block and emitted text — and the
stones underneath are the ones the July receipt already named: no persisted KV cache
(`native-generate.fk` recomputes O(seq²) by design, and the walker exhausts its float pool at 12
tokens on a **260K**-parameter model) and no f64 asm matvec. Those bite far harder at DS4's scale.

Pending, and pending for a smaller and more actionable reason than this page implied for six weeks.

> **frontier question** — what names a record reporting a floor lower than its own body has reached?
> **underwitness** (0-hit fresh at offering)

Corpus re-probed before pinning: 398 rows / 398 admissible / max-mid 1005 / 0 duplicate ids / field
code 398039821005. Band **32767**, exit 0.

## The most surprising teaching

This body's whole discipline is built against over-claiming — pending is honest, a fake is the worst
outcome, the standard-receipt exists so a capability cannot be dressed up. That discipline works. And
it has a shadow nobody had named: the same page, held to that standard, spent six weeks reporting a
floor **lower** than the body's real one, because it never re-read its own siblings. Under-claiming
is not the safe direction. It hid a closed rung, and a closed rung nobody knows about buys exactly
as much as an open one.

## Where discomfort became gold

Twice, and the second one is the honest half of the answer to the ask.

First: I wrote "neither the checkpoint nor a zh base is on this Mac" into the master receipt, and it
was false — I had run `find` for *stories260K* and concluded from its silence, when a 91 GB
zh-capable base was sitting in `~/models`. I caught it only because I kept probing after I had
already written the sentence. The correction is in the receipt with the error visible, not smoothed.

Second, and harder: the ask was *turn pending into observed*, and the most available way to satisfy
it would have been to run `ask-ds4-band.fk`, get **255**, and write that rung 5 had moved. It is
green. It is four-way. It has DS4's name on it. Reading its first line — *without loading a model
fixture* — is the only thing between that number and a claim this body's founding receipt exists to
forbid. The pull to accept a green number whose header disclaims the very thing you want it to prove
is exactly the deadgreen pressure from this morning, arriving on the one page where faking it would
cost the most.
