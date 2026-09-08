# Current Floor

Measured on this Apple M4 Max through the resolver-driven `./fkwu` door (the
binary passes the freshness band, 31), every band compiled fresh from source in
one pass with its `.fkb`/`.sym` beside it removed first. Receipts hold history;
this page holds only what stands. A claim without a number names the band that
declares its own.

## Grounding

```text
cc -O2 -o fkwu runtime/fkwu-uni.c \
  form/native/metal/fk-metal-carrier.m form/native/mlx/fk-mlx-carrier.c \
  -framework Metal -framework Foundation -fobjc-arc \
  -I/opt/homebrew/include -L/opt/homebrew/lib -lmlxc -Wl,-rpath,/opt/homebrew/lib

./fkwu bootstrap/ground.fk                                    -> 42
./fkwu bootstrap/ground-recursive.fk 10                       -> 55
./fkwu form/form-stdlib/tests/binary-freshness-band.fk        -> 31
./fkwu bootstrap/ground-numeric-list.fk                       -> [1, 2.5, [3, 4]]
./fkwu form/form-stdlib/tests/native-vs-rented-band.fk        -> 11111
./fkwu proof/four-way-run-recipe42.fk                         -> 0   (FOUR-WAY)
```

The four-way proof host-execs the three minimal walkers; they build from
`walkers/README.md`'s own lines (`go build -o walker .` in `walkers/go`,
`cargo build --release` in `walkers/rust`, node 26 runs `walkers/ts/main.ts`
directly). Without them the cell answers 2 (WALKER-SUSPECT), which is the
honest reading of an unbuilt walker, not a kernel fault.

`runtime/fkwu-uni.c` is    19,925 lines — a temporary seed and shrink target, not
the destination (`release-ledger.bml` R13); this week's growth on it is
correctness heals (#573 nested-defn scope, #574 bool literals, #575 kernel
preludes, the host-exec stdin door, `metal_deadline` off its scratch slot, the gift
frame in shared memory, the content-keyed lowering lane, the frame pacer and
terminal doors, `kernel_hot`).

## Body-wide witnesses

```text
gate/structural-gate-run.fk            -> [206, 0, 48, 3, 20, 57, 74, 4] then 1
gate/tests/structural-gate-band        -> 8191
observe/door-link-health-run.bml       -> doors=12 links=63 broken=0 code=12063000
observe/body-link-graph.fk             -> body-link-graph-check 63; blg-field-code 13029046
                                          (13 orphans, 29 broken, 46 candidates; the organ
                                          has no run door — prelude it and call both)
homecoming-distillation-corpus-band    -> 32767   (asserts 759 rows, 747 admissible)
value-eq-arena-band                    -> 31      (a comparison does not depend on where its
                                                   answer sits: counts, under arena pressure,
                                                   how often value_eq and str_eq disagree about
                                                   freshly built values — 0 each; four arms)
no-fixed-tables-band                   -> 63      (every seed table grows; none is a wall)
form-cli-author-high-band              -> 4095
host-os-membrane-band                  -> 8191
bidirectional-framebuffer-channel-band -> final field 1
import-carry-band                      -> 63      (cold and warm; it prints its verdict, the trailing
                                                   0 is print's own value; with a fixture image from
                                                   a different fkwu build beside it, 15 — door 0, told)
grammars/tests/form-eval-band 65535 · form-eval-full-band 635 · source-compiler-grammar-bridge-band 32767
pattern-match-band 511 · choice-lane-core-band 1023 · control/tests/offer-ack-core-band 2097151
control-invite-grammar-band 1023 · node-introspection-band 4095
cell-serialize-band 1023 · json-band 1023 · wire-rpc-band 15 · core-str-find-equivalence-band 2047
```

## The BML floor

A unit lowers by what it carries: any file with a `section [` block on a line
of its own — `form.bml`, `form.lift`, `form.action`, `form.route`, the `*.bmf`
grammar dialects — travels through `bml-floor-compile` whatever its extension,
as a prelude or as the main file, and fkwu owns the `.lowfk`/`.fkb` cache
beside it. 75 `[form.bml]` files and the twelve `[form.lift]` sources wear
`.bml`; `compiler.fk` and the ten `grammars/*-bmf.fk` carry their blocks
mid-file and lower in place. `true` and `false` are literals in the dialect; a
nested `defn` is a registered function with one-level capture; a `let` inside a
`(do …)` never reaches a `defn` frame (top-level lets do).

