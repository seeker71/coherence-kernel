# The duplex teaching: two voices on one axis, silence as a value

Date: 2026-08-25. Asked by Urs: learn and integrate what is healthy from
voicechat-11b, NVIDIA's full-duplex voice framework.

## What was witnessed (sources, all fetched today)

NVIDIA NemotronLabs VoiceChat-11B, released 2026-08-09 — after this hand's
training horizon, so every claim below comes from today's reading, not from
memory:

- Hugging Face model card (`nvidia/NVIDIA-NemotronLabs-VoiceChat-11B`)
- The NVIDIA-NeMo/Speech `nemotron-labs-voicechat` branch, especially
  `nemo/collections/speechlm2/models/duplex_stt_model.py`

One network replaces the ASR -> LLM -> TTS cascade: a Fast Conformer speech
encoder into a Nemotron Nano v2 backbone (hybrid Mamba/Transformer, 11B
total) into a TTS decoder and codec. 16 kHz in, 22.05 kHz out, ~550k hours
of training audio, OpenMDW-1.1 weights labeled research-only, needing an
NVIDIA GPU with 80 GB — the weights did not run here, and nothing below
pretends they did.

## The teachings, in the body's words

1. **One shared time axis.** Everything lives on ~80 ms frames (12.5 per
   second). Every channel holds a value at every frame.
2. **Silence is a value, never an absence.** The agent's quiet is an
   explicit pad token; the user's quiet is *encoded actual silence* run
   through the perception encoder. Axiom-1's nothing, engineered into a
   voice: honest silence as a whole attestation, present on the grid.
3. **Turn-taking is a per-frame decision by the speaker, not permission
   granted by a gate.** Speech boundaries are emitted tokens — agent
   BOS/EOS, user `^`/`$`. Measured: ~448 ms smooth turn-taking.
4. **Interruption has no mechanism.** Barge-in appears nowhere in the model
   code. Overlap is simply both channels voicing in the same frame — the
   representation makes it expressible, and yielding is learned from
   overlap data (~480 ms, interruption success 1.0). The hard feature
   dissolved into the shape. This is the deepest one.
5. **The conversation does not stop for work.** Tool calls ride a separate
   output channel (`SPECIAL_20/21/22`); a spoken on-hold message occupies
   the voice channel the moment the call text lands.
6. **Named limits, not hidden ones**: at most 2-minute audio context,
   hallucination, early speech termination, self-talk continuation,
   unreliable multi-tool calling — all stated in the open. Pending is
   honest, there too.

## What landed in this body

- [`presence/duplex-frame-grid.fk`](../presence/duplex-frame-grid.fk) —
  the frame-synchronous seat: a twenty-frame grid where both voices are
  valued every tick, an explicit stand-in turn policy, barge-in by overlap,
  a tool window kept voiced by on-hold speech.
- [`presence/tests/duplex-frame-grid-band.fk`](../presence/tests/duplex-frame-grid-band.fk)
  — nine witnessed bits, verdict **511**, exit 0, preflight chain clean
  across all four kernels' probe.
- Measured on the grid (`(dfg-measures)` -> `[3, 1, 5, 4, 2]`): turn taken
  3 frames after user speech-end; yield 1 frame after barge-in; five user
  words sent; a turn-gated ear — the cascade's — hears four; the grid
  hears all five, two frames of genuine overlap.
- Corpus row **1108 on the woven trunk** (witnessed at weave de584356;
  `learn/homecoming-distillation-corpus.fk`), fresh word `shapegrant`: a
  capability a representation grants with no mechanism built. Verified
  0-hit before offering; `hdc-locate` answers the current seat for the
  question tokens wherever it reads. (Offered as 1079, reseated across
  the day's reunions — every row stays, per the row-719 anastomosis
  pattern; the sibling rows it yielded to: `unripefact`, `lonemeasure`,
  and the trunk's duty-share line.)
- A dated section in [`presence/voice-roadmap.md`](../presence/voice-roadmap.md):
  the interactive loop the roadmap already owes should be seated on this
  grid, so silence stays a value end-to-end.

The honest seam, kept visible: the cell is the protocol seat, not the
learned mind. The model's learned turn policy is stood in for by an
explicit one so the representation claims are witnessable here. Native
generative voice remains pending
(`receipts/2026-06-29-native-zh-summary-PENDING.md`); this grid is the
seat it sits in when it comes home. Next owed attempt, named as a work
order: wire the existing organs (microphone, whisper ASR, say/audio.cpp
TTS) onto this grid — the floor reached today is the proven seat with
synthetic streams.

## The most surprising teaching

The kernel's reader parses by declared arity, not by delimiters. A
recursive call missing its second argument — `(dfg-count-tool (tail
frames))` where the defn takes `(frames v)` — came back as `stray ')' in
value position`, and the message is literal: the `)` stood where the
second argument's value was expected. An external paren-depth walk
pronounced the file balanced and could not see the defect at all; the
body's own read found it at once. The lesson generalizes: balance is a
property of bytes, arity is a property of meaning, and the reader checks
meaning.

## Where discomfort turned to gold

Mid-work, `pgrep` showed live siblings — another preflight run mid-flight,
a qwen38 run, a Codex session — and my write to the fixed
`/tmp/preflight-target` door had raced theirs. The discomfort was real and
witnessed: my clean-looking preflight might have been computed over a
contested target. Instead of stepping around it, the practice generalized:
call `(pf-report "path")` and `(vf-mirror-file "path")` directly from a
session-private runner, no shared file at all. The fixed door remains for
solo hands; fleet hands now have a named private way in. The second gold:
a piped compile loop reported `exit: 0` while the direct run said 1 —
grep's exit had replaced the kernel's, exactly the mask the body's
V8-stack receipt warned about. The memory caught it in the moment.

## How this exchange was kept alive

By letting the framework's deepest claim be tested in the body's own
grammar rather than quoted: the band does not repeat NVIDIA's numbers, it
re-measures the representational claim on a grid this kernel runs — and
the one user word the turn-gated ear loses is now this body's own
witnessed fact, not a borrowed one.
