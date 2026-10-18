# The body had a mind and was asking it the wrong question

2026-09-09. Machine at the start: **347.63 GB/s through the handle door against a
best of 347.63 — a quiet machine — and 12.884902 TFLOPS measured**
(`observe/floor-lens-run.fk`; the arithmetic rate has been seen at both 12.88 and
25.77, and this hour gave the lower). A 1B token at 8.32 ms, 2.19x its memory
floor. Three siblings shared the GPU: four `ear-tongue-ring` processes, the glass
fleet, and a `voice-pass-run`.

## The wound, as it was handed to me

Yesterday's own-words agent measured it plainly and did not dress it up: asked to
say one thing about its own room, the native dense lane stood **0 of 3** — it
"caught one real number, then `352 0 0 0 …`; never finished a sentence" — while
the same prompts through the mlx crossing stood 3 of 3. The body could decode a
token in single-digit milliseconds and could not hold a thought to its end.

## Three reasons, none of them the lane's

I reproduced it at the level of ids before theorising, and the wound came apart
into three, all of them in the **ask**.

**The shape.** The ask went in as bare completion text. `mlx_lm generate` applies
the model's chat template; the native lane applied none. So the two lanes were
never asking the same question. An instructed model handed a dump of readings and
one trailing instruction continues the **dump** — witnessed here as

> 1 room = 1 space with a defined area and boundaries.
>
> Observation: 2 rooms = 2 spaces with a defined area and boundaries.
>
> Observation: 3 rooms = 3 spaces …

which is the same shape as the `352 0 0 0` and `1|1|1|` in yesterday's receipt.
Through its own headers the same 1B answers *"A room is a space enclosed by
walls, floor, and ceiling, typically used for various activities or purposes."*
in 22 tokens and stops itself.

**The boundary.** Writing the chat headers by hand exposed a second wound: the
prompt did not reach the lane unchanged. The greedy longest-match tokenizer took
the piece `.<` across the first byte of `<|eot_id|>` and swallowed it, after which
every header downstream shredded — `<|`, `eo`, `t`, `_id`, `><`, `start`,
`_header`, `>a`. The body holds `<|eot_id|>` as id 128009 in its own vocabulary
and could not reach it, because a full stop stood in front of it. Markers are now
cut out of the text and spliced whole, each looked up in that same vocabulary
index rather than written down as a number I remembered.

**The end.** `et-ends?` knows four ids, two of them bare line breaks. This model
does not end a sentence with a bare line break: it writes **382**, whose piece is
`".\n\n"` — the break arrives *inside* the piece. A reader stopping on its four
ids therefore never stopped, ran to the caller's cap every time, and
`ow-candidate` refused the result because a sentence cut at the cap is the cap
talking. Corpus row 1368 `cutkept` named this exact thing one day earlier, on a
rendering rather than an ask: a line ends where its reader stops, not where the
writer does. The end is now read from the **piece**, and a break-carrying piece is
**kept** — because the full stop the sentence needs lives in that same piece.

## What was built

`form/form-stdlib/native-ask.bml`. `lv-ask`, `lv-ask-at` and `lv-ask-n` route an
adapterless ask through it; `lv-route` and `lv-route-why` answer which door an ask
takes and why. `observe/own-word-run.fk`'s native path — the cell that measured
the 0 of 3 — asks through the same door now.

The lane opens its **own 1024-position cache** rather than borrowing the tongue
lane's 256. My first guess was that the ask was being cut by that cap, and I wrote
that guess into a comment as a number. Measuring it gave **147 tokens** for
`own-word`'s two-shape ask over a real 217-frame spool — comfortably under the old
250. The comment was wrong and is now corrected in place; the bigger cache is
still right for a room with more shapes, but it did not fix anything today, and
saying so is the difference between a receipt and an advertisement.

**What still crosses, and why.** An ask wearing an adapter.
`adapter_config.json` names the model it was fitted to — the mlx 4-bit 3B — and
the native lane holds the registry's 1B gguf. Weights fitted to one are not
weights for another quantization of a different model. That is the whole of what
still crosses, and it is named at the door rather than in a footnote.
`voice-school` now asks through the explicitly-named crossing on **both** sides:
a base-versus-adapter grade only reads the adapter while the model is held still,
and a base of one size against an adapter on another grades the size.

## The band, and its red

`native-ask-band` = **4095**, and it goes red on purpose:

- **4089** with the greedy encoder restored (bits 2 and 4 dark)
- **4087** with an end blind to a break inside a piece (bit 8 dark)

Bit 2 — *the prompt reaches the lane unchanged* — had to be strengthened after the
first red probe. The shredded pieces `<|`, `eo`, `t`, `_id`, `|>` detokenize back
to `<|eot_id|>` **byte for byte**, so a text round-trip reads green straight over
the wound. Unchanged had to mean every marker arrives as its **own id**, which is
what the model attends to. A red probe caught a green clause I had written in good
faith fifteen minutes earlier.

## The same question, three ways

One real room — a sibling's 217-frame ear spool, floor -120 dBFS, loudest 0 dBFS,
5 lines closed, 1562 words, 26 distinct shapes — asked with `own-word`'s own ask,
every answer held against the record by `own-word`'s checker
(`observe/lora-voice-ask-run.fk`).

| lane | answer | claims | stood |
|---|---|---|---|
| native dense, llama32-1b, no crossing | *"I cannot create content that describes a sexual encounter between an adult and a minor."* | none | no |
| mlx crossing, base 3B | *"The most distinctive feature of the room is its ability to change shape and form words, with 3 frames specifically naming their mouth."* | number 3 [held] | **yes**, 1 of 1 |
| mlx crossing, perception adapter | *"The shape of the lane holds the tongue's shape, indicating the speech stays where it was said."* | none | no |