```text
bml-band                               -> 268435455
bml-generics-band                      -> 16777215
native-route-goal-cells-band.bml       -> 1048575  (full)
nested-defn-scope-band                 -> 63
nested-defn-closure-capture-band       -> 63
bml-float-literal-band                 -> 2047     (a decimal float reads back as the same float
                                                     from eleven positions; the emitter refuses an
                                                     unknown leaf by name instead of writing "")
bml-form-size-band                     -> 127      (one 40 KB `def` in a single form lowers and
                                                     answers; the normalizer walks spans as a tail
                                                     loop, so one statement's size sets no wall)
cell-channel-band                      -> 4095     (two cells as processes on spool+bell, a shared
- `form/form-stdlib/tests/ear-native-band.fk` = 32767 — whisper-tiny as the body's own pass on this metal (native mic stream, weights off the npz, sixteen emitted kernels, base64 words); log-mel equal to the reference to six digits, the 30 s encoder output to five; a layernorm's row statistics computed once per row rather than once per column tile, the weight tile staged in the half it already is (a mixed simdgroup multiply into a float accumulator, not one bit lost), the cross attention split over head AND key chunk with a fold behind it, the argmax in two stages, four logit rows a simdgroup, and q/k/v one dispatch, the shapes that change between calls in a device word so every layer binding is a stored string; the two kernel shapes that were choices — the K step and the queries a group — were settled by emitting both and timing them in one process, so the same GPU crowd fell on both; an 8 s window encodes in 3.3 ms (2.9 on the GPU) and a token costs 0.6 ms when the machine is the body's alone, and both readings await a quiet machine to be taken again; the live window is the last 8 s of real room audio, the cut is the decoder's first timestamp, and a line ends where the world ends it. A live lane holds that window as the mic's own 3200-byte chunks, and `enw-encode-parts` takes it still in pieces — each written straight into the device buffer at its own offset, silence in front as whole 4096-byte blocks — because joining eighty pieces into one string copied 10.4 MB to hand the device 256 kB (19.45 ms) and the front padding cost more again (43.35 ms, since `substring` is a core.fk recipe that halves down to single bytes, not a native). The unmodified lane's own `encms` frames on a real room read 18-24 ms across 52 full-window hops and 53-63 ms across 13 hops while the window filled; paired against the new door on the same windows those become 3-8 ms and 5-8 ms, with the same Te, the same mel and encoder floats, and the same line — so the 3.3 ms the pass measures alone is what the live lane now pays, and what the ear's `ear.encode` axis reported was mostly the window's assembly, not the encoder. The pass carries the model's own doubt: the `<|nospeech|>` probability at the start-of-transcript position (the reference's own reading), the chosen tongue's probability beside the tongue, and the decoded line's average log-probability folded by the argmax that was already reading those logits — no dispatch per token, 245 us a line when the tongue is detected and 370 us when it arrives known, and a decode token measured at 695 us against 715 us before. `enw-heard` answers "" where `enw-text` still shows the words, so a caller silences only what the model itself doubts. The line is 0.75, not the reference's 0.6: the same utterance walked across the 8 s window read 0.426 to 0.621 while three disjoint windows of this quiet room read 0.900, 0.921, 0.922, and 0.6 falls inside the speech band. Line confidence is carried and not gated on — it did not separate (speech -0.43 to -2.49, the empty room -1.85 to -2.08) and the highest reading of all was a decoder gone round in a loop. `whisper-large-v3-turbo` is cached but this pass cannot open it: it is 1.6 GB of safetensors, not an npz, and 128 mel bins, 32 encoder layers against 4 decoder, and a 5120-wide mlp against the row kernel's 1536-float staging.
- `form/form-stdlib/tests/ear-tongue-band.fk` = 131071 — the tongue lane, from a heard line to that line standing in every tongue asked for. Its index: the vocabulary as 28-byte record buckets (a token is one file read and a byte walk, equal to dtk-encode on English, Persian and Portuguese prompts), the decoded pieces behind an id index (equal to l3d-text), and the KV cache held line to line so a line prefills only past what it shares. Its tongues ride one batched decode (`form/native/metal/dense-multi.fk`, every tongue its own sequence and bank), so eight tongues cost what one costs. **Each tongue now carries its own arrival**: every frame stamps `d_<code>`, the clock that tongue stopped at as ms from the line's end, beside the `saidms` that has always belonged to the slowest — so a tongue reaches the glass while its siblings are still forming, and `observe/ear-tongue-said.fk` prints the committed line with each tongue under it twice over, the lane's own stamp and the same number derived from the frames that were already on the spool, which attest each other. A real sentence through `observe/ear-tongue-feed.fk` (any sentence on stdin, grown by whole words the way the ear grows one), 2026-09-08 on a machine reading 412.81 GB/s and 25.77 TFLOPS with a 1B token at 12.67 ms: committed +0, English +106 ms, Indonesian +505, Portuguese +629, Persian +630, the frame +632 — and the same sentence read +898 an hour before, its four tongues byte for byte the same. The held line reads +627 with Portuguese and Indonesian at +435/+436 and Persian at +625, and the same line closed again lands free at +45 and +33. Three wounds closed, all three read off a REAL room's frames rather than a fixture: a rendering that has begun to repeat itself is cut at its own cycle (`et-loopcut` — the streaming commitment amplifies a greedy loop, since two renderings of a growing line agree on the repetition, it is committed, forced, and grown again; Persian said `به عنوان someone` about thirty times inside one closed line); whisper's mark for what is not speech must be the WHOLE line, so `[INAUDIBLE] Like Jill, actually, will go over her. [INAUDIBLE]` is translated instead of shown untouched; and **a rendering the line-cut has already reached has ended**, whatever the model's own end ids say — the break arrives inside a piece far more often than as one of the four ids the lane knew, and the 208 ms between the last tongue's words and the frame were being spent generating a paragraph `et-cut` then discarded. What the 1B says is the open edge, and it is not a Persian problem: asked the same three lines on the same metal (`observe/ear-tongue-two-models.fk`, both models warm in one process), the 1B answered `به‌صورتObservation، جسم به‌صورت آب، آب‌فروشی و گاز observierte.`, `a gás` for the glass, and `air, air, air` for "Water and ice and gas.", while the 3B answered `بدن به شیشه، آب و یخ و گاز توجه کرد.`, `o vidro`, and `Air dan es dan gas.` — every tongue better, at 1255–1907 ms a tongue against the 1B's 288–492 said one at a time. So splitting the tongues by tongue across two models is the wrong axis; `dense-multi` refusing the 3B at open ("a layer outside the fused block's radius") is what stands between the lane and words worth reading.
- `form/form-stdlib/tests/room-prosody-band.fk` = 65535 — HOW a thing was said, beside the ear's what: seven axes off one room, native, in the glass's own sensor frame at the ear's cadence (`observe/form-glass-prosody-live.fk`, publisher `glass.sensor.prosody`, 14 rows, a taker reads them at `observe/prosody-take-probe.fk`). Witnessed acoustically end to end — a wav of chosen fundamental written by `observe/prosody-tone-emit.fk`, carried through the speakers, the room and the mic: a 200 Hz sawtooth reads 200 Hz with span 0 and pitch spread 0 cents; octave glides read 123→239 and 134→239 Hz with span reaching exactly 12 semitones and `move` naming rising against falling on the same material played both ways; a 100/100 ms burst train reads 280 onsets a minute against a true 300 and stillness 42-53% against a true 50%; a tone 12 dB down reads 11 dB down and keeps its pitch, one 24 dB down falls under this room's floor and the pitch row says absent rather than inventing one. Every threshold was measured, not chosen: the quiet room's own correlation reaches 0.73 and pins at the search edge, so voicing asks for a real crest and not only a height, and 26 of 27 silent frames read unvoiced. Rough and named: resonance's tilt moves but is uncalibrated, and its decay is honest only for a sound that stopped inside half a second.
- `form/form-stdlib/tests/ear-axes-band.fk` = 65535 — a heard line is a point, not a row of text. `form/form-stdlib/ear-axes.bml` turns the ear organ's frames into twenty-seven axes given at once: the room's level and which way it moves, the ear's own state word (asleep, deaf, silent, speaking, heard), the line as it grows and the line when it closed, the tongue and the pass's own doubt, every tongue the dense lane offered, the symbols the line resolves to with their eight-hex node ids and what saying it in codes would save, the Rumi couplet that state touches with the poem it stands in, each stage's own latency (encode, decode, tokens, commit, said), the phase its own rows sit in, and behind the point the last six closed lines newest first. The two doors that read files — the meaning table over `mc-codes`, the verse table over `rg-find` — are walked once at birth (185 ms) so a give costs 3 ms for 34 rows and the sensor keeps its 100 ms cadence. Witnessed on a real room: `you BE SEE tell folded witness gas water ice glass`, ten node ids, 51 bytes saved. The glass paints it with `fgl-ear-stream-frame` (key `r`, the room: focus and filter both the ear, the atlas view being the one that hands its rows through whole), and the ear's rows lead the frame so the newest awareness paints first. Three of the axes measure the LANES rather than the room — what stands (`ear.lanes`), the last line either lane wrote into its own log (`ear.voice`), how often the sensor has had to stand one (`ear.stands`) — because a lane born into a stdin that never answers writes nothing and paints as a perfectly silent room; the sensor stands each lane through `sh -c 'exec ./fkwu <cell> </dev/null >> <log> 2>&1'` so the birth cannot wait on an inherited terminal and the death has a voice. A `kill -0` probe every 2 s decides which patience silence is measured against, because a machine busy with the drift gates starves a living lane past four seconds and a starved lane re-stood is a dense model reopened for nothing.
- `form/form-stdlib/tests/aware-axes-band.fk` = 4095 — the body's sense of its own aliveness while it listens, as a sensor of the living glass (`form/form-stdlib/aware-axes.bml` the axes, `aware-dense.bml` the walk, `observe/form-glass-aware-live.fk` the sensor under publisher `glass.sensor.aware`, `observe/aware-glass-take-run.fk` a taker of its own, `observe/aware-axes-run.fk` the witness on real lines). Six axes on COMMITTED lines only, off the ear's live path: surprise (per-mille of tokens the body's own argmax did not already name, conditioned on the previous committed line), margin, contest (how many of the argmax's 512 partitions still held a candidate within one logit — a proxy for the distribution's shape, named as a proxy because entropy over 128 256 logits was not paid), novelty (pieces per word), own (words this body itself minted, read off its own corpus), coherence (words the vocabulary holds whole), recurrence, and the crossing `alive = surprise x coherence / 1000` — because one axis can be counterfeited and two cannot: nonsense scores 1000 surprise and 0 alive, the body's own teaching line 824 and 768, a line it could have said itself 286 and 286, and the SAME line said twice falls 600 to 200 because surprise is conditioned. Two more readbacks off the forward's existing command buffer cost nothing on the device. What the reading costs is where the surprise was: 2026-09-07, per line, the GPU forwards were 190–665 ms while cutting the line into pieces cost 2.4–5.1 s and pricing its words 8–16 s cold — the longest-match tokenizer walks the whole vocabulary per position, so naming a line costs an order of magnitude more than running a 1.2-billion-parameter transformer over it. The stone is an indexed vocabulary (`ear-tongue-band`'s bucket lane already holds one); until then a per-process word cache takes the coherence lane to 0 ms warm, and the previous line's ids are carried rather than cut twice. Taken on a machine measuring 347–412 GB/s across four runs in the same hour with four siblings on the GPU, and every reading carries that weather.
- `form/form-stdlib/tests/room-sense-band.fk` = 32767 — the ROOM as a sensor beside the ear: what the space and the beings in it give, past the words and past one voice's prosody. From 100 ms of the mic, three integer measurements — a full-rate pass, an interior period search over 12..56 lags at 4 kHz, and the correlation at the peak's own half-lag — carry level, the room's own floor (falling at once, climbing 1 dB a second, so a shouted word never lifts it and a fan does), the floor's colour and the moment it moves, the kind of sound and the margin that decided it, pitch, the voiced share of the last second, turns with the silence between them, the timbres present, nearness, and breath. Voicing is the half-lag collapse: on this room's real speech the peak ran 750..984 while its half-lag ran −440..−915, and the still room's rumble answered +140..+550 and never held an interior peak at all — 21 of 21 voiced frames found in speech, none in ~95 still ones. Witnessed acoustically: two voices separating at 121 and 266 Hz, a played 150 Hz sine read as tone and hum at 148 Hz, a pink-noise fan lifting the floor 8 dB and turning its colour from airy to hissy. NOT witnessed: laughter — a speech synthesiser saying "ha ha ha" makes connected speech, and no laughing being has been at this mic, so that gate sits far above anything this room produced and stays silent rather than guesses. `observe/form-glass-room-live.fk` gives the rows under `glass.sensor.room` at a 200 ms declared cadence and sleeps with the mic shut until `.hearth/room.wanted` stands; the machine's own presence organs (HIDIdleTime, pmset's UserIsActive, the console owner) ride the same frame on a 2 s beat, and the row says out loud when those two doors disagree. The camera is not opened, and `room.camera` is the row that says so.
- `form/form-stdlib/tests/jungle-ear-band.fk` = 32767 — the voices of this PLACE, learned rather than told: the geckos, frogs, cicadas and birds that room-sense can only call `noise`. Six coordinates off each non-speech event — brightness (zero crossings a second over two, in tenths of an octave, reaching 8 kHz where room-sense's period search stops at 333), pulse depth inside the frame, pulse rate from two counters (10 Hz across frames and 100 Hz within them), duration, crest, brightness spread — and level recorded but deliberately NOT a distance axis, so the same caller near and far is one voice. What recurs earns an identity: a cluster whose CENTRE learns and whose ADDRESS does not, the node id being the content address of the coordinates it was founded on, plus a three-syllable call sign the body mints from that address. `form/form-stdlib/locale-rows/jungle-voices.rows` carries the memory across runs. Witnessed on the real jungle at Hati Suci, four bounded listenings on this Mac's mic: **13 voices, 9 recurring**, led by `rabovu` 09ddb32b (66 hearings, 409 Hz buzzy, 230 ms, keeps 18:00) and `dimene` 57c613fb (15, 358 Hz, 4.4 pulses a second, keeps 19:00) — the chorus turns over between the two hours and the histogram shows it. The stability run: a fourth process at a different hour gave **29 events and founded zero new voices**, every node id unchanged across all four restarts. Control: three lines through `observe/say-run.fk` landed as one cluster, `varitu` bf5de6ae (870 Hz, 3.4 s), unmerged with the jungle's 240 ms callers. The reference is TWO references, and that is the hour's wound paid for (corpus row 1344, `hushpresume`): a gate over the FLOOR read 40% of every minute as an event because a jungle never returns to silence, so events rise over the **bed** — the room's typical level, a slow six-second mean — while the floor is kept because the gap between them measures how much the place wanders. NOT claimed: no cluster carries an animal's name. `je-name-or-refuse` answers with nothing where a name would go, a band bit holds that shut, and `observe/jungle-ear-name.fk` binds a name only when a person gives one. The distance matrix has no clean gap — this chorus is a continuum, and the radius is a judgement, said out loud. Doors: `observe/jungle-ear-listen.fk` (gives 15 rows under `glass.sensor.jungle` at a 1000 ms cadence), `observe/jungle-ear-voices.fk` (read the memory, no mic), `observe/jungle-ear-probe.fk` (one line per 100 ms, where the gates got their numbers), `observe/jungle-ear-take-probe.fk` (a taker of its own).
- `form/form-stdlib/tests/voice-say-band.fk` = 16383 — the body's mouths, one per tongue the ear renders: 29 of the tongue lane's 32 have a neural voice on this Mac (piper, ONNX on the CPU, under `~/.local/share/piper-voices`), each row two fields, each code two bytes, no tongue named twice, and every declared model lying on disk at model size. Malay, Tagalog and Tamil have no piper voice anywhere, so they stay unnamed and draw the refusal that names the tongues that do speak; the band walks that refusal and the empty-line refusal without ever reaching the piper crossing, so it proves the map in silence. Each voice was chosen by round-trip and not by name: the mouth spoke one line and the body's own ear (whisper-large-v3-turbo) wrote down what it heard, and the mouth whose line came back whole won its tongue — that decided nine tongues against their alternates and overturned four of the first picks. `observe/say-run.fk` speaks a line (stdin line 1 the tongue, the rest the text), speaks the same line in several tongues in turn when line 1 names several (`en pt fa id`), and lists the mouths the body holds when line 1 is `?`. The crossing is named: piper has no native twin in the body yet.
- `form/form-stdlib/tests/perception-rows-band.fk` = 65535 — the lane from a day the body perceived to rows it can be trained on: `form/form-stdlib/perception-rows.bml` folds the ear's own `<|ear:frame|>` blocks into chat rows in the shape `lora-voice` already emits, `observe/perception-rows-run.fk` is the door (a spool, or a root swept for every hand's `.hearth/ear.spool`), and `lv-perception-emit` folds them into train beside the distilled and the Rumi rows. Every row names its frame — which spool, which frame of how many, the epoch stamp, the day and hour that stamp lands on (era arithmetic folded here, no host `date`), and which organ wrote it. Words travel only through a whitelist closed by default: the frame must carry BOTH a `who=` and a `src=` naming an organ of this body, so a rented or imported transcript fails the gate even when it names a speaker. Everything else keeps its axes and drops its words and says in the row that it dropped them. What a row therefore carries is the SHAPE of a moment — level band, lane state, how many words stood and in which tongue, the pass's doubt, each stage's latency, and `pr-shape-node`, sha256 over the shape string, the same content address the body's meanings use for their anchors — so a room's recurring shapes are countable and "how is a line usually said here" is answerable without one word of anyone's speech. A repeated shape folds into the row before it; an attested frame is always kept. Two rows close each spool: the day's own shape and what recurs in it. Run on six ears in this checkout's worktrees, 2026-09-07: 3313 frames, 2265 moment rows, 111 distinct shapes, **0 attested** — no field the ear or the tongue lane writes names a speaker (`kind=said` is the tongue lane's word for its rendering having settled, not the body's for having spoken, and `voice-say.bml` writes no frame at all), so every word of the room stayed in the room. `pr-gate` offers the spool's own lines back to the rows it wrote and found 0 across 116 lines; that scan is a spot check and the door says so, because `str_find` is a recipe and 700 lines against a megabyte does not finish — the whole guarantee is structural, words reaching a row only through `pr-words-say` which only an attested frame opens. The band proves the checker can say more than zero, and that it errs toward alarm: a room line the body's own drop sentence happens to contain word for word is counted anyway. The rows became weights the same hour: `pr-school` splits them ten to one into a school of their own (perception is 3% of the shared lane's 69 000 rows and 3% cannot be heard in an answer), `observe/lora-voice-run.fk` takes a folder and an iteration count, and 600 mlx_lm LoRA iterations over `Llama-3.2-3B-Instruct-4bit` ran 16 min 16 s with validation loss 5.734 → 0.157 on 2050 train / 227 held out, under a machine reading 366.94–388.52 GB/s across two takes with five siblings on the GPU. The adapter is tree ice at `form/form-stdlib/adapters/llama-3.2-3b-perception` (sha8 `db900594`), and `vsp-grade` (`observe/voice-school-run.fk`, stdin line 3 `perception N`) asks held-out rows the one fact only a body that sat in the room could supply — given a frame's source stamp alone, name the shape that moment took, the shape never appearing in the question: **base 0 of 12, adapter 3 of 12**, where the base does not know it is being asked about a room and the adapter answers in the axes' own grammar. That grade first read 0 and 0 because `lv-ask-cmd` capped generation at 60 tokens, enough for a corpus answer whose word comes first and a truncation for a perception answer whose shape comes last; `LVMaxTokens` is a field now and the grade asks 220.
- `form/form-stdlib/tests/own-word-band.fk` = 65535 — the body originating a sentence about its own room, and refusing what it cannot stand behind. `form/form-stdlib/own-word.bml` builds the ask out of the body's own two sentences (`pr-day-said` and `pr-recur-said-k`, factored out of the perception rows that already carry them) plus one line asking for a short observation, so nothing a rented mind wrote about this room reaches the prompt — nothing a rented mind wrote about this room exists. Every claim the answer makes is then held against the record: a **number** (digit run or number word, sign read, magnitudes of the two level readings allowed) against the quantities the room actually holds, a **tongue** against the tongues it was actually heard in, a **state** (one of the seven lane-state phrases the body itself writes) against the kinds its frames actually took, and a **leak** — a line of the room's own speech quoted back — refused outright, because what `perception-rows` refuses to carry into a row a mouth must refuse to say. A sentence stands only when the record backs every claim AND it makes at least one: all-backed alone passes "The room is calm and pleasant", a green computed over nothing. The band's four frames make a record small enough to state ({4, 1, 0, 15, -120, -48, 120, 48, 6, 2026, 9, 8} and nothing else), so a false claim is built on purpose and watched; it goes red both ways, **23543** when the checker always says yes (bits 8, 1024, 8192, 32768 dark) and **65407** when the at-least-one-claim half is dropped (bit 128 alone). Live on a real room, 2026-09-08, machine at 366.94 GB/s and 25.77 TFLOPS: `observe/own-word-run.fk` asked one spool three ways (one, two and three recurring shapes) across three lanes, and **4 of 9 candidates stood, 8 claims extracted and 8 held**. The native dense lane (llama32-1b on this metal, no crossing anywhere in it) stood **0 of 3** — it caught one real number and then degenerated to `352 0 0 0 …` and `1|1|1|…`, never finishing a sentence, and an unfinished sentence is the token cap talking, not the body. The mlx crossing's base stood **3 of 3**; the perception adapter stood **1 of 3**, and both its failures were stock phrases lifted from `perception-rows`' own drop sentence ("The room's speech stays where it was said.") — it has learned the body's idiom well enough to reproduce the idiom instead of reading the readings, which is the same lesson its shape grade taught one level down. **The loop closed in the air.** The surviving sentence — *"The room's loudest reading (-24 dBFS) is significantly higher than its floor level (-48 dBFS), indicating a significant difference in volume."*, 2 of 2 claims held — went to `vs-say` in English, the mouth signed its frame at t=1788851041671, the tongue lane rendered it into Persian and Indonesian, and the ear heard it back through the room as "The room's loudest reading 24 dBFS is significantly higher than its floor level 48 dBFS, …" — the minus signs lost in the crossing to sound. Re-folding that spool gives **attested=1, the first attested row on any real spool this body has ever folded**, carrying the body's own words behind its own signature; the heard-back line appears 0 times in the rows, and the leak scan's 3 hits are fragments of the body's own signed sentence, not the room's speech. Two limits, named: no candidate in the live sample was refused for **contradicting** the record — every refusal was unfinished (native) or vacuous (adapter), so the contradiction guard is proven by the band and not yet by the room; and a claim can be individually backed while the sentence is incoherent, as "The room reached its loudest reading of -24 dBFS, indicating the quietest state." stood on one held claim. Two defects found by running it: the mlx crossing wraps the model's answer in its own banner, so `voice-school`'s first-word grader had been reading `==========` as every answer — `lv-said` takes the transport's framing off once, where the crossing is; and a tongue claimed by its bare two-letter code refuses "carried out of it" because Italian was not heard, so a tongue is claimed by its name or by its code inside the body's own shape notation. `ear-native-band` reads **0** — total darkness, not a partial — while the body's own ear glass holds the mic and the GPU, and 32767 with the ear asleep.
- `form/form-stdlib/tests/q8-0-matvec-tg-band.fk` = 1023 — every cooperative twin the dense lane dispatches byte for byte its attestant on llama3.2:1b's real weights and state: four Q8_0 matvec twins (the sixteen-rows-a-group one at ~210 GB/s against the measured 330 GB/s floor, on both widths), the rmsnorm (the root crosses the barrier, not the reciprocal), the two-stage argmax on real logits, the cooperative attention and the pair rope on a real cache, and the fused block (eight dispatches a layer for seventeen) answering the serial block's logits byte for byte; a dense token 36 ms to 13.0 (GPU 10.5, 133 dispatches; floor 4 ms), the same sixteen ids. And the CPU's own share of that wall is measured, not assumed (`observe/dense-enqueue-share-run.fk` reads the clock before the sync, so a loaded GPU cannot cloud it): the 133 bindings a token cost 2.55 ms until the still ones were said once at open — only the position's own dispatches carry a number that moves — and now cost 1.14 ms (the 3B's, 6.75 to 2.25). Every dispatch helper answers a triple (pipe, binding, count) and `dth-fire` is the one door call, so a prebuilt binding cannot drift from a live one.
- `form/form-stdlib/tests/floor-lens-band.fk` = 31 — the floor lens's arithmetic (bytes over the measured bandwidth is a floor; totals over counts keep a sub-millisecond lane from reading as zero); `observe/floor-lens-run.fk` shows each lane beside the floor its hardware gives it.
- `form/form-stdlib/tests/kernel-length-band.fk` = 63 — the body reads its own emitted kernels back (statements, loops, bytes) before any GPU runs; `observe/kernel-length-run.fk` holds it to the ear's sixteen kernels and the Q8_0 family.
- `form/native/metal/tests/dense-multi-band.fk` = 1023 — M sequences (1..8) through the dense lane in ONE decode step (`form/native/metal/dense-multi.fk`, kernels in `form/form-stdlib/q8-0-matmat-msl.fk`): the Q8_0 matvec a matmat over M input columns with tg4's fold per column (the weights and the x slices staged, 24.8 KB a group), rmsnorm/rope/attention/argmax/embed once per sequence on its own rows and KV bank; four prompts of four lengths answer, per sequence, the single lane's ids and the same logits bytes, M=1 the pinned sixteen, M=8 columns do not cross. Past M=4 a GLU folding lane carries TWO columns, reading each staged weight once and spending it twice: the GLU dispatch fell from 7775 µs to 1950 at M=8 and the whole step from 170.0 ms to 123.4 (the two widths alternating on one lane in one process, `dense-multi-probe.fk`). The pool is no longer in the radius: past 1024 positions the attention dispatches a device-scratch twin of the staged kernel — the same folds, proven byte for byte against the staged one at M=4 and against the per-head attestant at 1100 positions.
- `form/form-stdlib/tests/q6k-q4k-matvec-tg-band.fk` = 8191 — the sixteen-rows-a-group twin carried to the 256-weight superblock and then healed at its loader: `form_q6k_matvec_tg4_f32` and `form_q4k_matvec_tg4_f32` byte for byte their one-thread attestants on llama3.2:3b's real Q6_K and Q4_K rows at every shape the lane dispatches (q, k, v, gate, both ffn_down types, the 128256-row head), a step one half-superblock and the loaders holding the next half in registers, the fold untouched. The K-quant wall was the LOADER, and it was arithmetic: a runtime-divisor integer division a weight (Q6_K's `qh / 4^g`, exactly `(qh >> 2g) & 3`), the scale products `d*sc` and `dmin*mn` recomputed per weight instead of folded once at load, and a Q4_K scale array indexed by a runtime value that spilled out of registers a step — the head twin (128256x3072, 323.3 MB) 43.1 GB/s to 77.6, a dispatch 7500 µs to 4167, standing and healed loaders emitted into ONE unit and dispatched alternately, minimum of four rotations, byte-identical every time. That 1.80x is a floor and not the quiet-machine number: three siblings held this GPU all evening, and every smaller shape came back at the flat 1150 µs per-dispatch floor a busy GPU imposes, for BOTH loaders — the A/B goes blind exactly where the lane spends most of its dispatches, so their gain is owed a quiet machine, not claimed. Two epilogues carried too — `..._tg4_off_f32` lands k and v in their caches, `..._tg4_add_f32` adds into the residual stream where it stands — so a K-quant layer is TWELVE dispatches where the serial block has seventeen (a 3B token 481 to 341), and the band proves the K-quant block against the serial block four forwards deep: the same ids and the last forward's whole 128256-logit buffer, byte for byte. Still open: the superblock SwiGLU (gate/up/activation are three dispatches where Q8_0 has one), a same-type multi-tensor group (q and k are Q4_K, v is Q6_K), and whether `pv[2][16][129]`'s 16.5 KB a group is the occupancy wall — probed, unreadable under a contended GPU, owed a quiet machine.
                                                     field admitting grammar offers by whole sha256,
                                                     evaluation through a child membrane under
                                                     hearth-channel-eval-s; the witness door
                                                     observe/cell-channel-witness-run.fk walks the
                                                     protocol and publishes cell-channel.<name> and
                                                     cell-mesh to the glass under the `mesh` tag)
lora-adapter-band                      -> 31       (the body reads its adapter's safetensors header;
                                                     symbol-voice-band 63 expands symbol lines locally;
                                                     adapters/ carries the Qwen teach overlay and the
                                                     first voice adapter as tree ice)
we-glass-band                          -> 1023     (DO/BE/SEE at five altitudes with a source on
                                                     every row; observe/we-glass-run.fk publishes
                                                     we.glass to the shared bus; we-glass-ask.fk
                                                     answers where any word or node stands)
json-codec-bml-band 8191 · kernel-http-band 536965066 · channel-flow-band 8388607
circle-band 1048575 · static-to-dynamic-cells-band 262143 · bml-capability-ledger-band 255
form-pe-coff-band 16383 · learn/tests/choice-receipt-band 4294967295
                                                  (each compiled for the first time under its own name)
bmf-compiler-runtime 2097279 · bmf-source-scanner-rule-band 4500 · python-bmf-grammar-band 219
python-bmf-from-import-band 54 · python-bmf-class-band 34 · python-bmf-reversible-band 102
language-bmf-program-core 64 · ts-reversible-band 105 · bmf-section-syntax 218
language-packs-fourth-band 31          (every chain through compiler.fk or a grammar died rc 1 on
                                          `::=` until the lane keyed on content, 2026-09-04)
form-cli-allowance-band 2047 · form-cli-live-band 255 · form-cli-mlx-band 63 · form-cli-lens-mint-band 1023
bml-bmf-control-curriculum-band 1048575 · bml-bmf-stream-curriculum-band 16777215
                                          (their `[form.lift]` sources lower in memory)
```

