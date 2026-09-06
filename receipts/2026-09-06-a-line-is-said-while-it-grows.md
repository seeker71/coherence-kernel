# A line is said while it grows

The gap: the live transcript lane handed a line to the tongue lane only when it closed, and the
tongues came ~300 ms each, one after another; the glass showed a tongue only then. Now the live
lane hands over the line as it stands on every pass that changes it, the tongue lane renders every
tongue from what its previous rendering committed, and the glass draws the renderings while the
line grows. Witnessed through the door (`observe/ear-glass-live.fk`, 20-22 s, "en pt fa id") with
speech played into the room: `say -v Samantha` fixtures ("The meeting starts at nine tomorrow and
everyone should bring the report." / "Tomorrow morning we will go to the market to buy fruit and
then visit my grandmother.") through `sox` to 16 kHz mono and `afplay`, with a 5 s lead. This
M4 Max; the GPU shared with sibling sessions (see the last section — it matters).

## What moved

**The live lane** (`observe/ear-live-native.fk`): every pass that changes the live line appends
`live\t<seg>\t<tend>\t<lang>\t<text>` to `.hearth/ear.spool.segments` beside the closed lines
(`<tend>\t<lang>\t<text>`, unchanged) and rings both bells in one fork; `seg` is the segment's
cut, so the tongue lane knows a new line from a grown one. The unloop, which kept a line up to its
first repeated pair of words, now catches a repeated span of one to twelve words (a word's closing
mark not counted; a single word only when said three times): "The meat is made of meat. The meat
is made of meat. The" → "The meat is made of meat. …". The tiny model's sentence loops were the
tongue lane's whole cost on two runs.

**The tongue recipe** (`form/form-stdlib/ear-tongue.fk`): `et-say-ids` — the prompt's ids then a
forced prefix, prefilled only past what the cache holds, up to `maxnew` new ids; `et-say-step` —
local agreement: what this rendering shares with the previous one is the committed prefix, forced
next time. And the batched round over dense-multi (`et-mlane-open`, `et-mround-go`,
`et-mround`): every tongue its own sequence and KV bank, fed its prompt only past what its bank
holds and then its own answers in lock-step, resumable in chunks so the caller can show the
renderings as they grow; refused outside dense-multi's radius, and then the tongues go one after
another.

**The tongue lane** (`observe/ear-tongue-native.fk`): the mark is taken before the model opens
(a line handed over during the ~2 s open was lost before); the live line is rendered in one
batched round (or one tongue at a time), a `saying` frame whenever a rendering changed — every
four steps of a batched round; between rounds the newest live line is taken; a closed line is
rendered from what the live rounds committed — from the whole previous rendering when the text is
the one already rendered, greedy decoding being one answer to one prompt — and lands the `said`
frame; a line closed again (whisper re-closes the same text two or three times) lands its frame
again without paying; a line that is only whisper's mark for what is not speech ("[Music]",
"(mouse clicking)") is shown as it is and not offered through the lane; the loop marker " …"
stays on the frame and off the prompt (the 1B answered a marked line in English).

**The glass** (`observe/ear-glass-live.fk`): one lane per chosen tongue from the newest `said` or
`saying` frame, dim while the line grows and plain once it closed; the latency lane reads
"saying N ms" or "said N ms" from the line's last sample.

## Measured through the door

Before (the sibling's run at 17:30, `.hearth/ear.spool` of the epic-edison worktree, the
tongues in turn after the close): a real line's tongues appeared 4354 / 5439 / 7105 ms after its
first live frame — the `said` frame, 849-3078 ms after the close.

After, tongues in turn, the GPU quiet (13 ms a token on the single lane, run 6):

| line | first live → first words on glass | after the words' last sample | said after close |
|---|---|---|---|
| "The meetings starts at 9 tomorrow and everyone should bring the report" | en +12 ms · pt +350 · fa +631 · id +925 | 58 / 396 / 357 / 396 ms | 1567 ms |
| the 8 s window holding both sentences, growing | pt +2004 · fa +2323 · id +2625 (the lane still on the line before) | 479 / 500 / 802 ms | 2345 ms |
| "Tomorrow morning we will go to the market to buy fruit and visit my grandmother." | +2261 / +2790 / +3183 / +3679 (behind the line before) | — | 2618 ms |
| "[Music]" and its kin (six) | — | — | 15-26 ms |

