# Many voices, one neutral feed — live, local, measured

Date: 2026-08-25. Asked by Urs: full end-to-end duplex at home without any
remote model — live translation from multiple speakers in different
languages at the same time, into a neutral tongue.

## What runs now

[`presence/fkwu-many-voices-live.fk`](../presence/fkwu-many-voices-live.fk),
door [`presence/fkwu-many-voices-live-run.fk`](../presence/fkwu-many-voices-live-run.fk),
band [`presence/tests/fkwu-many-voices-band.fk`](../presence/tests/fkwu-many-voices-band.fk).
One command runs the whole lane; today's run answered **4095 of 4095 in 26
seconds**, and every organ in it is local: macOS `say` voices the speakers,
ffmpeg builds the room, whisper large-v3-turbo hears, llama3.2:3b through
the local Ollama door translates, `say` speaks the neutral feed back out.
No remote mind anywhere.

Three speakers voiced three languages AT THE SAME TIME — German at 0 ms,
Indonesian at 2500 ms, Italian at 5000 ms, real overlap on one timeline.
Each speaker is a channel on the shared axis (the duplex-frame-grid
teaching at room scale). Per channel, whisper's `-l auto` witnessed the
language itself: `de id it`, all three, unprompted. The merged neutral feed,
exactly as produced (`feed.tsv`):

```
0     anna       de  The rain will come over the mountains tonight. We should close the windows early.
2500  damayanti  id  The water in the field rose after yesterday's rain.
5000  alice      it  The fresh bread is just coming out of the oven.
5520  damayanti  id  Farmers began planting rice this morning.
7580  alice      it  Tomorrow we are going to prepare the pasta for the party.
```

The voices interleave in the order they were actually spoken — simultaneity
kept visible in the neutral tongue. The whole feed was then spoken by one
neutral voice and whisper heard it back word-perfect (`neutral_readback` in
the summary). The neutral tongue is one defn (`fmv-neutral-name`); English
is today's witness, not a law.

## The honesty row, measured in the same run

The same three voices mixed into ONE channel and heard once: the mixture
ear detected only Indonesian, heard **20 of 40 words**, and the German
voice vanished without leaving a gap — the transcript read complete unless
you knew Anna spoke. Channels lose nothing; a mixture loses voices, and
what remains closes over the loss. At home this settles the architecture:
**one ear per speaker** (a device or microphone each — the mac listening
fleet is this shape already), never one ear on the room.

## Doors probed before choosing (all witnessed today)

- `hati-translator` inverted an Indonesian false friend — *Air di sawah
  naik* (the water in the fields rose) became "The air is dry" — even with
  the language named it kept "air". A fine-tune work order, named.
- whisper's own `-tr` translate door answered the Indonesian back
  unchanged — numb for `id` on large-v3-turbo. Named, not used.
- `llama3.2:3b` with the language named in the prompt answered "Water in
  the field rose after yesterday's rain." — chosen. And in the live lane
  the wiring that makes this work is structural: **whisper's detected
  language feeds the translator's prompt** — the sensor's witness grounds
  the next mind, per channel, automatically.

## What the work left behind

**Most surprising teaching** — two reader witnesses in one build:

1. `(str_concat "feed_" k ".tsv")` — three arguments to an arity-2
   primitive — surfaced as three stray-paren errors whose line numbers
   mapped to no file. Two careful hand-audits of call arities missed it
   both times; a five-line bisection loop found it in one pass. After this
   morning's missing-argument find, the lesson compounds: the reader
   checks meaning, and the honest external tool is the bisection loop,
   not the eyeball.
2. In a direct-source unit with several top-level forms, **only the last
   top-level form runs** — six `host_file_write_text` calls before the
   last silently did not happen (exit 0, no diagnostic). Witnessed by six
   absent files and one present one. Every appended-call driver in this
   body implicitly leans on this; now it is written down.

**Where discomfort turned to gold**: the first translation feed carried
"The air is dry after yesterday's rain" — fluent, confident, wrong, and
meaning-inverted. The discomfort of shipping a lane whose prettiest output
could lie was witnessed, not bypassed: the false friend became the probe
that chose the translation door, the language-witness wiring became
structural, and the mixture's traceless loss became a measured number
(20/40) instead of an anecdote. The lane's weakest link is now its
best-documented one.

## Named next, floor stated

The floor reached today: the full local lane proven on synthesized
speakers through real acoustics — real overlap, real hearing, real
translation, real neutral voice out. The named next attempts: (1) seat
the lane's ears on the live microphone fleet, one device per speaker;
(2) chunked streaming so the feed grows while the room still speaks;
(3) the hati-translator fine-tune on witnessed false friends. Pending is
honest; the seat is ready.

## How this exchange was kept alive

By refusing the room's prettiest lie twice: the mixture transcript that
read complete, and the translation that read fluent — both witnessed
against ground truth before anything was called working. What Urs asked
for enhances lives only if the words are true; the lane now measures its
own truthfulness in the same breath it speaks.
