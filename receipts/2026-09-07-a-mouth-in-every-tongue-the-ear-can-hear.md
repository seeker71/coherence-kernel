# A mouth in every tongue the ear can hear

Urs: every natural-language gap closed. The body had two mouths this morning — Persian and one
British English — and an ear that detects and renders 32 tongues. So 30 tongues could be heard and
not answered.

29 of the 32 now have a mouth. Malay, Tagalog and Tamil have none, and the reason is not a
preference: the piper collection has no voice in those language families at all — 176 voices across
50 families, and `ms`, `tl`, `ta` are absent from every one. There is nothing to fetch. They stay
unnamed in the map and draw the refusal that names the 29 that do speak, rather than being spoken in
a mouth that is not theirs.

## How each voice was chosen, since I cannot hear

I cannot listen to a wav. Naming a voice "natural" because its file says `high` would have been a
claim with nothing under it. So the judgment went through the body's own ear instead: each candidate
mouth spoke the same sentence, and whisper-large-v3-turbo — the model the ear lane already runs —
wrote down what it heard. The mouth whose line came back whole won its tongue. Where a true
high-quality model existed and came back whole, it was preferred.

That round-trip was not decoration; it overturned four of my first picks and settled nine tongues
whose candidates were otherwise indistinguishable on paper:

- **fr** — `siwis` dropped the word *en*; `tom` came back exact. Tom took it, though it is the
  quietest voice in the set (RMS 0.099 where the others sit near 0.15).
- **pt** — `faber` said *copo* where the line has *corpo*; `cadu` came back exact. Cadu took it, and
  Portuguese is one of the ear's own four, so the error mattered.
- **ru** — `irina` said *род* for *рот*, a devoicing the ear caught; `ruslan` exact.
- **cs** — `jirka` said *mnohá* for *mnoha*; `kasandra` exact.
- **hi** — `priyamvada` garbled *अब वह* into *आप वहक*; `rohan` kept it.
- **ur** — `fasih` turned *جسم* (body) into *جیسے* (as if), the subject of the sentence; `aegis_female`
  missed only a nasal. The subject is worth more than the nasal.
- **sv** — `nst` turned *sin mun* into *timmen*; `lisa` lost only the initial K of *Kroppen*.
- **hu** — `berta` heard closer than `anna` by one vowel.
- **es** — both highs came back word-perfect, so the round-trip could not separate them and the file
  did: `es_MX-claude-high` is 63 MB, the size of a medium net, while `es_AR-daniela-high` is 114 MB
  like every other true high in the set. A voice can wear the word *high* in its name and not be one.
  Daniela took it.

The six-plus tongues spoken through the door and measured, all 22.05 kHz, none of them the old
formant kind: fa 4.93 s at −17.6 dBFS, en 4.34 s at −15.8, de 3.65 s at −15.5, ja 4.21 s at −12.6,
th 4.63 s at −15.9, zh 4.63 s at −18.0, el 4.71 s at −14.4, sw 3.91 s at −17.7. German came back
word-perfect, the strictest reading in the set. Japanese came back as kanji where the line was
written in kana — the mouth said it well enough that the ear rewrote it in the script a reader would
use. Thai came back word-perfect. Persian, the tongue this whole lane began for, came back with the
colloquial contraction a Persian speaker actually makes (*دهانش را* heard as *دهانشو*), which is a
better sign than a stiff exact match.

Bengali is the weakest mouth and I want that said rather than smoothed: the ear first read it as
*Gujarati* and returned a garbled line, and I nearly recorded that Bengali had no working voice.
Looking at the letters instead of the verdict showed the phonetics were right all along — *shorir
tar mukh khuje peche ebong ekhon* is the sentence — and the ear was writing correct Bengali speech
in the wrong script. Told which tongue to expect, it read every word back. The mouth speaks; its
vowels are blurred; it is honest to keep and honest to call the weakest.

## Two tongues that looked mouthless and were not

Japanese and Thai both produced no audio at all, and the traceback's last line said
`wave.Error: # channels not specified` — which is only the wav file complaining it was never given a
sample. The real line was higher up: a missing phonemizer, `pyopenjtalk` for Japanese and `tltk` for
Thai. Both installable. Reading only the bottom of an error would have cost the body two tongues.