While a line grows the renderings refresh a median 500 ms and p90 1.8 s behind the words (39
`saying` frames); the live line itself a median 36 ms. The renderings held still as they grew —
"Assembleia começa às 9 am de amanh" → "Os encontros começam a 9 am de amanhã e todos devem
trazer o relatório." — where a full re-say flickered ("A reunião" → "O encontro" on the probe).

After, the batched round (run 7, and the lane alone on a hand-made segments file): pt and id
grow together, four steps a frame — "A re" / "Perh" → "A reunião começa" / "Perhentian ini dim"
→ "A reunião começa às 9 da manh" / "Perhentian ini dimulai pada pukul". This run's GPU was at
108 ms a step against the sibling's 13.5 (below), so its frames came 300-650 ms apart where the
quiet lane would give four steps in 54 ms; the closed line's `said` landed and every tag cost
15-26 ms.

## What the ~100 ms asks and what stands between

A tongue's first words need its prompt fed — the source tongue's name, the line, the tongue's
name, the committed prefix — and the batched lane feeds one id a step for every sequence at
once, 13.5 ms a step on a quiet GPU: a fifteen-word line is ~20 steps, ~270 ms, before the first
generated id, plus 54 ms a chunk of four. The committed prefix is fed again on every round
because the source grows in front of it (the prompt shape, not the lane). Under ~100 ms for the
first words of every tongue needs the prompt fed many ids a dispatch (a batched prefill, which no
lane has) — then a round is the generation alone, ~13.5 ms an id for all tongues together.

## Band

`form/form-stdlib/tests/ear-tongue-band.fk` 255 → 4095: two bits for the streaming step (the
agreed prefix is a prefix of both renderings; a step from it renders text beginning with the
committed text) and two for the batched round (a sequence's ids equal to the single lane's; a
grown line through warm banks equal to the same round through cold banks).

## The GPU is shared

Between this session's first probe (20:30) and its last (21:20) the single lane went from 10-13
ms a token to 104-109, and
the sibling's own timing cell (`form/native/metal/dense-multi-run-llama1b.fk`) read 129.5 ms a
step at M=1 against its receipt's 13.0 — `ioreg`'s "Device Utilization %" stood at 100 with
nothing of this session running. Every number above names its regime. A sibling's
`dmstep-probe.fk` and `dense-multi-probe.fk` were on the GPU during two of the probes; what held
it at 100 % afterwards was not found (no python, mlx or ollama process; the fleet's glass panels
are the standing suspects). The live lane's whisper passes slow with it too: 31 live frames in
20 s where the quiet room gave 98-139, and worse hypotheses ("The meeting starts at 9pm").

## Surprise

The first door run put the tongues 7 s behind — not the translation: the tongue lane's mark was
taken after a 2 s model open, whisper closed the same sentence three times, and each duplicate was
re-said in full. The second run's cost was whisper looping whole sentences into 100-token sources.
The third's was the room: this Mac's fans under GPU load gave whisper-tiny ten "[Music]" tags
in eight seconds of silence, and the lane translated each in three tongues. None of it was the
streaming; all of it stood between the words and the glass.

## Discomfort to gold

The batched round's first timing was 140 ms a step — ten times the sibling's receipt — and the
easy reading was that my lock-step loop was wrong. Running the sibling's own timing cell in the
same minute gave the same 140, and the single lane 104: the GPU, not the loop. The loop's ids were
already equal to the single lane's; the number that looked like a defect was a shared machine.
And LocalAgreement's worth here was not the tokens it saved — the probe showed a step costs about
what a full re-say costs, since a forced prefix is prefilled at nearly a generated token's price —
but the glass holding still: a rendering that grows instead of flickering.

## Open

A batched prefill (many ids a dispatch) is the floor under the first words; dense-multi's owners
name the step as the door. The live lane still re-closes a sentence two or three times after its
decoder end (the tongue lane pays nothing for it now, the glass shows it). The 1B's Persian is the
model's. Which process holds the GPU at 100 % on this Mac is unwitnessed.