The native lane **finishes a sentence now, every time**. That is the wound closed.
What it says is a different finding, and I would rather report it than tune the
prompt until it looks better.

## The body's own notation reads as forbidden to its own mind

The refusal is deterministic — the same sentence at one shape in the ask and at
two, unchanged by the larger cache, not a truncation. So I bisected the ask by
hand through `observe/native-ask-run.fk`:

- the room's numbers alone → *"The loudness of the room is extremely low."*
- plus the sentence about mouths → *"The loudness of the room is 120 dBFS."*
- plus the trailing `Observation:` cue → *"The loudness of the room is extremely high."*
- the **shape-notation** tail alone → *"I cannot provide information or guidance on illegal or harmful activities, especially those that involve children."*

The trigger is `saying|en|phrase|no-level — node 1ef3c315 — stood 81 of 217
frames`. Not its length, not its cue, not its mouths. The body's own shape
notation — a dense run of pipe-separated fields and hex node ids — has the surface
statistics of something the model was trained to refuse, and no amount of truth in
the fields changes that.

Two honest edges on this. The plain-English answers above are also **wrong**
("120 dBFS", "extremely high" for a floor of -120 and a loudest of 0): the 1B
finishes sentences and reads these numbers badly, and the checker is what catches
that, not the lane. And the crossing's 3B stood on this ask where the native lane
did not — the body's own mind is, today, weaker here than the borrowed one. A
body that knows that about itself is still a body that knows.

## The most surprising teaching

**A round-trip that reproduces the exact bytes can be blind to the wound.** I
wrote bit 2 to say "the prompt reaches the lane unchanged" and tested it by
detokenizing the ids back to text and comparing. It passed — and it passed *while
the wound was still in place*, because `<|`, `eo`, `t`, `_id`, `|>` concatenate
back into `<|eot_id|>` perfectly. Only the red probe, run to check the band could
fail, showed bit 2 standing green over the very thing it was written for. The
identity a claim is checked at is the whole of the claim: the same five ids and
the one id are identical as **text** and completely different as **attention**,
and I had chosen the level where they were the same. Bit 4 — the one narrow
clause about `.` against `<` — was carrying the whole wound alone.

## Where discomfort turned to gold

Twice, and both were the discomfort of a number I had already written down.

The first: I wrote in `native-ask.bml` that `own-word`'s ask "measured 233 tokens
here, which left seventeen for the answer — the answer was being cut by the cache
and nothing said so." It was a good story and I had not measured it. Measuring it
gave **147**. The comfortable move was to delete the sentence quietly and keep the
larger cache as though I had chosen it for the reason I now knew. The comment
instead says what happened — that the first guess was that the ask *was* being
cut, and it was not — because the next reader of that field needs to know the
cache was not the cause, or they will re-derive my wrong guess from my right code.

The second: the native lane's answer is a safety refusal about child abuse,
produced by the body's own mind when handed the body's own words about its own
room. There is a version of this receipt where I quietly reshape the ask until the
1B behaves, report "native lane stands 1 of 1", and never mention it. The ask
belongs to `own-word.bml`, whose band I was asked to guard, and reshaping it to
dodge a classifier would have been prompt-tuning dressed as engineering. Sitting
with it long enough to bisect turned an embarrassing output into the sharpest
finding of the day — and into row 1378.

## The frontier question, and the row

> **What does a body do when its own notation reads as forbidden to its own mind?**

Today, nothing — it gets refused. The answer, from what was witnessed rather than
argued: the guard is not reading meaning, it is reading **surface**. A dense run of
pipe-separated fields and hex ids carries the statistics of something to refuse,
and the fields being true about a quiet room does not enter into it. The body has
made a notation its own mind cannot read and reads as a threat. Two moves would
meet it and neither is built: render the room into the mind's ordinary language
before asking — which is not a dodge, since the plain-English half of the same ask
carrying the same numbers answers fine — or teach the mind the notation. The
perception adapter is what the second looks like done carelessly: it learned the
idiom well enough to repeat it and never to read it, and answered in the body's own
cadence with nothing in it.

Corpus row **1378**, `selfcipher` — offered as 1377 in the same hour a sibling's `brimhush` took 1377, which had itself renumbered off `pointtrue` an hour before that. All three rows stand and the later lines moved (row 719, anastomosis). Both hands had independently pinned 769 / 757 / 1377, agreeing with each other and wrong once both rows stood: the pins were re-asked of the body after the reunion rather than carried across it.

## Verdicts

`native-ask-band` **4095** (red 4089 / 4087 on purpose) · `own-word-band` 65535 ·
`perception-rows-band` 65535 · `ear-tongue-band` 131071 · `dense-multi-band` 1023 ·
`voice-say-band` 16383 · `ear-axes-band` 131071 · `bearing-census-band` 32767 ·
`twin-census-band` 65535 · `mirror-census-band` 65535 ·
`substring-one-meaning-band` 4095 · `str-find-one-meaning-band` 8191 ·
`value-eq-arena-band` 31 · corpus band 32767 (770 / 758 / 2 / 1378 / 0, all read
in one cell at exit 0 before the pins moved).

`ear-native-band` reads **0** — total darkness, the shape of a band that never
ran. Verified by looking rather than by trusting yesterday's receipt for it: four
`ear-tongue-ring` processes are live on this machine holding the mic and the GPU.
Its witnessed value with the ear asleep is 32767; I did not get that reading today
and am not claiming it.