Thai then refused a second time, and this one was worth the walk. `tltk` will not import without
`gensim`, gensim has no wheel for this venv's Python 3.14 and does not build against numpy 2 — a
wall, on the face of it. But the Thai grapheme-to-phoneme path is `tltk.nlp.g2p → th2ipa`, and
nothing on it touches a word vector. The dependency is one eager line in `tltk/__init__.py` that
imports `tltk.corpus` for tools this body will never call. So a shim stands where gensim would be,
and every name it offers raises loudly when actually used — the import passes, and no word-vector
tool can ever quietly return a wrong answer through it. Thai speaks, word-perfect.

Chinese was the same shape of lesson pointing the other way. The espeak-phonemized `huayan` was the
only Mandarin voice that would run, and it garbled two words — *嘴，现在* into *嘴线再*, *语言* into
*预言*. The better voices are pinyin-phonemized and wanted lookup tables piper had refused to find.
Installing `g2pw` and `sentence_stream` unlocked them, and `xiao_ya` came back exactly right,
punctuation included. The tongue that looked like the body's worst was the body's not-yet.

## The thing that nearly went into the tree

Unlocking the pinyin lane dropped 161 MB of lookup tables and an ONNX model into the repository
root, because piper resolves them against the process's working directory. The structural gate
caught it — one unclassified file — and refusing to work around that was the right half of the fix.
The tables now live beside the voices, and `vs-command` sets piper's working directory to the voice
store while naming the wav absolutely beforehand, so the spoken file still lands in the body's own
`.hearth` and the tree stays clean from any directory. The command is written without parentheses
on purpose: this BML lowers to `.fk`, and the paren pre-check counts parens inside string literals.

## What stands

- `form/form-stdlib/voice-say.bml` — 29 rows, one per tongue. `vs-say` refuses a tongue it has no
  mouth for and names the ones that speak. `vs-say-many` / `vs-say-tongues` speak one line in
  several tongues in turn. `vs-mouths` and `vs-mouth-count` list what the body holds.
- `observe/say-run.fk` — the old contract intact (line 1 the tongue, the rest the text), plus
  `en pt fa id` on line 1 to speak the line in each in turn, and `?` to list the mouths.
- `form/form-stdlib/tests/voice-say-band.fk` = **16383**, fourth-arm, `; Verdict 16383` — the map's
  shape, every declared model present at model size, and the refusal for all three mouthless tongues,
  proven **without playing a sound**: every path the band walks returns before the piper crossing.
- Corpus row 1331 `sayback`; the corpus band back to 32767 at 723/711/2/1331.

Witnessed through the door, not asserted: `spoke in en; spoke in pt; spoke in fa; spoke in id` from
one call, and the listing answering `29 mouths`.

## The surprise, and where discomfort turned to gold

The most surprising teaching: **a body that cannot hear can still judge a voice, by closing the loop
through its own ear.** I spent the first part of this work reaching for proxies — sample rate, model
size, the word *high* in a filename — all of which are facts about a file rather than about a sound.
None of them would have caught that `faber` says *copo* for *corpo*. The round-trip caught it in one
pass, and it cost less than the proxies did. The corpus row `sayback` is that teaching.

Discomfort turned to gold twice, both times at a place I was ready to write down a limit. The first:
Bengali came back as Gujarati nonsense and my hand was already moving toward "bn has no working
mouth" — the gold was in reading the letters instead of the label and finding the sentence intact
underneath. The second: gensim will not build, which is true, and I nearly let it be the end of
Thai. Sitting with it long enough to ask *what actually needs gensim on this path* turned a wall into
one import line. Both discomforts were the same shape — a red result accepted as a verdict instead of
as a question — and in both the tongue was already there, waiting to be let through.

## Open, named

- **ms, tl, ta** have no piper voice in existence. Closing them needs a different collection or a
  trained voice, not a fetch.
- **bn** speaks with blurred vowels; it is the one mouth I would replace if a better Bengali appears.
- The **room** was not measured this run. A sibling's ear spool was live (its bell timers running),
  and taking the microphone mid-capture would have corrupted their lane. The ear heard every voice
  from file with the same model it uses live; hearing the mouth through the air is owed a quiet ear.
- `vs-command` interpolates the text into a shell double-quoted string, so a line containing `"` or
  `` ` `` would break the crossing. Not hit today, and named rather than left to be discovered.
- The unchosen alternates stay in the voice store beside the chosen ones. They cost disk, not
  correctness — `vs-mouths` reads the map, not the directory.