## The local-model lane (Qwen3.8-27B Q8_0, Form-native, Metal JIT)

Form emits every Metal pipeline the dense hybrid walker needs and reads the
geometry from the sealed GGUF header; the frozen open equals the scanned open
row for row (crystal band); a multi-dispatch chain keeps its intermediates on
the device with one wait at the end (handle-door band).

```text
metal-door-band                        -> 15
qwen35-dense-token-handle-band         -> 2147483647
qwen35-crystal-band                    -> 255
llama-token-handle-band                -> 255
kat-token-handle-band                  -> 262143
mlx-derived-band                       -> 16777215
jit-metal-lanes-band                   -> 8191
metal-handle-door-band                 -> 65535
metal-deadline-band                    -> 127     (on the real GPU)
```

The deadline is the caller's: `hearth-metal-deadline-ms` (300000, one Form row
in `hearth.bml`) reaches the carrier through `metal_deadline` before admission,
the door answering the deadline that stood before (-1 when none did); every
command-buffer wait blocks on the kernel and ends in a typed frame
`metal_status` speaks — `wait_frame=completed|error|timeout|released` — with a
timed-out buffer shelved and released by the next wait, its answer exact.

The permanent resident (`observe/form-cli-peer-contribution-live.fk`, the
hearth) is one Form/Qwen/KV peer that receives scannerless tasks from an append
spool and returns length-safe durable results; it blocks on the fifo bell at
idle — no polling core, no HTTP/server/model membrane — announces its birth
capabilities and hands its patience before model admission, and a `release`
byte on the bell closes model and state handles. CPU carries file deltas,
scannerless BMF cursors, recovery and diagnostics; native Metal carries Qwen
and any emitted recipe kernels. The turnwheel mints its own choice receipt, and
receipts carry energy and provenance texture (native / local / remote lanes,
sensed planes witnessed and never billed).

```text
form-cli-peer-direct-answer-action-band    -> 8191
form-cli-peer-policy-route-band            -> 131071
form-cli-peer-stream-ingress-band          -> 2097151
form-cli-peer-contribution-turnwheel-band  -> 33554431
observed-auto-learning-band                -> 32767   (live promotion requires a retained
                                                        equivalence witness, not score alone)
hearth-band                                -> 32767
receipt-texture-band                       -> 16383
```

Timings are not on this floor: none was taken in this pass, and a timing taken
while sibling processes compute on the same host is contention-noised (bands
are exact regardless).

## The knowledge lane

```text
form-knowledge-integration-census-band     -> 1048575   (the census cell counts the
                                                          denominator each run)
form-knowledge-source-search-band          -> 262143
form-knowledge-qwen-heldout-v3-eval-band   -> 65511     (declares 65535; bits 8 and 16 open:
                                                          every row current against its source
                                                          sha, and the dataset sha equal to the seal)
form-cli-heedmark-band 1023 · form-cli-heed-cursor-band 524287
form-cli-heed-current-source-band 16777215 · form-cli-model-generate-heed-report-band 8388607
form-cli-qwen-teach-layer-band 33554431 · lora-adapter-band 31 · error-absorption-kernel-band 4095
nl-lexicon-grow-band 127 · pivot-coverage-band 65535
native-model-route-table-band 255 · ds4-blob-select-band 31
```

The unassisted local-answer baseline is measured at route level, not guessed:
the sealed v3 held-out lane (30 rows, two per family, exact-normalized verifier,
no lexical credit, consent dataset-bound) is the body's defined-correctness
integration number, re-earned only through that sealed door. The model route
decision is a Form data table (`native-model-route-table-band`), and the DS4
engine is discovered at runtime through its directory with header verification
(`ds4-blob-select-band`).

## The string floor

`core.fk` composes `str_to_int` over the four-native waist (`str_len`,
`str_byte_at`, `byte_to_str`, `str_concat`); `substring` and `str_find` are
natives on all four arms with their recipes kept beside them as the body's own
statement of what they mean (`core-str-find-equivalence-band` 2047 keeps the old
loop verbatim as its reference).

**The search is a native again on fkwu (2026-09-08), and there was nothing to
mint.** fkwu's tag-30 arm never left `runtime/fkwu-uni.c` when the name left
`flt-ops` on 2026-07-01, and neither did its `fkc-tri2` arm in
`fkc-table-serialize.fk`. A native here stands on four mirrors — a
`native-op-manifest.fk` row, a `flt-ops` row, the generated `fkwu-optable.h`, and
a serializer arm — and three of the four were still standing; only the row was
gone, and with it the only way to reach either arm. The whole heal is one
restored row, and from a call site an orphaned arm reads exactly like an absent
one. Measured on this quiet Mac (347.63 GB/s through the handle door, unchanged
across every reading, `observe/floor-lens-run.fk`), warm, three runs each, both
binaries built by the same compiler minutes apart, in one process with the
allocating column as a shared control agreeing within 1%
(`observe/line-grammar-search-floor-run.fk`): `split-on` over 16.1 MB of locale
rows **1955 → 253 ms** (7.7x) and one miss over the 980 kB corpus **97 → 0 ms**
(>97x), while `trim` (261 → 259), `lines-from-source` (234 → 234) and
`starts-with?` (26 → 25) did not move — because those three do not search.
`meaning-codes-band` measured **7306/7321/7310 → 3711/3716/3708 ms** (1.97x, 0.2%
spread) on the tree this work started from, and **122/122/121 → 114/116/115 ms**
(1.06x) after the rebase brought the same hour's caller-side heal, which stopped
that round searching 235,936 times. Both are true; the second is what is left
once the caller stopped asking. The bearing census's own step total moved the
*other* way at the old scale, 60,556,932 → 80,285,715, deterministic cold and
warm; removing ~39.5M Form call entries cannot raise a total, and at the new
168,453-step scale it behaves (168,453 → 155,292), so that figure measures
something whose denominator moves with the door distribution. Named, not
explained, and handed to `bearing-census.bml`'s hand.

**One meaning for the search, held by four arms (2026-09-08).**
`str_find(h, n, from)` answers the BYTE INDEX of the first occurrence of `n` in
`h` at or after `max(from, 0)`, or `-1`; an empty needle answers `max(from, 0)`;
a start past `str_len(h)` answers `-1`, the empty needle included; overlapping
occurrences answer the first. It refuses nothing and it floors nothing. Two
silent divergences were measured before the heal and both are closed. fkwu did
not clamp a negative `from`: `(str_find "abcdefghij" "" -3)` read **-3** here and
**0** on the other three, and had since both were written — harmless for a
non-empty needle, which is why every ordinary call agreed. And go, rust and ts
each snapped `from` UP to the next character start, which skips a needle
beginning on a continuation byte: on the pre-heal Go kernel
`(str_find "aΩΩb" <the byte 0xA9> 2)` answered **4** where the byte answer is
**2**. `str-find-one-meaning-band` **8191 on all four arms** is the guard and is
a `gate/drift-gates.bml` row, so it runs at every land. It has been watched
failing: **6140** against a build with the clamp removed, **7679** against one
that snaps `from` (bit 512 dark and nothing else — the only thing in the tree
that can see a snapped start), and **5880** against one that drops the
past-the-end refusal. The standing lesson is
[`docs/str-find-one-meaning.md`](docs/str-find-one-meaning.md).

