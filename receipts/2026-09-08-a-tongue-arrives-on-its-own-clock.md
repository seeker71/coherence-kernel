# A tongue arrives on its own clock

The tongue lane could say how long a line took to reach four tongues. It could not say how long any
one of them took. `saidms` is the frame's own clock, and the frame lands when the last tongue lands,
so the number the glass showed had always belonged to the slowest — English standing complete was
invisible behind Persian still forming.

This session gave each tongue its own clock, and then used it to find where the milliseconds were.

**The weather, with every number.** `observe/floor-lens-run.fk`, this hour: **412.81 GB/s** through
the handle door against a best of 412.81 — a quiet machine — and **25.77 TFLOPS**, with the 1B's
token at **12.67 ms** and the 3B's at **29.55**. The earlier receipt's hour read the same bandwidth
with a **8.67 ms** token, so absolutes here run about half again longer than that day's for the same
work; what carries across hours is the shape and the before/after, which are from this hour and the
same feed.

## The room found the wounds, not the fixture

A sibling's ear was standing in the neighbouring checkout with a live microphone. Reading its spool
back — 112 said frames of a real room — three things stood out that no fixture had ever produced:

| what the room said | what my lane answered |
|---|---|
| `You have basically, you …` | `fa=شما … به عنوان someone به عنوان someone …` about thirty times |
| `the body is a body of the body of` | `fa=BODY BODY BODY BODY BODY BODY BODY BODY` |
| `I'm a little bit of …` | `fa=I'm a little bit of` — the English handed back |
| `[INAUDIBLE] Like Jill, actually, will go over her. [INAUDIBLE]` | shown untranslated in all four tongues |

The first two are one wound and it is the streaming commitment's own doing. Two consecutive
renderings of a growing line commit the prefix they share, because agreeing across *different*
sources is evidence the prefix does not depend on what is still being heard. A greedy loop agrees
with itself perfectly. So the loop is committed, forced on the next round, and grown again — and
`tl-live-new`, the cap on new ids, bounds the rate a loop grows and never its total. A loop entered
was a loop kept.

The fourth is smaller and worse: `tl-tag?` asked only whether the first and last bytes were a
bracket pair, so a real sentence that happened to open and close with `[INAUDIBLE]` was filed as
non-speech and never offered to the lane at all.

Both are healed in the tree and both are in the band: `et-loopcut` walks a tail that is three copies
of one block (1 to 8 ids, shortest wins) back to the block's first copy and keeps that; `et-tag?`
requires the opening bracket's own close to be the line's last byte. Three copies rather than two,
because a real sentence says "no, no" and does not say it three times.

## Each tongue's own clock

A tongue's state now carries `doneT`, the clock its rendering stopped at, and every frame writes
`d_<code>` — that clock as ms from the line's end — beside the `saidms` that belongs to the slowest.
A tongue still forming writes nothing rather than a number.

`observe/ear-tongue-said.fk` prints the table two ways and they attest each other. The second way
needed no lane change at all: the first frame carrying this line's end whose `t_<code>` already held
the words the said frame carries. That is how the arrival was read by hand in the last receipt, it
works on any spool ever written — which is how the room's wounds above were found — and where the
lane's stamp and the derived reading disagree, the disagreement is the finding.

## What the clock found: 208 ms spent on words nobody reads

A real sentence, fed as the ear feeds one:

```
line  You have basically, you know, a body that observes the glass and the water and the ice.
```

| | before | after |
|---|---|---|
| committed | +0 ms | +0 ms |
| **en** | +110 ms | +106 ms |
| **id** | (+563 derived, no stamp) | **+505 ms** |
| **pt** | +689 ms | +629 ms |
| **fa** | +690 ms | +630 ms |
| **the frame** (`ear.said`) | **+898 ms** | **+632 ms** |
| the four tongues themselves | | byte for byte the same |

Before, Indonesian never stamped an arrival, and the derived reading said why: its words were final
at 563 ms and its round ran to 898. It was generating, and the text was not changing.

`et-cut` takes a rendering at its first line break or the file's own `<|` marker, because the prompt
asks for one line. `et-ends?` knew four ids. **A line break arrives inside a piece far more often
than as one of those four**, so the round did not notice the line had ended and went on writing a
second paragraph that the cut then threw away. The whole gap between the last tongue's words and the
frame was that. A rendering the cut has already reached is now ended — whatever the model's own end
ids say — and the round ends at the chunk every tongue is done in.

The held line, for comparison with the last receipt's 557 ms on its faster token: **627 ms**, with
Portuguese at 435 and Indonesian at 436 — 190 ms before Persian's 625 — and the same line closed
again free at **45 ms** and **33 ms**.

The stamps land while the line is still being said. From the closed round's own frames:

```
saidms=442   d_en=106
saidms=505   d_en=106  d_id=505
saidms=569   d_en=106  d_id=505
saidms=631   d_en=106  d_pt=629  d_fa=630  d_id=505
```

So yes: a tongue reaches the glass without waiting for its slower siblings, and now says when it
got there.

## The two readings disagreed by one chunk, and that was honest

On the held line Persian stamped **+625** and derived **+562**. Its words stood at 562; it was done
at 625. In between it generated ids that the cut removed, until the break itself arrived and ended
it. The stamp answers "when was this tongue finished", the derived answers "when did its words first
stand", and on a tongue that trails past its own sentence those are not the same question.

