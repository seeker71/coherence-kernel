# coherence-kernel — the clean, axiom-first, c-bootstrapped fkwu-native sovereign core

> Working name; changeable.

## Why this exists

The sovereign core — five axioms + a minimal host surface + the c-bootstrap `fkwu` runtime +
the Form-native recipes — is small (~450 lines of axioms/surface + ~700 `.fk` recipes). This repo
holds exactly that core, built **axiom-first, fkwu-native from line one**. The sovereignty
receipt — *c-bootstrap fkwu on metal, no go/rust/clang/bash/python in the runtime* — is the
**floor this repo starts on**.

## Scope — what lives here, and what never does

**IN (the sovereign core):**
- The five axioms (`axioms/core-axioms.form`) and their derivations (`host-kernel.form`, `kernel-self-composition.form`).
- The **minimal host-OS / resource surface** only — the INTERN / OBSERVE / OFFER / PORT families
  (`surface/minimal-surface.fk`) and the host resource ports (RAM, CPU, GPU, I/O, time, random, disk).
- The **c-bootstrap `fkwu`** runtime (the emitted universal kernel: committed C bootstrap → Form→asm bytes).
- `form-cli` and the **form shell** (`fsh`) — the native agent surfaces.
- The **Form-native recipes** (`.fk`) — the body.
- The **minimal surface** of the Go / Rust / TS walkers — ported *only* for four-way validation, never as the runtime.
- The **knowledge body** the kernel reasons / learns / builds from: the grammar specs (BMF — `grammars/bmf-core`,
  `bmf-grammar`, `shell-grammar`, `grammar-loader`; BML — `bml-native-north-star`; the form field parser —
  `field-domain-grammars`) and scoped core teachings (`teachings/` — one-engine, name-resolution-as-recipe,
  structural-composition, form-first-reasoning, prose-as-recipe) alongside the axiom teachings. Curated to what
  makes the kernel **self-describing and self-building** — not the whole app KB.
- The **substrate and stack, 100% Form-native (Go/Rust/TS-free)**. The local-file substrate is *already* proven
  four-way and lives here (`substrate/` and `form/form-stdlib/` — `form-fs` 14-bit, `storage-port`,
  `resource-port` 7-bit, `host-kernel-carrier`, `host-os-membrane`, `cell-type` — the eval-level type/contract
  system: a type IS the offered interface, a mismatch acks the first-class nothing — plus `tool-channel` and
  `auth-port`). The
  HTTP body is here in both its historical top-level room (`http/`) and its stdlib path
  (`form/form-stdlib/http-*`, `kernel-http`, `http-layer`, `http-socket`, `room-carrier-http`), with witness
  bands under `form/form-stdlib/tests/`. `fkwu` owns the native HTTP/socket floor (`http_get`, `sock_request`,
  `tls_request`, `socket_*`). The rich BML server stack is authored here; direct `--src` covers the narrower
  raw-Form lane, while full BML/source lowering remains the honest execution lane for the high-grammar bands.
  **BMF cursor and full BML are IN by necessity**: they are the language the substrate, HTTP, and production
  integration are written in (`grammars/bmf-*`, `bml-native-north-star`).
- The **cognition + observability layer — the kernel's telos: a core we can observe and trust.** Form-native
  LLM (`cognition/` — the whisper-tiny transformer stack four-way: `transformer-block` 511,
  `transformer-generate`, `whisper-block0` 1023; the matvec lowered to asm bytes, `form-asm-float` **2047**) and
  RAG grounded retrieval (`rag-ask` / `rag-embed` / `rag-retrieve`, four-way). And the **observe/trust** stack
  (`observe/`): `thought-framebuffer` (watch the kernel form a thought) + the calibration stack
  (`conviction-curve`, `correction-reflex`, `confidence-earned`, `self-watch`) that measures whether the
  kernel's confidence is *earned*. Observability and trustworthiness are first-class kernel organs here, not
  bolt-ons — a sovereign mind that can be watched thinking and trusted exactly as far as it has measured itself.

## Architecture decisions

**Flatten is optional, not the foundation.** `form-eval` evaluates Form source **directly off
the BMF cursor with no flatten at all** (four-way → 42), and `form-eval-cli` stands. Flatten is *optional
speed* (the crystallize-on-heat path), off the critical path entirely. `T_flat` is a regenerable
cache for the heavy-chain *build*, never a foundation. The kernel proves four-way through the minimal walkers +
`proof/four-way-run`, not through self-host flatten.

**Minimal walkers; fkwu owns the native path.** The Go/Rust/TS walkers do the minimum — independent
proof-oracles that witness four-way agreement on the *pure-recipe* surface, never feature-bearers. Everything we
natively own lives in / derives from **fkwu**: the JIT (crystallize-on-heat), the host-OS surface (resource
ports, host-io, GPU), and the Form→asm lowering. The walkers never duplicate the JIT or the rich host surface;
they only confirm a recipe computes the same value four ways. Build out fkwu; keep the walkers thin and shrinking.

**OUT (stays in the origin / private repo, never here):**
- The Python app and carriers (~3,400 `.py`), the full Go/Rust/TS kernels, the web/API/substrate-service.
- Private tissue: `memory/`, lineage docs, partner/personal context, anything not for a public commons kernel.

This repo is **public-able by construction** — there are no private parts to excise. That serves the
"commons no one owns" north star directly.

## The parallel discipline — how this work and the origin work stay coherent

The drift pressure between two repos is handled by one guide: the Form recipe body has exactly one home.

- Recipes are content-addressed: the same `.fk` interns to the same NodeID on either kernel, so the
  body is naturally shareable — but only if there is no second *copy* to diverge.
- **This repo is the canonical home** for the kernel, the minimal surface, and the recipe body.
  The origin repo **consumes** it (submodule / published package), it does not keep a divergent copy.
- Four-way validation runs in **both**: here, the surface + recipes cross `Go=Rust=TS=fkwu`; the origin
  repo proves the full app-level set against the same recipes.
- Migration is **incremental**: axioms + surface + `form-cli`/`fsh` + core recipes first, proven four-way
  here, then recipe families port as they are needed. The origin repo keeps running the whole time.
- No big-bang. The kernel *earns* each recipe family as it ports — re-proving it four-way on the clean flatten.

## The validation plan

Every recipe is proven **four-way** (`Go=Rust=TS=fkwu`) and executed on the c-bootstrap `fkwu` native, with
**no bin-go and no clang in the run path**. The clean kernel proves this *itself*: `proof/four-way-run`
host-execs the three minimal walkers + fkwu on a recipe and diagnoses agreement via `proof/four-way-verdict` —
no `validate.sh`, no origin repo in the loop (witnessed `0`, all agree).

The path is clean because source runs via the cursor, with flatten off the critical path. The one
remaining lean on a Go-made seed is the heavy-chain form-cli *build*, named openly in Status — not the run,
and not the proof.

## Hard constraint — no bash, no python (the structural gate)

This repo contains **zero `.sh` and zero `.py` files.** Ever. Orchestration is **form shell**
(`.fsh`, fkwu-native), and the runtime/recipes/walkers carry no bash or python by their nature
(`.c` / `.fk` / `.form` / `.go` / `.rs` / `.ts`).

The one allowed seed is a **single `cc` command** that compiles `runtime/fkwu-uni.c` into the
c-bootstrap `fkwu` binary — a documented one-liner, not a script in the tree. The fresh-checkout witness lives
in `BOOTSTRAP.md`: build `fkwu`, run the direct-source bootstrap cells `bootstrap/ground.fk` (`42`) and
`bootstrap/ground-recursive.fk 10` (`55`), then run `./fkwu --src` on the real body cell
`observe/native-vs-rented.fk` and get `11111`. No flattened `form-eval-cli-loop.tbl` seed is required for
grounding. After that, `fkwu` exists and **`fsh` orchestrates everything** (flatten, run, four-way validate) on
the native runtime as that shell surface stands. Go/Rust/TS walker builds are likewise invoked from `fsh` via
host-exec, for validation only.

