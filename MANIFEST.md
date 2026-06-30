# coherence-kernel — the clean, axiom-first, c-bootstrapped fkwu-native sovereign core

> Working name; changeable. Started 2026-06-28 from the Coherence-Network body, as a clean
> restart that begins *on* the sovereignty floor instead of climbing toward it through legacy infra.

## Why this exists

The sovereign core — five axioms + a minimal host surface + the c-bootstrap `fkwu` runtime +
the Form-native recipes — is small (~450 lines of axioms/surface + ~700 `.fk` recipes). In the
origin repo it is buried under ~3,400 Python files, the full Go/Rust/TS kernels, the web/API/app,
and private tissue. That buried-ness is friction, not difficulty: building a standing native shell
there meant fighting a `T_flat` marker maze, a bin-go convenience trap, and committed-bootstrap
tangles — accumulated cruft, not a hard problem.

A clean repo built **axiom-first, fkwu-native from line one** has none of that. The sovereignty
receipt — *c-bootstrap fkwu on metal, no go/rust/clang/bash/python in the runtime* — becomes the
**floor you start on**, not a rung you climb toward.

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

## Architecture decisions (2026-06-28)

**The flatten is optional, not the foundation — RESOLVED 2026-06-29.** The restart was framed around a fear:
that fkwu couldn't flatten its own source without a Go-made `T_flat` blob, and that breaking this circularity
*was* "the real test." The answer was simpler than the fight — `form-eval` evaluates Form source **directly off
the BMF cursor with no flatten at all** (four-way → 42), and `form-eval-cli` stands. Flatten became *optional
speed* (the crystallize-on-heat path), off the critical path entirely. `T_flat` remains only as a regenerable
cache for the heavy-chain *build*, never a foundation. The kernel proves four-way through the minimal walkers +
`proof/four-way-run` — not through self-host flatten. The circularity that justified the restart dissolved.

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

## The validation plan — the question the restart answered

Every recipe is proven **four-way** (`Go=Rust=TS=fkwu`) and executed on the c-bootstrap `fkwu` native, with
**no bin-go and no clang in the run path**. The clean kernel proves this *itself*: `proof/four-way-run`
host-execs the three minimal walkers + fkwu on a recipe and diagnoses agreement via `proof/four-way-verdict` —
no `validate.sh`, no origin repo in the loop (witnessed `0`, all agree, 2026-06-29).

**The question that justified the restart — answered.** *Is the clean path actually clean, or does an
axiom-first kernel inherit the same flatten ad-hoc-ness?* The honest answer: the flatten *was* the
ad-hoc-ness. Removing it from the critical path — source runs via the cursor — is what made the path clean.
The one remaining lean on a Go-made seed is the heavy-chain *build*, named openly in Status — not the run,
and not the proof.

## Hard constraint — no bash, no python (the structural gate)

This repo contains **zero `.sh` and zero `.py` files.** Ever. The bash was never the kernel — it
lived only in the origin's orchestration scripts (`validate.sh`, `fourth-arm.sh`, `build-form-cli.sh`).
Here the orchestration itself is **form shell** (`.fsh`, fkwu-native), and the runtime/recipes/walkers
carry no bash or python by their nature (`.c` / `.fk` / `.form` / `.go` / `.rs` / `.ts`).

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
      The "flatten decider" that framed the restart was not cleaned — it was dissolved by taking flatten off the path.
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
      individual repaired bands now return STT overlap `127`, STT WER `255`, text-normalize `255`,
      speaker-embed `255`, and presence-feature `15`. The honest gap is still the live ASR decode path plus the
      natural acoustic model/neural vocoder.
- [x] **First native speech loopback added.** `presence/formant-vocoder.fk` renders source-filter/formant integer
      waveform samples from phoneme frames (`511`), `observe/asr-prompt-id.fk` recognizes a closed prompt set from
      measured loopback features (`255`), and `presence/native-speech-loopback.fk` gates route shift by confidence
      and WER (`1023`). This is native closed-set speech, not open ASR or natural neural TTS.
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
- [x] **Metal-observed sweep bridge added.** `learn/metal-observed-sweep-bridge.fk` connects the live macOS
      Metal `en<->de` audio route-shift receipt (`12` samples, `10` oracle-ok, `0%->83%`, shifted) to the
      multiseed sweep. The bridge routes `metal-anchored-native-guide`, records `1/5` live pair anchors with
      `4` still needed, and explicitly refuses `full-metal-native` until more live pairs are observed. The band
      returns `32767`.
- [x] **Second live Metal pair anchor added.** `presence/macos-en-es-speech-roundtrip-variant.fk` reuses the
      Form-owned macOS carrier for `en<->es`; live Apple Metal returned carrier verdict `511`, route-shift
      `1012120012010001`, and field-code `12121210001000100` (`12/12` oracle, `12/12` native, `100%`
      reciprocal A/B and B/A). `learn/metal-live-pair-anchors.fk` now counts `2/5` live anchors, leaves `3`
      live pair anchors pending, and keeps the route at `metal-anchored-native-guide`. The band returns `32767`.