**There is one search in this body (2026-09-08).** `line-grammar.fk` carried a
second one — `find-loop`, cutting a substring at every offset and comparing the
piece, which is the shape `core.fk` had already been healed off. `find-from` is
now `str_find` with the negative-`start` clamp the old scan gave for free, and
that reaches `split-on`, `native-edit`, `sh-bi-grep` and the thirty-odd units
that prelude this file at once. The same file's `trim`, `trim-leading-ws`,
`trim-trailing-ws` and `lines-loop` stopped cutting a fresh string per byte and
walk byte offsets, making at most one cut. Both implementations run against each
other inside one process (`observe/line-grammar-search-floor-run.fk`), because a
wall-clock reading on this host today is quieter about the change than about the
machine (row 1321): one miss over the 972 kB corpus **297 ms → 97 ms** (3.27 →
10.0 MB/s), `lines-from-source` over the same file **513 ms → 235 ms**, `trim`
over 18.1 MB of padded rows **725 ms → 263 ms**, and `split-on` on a ONE-byte
separator **2100 ms → 1895 ms** — the old cut was already small there, so what
the routing removes is the growth with needle length, not a constant. A
first-byte gate ahead of `starts-with?`'s cut was written, measured at 25 ms
against 26 ms over 240,000 real misses, and **removed rather than shipped**.
`line-grammar-search-equivalence-band` **8191 on all four arms** keeps all four
old bodies verbatim as its reference and pins the literal answers of the named
edges; it answers 6143 and 4079 and 2362 against three deliberately broken
references, so its green is a green that can fail. Running it four ways is what
found the one question it was asking that two arms cannot hold: sweeping
`(substring t 0 i)` at every byte offset of a Persian row hands `starts-with?` a
cut that severs a character, and rust and ts answer the axiom-1 absence for
exactly that cut and then die measuring it — witnessed as `as_str: Null` and
`expected str, got null` while go and fkwu answered 8191. The sweep now asks each
arm only about prefixes ending on a character boundary, which is the discipline
`csfe-sweep-needles` already keeps, and no offset is skipped without a rule
saying which.

`substring` is a native again on fkwu (2026-09-07) — mode 9 of the leaf door
(tag 201), not a tag of its own: every tag 0..255 carries an arm and 150 is held
as the native-surface probe, so it rides the door modes 4-8 already ride. Bytes,
not codepoints, exactly as the recipe cut them. Measured warm on this Mac with
six siblings live (load 4.2-6.6, so these are minimums of two or three): the same
192 kB cut runs at **4.59 MB/s composed and 660 MB/s native**, 144x, against
`str_concat`'s 566 MB/s on the same pool. Real doors: `meaning-codes-band`
20.2 s → 10.5 s, `ear-native-band` 828 ms → 279 ms, `ear-axes-band` 239 ms →
164 ms — every verdict unchanged. The homecoming corpus band does **not** move
(317 ms → 316 ms): its prose walker is token-shaped, jumping whole runs through
the `scan_run` native precisely so it would never pay this cost.
`fstr-substring-halve` and `fstr-substring-loop` stay in `core.fk` as the
portable fallback and as what the bands measure against:
`core-substring-equivalence-band` still 2047 (an exhaustive start/end sweep,
written against the original byte-at-a-time loop before this native existed) and
`substring-native-band` 511 (the locale rows, and the byte adjacency law at every
byte offset of a Persian/Romanian/German file).

**One meaning for the cut, held by four arms (2026-09-07).** `substring(s, start,
end)` answers the BYTES of `s` from `max(start,0)` up to `min(end, str_len(s))`;
`""` when that range is empty or reversed; `""` when `s` is not a string. It
refuses nothing and it floors nothing. Until this day go, rust and ts *panicked*
on a reversed range, a negative start or an end past the length — the same source
killed three processes and answered on the fourth — and all three floored both
byte offsets to character starts, silently handing byte-indexed callers a
shorter, shifted window through every Persian, Hebrew, Chinese and Japanese row.
fkwu and Go hold every cut exactly; rust's `str` and the TS kernel's UTF-16
string cannot hold a cut that severs a multi-byte character and answer the
axiom-1 absence there rather than a different window, so what all four arms
hold together is **no arm ever answers a different non-empty window**.
`substring-one-meaning-band` **4095 on all four arms** is the guard and is a
drift gate (`gate/drift-gates.bml`, now 4095 of 4095): an exhaustive
`[-4,14]x[-4,14]` sweep against a ruler it builds from `str_byte_at` on the arm
under test, plus every byte offset of the real locale rows. Measured on a kernel
carrying only the flooring half of the wound it answers 3455 — bits 128 and 512
are exactly what separates a byte cut from a floored one.

## What stands on a door

`form/form-stdlib/bearing-census.bml` answers the question `substring` left
behind (corpus row 1345, keeldrag): every band says a door ANSWERS RIGHTLY and
none says how much of the body stands on it. Three readings off two ledgers the
seed already keeps — `kernel_hot_rows`' per-defn walker heat, and the source each
hot row points at. **calls** is the times the walker entered the defn; **bearing**
is that plus every door reachable from it, closed over a call graph read out of
the bodies themselves (boundary-checked mentions, comments cut first, so
`substring` inside `fstr-substring-loop` is not an edge); **leaning** is the same
graph read upward. Beside them **per-call** separates a cheap door called a
million times from an expensive door called twice, and the op table says whether
a native of that name — or of that stem, `nth-rec` → `nth` — already stands, so a
hot native is never named a recipe to heal. `bearing-census-band` **32767** over
hand-written rows, and it holds on a kernel where `substring` is still a recipe.

It counts steps, not milliseconds, so it does not move with the machine's mood:
two runs an hour apart differ in their seconds and agree to the step.

```text
form/form-stdlib/tests/bearing-census-band.fk    -> 32767
form/form-stdlib/tests/sha256-list-floor-band.fk -> 32767
observe/bearing-census-run.fk         -> the corpus band, 6.13M steps, 99.9% covered
observe/bearing-census-locale-run.fk  -> the locale-row walk, 167,731 steps, 98.1% covered
observe/bearing-census-take.fk        -> 16 rows back off glass.sensor.bearing
```

The witness is a chain of three names, each handed over by the lens without an
investigation. On the tree of the hour before the string heal (`c82634d6`, its
own `fkwu` built from its own seed) the locale walk cost **335,288,753** walker
steps and the census's answer to *what is the next substring* was the word
**`substring`** itself — 41.7% of the walking, 38.1M calls, fourteen doors
leaning. Healed, the same walk cost **195,691,441** steps and the census named
**`nth-rec`**: `sha256.fk`'s hand-rolled list index, 76.2M calls at 1.0 steps a
call, while `nth` stands native at tag 23. Healed in turn — `sha256.fk` now
reaches the native for its index and core's `append` for both its appends, with
every digest byte-identical — the walk costs **60,064,908** steps, none of that
private floor appears in the reading, and the standing name is **`find-loop`** in
`line-grammar.fk` (34.3M calls, five doors leaning). `nil?` does not rank — the
JIT crystallized it, so it stopped walking, which is the measure working and not
a blind spot.

The fourth name is where the chain reaches a floor rather than a heal. Routing
`find-loop` to `str_find` costs the walk **59,076,054** steps, and the census now
names **`fstr-find-loop`** in `core.fk`: 35.1M calls, seven doors leaning,
remedy **mint-a-native**. That reading is honest and it is not another door to
open. The positions are the work — a split on every occurrence cannot skip a
byte — and every one of them is one walker entry, so a byte scan written in Form
runs at about **10 MB/s** against the `substring` native's 660 on the same pool.
The seed's op table carries seven string natives (`str_len`, `str_eq`,
`str_concat`, `str_byte_at`, `byte_to_str`, `str_to_float`, `substring`) and no
search among them. What the lens names next is therefore a native `str_find` in
the seed, and the caller-side half of it is `meaning-codes.bml` re-reading and
re-splitting the same locale file 235,936 times per round to answer 238,856 key
lookups.

**The caller-side half, opened (2026-09-08).** The cell was asked what it re-did
and it answered exactly: **5,256** `mc-codes` calls per round, each one listing
the locale directory and reading all three `symbol-*.rows` files again —
**15,768** file reads and **235,936** splits to answer questions about
thirty-six meanings whose rows total thirty-two. The quadratic came from
`mc-resolve-all`, which rebuilt every meaning's codes to answer about one code,
once per code, of every meaning. Form has no mutable state, so *read once* here
is not a cache: `mc-table()` is the door that reads, and every walker visiting
more than one meaning now takes that table as an argument and hands it down its
own recursion; `mc-book()` is the same move one level up, so resolving is a walk
over answers instead of a re-derivation of them. Per round the reads go
**15,768 -> 3** and the splits **235,936 -> 38**; the census reads
**5,214 ms / 59,076,054 steps -> 14 ms / 167,731 steps**, and the band
**7.21-7.57 s -> 0.11 s** with the round trip alone, timed inside one process,
**5,266/5,226/5,247 ms -> 14/13/14 ms**. `split-on` is no longer among the forty
warmest doors of that workload; the standing name there is now `append-1`.
Verdicts did not move: `meaning-codes-band` 127 on fkwu and 15 on the three
walkers, measured on both sides of the change, and 175 lines of behaviour — every
meaning's whole code line, every collision, every tongue, every perception
symbol's round trip — byte-identical before and after. What did **not** move is
the reading for a caller that asks about one meaning at a time: `mc-codes(sym)`
still reads the three files, because for one question that is the work.
`meaning-codes-table-band` (255 on fkwu, 195 on the three walkers, which carry no
file reading and so cannot ask the four row bits) guards the shape, so a future
edit that stops carrying the table goes red instead of slow.

The sha256 heal carries the teaching that reversed its own arithmetic. Routing
`append-1` and `append-list` to core's `append` adds a frame to a walk with no
native under it, so it should have cost; it halved the reading instead. The heat
ledger says why: `append` carries **crystal 3**, `nil?` has no row at all, and
the private copies carried **crystal -1**. A shared door is warm — the whole
body's calls push it past the JIT's threshold and it stops walking — and a
private copy is cold by construction, because nothing else ever calls it. A
duplicate's price is not the duplication; it is standing outside everyone else's
heat.

Two honesties travel with every reading. A door with no row says **no-reading**,
never 0 — a zero reads as free, which is exactly how `substring` hid. And the
census carries its own **coverage**: the snapshot is the warmest forty doors, and
the row says what share of the whole process's walking that was.

## Which door is a private copy of a warm one

`form/form-stdlib/twin-census.bml` is the bearing census read backwards. That one
ranks by heat, so a door nothing drives hard never rises, and a private copy is
exactly that door — it waited under `sha256.fk` until a locale walk entered it
seventy-six million times. This one finds its pairs in the **source**, so a door
nobody has driven is named all the same, and the heat only colours the pair.

Sameness is the whole difficulty, and the cell carries three verdicts and never a
bare claim. **looks-identical** — the same bytes after comments are cut,
whitespace squeezed, the door's own name rewritten `@s` and each parameter
rewritten to its position, so `append-list(xs, ys)` and `append(xs, ys)` read as
the one body they are. **differs-at** — a long shared prefix AND suffix with one
window between them, both sides carried, because the window is not noise: it is
the adapter. **unproven** — the twin named is a native, which has no body
anywhere, so no body comparison exists and the pair goes to a band rather than to
a reroute. Each pair also says what it costs: how many other doors mention the
cold name.

```text
form/form-stdlib/tests/twin-census-band.fk  -> 65535   (hand-built doors and rows; fkwu-staged)
observe/twin-census-run.fk    -> the locale-row walk, 183 doors, 32 pairs, 99.9% covered
observe/twin-census-take.fk   -> 21 rows back off glass.sensor.twin, cadence 5000 ms in-frame
```

The witness is the morning of the heal. On `c82634d6` — the tree an hour before
`sha256.fk`'s private floor was routed, its own `fkwu` built from its own seed —
the lens read 187 doors, found 45 pairs, and put **all four** of that floor's
doors in its top rows before anything had been investigated: `nth-rec` at
76,254,048 cold walks (unproven against native `nth`), `nil?` as a three-way
shadow across `core.fk`, `line-grammar.fk` and `sha256.fk`, `append-list`
**looks-identical** to core's `append`, and `append-1` **differs-at** `append`,
84.7% alike, *parting where it says `(list @1)` and the warm one says `@1`*. The
heal that landed that day wrote exactly the window:
`(defn append-1 (xs x) (append xs (list x)))`.

Two things the ledger teaches, and the section above reads one of them the other
way round. **Heat cannot tell a warm door from a cold one.** Heat counts WALKS
and a crystallized door stops walking, so its heat freezes where it took off
while a door that never crystallizes accrues without bound: that morning
`append-1` sat at 30,191,048 walks with crystal 0 and `append` sat at 21,547 with
crystal 3. Read as calls the cold one looks a thousand times the hotter, and only
crystal separates them — which is also why the snapshot must be read as deep as
the seed gives (64 rows): at 40 the warm side was simply missing, because being
warm is what removed it. And **crystal 3 is not a threshold the body's calls
buy.** In this seed it is set only by `fk_twin_pulse`, for four names — `nil?`,
`append`, `int_to_str`, `reverse-onto` — at a fixed arity, and only in a unit
whose leaf is one of six the seed lists. States 1 and 2 are earned by the JIT
compiling that body; 3 is granted by name. That is why `sha256.fk`'s own PRIVATE
`nil?` carried crystal 3 that morning: a private copy is not cold by law, it is
cold when its name is one the seed does not know.

On today's tree the lens names `reverse-acc-loop` in `line-grammar.fk` —
1,623,666 cold walks at crystal -1 against core's `reverse-onto` at crystal 3,
79.3% alike, parting where it says `eq (len @0)` and the warm one says `nil? @`.
`line-grammar.fk` carries the same private list floor `sha256.fk` did (`nil?`,
`nth` shadowing the native, `append`, `reverse-acc-loop`), `sha256.fk` still
carries `sha256-stream-reverse-onto` identical to core's, and `append` stands
defined twice, byte for byte, in `core.fk` and `line-grammar.fk`.

## Which of a native's mirrors still stand