## Should the tongues split across two models by tongue?

Measured rather than argued, both models warm in one process on the same three real lines
(`observe/ear-tongue-two-models.fk`):

| line | 1B | 3B |
|---|---|---|
| `The body observed the glass, water and ice and gas.` fa | `به‌صورتObservation، جسم به‌صورت آب، آب‌فروشی و گاز observierte.` | `بدن به شیشه، آب و یخ و گاز توجه کرد.` |
| the same, pt | `O corpo observou o vidro, a água e o gás.` | `A corpo observou o vidro, água e gelo e gás.` |
| `Water and ice and gas.` id | `air, air, air` | `Air dan es dan gas.` |
| `…observes the glass and the water and the ice.` pt | `…observa a gás e a água e o gelo.` | `…observa o vidro, a água e a geladeira.` |
| cost, one tongue at a time | 288–492 ms | 1255–1907 ms |

The Persian is the loud one — the 1B's carries an English word, a German verb, and
`آب‌فروشی`, "water-selling", where ice belonged; the 3B's is a plain Persian sentence. But the
answer to the question asked is **no, and the axis is wrong**: the 1B is not weak in Persian, it is
weak in all three. It says `a gás` for the glass and `air, air, air` for a five-word line. Moving
Persian alone to the 3B would leave the other two saying that. What stands between this lane and
words worth reading is one refusal — `dense-multi: refused at open: a layer outside the fused
block's radius` — and that lives in a file this work does not own.

## Bands

- `form/form-stdlib/tests/ear-tongue-band.fk` = **131071**, up from 16383. Three new bits: the
  cycle cut and its fixed point (16384), whisper's mark told from a line that merely opens and
  closes with one (32768), and a rendering the line-cut has already reached (65536).
- `learn/tests/homecoming-distillation-corpus-band.fk` = 32767 with the pins moved.

## What still needs a hand

- **A microphone did not close this loop.** A sibling's ear held the one mic for this whole hour, so
  I read its room rather than standing my own and drove my lane from segment lines — the same lines
  a live ear writes, carrying real sentences instead of the fixture. The wounds are the room's; the
  milliseconds are the feed's.
- **The 1B's words.** Every number above is about *when* a tongue arrives. What it says when it gets
  there is measured in the table above and it is not good, and the 3B door is closed at
  `dense-multi`'s open.
- **`et-ids-cut?` decodes the raw ids a second time each chunk** to ask whether the break has
  arrived. On this feed that cost is inside the noise of a 63 ms chunk and the total fell by 266 ms,
  so it is paid for; on a much longer rendering it would want the cut carried rather than re-asked.
- **The lane re-stands itself already** — `observe/form-glass-ear-live.fk` stands it through
  `sh -c 'exec ./fkwu … </dev/null >> <log> 2>&1'` and counts `ear.stands` — so nothing here is
  scaffolding run by hand. The `d_<code>` fields reach the frame; painting them as a row beside
  `ear.tongue.<code>` is one line in `ear-axes.bml`, which is the listening half's file this hour.
- **`observe/ear-tongue-native.fk` compiles with one error under preflight**, on main as well as
  here — it comes in through the prelude chain, not from this work, and the lane runs. Named, not
  chased.

## The frontier question

*How does a body tell that a rendering is finished, when the thing that finishes it is not the thing
it was watching for?*

The lane watched for four end ids. The rendering was actually finished by `et-cut`, one layer up,
and the two never spoke. So the round kept spending the machine on tokens that a function three
lines away was already throwing on the floor — and the cost was invisible precisely because the
*output* was correct. Nothing was wrong with what the tongues said. Only with how long the body
stood there saying it.

The answer, found by using it: **ask the consumer, not the producer.** The honest end of a rendering
is the condition under which its reader stops reading, not the condition under which its writer
stops writing — and when those are two different tests in two different cells, the writer will
always run past. So a lane that cuts its output somewhere should end its generation *by the same
test*, and the cut is the one that carries the authority.

Offered as corpus row 1368, fresh word **cutkept** (zero hits before it was written).

## Closing

**The most surprising teaching.** The wound that cost the most was not in the part I was asked to
speed up. I came to make the tongues arrive independently, built the clock that shows it, and the
clock immediately pointed somewhere else: at a round generating a paragraph that a `substring` call
was already discarding. The instrument found a bug that the instrument was not built to find,
because it measured the one thing nobody had measured — not the whole, but each part of it.

**Where discomfort turned to gold.** Indonesian showed `—` where every other tongue showed a number,
and my first reading of my own new field was that the field was broken. Sitting with the blank
instead of patching it gave the whole finding: the blank was correct, the tongue genuinely never
ended, and *that* was the defect. A missing number that means "this did not happen" is worth more
than a number that means "I do not know" — and I nearly made it the second kind. The second
discomfort was the two readings disagreeing by 63 ms on Persian, which I wanted to call a rounding
seam; it is two different true answers to two different questions, and both are now printed.

**How the exchange stayed alive.** By going to the room before writing anything. The offerings I was
handed were all reasonable and one of them — "each tongue could reach the glass the moment it is
done" — turned out to be mostly true already; the saying frames had been carrying each tongue's
growing words all along. Had I built it from the argument I would have shipped a change that changed
nothing. The room, instead, handed me three wounds nobody had named and the 208 ms that were really
there. And the one thing I could not witness — a live microphone — is named as an absence with the
sibling who holds it, not rounded off.
