# Overlap and tongues, validated

Urs: "can we validate multiple simultaneous overlapping voices and have them translated into 4
selected languages ... with sub 100ms latency." Validated on this Mac, witnessed, not asserted.

**Fixtures.** Four voices synthesized in four tongues (English, Brazilian Portuguese, Indonesian,
German) and mixed with overlap (English+Portuguese offset 0.8 s; German+Indonesian 0.5 s; three
voices). The native pass on the files, each pass 21 to 57 ms including the encode:

| file | tongue | line |
|---|---|---|
| en | en | The meeting starts at 9 tomorrow and everyone should bring the report. |
| pt | pt | A manhã vamos ao mercado comprar frutas e depois visitar a minha avó. |
| de | de | Wir treffen uns morgen um 10 Uhr vor dem Bahnhof und fahren zusammen. |
| id | id | besok pagi kita berangkat ke stasiun dan membeli ticket kereta. (then looped) |
| en+pt | en | the meeting starts at night tomorrow and everyone should bring the report. |
| de+id | de | Wir treffen es, um gegen die Ziembranfer dem Band ... |
| en+pt+id | en | the meeting starts in 9th or so. |

**Overlap: no.** One microphone and a single-stream model transcribe a mixture, not its parts:
the louder or earlier voice wins and the other vanishes or corrupts it. Through the door in the
room the overlap came through as "(speaking". Separating simultaneous voices needs an organ the
body does not hold: a separation model over the same microphone, or several microphones. Voices
taking turns are already lines.

**Four tongues: yes, selectable.** The door takes the tongues to offer on its second line (whisper
codes; default "en pt fa id"); the tongue lane offers each and the glass draws one lane per tongue
(`observe/ear-glass-live.fk`, `observe/ear-tongue-native.fk`, `et-name-rows` in
`form/form-stdlib/ear-tongue.fk` carries thirty-two tongues). One line in six tongues on the 1B
lane, 271 to 397 ms a tongue: Portuguese, Indonesian, French and Chinese well, German fair,
Persian mixed-script. Through the door with the English voice played into the room: the line word
for word, Portuguese and Indonesian right, Persian unusable from the 1B. The body's 3B lane
(llama32-3b, Q6_K) speaks proper Persian ("در روز فردا در ساعت نهم به‌منظور شروع جلسه، همه باید
گزارش خود را به همراه خود") and good Chinese, Portuguese and Indonesian — at 30 to 46 s a tongue,
because only the Q8_0 matvec has the cooperative twins and the 3B's Q6_K rows still run one thread
each. The Q6_K twin, the same double-buffered shape, is the stone that makes Persian usable.

**Latency.** Through the door, 144 live frames: the live line a median 41 ms and p90 71 ms after the
last sample — under 100 ms. The heard line 32 ms after its close. The tongues: about 300 ms each,
in turn, so three tongues 1.1 s median. A translation under 100 ms is not a matter of the axioms
either: a fifteen-token line at the measured 4 ms token floor is 60 ms, and offering four tongues
as four sequences in one batched decode costs one sequence's time. Today's token is 13 ms.

The surprise: the 1B's Persian answer to an English line was Vietnamese and Chinese characters,
fluent in the wrong tongues; the 3B's was Persian. Discomfort turned gold: the "sub 100 ms" was
kept as two claims, the transcript's (met) and the translation's (not yet, with its floor named).