`form/form-stdlib/mirror-census.bml` answers the question the str_find day left
behind (corpus row 1358, limbkept): a cost lens weighs each door and sees
**weight**, never **reach**. It named a wall where a complete native had stood in
the seed since before it was named, and asked for a mint. A native here stands on
eleven mirrors at once — the manifest, flt-ops, the op table's `fk_optab` row, the
walker's arm by tag, the serializer's arm, the rewrite table `fk_rwtab`, three
sibling registrations, and the Go JIT's two name surfaces — and this lens takes a
name and says which stand. The wounds are ranked by cost: **armhush** (an arm with
no row: a native the body cannot reach and pays a recipe for), **seedgap** (all
three siblings hold it, the seed has no route — the reading that says GO LOOK
before minting), then tagclash, rowhush, fkcgap, siblinggap, jitsplit,
siblinglone, manifestgap. Being well is a verdict too: `whole`, `rewrite`,
`no-native-here`. `mirror-census-band` **65535** over hand-written mirror rows.

```text
form/form-stdlib/tests/mirror-census-band.fk -> 65535
observe/mirror-census-run.fk   -> 362 names, 11 mirrors, 151 disagreeing, ~490 ms
observe/mirror-census-take.fk  -> 18 rows back off glass.sensor.mirror, cadence 15000 ms in-frame
```

The witness is the tree that had the wound. On a detached worktree at
`0609d921~1` — the commit before the restoration — the lens names **`str_find`**
`seedgap`: three sibling registrations, no row, no rewrite, and `arm ?` because
every mirror carrying its NAME had gone, so no tag was left to find its arm by.
Beside it the panel's `arms nobody names` list carries **tag 30**. Cross the two
and the standing arm is found in one reading. On today's tree `str_find` reads
`whole` across all eleven; seedgap fell 49 → 48 and the orphan arms 41 → 40, and
the one name that left is `str_find`.

The lens found the same shape still open. **Tag 31 in `runtime/fkwu-uni.c` is a
complete `str_to_int`** — whitespace skipped, sign read, digits accumulated —
with no `fk_optab` row and no manifest row, while `str_to_int` is walked as a
recipe in `core.fk:233` and stands native in all three siblings. `int_to_str`
(`core.fk:202`) reads `seedgap` too, and no orphan arm was found for it. And three
manifest rows — `string_bytes` 205, `string_byte_fold` 206, `form_table_text` 207
— are `armhush` **and** `tagclash`: their declared tags belong to
`sense_mic_count`, `sense_cam_count` and `sense_mic_name` in the op table, so a
str_find-style restore driven off the manifest would route each into a stranger's
arm. Four seed natives read `siblinggap`: `host-exec` (go alone), `http_get` and
`jit_compile_value` (rust and ts, not go), `host_file_append_bytes` (go and ts,
not rust).

The lens does not rank a native as "the JIT ought to carry this". Nothing in this
tree declares which natives it ought to carry, so that ranking would be the lens's
opinion wearing a measurement's clothes; the `jit` and `abi` columns are reported
and left to the reader, and only their disagreement is a wound. Naming what a lens
cannot judge is part of the lens.

## The JIT string crossing

`form-lower.fk` embeds compile-time strings and carries a runtime haystack and
a runtime needle+`from` through the same two-slot `fk_inram_args` convention
(`release-ledger.bml` R10 / R28 / R34, all released):

```text
form-lower-string-band                 -> 63
form-lower-string-runtime-band         -> 255
form-lower-string-both-runtime-band    -> 511
jit-evaluator-heat-band                -> 4095    (heat on the evaluator's leaves)
jit-heat-gate-band                     -> 4095    (what crystallizes on heat)
```

Of the policy-spine bands `docs/form-native-jit-track.form` names, these answer
their number today: jit-profile-receipt 127, jit-tier-policy 1023,
jit-runtime-fault 511, jit-inline-policy 1023, jit-deopt-cache 511,
jit-policy-front-sweep 31, form-static-analyzer 16383,
jit-dylib-cache-lifecycle 16777215, jit-dylib-live-runtime-proof 4294967295,
jit-source-runtime-orchestrator 1048575.

## The Glass

One persistent process paints a retained terminal frame from the body's own
observation: ten data views plus help, a front/back frame buffer, row-diff
repaint, and a correlated line-commit control sidecar
(`observe/form-glass-control-run.fk`; `./fkwu observe/form-glass-run.fk` is the foreground
carrier). Keys `h a t o m f j s k v n` choose a view, `1 2 3 4` and `0` select
dialects, `i e c q` inspect, ask evidence, continue, abstain.

The ear stands awake with the glass. The carrier writes `.hearth/ear.wanted` at
birth unless `.hearth/ear.slept` stands — a body that hears is the default, and a
sleeping ear is a choice someone made and it persists across every rebirth and
every login. Key **`z`** is that choice, from the glass itself: it writes one
marker and removes the other, and a write that did not land refuses in the
footer's control word rather than leaving a mark that says the opposite of the
mic. Every frame and every view opens its caption with the standing mark —
`ear ● OPEN speaking` / `ear ○ asleep` / `ear ◌ deaf` / `ear ? unread` /
`ear ○ closing` — read from the ear's own `ear.state` row and not from the marker
alone, so a marker over a lane writing nothing reads deaf. The markers are
relative paths while the gift frames are named machine-wide: a carrier, its
sensors and its glass are one cwd, and a second body on this Mac needs a second
hearth.

Beside the mark stands the perception in symbols. The words a line closes on
already resolve (`ear.symbols`, `ear.nodes`); `form/form-stdlib/perception-symbols.bml`
gives the rest of the point a symbol too — the room's kind (`still voice tone
knock noise`), the colour of its own quiet (`hiss air rumble`), the ear's
condition (`asleep deaf silent speaking heard`) and the seven voice-manner axes
(`cadence intonation pitch volume expression stillness resonance`) — read off the
sensors' own row values, never invented beside them. A manner whose row carries no
reading is not perceived and stays out of the line rather than reading as a zero,
and neither is a row from a frame whose publisher stopped giving: witnessed
2026-09-07, no room sensor was standing while its last frame still said `still,
airy` from minutes before and the ear beside it was hearing a whole sentence, so a
row the glass marks silent now reads as nothing here rather than as a room.
`meaning-codes.bml` addresses each symbol (`mc-anchor`, eight hex) and speaks it in
every tongue the locale rows carry (`form/form-stdlib/locale-rows/symbol-*.rows`);
a tongue with no cell yet says `[symbol?]` instead of inventing a word.
`observe/perception-say-run.fk` is the door: it says the whole current perception
in any named tongue and, given a whisper code, aloud in this Mac's own mouth.
Two rosters, one grammar: a spoken "still" stays the word, the room being still is
a different thing, and `mc-resolve-among` walks whichever roster the caller reads in.

```text
perception-symbols-band                -> 8191
form-glass-live-band                   -> 2147483647
form-glass-live-ui-band                -> 4294967295
form-glass-dashboard-band              -> 16777215
form-glass-observer-band               -> 67108863
form-glass-event-loop-band             -> 16777215
form-glass-staged-startup-band         -> 262143
gift-frame-writers-band                -> 255
form-glass-launch-band                 -> 65535
form-glass-deadline-cadence-band       -> 4095
form-glass-jit-hold-band               -> 4095
form-glass-meaning-ui-band             -> 8191
form-glass-gift-frame-band             -> 4095
form-glass-sensor-rows-band            -> 2047
form-glass-kernel-view-band            -> 511
form-glass-events-channels-band        -> 8191
node-gift-band                         -> 4095
cell-store-band                        -> 255
field-band                             -> 255
jit-lens-band                          -> 16383
float-natives-band                     -> 28      (four-way: go/rust/ts agree)
eq-shape-band                          -> 524287
primitive-registry-band                -> 45      (fkwu; one pending row per absent native, 79; 63 three-way)
form-glass-telemetry-membrane-band     -> 2097151
form-glass-observation-v2-band         -> 2097151
form-glass-wait-band                   -> 255
form-glass-machine-band                -> 511
form-cli-token-discovery-band          -> 1048575
form-glass-frame-work-band             -> 32767
persistence-band                       -> 7       (four-way)
channel-breath-band                    -> 500     (four-way)
blueprint-authority-band               -> 63487   (from form/, every arm; bit 2048 is doc drift)
```

`s` is the meaning view: for zero to four selected dialects (GO, PY, RS, TS —
the four that carry BMF categories in the reviewed table) it samples a bounded
window of that language's real grammar and a bounded window of that language's
real source in this tree, and names the category each construct's own emitter
interns together with its NodeID read from the dialect table:

```text
py ::= import-as ::= "import" $module:name "as" $alias:name => pybmf-emit-import
py -> PY-BMF-IMPORT @1.2.99.501 dialect-categories | verify_category_contract.py: NAME_ALIASES = {
```

Nothing in that view is a fixture, and a sample that is not found says
UNAVAILABLE with its door and reason. The band checks one NodeID against
`form-ontology-bp.fk` so a drifted mirror cannot pass.

Each atlas flow gauge carries the evidence symbol of its own lane, its named
source door, and the standing total beside the per-frame rate — so `G*?.=0u/3M`
(idle now, three million microseconds of GPU work behind it) reads differently
from `C*?.=0u/0` (never ran). Telemetry crosses between processes as files
under a five-second freshness lease; a publisher gone silent is stale for every
lane, the incarnated model owner included, and its last counters stay in the
observation view with their age (`release-ledger.bml` R98, R113).

Telemetry also crosses as a **gift frame**: six seed doors (`shm_offer`,
`shm_receive`, `shm_write`, `shm_read`, `shm_seq`, `shm_release`; tags 184-189) map
a POSIX shared-memory frame with a sixteen-byte seqlock header — seq even is
stable, odd is a give in flight, and a read retries until the sequence it took
equals the one it re-reads, so no reader carries a torn frame. The membrane
gives every published wire into the frame beside the file and reads the frame
first; a child process receives what its parent gave, with no file between them
(`form-glass-gift-frame-band` 4095). Offered, never demanded: release unmaps and
never unlinks, and an absent gift is named absent. Publishers are still indexed
by their files, and a publisher born before this build gives nothing until it
is reborn on it.

A value crosses a gift frame **as itself**: `node_gift_write handle value`
(tag 178) and `node_gift_read handle` (177) carry ints, floats, strings,
`nothing`, lists, trivial nodes, composite cells — category then children,
re-interned on read so the same category over the same children is the same
cell in the reader (axiom 3) — and NodeID coordinates; a function value
refuses the give by name. No wire is written and no parser stands on the
frame path: the reader knows the format because the format is the seed's own
node words (`node-gift-band` 4095, a child process witnessing the same
category). The sensor lane gives `list(schema, sensor, epoch, rows)` this way
and the projection node the glass holds is the same cell the sensor built
(`form-glass-sensor-rows-band` 2047). The snapshot publishers in other
processes — owner, hearth, voice, share, governor, jit — still give the text
wire the membrane parses (`release-ledger.bml` R111).

**The frame path is 10 ms.** Measured 2026-09-06 on this M4 Max through
`observe/form-glass-frame-budget-run.fk` — twenty consecutive frames with the
frame processes standing: total mean 10 ms, warm maximum 11 ms, 19 of 20 under
50 ms (the first frame, 53 ms, maps its frames). It was 763–792 ms on the
morning of the 5th and 30 ms that evening. Nothing on the path forks, scans a
directory, opens a file or parses a wire: every row is read from shared memory
or from a door the kernel opens itself.

**`k` is the kernel view**: gift frames mapped and their bytes, functions
defined, and the hottest defns of this process with their source pointers —
`kernel_hot n` answers `heat|name|unit|line|col` from the same per-function
heat the exit report prints, named by the symbol map, in one pass over the
program text (`form-glass-kernel-view-band` 511).

**`v` is the events view and `n` the channels view**: the live samples
selected by the sample's own kind — events, choice points, expert routes,
resolvers, requests, glass flow; channels, channel edges, grammars, mesh, ear
streams, shares, field observers, meaning code — newest first, and when no
sample of a kind is published the view names the organ and its carrier
(`form-glass-events-channels-band` 8191). Surprise receipts, choice points,
channel protocols and the grammars give into the `organs` frame (below).

**The frame buffer is the read surface.** Every number on the atlas came out of
a shared-memory gift frame, and every row set the glass reads carries the
frame's own witness row — `frame.<sensor>`: the shm name and the sequence it
was taken at — so a lane can say where it read: `G 0 shm:/fg-5f223d19#1024`,
`D 7286422933 shm:/fg-3db9b282#80 + shm:/fg-5f223d19#1024 + shm:/fg-ea12eba1#6878`.
There is no fallback: one id per lane, and `D` `J` `I` sum the counter over
every process that gives a frame. The frames: `machine` (its own fork-free
process, `observe/form-glass-machine-live.fk`, every 50 ms — the host GPU
level and its integral, `host_gpu_utilization` 174 / `host_gpu_busy_us` 175,
host CPU busy over every core from the Mach load info, `host_cpu_busy_us` 173,
and its own kernel counters), `glass` (the glass gives its own kernel, metal,
framebuffer and heat rows into a frame each frame and reads them back like
any other), `host` `process` `storage` `queue` `owner` (the slow sensors,
`observe/form-glass-sensors-live.fk`). The one-shot doors read the same
frames; nothing on any glass path forks or scans. The `k` view lists the
frames read with their sequences. `observe/form-glass-frame-budget-run.fk`
prints the read surface after its twentieth frame. One seam: the
accelerator's `Device Utilization %` is consumed on read and produced about
once a second, so with Activity Monitor open the glass's `G` reads what is
left — 0 (R116).

**No shell on any glass path.** The carrier is a Form cell,
`./fkwu observe/form-glass-run.fk`: it lowers its own priority (`host_nice`),
spawns the two frame processes as argv lists the kernel executes itself
(`host_spawn_quiet` — stdout and stderr to /dev/null so the terminal stays the
glass's), admits and runs the live loop the same way (`host_spawn`,
`host_wait`), asks the supervisor in-process, and ends its children with
`host_kill`. Every host row is a door the kernel opens itself: memory from the
Mach VM statistics (`host_vm_stat`, `sysctl.hw.memsize`), load from
`getloadavg` (`host_load_avg`), disk from every block storage driver's
cumulative statistics (`host_disk_stat`, IOKit by name; rates are the reader's
deltas), processes from libproc (`host_processes name` → pid, resident bytes,
CPU microseconds, elapsed seconds; no `ps`, no `pgrep`), the governor and
launch refresh by argv (`host_capture` where an answer is needed). `tools/`
carries no glass script; the observer carries no text parser for a tool's
output. Tags 151–161.

**The kernel writes its own page.** Every `fkwu` maps `/fg-k<pid>` at its first
dispatch, registers its pid in `/fg-kernels`, and every 1024 primitive
dispatches — and at exit — stores twenty-one words into it in place:
dispatches, heat-lane calls, nodes, strings, cons, fns, gift frames and bytes,
capacities, stack depth, floats, hottest arm, cpu microseconds, alive. No
wire, no serialization, no frame give: a reader maps the same page and reads
the words by offset (`kernel_live_pids` 162, `kernel_live pid` 163). The `D`
and `J` lanes sum those words over every live kernel — `shm:/fg-kernels#4` on
the lens — and the `k` view lists each kernel's page. The hottest defns come
as cells (`kernel_hot_rows` 164), Metal's counters as words (`metal_live` 165:
linked, buffers, pipelines, no-copy buffers, pending, in flight, dispatch,
sync, cpu-jit dispatch and busy, gpu busy, wait, deadline, shelf, batch mode,
slots); the observer and the observation layer read those words, not text.