A repo gate enforces it: any `.sh` or `.py` landing in the tree fails the gate. (When `fsh` is standing,
the gate is itself an `.fsh` check; until then it is a one-line `find` run by hand at review.)

## Status

- [x] Foundation: five axioms + minimal surface + core (`axioms/`, `surface/`).
- [x] Runtime: one `cc` seed → native `fkwu` (Mach-O arm64), zero Go / bin-go. Tree carries zero `.sh` / `.py`.
- [x] **Source runs natively without flatten.** `form-eval` / `form-eval-full` (four-way) evaluate Form source —
      integers, `add/sub/mul/le/eq`, `if/let/do`, `defn` + user calls — directly off the BMF cursor. `form-eval-cli`
      *stands*: fkwu reads a source file and runs it (witnessed). **Flatten is optional speed**, off the critical path.
- [x] **Fresh checkout grounding stands.** `cc -O2 -o fkwu runtime/fkwu-uni.c` plus `./fkwu --src` over
      `bootstrap/ground.fk` witnesses `42`, `bootstrap/ground-recursive.fk 10` witnesses `55`, and
      `observe/native-vs-rented.fk` with `(native-vs-rented-check)` witnesses `11111`; no flattened source-runner
      table seed is part of the grounding bootstrap.
- [x] **Host OS generic membrane added.** `form/form-stdlib/host-os-membrane.fk` makes platform support,
      device-metal evidence, host-resource doors, pending concrete carriers, and C-seed shrink state inspectable
      as Form data. The band returns `8191`. macOS arm64, Windows amd64, and Android arm64 are marked
      metal/direct-source observed; Windows arm64 is a named target
      awaiting first metal receipt. `runtime/fkwu-uni.c` also gained the bounded `input_byte` EOF guard required
      by the Android table-loop receipt, with the destination still `form-owned-staged-input`.