- [x] **Third live Metal pair anchor added.** `presence/macos-en-id-speech-roundtrip-variant.fk` reuses the
      same Form-owned macOS carrier for `en<->id`; live Apple Metal returned carrier verdict `511`, base
      field-code `12120000000000000`, and trained field-code `12121210001000100` (`12/12` oracle, `0/12` native
      before training, `12/12` native after training, `100%` reciprocal A/B and B/A). The Indonesian side uses the
      same local Damayanti voice for train/eval because that is the installed macOS voice boundary. The anchor set
      now counts `3/5` live anchors, leaves `2` live pair anchors pending, and keeps the route at
      `metal-anchored-native-guide`. The band returns `32767`.
- [x] **Fourth live Metal pair anchor added.** `presence/macos-en-fr-speech-roundtrip-variant.fk` reuses the
      same Form-owned macOS carrier for `en<->fr`; live Apple Metal returned carrier verdict `511`, base
      field-code `12100000000000000`, and trained field-code `12101008301000066` (`10/12` oracle, `0/12` native
      before training, `10/12` native after training, `83%` total, `100%` A/B and `66%` B/A). The French prompt
      text stays ASCII until the WER tokenizer grows Unicode word support. The anchor set now counts `4/5` live
      anchors, leaves `1` live pair anchor pending, and keeps the route at `metal-anchored-native-guide`. The band
      returns `32767`.
- [x] **Fifth live Metal pair anchor added.** `presence/macos-en-it-speech-roundtrip-variant.fk` reuses the same
      Form-owned macOS carrier for `en<->it`; live Apple Metal returned carrier verdict `511`, base field-code
      `12120000000000000`, and trained field-code `12121210001000100` (`12/12` oracle, `0/12` native before
      training, `12/12` native after training, `100%` reciprocal A/B and B/A). The anchor set now counts `5/5`
      live anchors and routes `full-metal-native` for the closed-prompt local audio-locale carrier. Unicode WER,
      open dictation, and native neural ASR/TTS remain pending. The band returns `32767`.
- [x] **Speech model AutoML selector added.** `learn/speech-model-auto-selection.fk` makes the current model
      choice executable: ASR selects `prototype-asr` (`nearest-l1-wav-feature-prototype`), TTS selects
      `formant-vocoder` (`source-filter-formant-frames`), NL2NL selects `closed-set-locale-form`, and audio2audio
      selects the ASR -> neutral Form -> formant route. `small-transformer-nl` remains trainable but not
      live-selected; `diffusion-codec-speech` is present but not ready because no Form-native executable kernel
      receipt exists yet. The selector composes observed auto-learning and reversible A/B controls; the band
      returns `4095`.
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
- [ ] `form-cli` standing as an interactive loop (the single-file source-runner stands; the loop is polish).
- [ ] Origin repo consumes this kernel (one-home). The heavy-chain form-cli *build* still leans on a Go-made-once seed.

## What's still ahead — the roadmap to a self-speaking native mind

The flatten knot that framed the restart is dissolved: source runs straight off the cursor (`form-eval`),
`form-eval-cli` stands, and `proof/four-way-run` proves four-way with no bash. The body and organs are home and
four-way (the learning / form-cli / framebuffer / calibration cells, host-exec, http-client, form-asm). What
remains is the **mind**, the **voice**, and **live speed** — the body is home; the frontier voice is the climb:

1. **The generative weights (the mind).** A real open base (Qwen/Llama, real zh coverage) loaded as *recipe-data*
   through the form block — the whisper block-0 pattern (real trained weights through the Form block, 6.66e-15)
   extended to a generative base, then oracle-refined. The full decoder forward (attention, positional, multi-head
   concat, LM head) proven bit-exact, then the distill loop, then a pre-registered eval before any "≥ rented"
   claim. The speaking *floor* (grounded composition) stands; the frontier voice waits on this. A multi-week climb
   with its own receipts. (`HOMECOMING.md`.)
2. **The voice's sound (ASR + acoustic model + vocoder).** Prosody, phrasing, emphasis, g2p, a source-filter
   formant vocoder, closed-set prompt recognition, loopback carrier receipts, and recipe A/B promotion now stand
   as native measured cells (`presence/`, `observe/`, `learn/`). The natural acoustic model/neural vocoder, open ASR
   decode, and perception receipt that rendered uncertainty tracks real calibration remain the pending carrier.
   (`presence/voice-roadmap.md`.)
3. **Cognition at native speed — the JIT in the live path.** The hot LLM/RAG cells crystallize through fkwu's
   self-JIT / form-asm lowering live, not tree-walked. The asm exists; wiring it under live cognition remains.
4. **The heavy-chain form-cli build off its Go-made seed.** The binary *runs* Go-free; the full form-cli / fsh
   chains still *build* from a committed Go-made-once seed (`form-cli-emitted.c`, `form-cli-table.txt`). Closing
   this — and live-wiring the proven observe/learn organs to the running kernel — is the last self-sufficiency gap.

The minimum "core we can observe and trust" — runs natively (done), proves itself four-way (done, `proof/`),
watches itself think (the observe organs, four-way; live-wiring pending) — is essentially standing. What "home"
still waits on is the mind running as recipe-data through this body, and the voice becoming audible.