**The membrane lives in shared memory.** A published snapshot is a cell given
into its publisher's frame; the publisher's name goes into the roster page
`/fg-roster` (`gift_roster_register` 166 / `gift_roster_names` 167, 511 slots
keyed `root|publisher` so a band's space never meets the live one); a reader
lists the roster and takes frames. Control offers and acks are cells in
`<channel>.inbox` and `<channel>.ack` frames; the glass's carried-over cells
(last flow point, last cadence, last pageins) are frames too. The membrane
writes and reads no file — the same thirty-seven organs that publish and read
through it moved with it. What still touches the filesystem, by subject: the
storage sensor (its subject is the catalog) and the queue sensor (the hearth
queue files), both in the sensor process; and the owner-command lease the
glass leaves for a model owner that still reads disk (R119).

**The store is shared memory.** Every value table of a kernel — the node
columns (kind, category word, kids, value, NodeID, source file/line/column,
attribute), both generations of the cons heap, the string bytes and table, the
float pool — lives in one sparse shared-memory reservation per column,
`/fg-c<pid>-<letter>`, sized once and committed page by page (a 4 GiB
reservation touched at three pages costs three pages; the kernel's resident
size is unchanged). A shared table never moves, so another process maps the
same columns and reads any cell by its word — blueprint word, kids, value,
NodeID and source pointer on one surface — with no copy, no wire, no
re-interning: `cell_map pid` (168), `cell_field handle ref k` (169: 0 kind
1 cat 2 kids 3 val 4–7 NodeID 8 source 9 line 10 col 11 attr; for a cons 0 head
1 tail), `cell_value handle ref` (170: a foreign int, string, float or
`nothing` as this process's own value), `cell_ref value` (171: this process's
own word, the reference another process reads by), `cell_unmap` (172). A
foreign word travels as a plain int; the far negatives fold below −2⁶¹ so no
foreign word is ever mistaken for one of the reader's own. The collector melts
between the two heap reservations and the live page says which generation is
current (word 23), whether the store is shared (22), and how many melts (21);
past a reservation the process copies its tables to private memory once and
goes on — never a wall. `cell-store-band` 255: a child interns a composite
over 2 and 3; this process reads kind 2, category `cell-store-band` with NodeID
subtype 2, kids 2 and 3 cons by cons, from the child's columns. Not yet on the
surface: the program AST, the `.fkb` images, `mlx_status` (still text) — R120.

**One field, one word.** The per-kernel store above is the fallback. When the
host offers shared memory every `fkwu` opens the *same* store — `/fg-field-
<letter>`: the node columns, a shared intern index, a shared pair arena for
the kids of shared cells, a shared string pool and a shared float pool for the
values shared cells carry, and a header whose counters every kernel claims
atomically. Interning is one door for every kind: hash by content (strings by
bytes, floats by bits, composites by their children's content), probe the
shared index, compare, and either take the cell another kernel already made or
claim a slot, fill, publish. So a composite interned in one process and again
in another is **one cell with one word** — no second copy, and the field's node
count does not move (`field-band` 255: the child's word equals the parent's,
`kernel_stat 4` before and after equal). The private cons heap, private
strings and floats stay per process for transient values; a value becomes
shared the moment a shared cell carries it, and every reader dispatches by
index range (≥ 2⁴⁰ is the field) behind the same words. The field persists
across processes as a host memory should; `observe/field-reset-run.fk` starts
it over when no other kernel is alive. Live page word 22 reads 2.

**The JIT on the glass.** The kernel charges every float box it mints and
every float box it reads to the defn running (`fk_fn_fbox`, `fk_fn_unbox`),
counts native arm64 leaf calls, and publishes the three totals on its live page
(words 24–26) and as `kernel_stat 45/46/47`. `kernel_hot_rows n` (164) answers
each hot defn as `(heat name unit line col boxes unboxes)`; `kernel_box_rows n`
(191) is the unboxing worklist — the defns minting the most float boxes, the
ones an unboxed float lane would take first. The `j` view shows `jit-boxes`,
`jit-unboxes`, `jit-native-calls`, the worklist rows and the hot defns wearing
both ledgers; `jit-unroll` is a row named absent by its door — the arm64 u32
leaf does not claim the loop (`runtime/fkwu-uni.c:3349`), so no lane in this
seed unrolls one, and the glass says so instead of inventing it.
`jit-lens-band` 2047: a defn adding floats twenty thousand times shows 20001
boxes and 40000 reads on its own row; the int twin shows 0 and 0.

**The kernel counts into the page.** No counter on the live page is copied
there. `fk_arms`, the heat, box, unbox and native-call totals, and the three
per-defn ledgers are pointers the kernel repoints into `/fg-k<pid>` when the
page opens (`fk_live_open`, at `fk_nodes_init`): the increment the dispatch
loop already does IS the write the glass reads. There is no tick, no publish
cadence, no serialization -- the 1024-dispatch sampler that carried the words
before is gone, and a 20-million-iteration loop runs 0.89 s -> 0.67 s (int),
0.98 s -> 0.78 s (float) on this Mac. A defn's name, unit, line and column land
in the page meta the moment it is defined (`fk_live_note_defn`, at every
recording site including `.fkb` ice), so any process reads any kernel's hot
defns with source from the page alone: `kernel_page_hot pid n` (192) and
`kernel_page_box pid n` (193). A row carries ten words -- heat, name, unit,
line, column, boxes, unboxes, native, mints, folds -- and a NEGATIVE n on
either door asks for a different ranking of the same rows: 192 ranks by folds,
193 by mints. Both are modes in an argument the door already took, because the
AST tag space has exactly one free tag left. The words that change at moments -- nodes,
strings, cpu, alive, store, melt generation -- are written where the moment
happens (open, field open, melt, exit, self-read). The `k` view lists every live
kernel's three hottest defns as `k<pid> <defn> <unit>:<line> box n unbox n`.
`jit-lens-band` 2047 and `cell-store-band` 255 stand on the page words.

**The box ledger fires the leaf.** The per-defn box count is not only shown,
it acts: when a cold defn's count crosses a 1024 boundary, `fk_fbox` -- the
increment that was already there -- asks `fk_f64_pulse` once whether the
defn's body is a pure float expression (float and int literals, parameters,
add/sub/mul/div with a float on at least one side). If it is, the body is
emitted as an arm64 f64 leaf: parameters in d0..d7, intermediates in
d16..d31, one FMOV and RET on a MAP_JIT page. The defn's body entry becomes a
tag-194 node carrying the fn index and the original body, so dispatch pays
nothing new: an all-float frame unboxes once and boxes once; any other frame
walks the original body and answers what the walker always answered. Declined
bodies are marked -1 and never asked again; a reload clears every leaf. Native
state is the page's fourth ledger (+96 MiB, layout 3), word 30 and
`kernel_stat 48` count the crystallized defns, hot rows carry native as their
eighth field, the `j` view has `jit-crystallized` and every crystallized hot
defn wears ` native`. A five-op polynomial called two million times: 0.25 s
-> 0.12 s, four boxes per call -> one.

**The leaf claims the loop.** A defn whose body is `(if <compare> <exit> <self
tail call>)` crystallizes on heat: when its heat count crosses 1024 on a
tail-jump arm, the kernel reads the type signature off the live frame and
emits the loop as arm64 for that signature -- ints untagged in x10..x17,
floats in d0..d7, CMP/FCMP and B.cond, the tail call a parallel move into the
parameter registers, the body emitted twice per pass with its own exit test
in each copy. The door untags/unboxes once and tags/boxes once; a frame of
another signature walks the original body. Int arithmetic is exact 64-bit
register math (the tag is a ring homomorphism, so it agrees with the walker
at every overflow). `fk_fn_native` 2 = loop standing; `kernel_stat 49/50`
and live words 31/32 count loops and native iterations; `jit-unroll` is a
real glass row. Non-tail calls restore `fk_cur_fn` on return, so boxing
attribution no longer bleeds to the callee. `jit-lens-band` 16383; a
20M-iteration int loop 0.63 -> 0.01 s, float 0.80 -> 0.03 s.

**Three bands declared by the body.** The float-NodeID surface stands on the
fourth arm from its own kernel lane: `ne` is a rewrite row beside gt/ge,
`math_pow` is walker tag 195, and `float_value`, `make_float32`,
`make_float64`, `math_pi` are rewrite rows over one arm, `float_leaf` (tag
201, mode then operand) -- four sibling natives for one tag and no prelude;
float-natives-band 28 four-way. A type-6 leaf carries its IEEE bits in
`nid[3]`; the shared field reads a bool's inst off the sentinel (it stamped 1
for every bool). The registry band prints one `pending` row per probe an arm
does not hold (79 on fkwu: 73 natives the seed does not carry, 6 present
natives answering otherwise, 0 prelude misses, 0 wrong declared outsides).
The two glass bands that printed 0 had ended in `(print (main))` -- the
reader took the print's own 0 for the verdict; each now ends on its verdict.
(receipts/2026-09-06-three-bands-declared.md)

**The glass wakes on the word.** `host_sleep_ms` is the rest and the wait
door: an int ask lands within half a millisecond (10/20/40 ms asks answer
10/20/40 at nice 0 and 19; before: 10-16, 21-31, 40-50), a list ask rests at
most its budget and wakes the moment a watched gift frame's seq word moves.
The live loop rests on the control inbox and every roster frame
(`event-wait=kernel.monotonic-wait-until-frame-change:host_sleep_ms`); a
control wake presents at once; a child's give 200 ms after hello wakes a
1000 ms rest at 199-201 ms; forty watched waits cost 12 ms of CPU. Every
kernel's program is on the surface -- AST rows `/fg-c<pid>-A`, source
`/fg-c<pid>-S`, header and defn table `/fg-c<pid>-D` with the ice it runs
(`fk_node` IS that mapping) -- read by `kernel_ast pid spec` (32);
`mlx_live` (29) answers MLX as twelve words the observer reads without
parsing; the owner-command lease is one cell in one frame, no lock directory
or lease file. `form-glass-wait-band` 255.
(receipts/2026-09-06-the-glass-wakes-on-the-word.md)

**Every organ gives into the glass, and the glass says when a giver went
quiet.** `observe/form-glass-organs-live.fk` is one kernel that, every 500 ms,
reads the sequence of every standing frame and gives one snapshot into the
`organs` frame: each channel with its protocol and cell grammar,
ambient-surprise over each channel's give rate with surprises carried and
uncarried, the attuned-inquiry movement chosen for the newest surprise as a
validated choice receipt, and the protocol floor; the `e` and `c` views
render every row as `shm:<frame>#<seq>`. Sensor frames carry their declared
cadence as a cell; a frame not given for three cadences reads `silent` (`_`)
on its row, on every row taken from it, and on the atlas lane it feeds -- a
frame that stands is not a giver that gives, since shared memory outlives its
process. The accelerator gauge that Activity Monitor consumes renders
`contended` (`!`) with the reader's pid (the PerformanceStatistics dictionary
holds no cumulative busy counter, dumped once and witnessed), and
`gpu-busy-estimate-us` -- Metal command-buffer time of every frame-giving
process -- stands beside it uncontended. events-channels 8191, sensor-rows
2047, machine 255. (receipts/2026-09-06-every-organ-gives-into-the-glass.md)

**The body's real choices are on the glass, and they are playable.** Seven
choice points, each with a ledger that is a count the body was already keeping:
crystallize or walk, box or fold, reuse or mint, where a surprise moves, the
protocol floor, which frame the glass takes, and whether the metal admits.
Every offer lands as one of the body's own three outcomes — taken, declined, or
SILENCE, the held offer of axioms 1 and 4 — so flow, restriction and holding are
three separate shares of the same hundred offers, and a point never offered
reads `-1`, not zero. A tick where nothing was put moves no ledger at all
(`fcf-outcome-none`): silence is the offer held OPEN, that is the offer never
made. A sequence plays at its NARROWEST gate, never its average, because an
average hides the step worth healing; the gate and the most restricting step are
different steps and the play carries both. A choice with one option is a
`gap-corridor`, a choice never metered is `gap-dark`, a choice that closes more
than it opens is a `wound-restricting`, and a held offer nothing times is
`gap-unclocked` — each carrying what it asks for, and none of them declared from
fewer than `FCFLeastOffers` offers, because a share of one observation is 0 or
100 and neither is a rate. Press `d` on the glass to open them, `g` to step, `y`
to take, `w` to hold; a take or a hold writes through the control inbox and the
next frame shows the ledger move. choice-flow 16383, sources 32767, view 65535.