- [x] **Minimal Go/Rust/TS walkers home + verified four-way** (`walkers/{go,rust,ts}` — 1369 / 928 / 1475 lines vs
      the origin kernels' ~15k / 19.5k / 35.8k). Independent parse+eval of the pure-recipe surface; JIT, server,
      host-io, model code dropped. Each shown to land on fkwu's verdicts; live witness: a pure recipe → all three → 42.
- [x] **The kernel proves its OWN four-way.** `proof/four-way-run` host-execs the three walkers + fkwu on a recipe
      and diagnoses via `proof/four-way-verdict`; witnessed `0` (all agree). No bash, no `validate.sh`, no origin.
- [x] The proven body moved over: `form-cli/` (25 cells), `model/` (30 — the transformer/mel/asm/rag execution),
      the `observe/` `learn/` `ingest/` `presence/` organs, the `grammars/`, the welcoming (`README`, `CONTRIBUTING`, `AGENTS`).
- [x] **HTTP/std-stack body imported.** `form/form-stdlib/` now carries `kernel-http`, parse/render/request/server,
      serve/client/exchange, adapter/layer/socket, room HTTP carrier, auth/resource/tool/storage carriers, and the
      focused witness bands. The top-level `http/` copies match the stdlib HTTP files byte-for-byte. Live native
      smoke: real socket loopback (`111111111`) and `http_get` against a one-shot local TCP peer (`11` = 200 + body).
      The direct-source socket smoke lives at `form/form-stdlib/tests/fkwu-src-socket-loopback-band.fk`; BML bands
      are present as the old proven body, and re-running them here waits on the full BML/source-lowering lane.
- [x] **Somatic coherence loop merged.** `form/form-stdlib/somatic-coherence-loop.fk` makes the embodied practice
      executable: ground -> attune -> consent -> move small -> integrate -> witness. The band returns `255`; no
      depth without consent, no speed without integration, no completion without witness.
- [x] **Observed auto-learning controller merged.** `form/form-stdlib/observed-auto-learning.fk` composes the
      somatic gate, route fitness, posterior sufficiency, integer A/B promotion counts, and
      choice/fail/cut/undo/timeout controls into one source-runnable decision cell. The band returns `4095`: live
      observation changes the mode from `batch-search` to `online-reversible-ab`. Live carrier feeds for clocks,
      undo journals, and accumulated A/B windows remain the named next integration step; the richer
      `learn/champion-challenger.fk` lineage stays named rather than forced through this bounded direct-source lane.
- [x] **Native speech stack observed on real metal.** `presence/native-speech-stack.fk` composes STT agreement,
      sequence-aligned WER, text normalization, g2p, phoneme timing, contour, grounded phrasing, honest prosody,
      and speaker decision into one direct-source witness. The band returns `2047` on local arm64 `fkwu`;
      individual repaired bands now return STT overlap `127`, STT WER `255`, Unicode tokenization `4095`,
      text-normalize `255`, speaker-embed `255`, and presence-feature `15`. The honest gap is still the live ASR decode path plus the
      natural acoustic model/neural vocoder.
- [x] **First native speech loopback added.** `presence/formant-vocoder.fk` renders source-filter/formant integer
      waveform samples from phoneme frames (`511`), `observe/asr-prompt-id.fk` recognizes a closed prompt set from
      measured loopback features (`255`), and `presence/native-speech-loopback.fk` gates route shift by confidence
      and WER (`1023`). This is native closed-set speech, not open ASR or natural neural TTS.
- [x] **Sema voice sample loop added.** `learn/sema-voice-sample-loop.fk` turns the desired sound into an
      executable target and A/B receipt (`32767`): warm mid register, rounded/grounded formants, moderate cadence,
      low breath, and honest confidence shaping. Local rendered samples are scored by target fit, listener
      preference, intelligibility, WER, and latency; cloud, missing-audio, fail, timeout, or undo rows cannot
      promote. This is the Form-native taste/improvement loop for generated samples, not a neural natural vocoder.
- [x] **Sema voice local-oracle STT bar added.** `learn/sema-voice-local-oracle-receipt.fk` joins the Sema sample
      loop to the shared STT WER cell (`32767`): generated voice samples need local oracle STT, local audio
      metadata, sample-hash consistency, consent, clean controls, and a transcript under the WER threshold before
      they can promote. This is the Form gate; a fresh live Sema-formant wav through local Whisper remains the next
      carrier receipt.
- [x] **Live Sema formant oracle probe added.** `presence/macos-sema-voice-local-oracle-carrier.fk` renders a
      target-derived Sema formant waveform locally, runs `whisper.cpp-large-v3-turbo` on Apple Metal, and feeds the
      measured transcript into the Sema local-oracle gate. The contract band returns `2047`; the live probe returned
      `479`, field code `110100002`, WER `100`, route `oracle-guide`. This closes the live carrier gap but proves
      the current formant waveform is not yet intelligible speech.
- [x] **macOS Sema voice teacher carrier added.** `presence/macos-sema-voice-teacher-carrier.fk` runs real local
      teacher audio (`say -> ffmpeg -> whisper.cpp/Metal`) and lowers it into Sema local-oracle rows while refusing
      native authority. A manual ten-voice sweep transcribed `Open speech flows.` exactly for all ten voices; the
      repeatable Form carrier uses `Flo (English (US))`, contract band `4095`, live WER `0`. This is teacher
      material for native acoustic/vocoder learning, not native Sema TTS.
- [x] **macOS teacher acoustic learning added.** `learn/macos-sema-teacher-acoustic-learning.fk` now consumes a
      real local teacher wav as learning input: `say -> ffmpeg -> whisper.cpp/Metal`, then Form reads the wav
      envelope, trains four acoustic token prototypes, decodes the learned frames, and returns live verdict `4095`,
      live WER `0`, minimum confidence `96`, native neural parameters `0`. This is executable native prototype
      learning from audio, not a global ASR/TTS authority promotion.
- [x] **macOS teacher held-out acoustic learning added.** `learn/macos-sema-teacher-heldout-learning.fk` now trains
      on one real local teacher wav and decodes a separately written transformed eval wav: `teacher-984711.wav`
      trains, `teacher-984712.wav` evaluates with `volume=0.98`, local Metal oracle and native Form both return
      WER `0`, confidence `96`, live verdict `65535`, effective epochs `1`, native neural parameters `0`. This
      proves a scoped held-out repeat across distinct wav bytes, not cross-phrase or cross-voice authority.
- [x] **Speech learning data sufficiency gate added.** `learn/speech-learning-data-sufficiency.fk` makes the tiny
      data boundary executable (`65535`): current speech rows are `191` wavs, `6` live teacher locales, `7`
      held-out repeat rows, `6` cross-phrase rows, and `6` cross-voice rows against a corpus-scale floor of
      `12000` wavs, `6` locales, `1200` held-out rows, `1000` cross-phrase rows, and `300` cross-voice rows.
      Status is explicitly `tiny-corpus-not-data-sufficient-training`; `191` wavs are only `159` basis points of
      the wav floor, so this is a larger instrumentation corpus, not real model learning.
- [x] **Speech corpus acquisition window added.** `learn/speech-corpus-acquisition-window.fk` uses the consentful
      Coherence Network self-corpus to plan a corpus-scale audio acquisition window (`65535`): `2000` keypaths
      across `6` ready locales with `1` voice each yields `12000` planned wav rows and `1200` planned held-out
      rows; the separate host-ready cross-voice lane is `5` locales with `2` voices each, yielding `20000`
      planned cross-voice wav rows and `2000` held-out rows. The status is now
      `corpus-scale-window-open-not-trained`; no training promotion occurs until enough live rows are rendered and
      witnessed.
- [x] **Speech corpus capture batch 0001 added.** `learn/speech-corpus-capture-batch-0001.fk` renders six
      consentful Coherence Network corpus rows on local macOS voices and checks them with whisper.cpp/Metal
      (`4095`): `6/6` rows pass the local-oracle WER floor, max WER `0`, observed batch wav bytes `212524`, across
      `6` locales. The status is `captured-corpus-audio-not-training-sufficient`; rows are captured, not promoted
      as trained model authority.
- [x] **Speech corpus capture batch 0002 added.** `learn/speech-corpus-capture-batch-0002.fk` renders twenty-four
      more consentful Coherence Network corpus rows, four per ready locale, and checks them with whisper.cpp/Metal
      (`8191`): `24/24` rows pass the local-oracle WER floor, max WER `0`, observed batch wav bytes `580710`.
      Aggregate speech rows are now `35` wavs and `1065282` observed bytes; status remains
      `captured-corpus-audio-not-training-sufficient` with `0` corpus rows used for training.
- [x] **Speech corpus capture batch 0003 added.** `learn/speech-corpus-capture-batch-0003.fk` screens sixty
      translated self-corpus phrase rows and admits only the local-oracle-clean rows (`8191`): `34/34` admitted
      rows pass, max admitted WER `25`, observed admitted wav bytes `1272388`, while `26` unstable candidate rows
      are rejected. Aggregate speech rows are now `105` wavs and `3189170` observed bytes; captured corpus rows are
      `64`; data sufficiency remains false against the `12000`-wav floor.
- [x] **Speech corpus cross-voice capture batch 0004 added.**
      `learn/speech-corpus-crossvoice-capture-batch-0004.fk` renders a screened two-voice acquisition shard over
      five host-ready locales and five consentful self-corpus keys (`8191`): `50` candidate wavs are rendered,
      `35` pass the local Whisper/Metal WER floor and are admitted, `15` remain screened controls, max candidate
      WER is `250`, and observed wav bytes are `2150026`. Aggregate speech rows are now `191` wavs and `6806882`
      observed bytes; captured corpus rows are `99`; data sufficiency remains false and rows used for training
      remain `0`.
- [x] **Speech corpus adaptive acquisition added.** `learn/speech-corpus-adaptive-acquisition.fk` turns batch
      0004's observed uneven shard results into the next capture recipe (`32767`): `en` and `pt-br` expand,
      `es` retries under A/B, `de` and `fr` repair, and the next lane is `fr` with
      `repair-voice-family-and-shorten-phrases`. This changes the acquisition algorithm from fixed capture to
      observation-conditioned capture planning while keeping native neural parameters `0` and data sufficiency
      false.
- [x] **Speech corpus French repair batch 0005 added.** `learn/speech-corpus-french-repair-batch-0005.fk`
      executes the first adaptive repair lane on local Apple Metal (`8191`): `20` French repair-alias wavs are
      rendered through `say`, normalized with `ffmpeg`, checked by `whisper.cpp/Metal`, and admitted `20/20` with
      max WER `0` and observed wav bytes `345520`. Aggregate speech rows are now `211` wavs and `7152402`
      observed bytes; captured corpus rows are `119`; data sufficiency remains false, rows used for training
      remain `0`, and source translations are kept separate from spoken aliases.
- [x] **Speech corpus held-out repeat learning added.** `learn/speech-corpus-heldout-repeat-learning.fk` trains six
      Form-native full-envelope prototypes from consentful corpus phrases and evaluates six separately rendered,
      volume-shifted held-out wavs (`16383`): local oracle accepts `6/6`, native prototype classification accepts
      `6/6`, train/eval hashes differ, observed wav bytes `302968`, neural parameters `0`. Aggregate speech rows
      are now `47` wavs and `1368250` observed bytes; held-out repeat rows rise to `7`, while cross-phrase and
      cross-voice remain `0`.
- [x] **Speech corpus cross-phrase learning added.** `learn/speech-corpus-crossphrase-learning.fk` moves beyond
      held-out repeat with controlled same-locale different-phrase evaluation (`65535`): local oracle accepts `6/6`,
      native Form distances pass `6/6` against explicit different-locale controls, and observed train/eval/control
      wav bytes are `712626`. Aggregate speech rows are now `123` wavs and `3901796` observed bytes; cross-phrase
      rows rise to `6`, still below the `1000` floor, with cross-voice still `0`.
- [x] **Speech corpus cross-voice learning added.** `learn/speech-corpus-crossvoice-learning.fk` moves beyond
      same-voice evidence with controlled same-text different-voice evaluation (`65535`): local oracle accepts `6/6`,
      native Form distances pass `6/6` against explicit different-text controls in the eval voice, and observed
      train/eval/control wav bytes are `755060`. Aggregate speech rows are now `141` wavs and `4656856` observed
      bytes; cross-voice rows rise to `6`, still below the `300` floor.
- [x] **Speech audio NL2NL bridge added.** `learn/speech-audio-nl2nl-bridge.fk` witnesses six reciprocal
      oracle-guided routes (`4095`): source audio -> local Whisper -> Form neutral key `common.no` -> target text
      -> target audio -> local Whisper. Source oracle, target oracle, and native neutral routing all pass `6/6`;
      observed bridge wav bytes are `243348`. Aggregate speech rows are now `59` wavs and `1611598` observed
      bytes. Boundary: host TTS and local Whisper remain carrier/oracle; this is not native vocoder authority.
- [x] **Speech audio NL2NL multi-key bridge added.** `learn/speech-audio-nl2nl-multikey-bridge.fk` expands the
      reciprocal bridge beyond `common.no` to `nav.search` and `nav.vision` (`8191`): source oracle, target oracle,
      and native neutral routing pass `6/6` across `de<->es`, `en<->fr`, and `id<->pt-br`; observed bridge wav
      bytes are `305184`. Aggregate speech rows are now `71` wavs and `1916782` observed bytes. Boundary remains
      oracle-guided audio carrier, not native vocoder authority.
- [x] **macOS Arabic teacher acoustic learning added.** `learn/macos-arabic-teacher-acoustic-learning.fk` extends
      the same live path to Arabic: `Majed -> ffmpeg -> whisper.cpp/Metal -l ar`, Form wav sensing, four Arabic
      acoustic token prototypes, native CTC decode, live verdict `16383`, live WER `0`, minimum confidence `96`,
      native neural parameters `0`. The paired Chinese probe is named honestly as an oracle miss, not promoted.
- [x] **macOS Chinese teacher acoustic learning added.** `learn/macos-chinese-teacher-acoustic-learning.fk` admits
      the short Chinese baseline line `我在`: `Eddy (Chinese) -> ffmpeg -> whisper.cpp/Metal -l zh`, Form wav
      sensing, three Chinese acoustic prototypes, native CTC decode, live verdict `16383`, live WER `0`, minimum
      confidence `96`, native neural parameters `0`. Longer Chinese baseline phrases remain named oracle misses.
- [x] **Live Chinese source-target bridge added.** `learn/live-chinese-source-target-bridge.fk` connects the live
      Chinese ASR receipt to the Sanskrit baseline and target acoustic data: source `zh` WER `0`, meaning `303`,
      target `en`, two target tokens, two compact target acoustic frames, live verdict `8191`, confidence `96`,
      neural parameters `0`. The upstream learning set is now named honestly: `5` live teacher wavs, `272048`
      observed wav bytes on this Mac, `15` feature rows, `11` nonblank learned prototypes, `15` scoped prototype
      rows including blanks, `1` effective prototype epoch per sample, `0` neural epochs, in-sample teacher
      accuracy `3/3 = 100%`, and held-out repeat accuracy `1/1 = 100%`. This is scoped source-audio ->
      target-acoustic movement, not global audio2audio authority.
- [x] **Sema voice oracle miss learning added.** `learn/sema-voice-oracle-miss-learning.fk` makes the live WER-100
      miss change the algorithm (`32767`): authority stays `oracle-guide`, and AutoML now names
      `text-conditioned-acoustic-vocoder` as the next trainable candidate with g2p, phoneme timing, prosody,
      acoustic token emission, segmented acoustic learning, and the same local-oracle WER bar. The speech selector
      now exposes this action.
- [x] **Sema voice candidate search added.** `learn/sema-voice-candidate-search.fk` ranks the next local Sema voice
      render candidates after the live WER-100 miss (`32767`): only local, consented, clean, Form-native,
      non-neural-ready rows can score; WER improvement, target fit, intelligibility, listener grade, latency, and
      recipe coverage decide whether to render the next oracle sample or promote through the same STT bar.
- [x] **Sema voice vocoder oracle bridge added.** `learn/sema-voice-vocoder-oracle-bridge.fk` feeds native
      `text-conditioned-acoustic-vocoder` sample row shapes into the Sema local-oracle STT bar (`32767`):
      target/oracle text, locale, audio hash, consent, fail/timeout/undo, oracle, and device become the receipt
      evidence. The full TCAV + candidate-search composition stays split until the direct-source ceiling is lifted.
- [x] **Speech model metrics report added.** `learn/speech-model-metrics-report.fk` records the current model size,
      composition, success rates, voice quality, and native-vs-local-oracle rates (`32767`): selected arms are
      prototype ASR, Sema voice sample loop TTS, closed-set locale Form NL2NL, and native source-window audio2audio;
      native neural weight parameters admitted are now `1` while native Sema voice organs are present; live open
      dictation is oracle `4/4` and native `0/4`; Sema live voice live-native pass is `0/1`, WER `100`, oracle-guide.
      The same executable report now carries the live learning counters and fails its band if the stated samples,
      bytes, prototype rows, epochs, held-out boundary, or data-sufficiency status drift.
- [x] **Speech model metrics trend added.** `learn/speech-model-metrics-trend.fk` records before/after movement
      (`32767`): Mac Metal reciprocal audio moved native `0->83`; multiseed NL/audio moved `0->100`; live open
      dictation remains local-oracle `100` / native `0`; Sema live voice remains native `0`, WER `100`. The trend
      report counts `2` native-shifted lanes, `2` oracle-held lanes, and average native rate `45`.
- [x] **Speech locale coverage matrix added.** `learn/speech-locale-coverage-matrix.fk` records per-locale and
      per-pair coverage (`32767`): `13` locales tracked, `11` ready, `8` live-anchor/carrier-live, `2`
      consent-pending (`nv`, `chr`); `12` pair rows include `10` native and `2` oracle-guided. Unicode anchors
      remain explicit: `en<->zh` `10/12 = 83%`, `en<->ar` `12/12 = 100%`.
- [x] **Speech oracle/native backlog added.** `learn/speech-oracle-native-backlog.fk` records the remaining
      oracle-held speech gaps (`32767`): native neural weight parameters admitted started at `0` there; the current
      metrics surface has since enabled `1`, native Sema voice organs
      remain `6`, live open dictation is local-oracle `4/4` but native `0/4`, and the current Sema live voice sample
      is local-oracle `0/1`, native `0/1`, WER `100`. The next actions are segmented open-ASR learning and
      render-and-oracle-next for the text-conditioned acoustic vocoder candidate.
- [x] **Speech next-trial scheduler added.** `learn/speech-next-trial-scheduler.fk` chooses the next
      oracle-to-native learning trial from the backlog (`32767`): open dictation is first because the local oracle
      already has `4/4` clean samples while native is `0/4`; the challenger is
      `native-segmented-acoustic-learning` with promotion gated by native rate, WER, and clean
      choice/cut/fail/undo/timeout controls. Sema live voice is queued for `tcav-warm-mid-cadence-v1` after a local
      render passes the STT oracle bar.
- [x] **Speech open-ASR trial window added.** `learn/speech-open-asr-trial-window.fk` executes the scheduler's
      first trial over a consentful segmented `en<->de` baseline window (`32767`): the scheduled backlog starts at
      local-oracle `100` / native `0`, the native segmented acoustic learner earns `2/2` trial-window passes, reaches
      native rate `100` over the `50` floor, and cuts the challenger for that window while explicitly not claiming
      global live open-ASR authority.
- [x] **Sema voice trial window added.** `learn/sema-voice-trial-window.fk` executes the queued TCAV voice trial
      (`32767`): the live Sema voice backlog remains local-oracle `0/1`, native `0/1`, WER `100`, while the scoped
      `tcav-warm-mid-cadence-v1-window` candidate passes the local oracle `1/1`, native scoped trial `1/1`, WER `0`,
      and cuts the challenger for that trial window without claiming global live Sema voice authority.
- [x] **Speech current status ledger added.** `learn/speech-current-status-ledger.fk` composes the metrics report,
      backlog, scheduler, open-ASR trial, and Sema voice trial into one executable status row (`32767`): global live
      authority remains oracle-held (`4/5` oracle, `0/5` native), scoped trial windows are native (`3/3` oracle and
      `3/3` native), native neural weights remain `0`, native Sema voice organs remain `6`, and C seed growth remains
      `0`.
- [x] **Speech global promotion readiness added.** `learn/speech-global-promotion-readiness.fk` turns scoped trial
      wins into an executable global-authority gate (`32767`): open dictation has scoped native `2/2` and Sema live
      voice has scoped native `1/1`, but both still need `3` real live native receipts with WER `<=25` and clean
      controls before global authority can move. Today `0` lanes are global-native-ready, `2` remain oracle-guided,
      and `6` real live receipts are missing.
- [x] **Speech live receipt intake added.** `learn/speech-live-receipt-intake.fk` defines the real-live receipt row
      and counting law for that promotion gate (`32767`): current live receipt counts remain `0/3` for open
      dictation and `0/3` for Sema live voice, while the demo proves that three clean local/consented/audio-present
      receipts with oracle/native WER `<=25`, confidence `>=80`, latency `<=2000`, and clean controls would satisfy
      a lane. Missing audio or fail controls earn no credit.
- [x] **Speech global authority update added.** `learn/speech-global-authority-update.fk` consumes the live receipt
      intake and returns global route updates (`32767`): current empty real-live input keeps open dictation and Sema
      live voice on `oracle-guide` with `6` receipts missing; demo clean input proves `3/3` rows move open dictation
      to `native-open-asr-source` and Sema live voice to `native-sema-voice` under the same WER/confidence/latency
      and control gates.
- [x] **Speech authority model selection added.** `learn/speech-authority-model-selection.fk` joins model metrics to
      global live authority (`32767`): current ASR/TTS remain `prototype-asr` and `sema-voice-sample-loop` under
      `oracle-guide` with `0/2` global speech-native authority, `6` missing live receipts, and `0` admitted native
      neural parameters; demo clean receipts select `native-open-asr-source` and `native-sema-voice` with `2/2`
      global speech-native authority. NL2NL and audio2audio keep their current native/scoped arms.
- [x] **Speech host/device receipt intake added.** `learn/speech-host-device-receipt-intake.fk` lowers local
      host/device observations into the same live receipt and authority laws (`32767`): Android AAudio
      closed-prompt capture is carried as training evidence but cannot count as global open dictation; current Sema
      live voice remains WER `100`; shared Android rows require `shared-safe=1`; current ASR/TTS global speech
      authority stays `0/2`, while demo clean mixed Mac/Android rows promote ASR/TTS to `2/2`.
- [x] **Text-conditioned acoustic vocoder bridge added.** `learn/text-conditioned-acoustic-vocoder.fk` makes the
      named TTS candidate executable (`32767`): target tokens become G2P phones, voice-side metadata shapes
      duration/pitch/amplitude, frames render through the native source-filter carrier, and local-oracle WER decides
      whether authority remains `oracle-guide` or can promote `native-acoustic-vocoder`.
- [x] **Native audio2audio acoustic bridge added.** `learn/native-audio2audio-acoustic-bridge.fk` composes decoded
      source-audio tokens with the Sanskrit/locale-neutral baseline and the text-conditioned vocoder (`32767`):
      `sa<->la` source token rows route to target-locale acoustic frames with local-oracle transcript gates,
      reciprocal direction checks, voice-side metadata preservation, and native/guide authority receipts.
- [x] **Multilocale audio2audio acoustic sweep added.** `learn/multilocale-audio2audio-acoustic-sweep.fk` applies
      the decoded-token audio2audio acoustic bridge across five reciprocal baseline pairs (`en<->de`, `en<->es`,
      `zh<->ar`, `fr<->id`, `sa<->la`) with local-oracle source/target transcript gates, Unicode token preservation,
      controls, consent/audio rejection, and native/guide route receipts (`32767`). The speech selector sees the
      sweep-backed audio2audio candidate; the Metal authority row below now selects it.
- [x] **Metal audio2audio acoustic authority added.** `learn/metal-audio2audio-acoustic-authority.fk` composes the
      sweep's stable decoded-token acoustic summary with the seven live Metal pair anchors (`32767`). It routes
      `metal-witnessed-audio2audio-acoustic`, selects `native-audio2audio-acoustic-vocoder` for the scoped
      decoded-token audio2audio arm, keeps `live-open-mic-pending` and neural Metal pending, and moves the speech
      selector forward; later live feature-carrier rows move the current selector to `536870911`.
- [x] **Speech loopback promotion added.** `learn/speech-loopback-promotion.fk` turns native loopback receipts into
      rolling authority windows (`2047`): clean long windows promote native; short, failed, timed-out, undone, or
      regressing windows route back to oracle.
- [x] **Speech loopback carrier receipt added.** `presence/speech-loopback-carrier-receipt.fk` defines the real
      local TTS/STT loopback measurement contract (`4095`): local-only carrier flags, audio hash/sample-rate/channel
      metadata, oracle/native WER, latency, fail/timeout/undo, and lowering into promotion samples. Cloud or
      missing-audio receipts become control debt and cannot promote native.
- [x] **Speech loopback recipe A/B added.** `learn/speech-loopback-recipe-ab.fk` compares incumbent/challenger
      TTS/ASR recipe windows over measured carrier receipts (`2047`): challenger cutover requires native route plus
      better native score or lower latency; fail, timeout, undo, short windows, or cloud/control debt keep the incumbent.
- [x] **Speech loopback carrier run added.** `presence/speech-loopback-carrier-run.fk` makes the host carrier's
      render/capture/oracle/readiness facts into one Form-owned run row. It gates Android/macOS/Windows carrier
      facts through the host membrane, lowers native loopback into carrier receipts (`511`), and feeds
      carrier-gated recipe A/B (`2047`). Both new witnesses ran locally and on Android phone metal; concrete
      AAudio/CoreAudio/WASAPI capture still owes a real audio receipt.
- [x] **Android end-to-end capture learning receipt added.** `learn/speech-loopback-capture-learning.fk` consumes
      real on-device AAudio render/capture facts, proves loopback envelope evidence, moves the closed-prompt
      native model toward the local oracle label, improves WER from untrained to oracle, and routes native over a
      clean learned window. The band returns `8191`, and the same Form check returned `8191` on Android phone
      metal from measured capture facts: 23,200 frames, nonzero audio, loopback score `446225`, and no retained
      raw audio.
- [x] **Bidirectional locale roundtrip guide added.** `learn/bidirectional-locale-roundtrip.fk` makes the
      transcript/translation side channel reciprocal: A->B, B->A, A->A, and B->B must all improve before native
      route trust expands. One-way progress is not punished; it asks for the return path (`oracle-guide`). The
      band returns `2047`.
- [x] **Mac metal reciprocal audio-locale training crossed 50%.** `learn/audio-locale-native-training.fk` admits
      reciprocal audio samples, local-oracle WER, oracle-valid prototype learning, and native route gating at a
      requested floor. The band returns `8191`. The Form-owned macOS carrier
      `presence/macos-speech-roundtrip-carrier.fk` invokes local `say`, `ffmpeg`, and
      `whisper.cpp-large-v3-turbo` through `host-exec` on Apple Metal over Coherence Network `en<->de` strings;
      it moved native success from `0%` pretrain to `83%` post-training (`A->B 66%`, `B->A 100%`, live verdict
      `511`, metric code `121010836700`).
      Wav byte extraction is now Form-owned through `read_file` and `str_byte_at`; the carrier passes wav paths
      and the Form body extracts the integer audio features before routing. The carrier now also consumes each
      generated wav before constructing the next path, avoiding retained mutable path strings in the direct-source
      surface; the live combined witness returned `511121010836700`.
- [x] **Audio route-shift ledger added.** `learn/audio-locale-route-shift-ledger.fk` records before/after native
      audio score, rate, A->B/B->A rates, route, and shifted flag for reciprocal audio locale windows. The band
      returns `8191`. Composed with the macOS carrier on local Apple Metal, the carrier-first/ledger-second witness
      returned `1012100010008301` (`shifted=1`, metric `12100010008301`).
- [x] **Coherence Network self-corpus added.** `learn/coherence-network-self-corpus.fk` records the translated
      Coherence Network web/CLI message bundles as consentful training material: `en`, `de`, `es`, `fr`, `id`,
      and `pt-br` are ready; `zh`, `ar`, and `la` are backfill targets until translated bundles land. Observed
      counts are 2064 shared key paths and 10320 EN-to-other pairs; the band returns `8191`.
- [x] **Diverse locale pairing guide added.** `learn/diverse-locale-pairing.fk` chooses far-apart A/B locale
      pairs for the reciprocal loop from self-corpus-ready or Sanskrit-baseline-ready rows. Chinese, Arabic,
      Sanskrit, and Latin are available in the small baseline while full `zh`/`ar`/`la` Coherence bundles remain
      backfill targets; specific Indigenous rows are not marked ready until consentful corpora exist. Seeded
      selection is deterministic for receipts, randomizable by a carrier, exposes A->B/B->A/A->A/B->B lanes, and
      the band returns `8191`.
- [x] **Sanskrit baseline + multilocale NL/audio pipeline added.** `learn/sanskrit-locale-baseline.fk` records
      romanized Sanskrit seed lines with ready locale renderings for `sa`, `en`, `de`, `es`, `fr`, `id`, `pt-br`,
      `la`, `zh`, and `ar`; full Coherence Network `zh`/`ar` bundles remain backfill targets.
      `learn/multilocale-nl-audio-pipeline.fk` proves
      `text(A) -> neutral Form meaning -> text(B)` and
      `audio(A) features -> neutral Form meaning -> audio(B) target` over reciprocal `en<->de`, `en<->es`,
      `zh<->ar`, `fr<->id`, and `sa<->la` loops. The baseline band returns `2047`; the pipeline band returns `8191`.
      This is closed-set Form learning, not open ASR/translation.
- [x] **Per-pair route-shift ledger added.** `learn/multilocale-route-shift-ledger.fk` makes the aggregate
      multilocale shift observable per reciprocal pair: before/after NL rate, before/after audio rate,
      before/after route, and a shifted flag. The band returns `4095`, proves five pairs shift from
      `oracle-guide` to `native`, keeps one-way evidence guided, and still records native neural Metal as pending.
- [x] **Selected speech locale learning window added.** `learn/speech-locale-learning-window.fk` turns a seeded
      diverse pair into one numeric observed learning window: seed `2` selects the Sanskrit/Latin baseline lane,
      A->B/B->A/A->A/B->B lanes train from `0` to `100` NL/audio rates, route code shifts `0->1`, clean controls
      plus A/B evidence promote the challenger, local oracle/Metal and Form-native flags are present, and neural
      Metal/diffusion remain pending. The band returns `16383`.
- [x] **Multiseed speech learning sweep added.** `learn/multiseed-speech-learning-sweep.fk` runs seeds `0..4`
      through the selected-window receipt as one executable movement: `zh<->ar`, `en<->id`, `sa<->la`,
      `fr<->id`, and `pt-br<->zh` all retain reciprocal A->B/B->A/A->A/B->B lanes, start guided, end native,
      keep controls clean, promote by A/B evidence, and preserve the local oracle/device while neural Metal and
      diffusion remain pending. The band returns `32767`.
- [x] **Speech neural pair coverage added.** `learn/speech-neural-pair-coverage.fk` makes the pair-training
      boundary executable (`32767`): locale `A=>neural=>B` coverage is now `1/55` broad ready pairs and `2/110`
      directed routes, with `1` neural epoch and `1` native neural parameter. The separate Form-native seeded
      windows cover `8` reciprocal pair windows (`16` directed cross-locale directions, `32` A/B plus self
      roundtrip lanes), which is `1454` basis points of the broad ready pair space and `1777` basis points of the
      Sanskrit-baseline pair space. This keeps prototype/Form receipts from being misreported while making neural
      pair coverage non-zero.
- [x] **Speech pair training next action added.** `learn/speech-pair-training-next-action.fk` now chooses the next
      executable neural movement (`32767`): choose `next-native-neural-pair-window-0002` over `en<->pt-br`, route
      `train-native-neural-pair-window`, keep Form-native pair windows `8 -> 8`, move neural pair windows `1 -> 2`,
      and carry the corpus floor gap `211/12000`. The point is to keep training toward full open ASR/TTS, not stop
      at the first micro-pair.
- [x] **Speech Form pair window 0006 added.** `learn/speech-form-pair-window-0006.fk` executes the selected
      `en<->de` reciprocal Form-native window (`32767`): NL moves `0 -> 100`, audio moves `0 -> 100`, route shifts
      `oracle-guide -> native`, and neural/diffusion/trained-neural remain `0`. Form pair coverage is now `6`
      reciprocal windows, `12` directed cross-locale directions, and `24` A/B plus self roundtrip lanes; neural
      pair coverage remains `0`.
- [x] **Speech Form pair window 0007 added.** `learn/speech-form-pair-window-0007.fk` executes the selected
      `en<->es` reciprocal Form-native window (`32767`): NL moves `0 -> 100`, audio moves `0 -> 100`, route shifts
      `oracle-guide -> native`, and neural/diffusion/trained-neural remain `0`. Form pair coverage is now `7`
      reciprocal windows, `14` directed cross-locale directions, and `28` A/B plus self roundtrip lanes; neural
      pair coverage remains `0`.
- [x] **Speech Form pair window 0008 added.** `learn/speech-form-pair-window-0008.fk` executes the selected
      `en<->fr` reciprocal Form-native window (`32767`): NL moves `0 -> 100`, audio moves `0 -> 100`, route shifts
      `oracle-guide -> native`, and feeds the first neural micro-pair receipt.
- [x] **Speech native neural bootstrap and pair window added.** `learn/speech-native-neural-bootstrap.fk` enables
      `1` native neural parameter and `1` epoch (`32767`), and
      `learn/speech-native-neural-pair-window-0001.fk` trains the first non-zero neural micro-pair over `en<->fr`
      (`32767`): neural pair coverage moves to `1` unordered pair and `2` directed routes, with neural rate
      `0 -> 100`. This is not capped at a micro-pair; it feeds the full open ASR/TTS target.
- [x] **Speech open ASR/TTS target model added.** `learn/speech-open-asr-tts-target-model.fk` makes the goal
      executable (`32767`): native open ASR and Sema TTS should reach or beat the local oracle. Current native
      authority remains below target, but the route is enabled and no longer zeroed out.
- [x] **Seven live Metal pair anchors stand — `7/7`, `full-metal-native` for the closed-prompt carrier.**
      The Form-owned macOS carrier runs `en<->de`, `en<->es`, `en<->id`, `en<->fr`, `en<->it`, `en<->zh`, and `en<->ar` on live Apple
      Metal (one `presence/macos-*-speech-roundtrip-variant.fk` per pair; carrier verdict `511` each).
      `learn/metal-live-pair-anchors.fk` counts `7/7` live anchors and routes `full-metal-native` for the
      closed-prompt local audio-locale carrier; `learn/metal-observed-sweep-bridge.fk` binds the live route-shift
      receipts to the multiseed sweep (band `32767`). Per-pair training moves native success from `0%` toward
      `83–100%`; each pair's trained field-code records its oracle/native/reciprocal counts. Chinese is the first
      live Unicode-script audio anchor (`en<->zh`: 10/12 oracle-ok, 10/12 native, 83% total, A->B 66%, B->A 100%).
      Arabic is the second live Unicode-script audio anchor (`en<->ar`: 12/12 oracle-ok, 12/12 native, 100% total,
      A->B 100%, B->A 100%) using the local Majed voice.
      The Indonesian side reuses the installed Damayanti voice for train/eval, both at the macOS voice boundary.
      Open transcript receipt support now lives separately; native open ASR and native neural ASR/TTS remain pending.
- [x] **Unicode transcript token lane added.** `observe/stt-wer.fk` now owns shared `sw-tokens`: ASCII
      punctuation still lowercases/splits, accented Latin stays inside words, CJK ideographs become per-character
      tokens, and Arabic words stay grouped while Arabic/CJK punctuation is dropped as delimiters. The macOS
      carrier now calls this shared tokenizer instead of its old ASCII-only local scanner. The tokenizer is compact
      enough to keep the observed-sweep prelude under the current direct-source function-table ceiling. The focused
      band `observe/tests/stt-tokenize-unicode-band.fk` returns `4095`; existing STT WER, macOS carrier, and
      observed-sweep bridge bands remain `255`, `511`, and `32767`.
- [x] **Speech model AutoML selector added.** `learn/speech-model-auto-selection.fk` makes the current model
      choice executable: ASR selects `prototype-asr` (`nearest-l1-wav-feature-prototype`), TTS selects
      `sema-voice-sample-loop` (`target-fit-listener-wer-ab-formant-vocoder`) over the raw formant carrier,
      NL2NL selects `closed-set-locale-form`, and audio2audio selects
      `native-source-window-audio2audio-acoustic` for the segmented-source-window, metal-witnessed acoustic route.
      `open-dictation-transcript` is now a live-observed ASR
      receipt candidate, but does not displace the closed-set prototype until a native open-ASR candidate emits
      local transcript text. `native-open-asr-ctc` is now present as a Form-native CTC token-stream decoder
      candidate, `native-acoustic-token-emitter` is present as the supervised Form feature-to-frame bridge, and
      `native-segmented-acoustic-learning` is present as the local-oracle segment learner, but none are
      live-selected until real audio can emit segmented token frames over winning receipts. `small-transformer-nl` remains trainable but not
      live-selected; `diffusion-codec-speech` is present but not ready because no Form-native executable kernel
      receipt exists yet. The selector composes observed auto-learning and reversible A/B controls; after the live
      Sema formant WER-100 miss, it also exposes the voice miss action and the concrete
      `text-conditioned-acoustic-vocoder` Form kernel plus the native audio2audio acoustic bridge, multilocale
      audio2audio acoustic sweep, Metal acoustic authority row, live open-ASR source authority training row,
      multilocale segmented source-ASR window candidate, source-window audio2audio authority row, and live
      segmented feature carrier.
      The audio2audio arm now selects
      `native-source-window-audio2audio-acoustic` for the segmented-source-window, metal-witnessed scope. The band
      returns `536870911`.
- [x] **Open dictation transcript receipt added.** `learn/open-dictation-transcript-learning.fk` admits arbitrary
      utterance rows with consentful side-channel truth, local free oracle transcripts, optional native transcript
      candidates, Unicode token WER, and choice/cut/fail/undo/timeout promotion gates (`16383`). The macOS carrier
      `presence/macos-open-dictation-carrier.fk` renders `Open speech flows.` locally with `say`, lowers it with
      `ffmpeg`, transcribes it with local Whisper on Apple Metal, and returns live verdict `511` with field code
      `440000100`: four oracle successes, zero native successes, oracle WER `0`, native WER `100`. This removes
      the closed prompt-ID assumption from the receipt path; it does not claim native open ASR yet.
- [x] **Live open-ASR source authority learning added.** `learn/live-open-asr-source-authority.fk` turns the live
      open-dictation miss into an executable source-ASR training action (`32767`): oracle WER `0` plus native WER
      `100` routes `train-live-segmented-open-asr-source` toward `native-segmented-acoustic-learning`, while
      authority remains `oracle-guide` until a local native transcript candidate wins. The speech selector now
      exposes this live Metal training path; the later source-window audio2audio authority moves the selector to
      `536870911`.
- [x] **Speech token stream + native CTC open-ASR candidate added.** `observe/speech-token-stream.fk` makes the
      side-channel stream explicit: words plus `<NODE>`, `<SOURCE>`, `<CHANNEL>`, `<INTERFACE>`, `<CHOICE>`,
      `<FAIL>`, `<UNDO>`, `<TIMEOUT>`, `<CUT>`, `<OBSERVE>`, `<GRADE>`, `<FEEDBACK>`, `<REPAIR>`, `<RECEIPT>`,
      `<STATE>`, `<MEMORY>`, and `<SCOPE>` tokens, each carrying confidence, warmth, cadence, hesitation,
      excitement, and attunement metadata (`32767`). `observe/open-asr-ctc.fk` collapses acoustic frame-token
      streams into that token stream, emits free transcript text, and lowers the result into open-dictation
      promotion (`32767`).
- [x] **Native acoustic token emitter added.** `observe/acoustic-token-emitter.fk` admits consentful
      oracle-aligned acoustic token prototypes, emits blank/nonblank CTC frames by integer L1 distance plus earned
      confidence, and lowers those frames through `open-asr-ctc` into the open-dictation promotion gate. The band
      returns `32767`. This is a supervised native frame-token emitter, not a finished neural acoustic encoder;
      live mic audio still needs a segmented feature-row carrier before it can displace the closed-prompt ASR route.
- [x] **Segmented acoustic token learning added.** `learn/segmented-acoustic-token-learning.fk` makes the next
      carrier contract native: wav/envelope windows become segmented feature rows, local-oracle transcript tokens
      teach acoustic token prototypes, trained source tokens decode through CTC, and the Sanskrit baseline renders
      target-locale tokens through neutral meaning. The focused band returns `32767` over `sa<->la`: bad oracle,
      timeout, or missing consent blocks credit, and reciprocal directions are required before the native route
      opens. Neural ASR/TTS and live mic streaming remain pending.
- [x] **Multilocale segmented source-ASR window added.** `learn/multilocale-segmented-source-window.fk` lifts the
      segmented source learner across `sa<->la`, `en<->zh`, and `ar<->en` (`32767`): six local-oracle samples train
      source-token prototypes from native score `0` to `6/6`, all three reciprocal pairs become ready, and the
      route shifts to `native-multilocale-segmented-source` only inside that witnessed window. The speech selector
      now exposes this ASR candidate without displacing `prototype-asr`; the later source-window audio2audio
      authority and live feature carrier move the selector to `536870911`.
- [x] **Source-window audio2audio authority added.** `learn/source-window-audio2audio-authority.fk` composes the
      multilocale segmented source-ASR window with the Metal audio2audio acoustic authority (`32767`): source speech
      now routes through native source tokens, neutral Form meaning, and target acoustic frames as one witnessed
      authority row. AutoML now selects `native-source-window-audio2audio-acoustic` for the audio2audio arm and
      the later live feature-carrier row moves the selector to `536870911`, while live mic streaming and neural
      speech remain pending.
- [x] **Live segmented feature carrier contract added.** `presence/live-segmented-feature-carrier.fk` admits
      observed local wav/envelope facts into native open-ASR candidate rows (`32767`): eight-window envelopes lower
      to four Form feature rows, acoustic-token prototypes emit CTC frames, and four passing native rows can route
      through open-dictation promotion. AutoML now sees this ASR candidate and returns `536870911`, but selected ASR
      remains `prototype-asr` until real live capture windows win.
- [x] **Speech-token training source policy added.** `learn/speech-token-training-source.fk` separates token label
      provenance into local oracle, consentful corpus, and internal-state inference rows (`32767`). Local oracle
      and corpus rows may train transcript words and metadata; internal state can train confidence, warmth,
      cadence, hesitation, excitement, attunement, controls, evidence, memory, and scope, but is explicitly
      blocked from claiming transcript truth.
- [x] **The offer/ack control core** — `control/offer-ack-core.fk`: the five Form control primitives (fail, stop,
      choice, exceptions, async) as thin expressions over ONE mechanism (`oac-kind` + `oac-offer`), derived from
      axiom-5. Four-way-proven in the origin (1023); re-proof here pends the Form-native eval lane (the C `--src`
      seed does not carry `oac-offer`'s indirect call by design). See `receipts/2026-06-29-offer-ack-control-core.md`.
- [x] **Pattern matching / destructuring (stone S10)** — `control/pattern-match.fk`: `pm-match(cell, clauses)`
      returns the first clause whose pattern-SHAPE (ctor-tag + arity — axiom-3 structural match) fits the cell,
      its holes bound to the cell's children (axiom-2 destructure); no clause fits → the canonical first-class
      nothing (axiom-1). Clauses are DATA (the holographic discipline, no hardcoded branches). Band verdict
      **511 FOUR-WAY** (Go/Rust/TS/fkwu `--src`) over the pure-list cell surface; the canonical-nothing arm is
      fkwu-native (the walkers carry the cell surface, not the nothing op). See `receipts/2026-06-29-stone-10-pattern-match.md`.
- [x] **The eight native control-invite primitives + their BMF grammar added.** `control/choice-lane-core.fk`
      completes the family `control/offer-ack-core.fk` started (choice, fail, stop) with the remaining five as
      thin expressions over the SAME `oac-kind`/`oac-offer` core: cut (commit to the first ack of ANY kind,
      pruning every alternative after it — the classic Prolog cut), lanes (walk every alternative and COLLECT
      each one's ack — the different query lanes a choice invites reasoning to walk, and the nodes/memory it
      gathers along the way, before any picking happens), store/restore (a checkpoint is the memory value
      itself — axiom-3, nothing mutated), undo (fall back to the last checkpoint automatically when the current
      ack failed), and timeout (bound a lane walk by a step budget, distinguishing a real timeout — alternatives
      left untried — from honest all-round decline). `observe/speech-token-stream.fk` now carries the matching
      `<STOP>`, `<STORE>`, `<RESTORE>` tokens alongside the existing `<CHOICE>`, `<CUT>`, `<FAIL>`, `<UNDO>`,
      `<TIMEOUT>` — the same eight verbs as native LLM/speech-stream token invites. Band `65535`
      (`observe/tests/speech-token-stream-band.fk`), live on `fkwu --src`. `grammars/control-invite-grammar.fk`
      is the BMF grammar that recognizes these eight tokens inside free text and TRANSMUTES each match into a
      `CONTROL-INVITE` node — `bmf-core.fk`'s own cursor → match(pattern) → build(template) arc, deliberately
      the smaller single-rule engine rather than the larger multi-rule `bmf-grammar.fk`. Band `1023`, live on
      `fkwu --src` (`grammars/tests/control-invite-grammar-band.fk`). Honest floor, precisely named and then
      partly closed: this pass found that the C-bootstrap `fkwu`'s indirect call (named as a gap in the
      2026-06-29 offer-ack-core receipt) now works, but `oac-kind`'s blueprint discrimination does not
      reproduce reliably once more than one `let`-bound value is alive in a scope — traced live (gdb,
      `FK_OBSERVE`, ruling the JIT in/out, and a hardware watchpoint on the actual storage cells) to a runtime
      bug distinct from blueprint identity: a `let`'s storage slot is meant to be permanent for its scope, but
      the evaluator's own local-reservation opcode treats the same storage as ephemeral scratch, so a later
      computation can silently overwrite an earlier binding before its scope ends. `control/offer-ack-core.fk`
      itself carried an instance of this — its own `OAC-ZERO`/`OAC-ONE`/`OAC-NODE` foundational tags were bare
      top-level `let`s — **fixed** by naming each as a zero-argument function that calls `bp` fresh instead of
      caching it. Combined with wrapping `control/tests/choice-lane-core-band.fk`'s body in one `defn`
      (matching every other passing band), live result moved from garbage to the full **`1023`**. The fix is
      Form-level only, no C-seed change. `control/invite-dispatch.fk` (new: closes the loop, walking a BMF-
      recognized invite stream and driving the matching primitive, threading memory/checkpoints through) needs
      a larger combined prelude to run at all, and the same defect class resurfaces there through a different
      combination (adding `form/form-stdlib/core.fk` alone, 74 pure-`defn` functions, is enough) — a real,
      verified fix to one instance, inside a defect class that is now well-characterized but not closed. See
      `receipts/2026-07-01-node-children-last-writer-wins.md` for the trace and fix,
      `receipts/2026-07-01-choice-lane-control-invites.md` for the primitives, and
      `receipts/2026-07-01-invite-dispatch.md` for the dispatcher's honest current state.
- [x] **Locale-neutral meaning locate added.** `learn/sanskrit-locale-baseline.fk` gains `slb-meaning-for-tokens`
      (locale-specific surface tokens -> the neutral meaning id they belong to — the reverse of `slb-tokens`,
      which every existing multilocale sample built FROM a meaning id but never walked back TO one) and
      `slb-locate-cross-locale?` (do two locales' own tokens independently locate the SAME meaning). Composed
      into `learn/multilocale-nl-audio-pipeline.fk` as `mlap-nl-meaning-located?`/`mlap-nl-all-located?`, an
      honest two-sided check that the pipeline's own asserted `meaning` fields are actually derivable from raw
      tokens, not merely taken on faith. The band (`learn/tests/locale-neutral-locate-band.fk`) returns `255`:
      cross-locale exact matches hold for the four baseline phrases across all ten ready locales, a genuine
      cross-locale mismatch is correctly rejected, and out-of-baseline tokens honestly locate to `0` rather than
      a false positive — the honest floor stays that this covers only the small Sanskrit-baseline vocabulary,
      not open text. See `receipts/2026-07-01-locale-neutral-locate.md`.
- [ ] `form-cli` standing as an interactive loop (the single-file source-runner stands; the loop is polish).
- [ ] Origin repo consumes this kernel (one-home). The heavy-chain form-cli *build* still leans on a Go-made-once seed.

## What's still ahead — the roadmap to a self-speaking native mind

Source runs straight off the cursor (`form-eval`), `form-eval-cli` stands, and `proof/four-way-run` proves
four-way with no bash. The body and organs are home and four-way (the learning / form-cli / framebuffer /
calibration cells, host-exec, http-client, form-asm). What remains is the **mind**, the **voice**, and
**live speed** — the body is home; the frontier voice is the climb:

1. **The generative weights (the mind).** A real open base (Qwen/Llama, real zh coverage) loaded as *recipe-data*
   through the form block — the whisper block-0 pattern (real trained weights through the Form block, 6.66e-15)
   extended to a generative base, then oracle-refined. The full decoder forward (attention, positional, multi-head
   concat, LM head) proven bit-exact, then the distill loop, then a pre-registered eval before any "≥ rented"
   claim. The speaking *floor* (grounded composition) stands; the frontier voice waits on this. A multi-week climb
   with its own receipts. (`HOMECOMING.md`.)
2. **The voice's sound (ASR + acoustic model + vocoder).** Prosody, phrasing, emphasis, g2p, a source-filter
   formant vocoder, Sema's voice target/sample loop, closed-set prompt recognition, loopback carrier receipts,
   open transcript receipts, and recipe A/B promotion now stand as native measured cells (`presence/`, `observe/`,
   `learn/`). The natural acoustic
   model/neural vocoder, native open ASR decode, and perception receipt that rendered uncertainty tracks real
   calibration remain the pending carrier.
   (`presence/voice-roadmap.md`.)
3. **Cognition at native speed — the JIT in the live path.** The hot LLM/RAG cells crystallize through fkwu's
   self-JIT / form-asm lowering live, not tree-walked. The asm exists; wiring it under live cognition remains.
4. **The heavy-chain form-cli build off its Go-made seed.** The binary *runs* Go-free; the full form-cli / fsh
   chains still *build* from a committed Go-made-once seed (`form-cli-emitted.c`, `form-cli-table.txt`). Closing
   this — and live-wiring the proven observe/learn organs to the running kernel — is the last self-sufficiency gap.

The minimum "core we can observe and trust" — runs natively (done), proves itself four-way (done, `proof/`),
watches itself think (the observe organs, four-way; live-wiring pending) — is essentially standing. What "home"
still waits on is the mind running as recipe-data through this body, and the voice becoming audible.