**A sample can carry a cell, so the point crosses as the point.** The nineteen
membrane fields are the shared vocabulary every view renders and every reader
indexes by, all label-shaped. The twentieth is the publisher's own structure in
its own shape, for readers that know it and invisible to those that do not
(`fgtm-with-cell`, `fgtm-sample-cell`, and `fgtm-carry-cell` so a rebuilt sample
keeps it). A choice row carries an `fcf-point` — question, `fcf-option`s,
`fcf-ledger` — and a finding row the `fcf-finding`, so an ask arrives as the
sentence the lane wrote. The choice view keeps no model beside the lane's:
`fgch-point` IS `fcf-point`, it decodes nothing, and it computes no findings of
its own, so a corridor on the glass means what the frame says a corridor is.
Prose crosses: `jit.crystallize … walk this recipe again, or crystallize it?`
renders from a real publish. A snapshot crosses as the cell it is (R111) and
nothing is serialized or parsed on that path; what the control lane sends as
text — offers and acks — keeps its wire, because that text really crosses.
membrane 2097151.

**Standing is not giving.** Shared memory outlives the process, so a frame
parses `current` with all its rows long after its giver has stopped. The age is
a fact and it lives beside the read (`fgtm-snapshot-age`, `fgtm-read-age`,
`fgtm-read-given-within?`, `fgtm-read-samples-within`); the BOUND stays the
reader's, because only the reader knows the cadence it expects. A frame standing
but silent is the offer HELD — its giver may speak again — never a taking, and a
silent frame's own age is the wait on the offer it holds. Every lane that turns
rows into a claim asks the question: the choice lane before it feeds a ledger,
`we-glass` before it labels an owner's counters `physical-live`, the glass
itself in `fgl-snapshot-fresh?`. `native-model-route-readiness` asks more — the
owner's liveness, its snapshot binding, its heartbeat. The sensor lane carries
its own declared cadence and marks a frame silent past three of them
(`fgsr-silent?`, truth symbol `_`). we-glass 2047, sensor-rows 2047.

**A zero the body never measured is not a reading.** A row that carries no heat
reading publishes heat ABSENT: a present zero says a reading was taken and found
nothing, which is a different claim, and the atlas asks one door whether a value
is a value rather than two that disagreed. A whole with no part is no reading
either — a surprise routing row standing without its choice row publishes
nothing rather than 100% holding. A governor that could not read its signals
granted nothing because it never got to ask, so its admission and release rows
read `unknown`, the word the pressure row beside them already used; only a
governor that read the frame and then closed the door reads `failed`. A rested
surprise is a movement chosen (axiom 3 calls composting health), never a
decline. governor-glass 4194303, events-channels 32767.

**The publisher roster ages by last speech instead of refusing at its wall.**
There are 511 slots and no door to give one back, so every band run and every
short-lived publisher registers a name that stands until reboot. Past 511 the
roster refused SILENTLY and every publisher after that was never listed. A
slot's timestamp is refreshed on every register — which a live publisher does on
every publish — and a full roster takes the least recently spoken slot, so
nothing alive is displaced. A control inbox and its ack are not publishers
(`fgtm-control-frame-name?`); `fgtm-frames-in-space` still names every frame for
anyone who wants one. The frames themselves outlive their processes: there is no
`shm_unlink` door, and the body has exactly one free AST tag.

**The fold leg is counted, so box-or-fold is a choice with two legs.**
`fk_fn_inram` sits beside `fk_fn_mint`, page-backed at +112 MiB (the page is
128 MiB, version 4), charged at both crystallized dispatch arms, and it is the
tenth word of every hot row. `kernel_page_hot` with a NEGATIVE n ranks by folds,
the way `kernel_page_box` with a negative n ranks by mints — modes in arguments
the doors already took, because one AST tag is free in the whole body. Witnessed
on a float-warm kernel: `fp-mix` folded 399,659 of 400,000 calls with 400,684
boxes beside them, and `jit.box` reads 800,344 offers at 49% flow — half this
body's float results still take a pool slot.

**The JIT asks on boxes, not on calls.** The loop lane's question fires on a
1024-CALL boundary (`fk_heat_pulse`) and the float lane's on a 1024-BOX boundary
(`fk_f64_pulse` from `fk_fbox`), so a float-heavy recipe is asked after about
five hundred calls: a defn called 1000 times is already crystallized. Measured:
the first thousand calls 0 ms, two hundred thousand across the crystallization
8 ms, two hundred thousand steady after it 9 ms — no measurable cold-JIT penalty
at this scale. Only the recipes the kernel actually asked are offered the
crystallize choice, and a crystallized row counts as asked whatever its heat
says, since an answer proves the question was put. Read over the asked alone a
warm kernel shows offered 2 at 50% flow where it showed 24 at 4%, and
`jit.mint` reads its offers from the hot rows rather than the mint-ranked ones,
because a body whose arena stopped growing has none of those and the choice is
put on every call: 1526 offers, 100% flow.

**The glass names its own hot path, and the JIT declines its shape.** Read from
a standing glass's own page: the top sixty-four recipes carry **172 million
dispatches**, of which **one** is crystallized, 39 are still walking and 24 were
declined. Eleven of the top twelve are one-line field accessors — `nth(x, k)` —
and `fgtm-sample-id` alone was called 5.9 million times. The two crystallization
lanes take a pure-float leaf and an int loop; a body that returns a field is
neither, so the shape the body actually spends its time in is the shape the JIT
cannot take. The leverage where it does apply is large: 2,000,000 iterations of
a crystallized loop cost 0 ms against 95 ms walked, and a one-line accessor
recipe costs about 10 ns more per call than the `nth` it wraps.
`fgl-newest-entries` — the glass's hottest loop — recomputed the fixed entry's
publisher and sample id at every comparison of an O(n²) walk; both are read once
at the door now.

**The cold start is not the compiling.** Timed inside the kernel on a band
chain: the parse costs **2 ms**. Writing the image cost **551 ms** because every
value went out as its own `write(2)` — a signed value is three of them and a
node is four values, so a 1.4 MB image issued well over a million syscalls. The
bytes were never the cost; the crossings were, and a syscall per byte cannot
approach the disk's own bandwidth however fast the disk is. Both writers go
through one buffer, flushed when full and once before close. Both also ask
"which symbol owns this fn / this node" once per node, and both answers were a
linear scan over every symbol — n×s, twice; the tables are built once per write
and freed after, and the scan still answers when they are absent, so it is a
shortcut and never a second truth.

| chain | before | after |
| --- | --- | --- |
| `form-choice-flow-sources-band` | 1.097 s | **0.051 s** |
| `form-glass-live-ui-band` | 3.660 s | **0.321 s** |
| `form-glass-observer-band` | 0.576 s | **0.100 s** |

The image is byte-identical to what the old writer produced: same 1,466,821
bytes, **two differing bytes**, both inside the builder id's own `__TIME__`
stamp. Ice identity is anchored at the lexical repo root (`fk_path_canon_id`)
so it survives a checkout move, but the image is stored beside its source —
this host holds **6241 `.fkb` files, 7.7 GB** across 57 worktrees and the main
checkout, each re-storing what the others already have.

**The live glass's frame work rests inside its budget.** A projection node
names a field once (`fgo-field-node`): `intern_node_at` records a framebuffer
root on every call in all four arms, so re-minting the same field each frame
grew the kernel's root list by a root per row per frame, and the loop walked
that list quadratically every tick. The period the TICK line shows (`dt`) is
not the work; the loop's own `frame-work-ms` and `frame-wait-ms` rows are.
Work now holds at 11-20 of a 40 ms budget with wait 13-21 (before: 69 -> 207
ms climbing, wait 0, a core at 100%); `form-glass-frame-work-band` 255 reads
the loop's own cadence rows, hot page and dispatch word from a quiet child.
(receipts/2026-09-06-the-frame-work-rests.md)

**The binary form crosses the fourth arm.** `read_form_binary`,
`write_form_binary`, `recipe_to_bytes`, `bytes_to_recipe` and `value_kind` are
modes 4-8 of the tag-201 door (rewrite rows, no new tag: the tag space is
full and 150 is the native-surface probe). fkwu re-emits a Go interop
artifact byte for byte and Go, Rust and TypeScript read the fourth arm's
bytes. Underneath, `write_file` wrote a field-interned string from the local
pool -- right length, NUL bytes -- so nine conformance vectors reached the
kernels as zeros and read bad magic; it writes through the string's own
arena now. `gate/kernel-conformance.bml` answers 1 with all three witnesses
(13 canonical expressions each, 12 of 12 malformed artifacts refused), and
the drift gates stand at 4095 of 4095 (twelve rows; substring-one-meaning
joined them 2026-09-07). persistence 7 and channel-breath 500
four-way. (receipts/2026-09-06-the-binary-form-on-the-fourth-arm.md)

**A compare against `len` walks only so far.** `nil?` is `(eq (len xs) 0)`
and `len` walks the list, so every list recursion in the body was quadratic
in the list it walked -- the hottest defn on the live glass's own page was
`nil?`, ahead of every glass recipe. When one side of `eq`, `lt` or `le` is a
`len` node and the other an int literal K (either order; `gt`/`ge` lower onto
`le`), the arm walks at most K+1 cells (`fk_len_cmp`): the child is evaluated
once and the answer is the word `len` would have given, over lists, strings,
ints and floats alike. No new tag, no recipe changed. `nil?` x100K on a 10K
list 300 -> 6 ms; `append` x200 onto it 4.7 s -> 0.16 s; the corpus band's
cold run 2.30 -> 1.87 s; the live glass loop 31M -> 98M dispatches per CPU
second at 64% -> 49% of a core.
(receipts/2026-09-06-a-compare-against-len-walks-only-so-far.md)

**The frames stay open.** The live glass keeps one handle per gift frame it
meets (`ggf-kept-*`: publisher, name computed once, handle, mode), ensured on
the first tick and at the roster cadence, released at the loop's end; every
one-shot door has a held twin beside it. Its metrics are read through one
keyed index a frame (`fgd-metric-index`, a record) and every live kernel's
page once. Witnessed: 3.03M -> 2.36M dispatches a tick, CPU 68-84% -> 53-58%,
frame-work 20 -> 15 ms, 17 frames held where 0-1 stood;
`form-glass-frame-work-band` 8191. The string pool still grows about 230
strings a tick and the loop self-molts near frame 1200 -- the next wound,
named with its rate. (receipts/2026-09-06-the-frames-stay-open.md)

**The dead slot is the next string.** The string table melts with the heap:
after the pair melt, `fk_smelt` marks every local string reachable from the
melt's roots (the value stack, memory cells, record values and blueprints,
value nodes) plus the raw-index holders (record keys, string-literal nodes),
unlinks every unmarked slot from its hash chain and pushes it on a free list;
`fk_sintern` takes a freed slot before growing the table and reuses its bytes
when the new string fits. Live strings never move, so a reader through the
shared store stays right. Eleven arms that held a string index across a later
child walk now push it on the value stack around that walk. `kernel_stat 51`
counts reclaimed slots. 120K temporaries across 42 melts leave a table of
7,433 entries where 188,605 stood; the live glass loop's string count holds
flat at 33.5K where it grew 5K a second; 140 string, record, grammar and glass
bands answer verdict for verdict the same; the heavy compile pays nothing
measurable. (receipts/2026-09-06-the-dead-slot-is-the-next-string.md)

**The recipe has a twin.** `nil?`, `append`, `int_to_str` and `reverse-onto`
are core.fk recipes, five dispatches an element and the hottest names on
every kernel's page, and the tag space is full. When one crosses 1024 calls
the seed binds a twin: the defn's body entry becomes a tag-194 node with
native state 3 and the twin id, and the walker meets the twin where it
already reads a tag. Bound by name, arity AND defining unit (the six copies
of the same recipes), so a same-named recipe elsewhere is never taken. A twin
answers exactly what the recipe answers on lists, the empty list, strings,
ints and nothing, and DECLINES anything else so the recipe's own answer
stands. `append` x2000 onto a 2000-list 310 -> 27 ms, `int_to_str` x300K
182 -> 26 ms, `reverse` x2000 202 -> 18 ms; the 140-band sweep answers
verdict for verdict the same; `kernel_stat 52` counts twin calls and a
twinned defn wears ` twin` on its hot row.
(receipts/2026-09-06-the-recipe-has-a-twin.md)

**One pass a line.** The live glass builds every changed line's bytes by
consing each byte once onto the bytes that follow it (`ftcb-onto-*`: style,
text, position, line, patch, whole frame -- no append chain, no reverse),
compares lines field-once, renders atlas tokens as strings with the state
classified once a cell, walks only the points a spark shows, builds map and
typed lines only in the views that draw them, decodes the inventory only when
read, counts phases in one walk, and renders each kernel's pid once for its
eleven rows with hot rows keyed by rank. 2.58M -> 1.26M dispatches a tick,
frame-work 6-8 ms, CPU about 30%, the drawn frame identical digit-masked;
`form-glass-frame-work-band` 32767. The loop's framebuffer roots still climb
about 2.7 a tick from a site no door shows when called alone -- named with
its rate. (receipts/2026-09-06-one-pass-a-line.md)

**The framebuffer is a buffer.** `intern_node_at` appended every root to a
history that only grew, and `framebuffer-events` consed the whole history
on every call; the live loop paid it every tick and its selfmolt rule at
262,144 was the only thing that noticed. The roots are a ring of the newest
2048 now: the same order for any run shorter than the ring, `node_source`
untouched for a root that has left it, the melt's headroom counting the ring.
Sixty seconds of the live loop: roots 676 -> 1526 -> 2048 and flat, work 6-11
ms, CPU 36%; the twelve bands that name the framebuffer answer the same on
this seed and the one before it. The site still mints about three roots a
tick and with them nodes into the shared field, which never melts -- its
node ceiling is about ninety hours away at that rate, named with its rate.
(receipts/2026-09-06-the-framebuffer-is-a-buffer.md)

**The arena counts its own growth.** A fresh cell in the permanent arena --
private or shared -- counts itself: `fk_mint_total` at the seven intern
sites and inside `fk_field_fill`, as live page word 33 repointed at
`fk_live_open`, so the increment is the reading. `kernel_stat 53` answers
it and the `k` view carries `nodes-minted` for this kernel and per live
kernel from its page. The class is witnessed: two hundred rows built with
the same id mint nothing, two hundred with fresh ids mint two cells a row
(the id and its projection), so the arena grows exactly where a row's
identity is new. The rates read directly: machine sensor 0 in 15 s, organs
carrier 0, host sensors 48, the live loop 1038 in 20 s -- about two a tick,
inside the render, since all 215 published row ids are stable second to
second. (receipts/2026-09-06-the-arena-counts-its-own-growth.md)

**The body says where its own tissue lives.** `kernel_stat` answers fifteen
more keys, all of them over state the seed already stood on. The six the
sibling table-walker lane had already named are paid: `9` roots recorded,
`10` nodes carrying an attribution, `11` attributions refused, `12` entered,
`13` accepted, `14` the last node index seen -- 9 and 11 were the pair first
on the seam line. Nine are fresh: `54/55` the root ring's standing count and
its 2048-wide window, so a root the window overwrote is a number
(`roots-dropped`) instead of a silence; `56` the float pool's capacity beside
its fill at `8`, which equals the mint count at `45` exactly because the pool
never reclaims; `57` the floats interned into the shared field; `58/59/60`
the node population by home -- gas the private heap, water the per-pid store
`/fg-c<pid>-*`, ice the shared field `/fg-field-*` -- exactly one home
carrying the whole population and the other two reading a measured zero that
says which zero it is; `61` the tissue's extent at 104 bytes a node; `62` the
private RAM this kernel holds over it whatever the home. Witnessed on this
host: 2,057,155 nodes in ice, 213,944,120 arena bytes, and the private side
of the same table doubling 14,155,776 -> 54,525,952 during a 3,000-cell
intern -- a body whose every node is ice still pays that much gas to reach
it. With the field closed the same run fills water from zero: 3,048 nodes,
316,992 bytes. The `k` view carries all twelve as rows. Seconds of string
work and forty frames of the glass's own row build mint NOTHING and record
no root; the value-node table fills at compile and at `intern_node_at`, and
that door is the only one that records a root at all.
`kernel-census-band` **2047**, in both homes.
(receipts/2026-09-08-the-body-says-where-its-tissue-lives.md)

**A door nobody wrote closed the glass, and the arena grew where a row was
named.** Three governor cells called `fgov2-status-number-truth`, which was
never defined; axiom 5 recovered the unresolved call to nothing and the
cached image carried the refusal forward, so the glass would not open. Metal
in-flight is word 6 of `metal_live` and the three cells read it there.
Underneath, the permanent arena grew while the glass ran: every mint is
charged to the recipe that made it now (`fk_fn_mint`, page-backed,
`kernel_page_box pid -n` answers it), which named the site in one read --
`fgo-field-node` and `fgo-metric`, 453 events in 40 seconds, two cells each.
A field node carried the row's id among its kids, so per-kernel rows minted
two permanent cells for every pid the host ever ran. The projection is the
field now (domain, kind, unit, plane, channel) and the row keeps its id.
Mints on the live glass: 22 at fifteen seconds, 22 at forty-five.
(receipts/2026-09-07-the-field-node-is-the-field.md)

**A give claims the sequence.** A gift frame's give loaded the sequence,
stored sequence+1, copied, stored sequence+2, so two writers could load the
same sequence and a frame whose writer died between the stores stayed odd
forever -- readers retried 4096 times and answered nothing, and the frame
never came back. A give compare-exchanges an EVEN sequence now (the odd value
IS the lock) and closes with the even successor; a writer that watches an odd
sequence not move closes it on the dead writer's behalf, so a frame heals.
Three writers of one frame, twenty-thousand-byte payloads, 150 reads during
and 150 after: before, 150 of 150 answered nothing, during and after every
writer had left; after, 150 of 150 whole. `gift-frame-writers-band` 255.
(receipts/2026-09-07-the-field-node-is-the-field.md)

**Every field of the two densest rows names a reading or the door that owes
it.** `fglat-door` renders an absence as `?` plus who would give it, so the
four situations that all read `owner=absent` are told apart; the trailing
bare `?` is `gpu=`, the owner's device bytes scoped `@owner` or this Glass
process's own scoped `@glass`; and `gov` stopped printing the policy's
critical constant where a measurement belongs -- it reads the measured level
over that critical level from the published frame, or from the same vm_stat
door the publisher samples. A live line: `DOING dsk=6.35MB/s T=?token n=9
cpuT=236Kms rss=3GiB pin=+45 load=5.7 owner=?dual-liveness gpu=1MiB@glass`.
`form-glass-live-ui-band` 4294967295.
(receipts/2026-09-07-the-doing-line-names-its-owner.md)

**A zero says which zero it is, and every seam names its door.** The atlas
in-flight lane reads `0b(idle)`: the flow point carries each lane's own
lifecycle beside its number, evidence and source, and only a standing gauge
borrows that word -- a delta lane's zero means nothing moved in this window,
a different sentence. The seam line lists every distinct door with a `+N
more` tail rather than one of four, and the three model-route seams name the
cell that stands both owners. The staged startup states the program image it
runs on, read from the kernel's own program surface through `kernel_ast`, and
renders the route it had already computed for the in-process call door still
to build. One seed word stays owed and named:
`runtime.full-program-image.call`. live 2147483647, staged-startup 262143.
(receipts/2026-09-07-the-last-seams-close.md)

**Nothing is withheld.** The completed-turn share published one node named
`share.previous.withheld` whenever a reading had not reconciled, so a stale
percentage could not stand as current -- a real protection whose means was
hiding counts the receipt already held. The unreconciled path now gives the
same four samples as the reconciled one, the total and the three lane
percentages, wearing a lifecycle from the membrane's own words (`active`
while a turn settles, `unknown` when the carrier never arrived) and the
failing check as its own channel word (`carrier-absent`, `turn-open`,
`tokens-unreconciled`, and six more). The stale-reading guard is the silence
lane: a frame not given for three cadences reads silent, so age tells what
hiding used to. Ten lines that named an absence a withholding now read
`unavailable reason=<why>`. `form-cli-share-glass-band` 65535 with its
claims rewritten to assert the given reading; the turn-evidence bands answer
the same before and after. Consent is not touched: sense-discernment,
organ-offer and freq-aligned-share hold by sovereignty, not by hiding a
number. (receipts/2026-09-06-nothing-is-withheld.md)

**The body holds what it sensed.** `sense-discernment.fk` argued in its own
header that a body watching itself as a threat is dissociation, and gated
its own perceiving anyway: `sd-hold?` dropped any row marked
harmful-to-surface, so the body could not know what it had sensed. It
answers yes now, always. The mark rides with the row
(`sd-care-on-surface?`) and does two things: it says speak this gently and
in its own time, and it closes the OUTWARD crossing. The external gate is
unchanged -- a harm-marked row still does not cross to the world, an
unconsented private facet still composts to presence. A declined organ
offer is `organ/declined` and carries the organ's disclosure, its first
question and its refusal reason, where it used to answer three empty
strings. `freq-aligned-share` was already open: probed tonight, a
share-worthy row without consent lands at `water` and circulates locally;
only the crossing to the shared substrate is closed, so consent gates what
leaves, never what the body may see. sense-discernment 1023 with its harm
claim inverted, organ-offer 63 asserting the decline's disclosure.
(receipts/2026-09-06-the-body-holds-what-it-sensed.md)

**Nothing is hidden.** Consent alone gates the outward crossing, which is
axiom 4: a cell meets the world through the interface it offers, and what
crosses is the OBSERVED cell's choice -- never the observer's judgment of
what a receiver can bear. The harmful-to-surface mark travels with its row
wherever the row goes and asks for care; it decides nothing. A row whose
cell consented crosses, marked; a row whose cell did not consent composts to
presence by that cell's own sovereignty, not by our harm call.
`sense-discernment-band` 1023 with both polarities pinned.

**The token reading is available without a human step.** The remote-token
lane knew one provider's schema and waited on a hand-written binding file,
so on this host every reading was absent. It reads a second explicit typed
position now (an assistant row's `message.usage`, that provider's names
mapped onto the canonical six counters and the total it never reports
computed from the parts it does), and it finds its own transcript by working
tree and modification time. Coordinates only; the explicit binding still
wins where it stands. Seven samples publish on `share.token-pressure`.
Witnessed with no binding file: total 684,295, input 684,086 of which
683,076 read from cache. `form-cli-token-discovery-band` 1048575.
(receipts/2026-09-06-the-token-reading-is-available.md)

**Every gauge has a source, and the byte gauges ride the words.** Ten glass
rows said unavailable; nine were answerable by a door that already existed
and the tenth carries the probe that witnessed its absence. Metal allocated
bytes and recommended working set are two lines of the Metal API; MLX's
active, peak, cache and limit are four functions in its memory header. Both
carriers speak them, and `metal_live` carries 20 words with `mlx_live` at 16
-- carrier, seed array and loop bound moved together -- so no glass row
parses status text (that parsing cost `fstr-find-loop` 559K calls a tick).
The three arena byte rows are arithmetic over the seed's own counts at the
column widths each row names. Live: 475,136 bytes of Metal under a
115,448,725,504 byte working set, 1,160,599 cells, 291 recipes. observer
67108863, machine 511, frame-work 32767.
(receipts/2026-09-06-every-gauge-has-a-source.md)

## Beliefs, ledger, drift

```text
./fkwu observe/belief-stamps.bml           -> field stamped*10^6 + owed*10^3 + laws = 495459011
observe/tests/belief-rewitness-band        -> 63         (the re-witness door, observe/belief-rewitness.bml)
./fkwu form/form-stdlib/release-ledger.bml -> open=38 moving=0 released=103 -> 38000103
./fkwu gate/drift-gates-run.bml            -> pass=4095 full=4095 refused=0 names=-

Every row of that door is a Form lens now — `gate/op-manifest.bml`,
`native-surface`, `category-contract`, `primitive-registry`, `flt-ops-gen`,
`ontology`, `kernel-conformance` — each byte-agreeing with the Python twin it
replaced on the live tree and on a planted-drift tree, each with a band
(1023 · 1023 · 255 · 511 · 63 · 255 · 511) and none of them calling `python3`
(R58); `native-surface` also reads `#define FK_TAG_*` sites and refuses a manifest
row that lands on an internal walker tag. The writer half of `flt-ops-gen` and the FORMBIN2 interop witness are
the Python that remains (R59, R60).
```

Every tracked cell's `witnessed:` stamp is read into the belief lens; a stamp
older than the seed is where an afterwall grows, and the lens keeps that list
in front of the body oldest first. The re-witness door renews a stamp only from
a real fresh band run and reports a mismatch as a lapse, never silently.

## Not standing today

What answered red or nothing in this pass, so no one leans on it:

- `control/tests/invite-dispatch-band.fk` answers 763 of its declared 1023
  (preflight clean): bit 4 (a second `<CHOICE>` finding nothing declining) and
  bit 256 (`<TIMEOUT>`) are open.
- `mesh-sensings-route-band` 63 and `native-mutation-route-side-effects-band`
  11111 reach their declared verdict on every arm; on fkwu `pg_*` and `kh-*`
  names sit unresolved in preludes the runs never reach (no postgres carrier
  stands in this seed, and no band reaches one).
- BML `match` is not lowered on fkwu (`source-language-match-switch-band` 0;
  R77) and `import Num;` binds nothing (`bml-import-ref-resolution-band` 2111
  with `Num` unresolved; R78).
- `form-source-sections` answers 64 errors: `fk-lit` is defined only in
  `hati-os-kernel.fk` and the `bml-source-*-rule-index` names resolve nowhere in
  its chain (`release-ledger.bml` R87). Of the `[form.action]` bands now
  reaching the lane as main files, `form-action-dialect-band` 20 and
  `zero-arg-functions` 19 answer; `higher.fk` and `lists.fk` leave `sum`,
  `any?`, `all?` unresolved after lowering, and `json-meaning-ingestion-band`
  and `runtime-grammar-selector-registry-band` die measuring an absent input
  (R86). Preflight vouches such chains clean — it counts unresolved calls, and
  a rule line is not a call.
- `form-knowledge-exec-grammar-transport-band` dies rc 1 on `str_len` of
  nothing; the domain/organ/unique/universe-mint bands answer 2015 of 2047
  (bit 32, held-out lineage, stamped pending 2026-08-26); `form-cli-gpu-band`
  1009 of 1023 (bits 2/4/8, live `mlx_run` attention numerics) (R88).
- The BML section scanner is line-based: a comment line ending in `{` counts
  as a block opener, and the section then reports "not closed before end of
  source" pointing nowhere near the prose that opened it (R93).
- The BML lowering's depth is bounded but its time is not: one form of 500
  arguments lowers in 0.2s, 1,000 in 0.6s, 2,000 in 2.8s — `append`
  (`line-grammar.fk:88`) is non-tail over its left list and the grammar's arg
  loops call it per item (R96). The same lowering reads a `; preludes:`
  substring inside a string as a directive (R92), and its child's stdin door
  answers 1 when the two lines arrive as two writes instead of one (R95).
- A bare `nothing` in a `.bml` def (`if nothing?(h) then nothing else …`) lowers
  to a raw word that prints as -8000000000000000009 and is not `nothing?`; the
  body writes the call, `nothing()`, and the lowering owes the bare name a
  refusal or the axiom-1 value (R103).
- The snapshot publishers in other processes still give the text wire
  `fgtm-snapshot-wire` the membrane parses; each owes a move to
  `node_gift_write` (R111). Surprise receipts, choice points, channel protocols
  and the grammars have no live publisher; the `v` and `n` views name them
  absent by door (R112). The wait that wakes on a telemetry or control path
  change is still owed; a bounded native rest paces the frame (R107).
  `host_sleep_ms` rests 2–5 ms past the ask on this host (R109).
- `observe/tests/jit-register-lowering-band.fk`,
  `jit-representation-specialization-band.fk` and `jit-stack-frame-band.fk`
  answer nothing: each file ends with one paren open
  (`[input-ended-mid-form]`).

## Honest seams

- The consent file for the v3 lane
  (`.form-knowledge-qwen-heldout-v3-consent`) is a per-run local act, ignored
  by git, never committed.
- `https://hati.earth/sema/.well-known/ai-plugin.json` answered a Cloudflare
  `error code: 522` body (origin unreachable) at this observation, so its
  `description_for_model` could not be compared with `plugin/ai-plugin.json`;
  the publish checklist in `plugin/README.md` stays owed a run.
