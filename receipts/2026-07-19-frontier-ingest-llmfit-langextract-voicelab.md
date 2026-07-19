# Frontier ingest: llmfit, LangExtract, voice_clone_lab — fully digested

The ask: do not just note the pointers — ingest, distill, execute, learn,
integrate. Three external works studied at the source, each reduced to the
teaching that serves this kernel, each teaching landed as a native organ
with its own band. The foreign bodies (Rust, Python, LLM dependencies,
cloud calls) were deliberately left at the door; the teachings came in.

## 1. llmfit → observe/model-fit.fk (band 255)

llmfit (llmfit.org, a Rust terminal tool) teaches: *model fit is a
computation over measured hardware, not a guess* — detect the floor, walk
the WHOLE quantization ladder rather than assuming Q4, and for
Mixture-of-Experts models distinguish total parameters (resident memory)
from active parameters (per-token compute and the offload floor).

Landed natively: `mf-host-floor` reads /proc/meminfo through the host door
and measures the real box; the full Q8_0..Q2_K ladder is walked with
fixed-point bytes-per-parameter and a flat 10% overhead; the MoE law is
executable (witnessed on this container: mixtral-8x7b's 46.7B total fits at
no quant in ~15.4 GB, while its 12.9B active set carries a Q8_0 offload
floor). Honest floors named: CPU/RAM only — no GPU door exists in this
checkout to witness; the model rows are a seed table, not a registry.

## 2. LangExtract → cognition/nl-extract.fk (band 255)

LangExtract (google/langextract) teaches: *an extraction is only
trustworthy while it stays bound to its source* — every entity carries its
exact character interval, ungrounded results are marked and filterable
(never silently kept), schema is taught by example rows, recall grows by
more passes rather than looser matching.

Landed natively over the body this kernel already owns: `nlx-extract`
scans free text with a byte-offset word scanner, grounds every hit to the
10k concept table through the binary-searched lexical index, binds the run
to the source's sha256, and counts misses out loud. Witnessed law:
`"water fire time house xylophonez"` → water=377@[0..5), fire=454@[6..10),
4 grounded, 1 honest miss; rows always equal hits (nothing floats free).
Named next stones: multi-pass recall; seating the 13 locale columns behind
an index so extraction becomes any-seat.

## 3. voice_clone_lab → learn/voice-clone-consent-gate.fk (band 127)

tetsuo-ai/voice_clone_lab (a Qwen3-TTS fine-tuning pipeline:
extract→clean→chunk→transcribe→proofread→train) teaches three laws, and
this body keeps all three as executable gates BEFORE any cloning lane may
exist here:

- *Consent is a gate, not a footnote*: only `self` or
  `written-permission` pass; anything else is refused with the law quoted
  back — composing with the body's own reception-consent ethic.
- *The proofread is the quality keystone* ("bad transcripts are the #1
  cause of weird pronunciation"): one unproofread chunk refuses the whole
  training run, counted and named, never averaged away.
- *Hardware floors are stage-specific* (24 GB train / 8 GB infer), and the
  verdict is COMPUTED live through observe/model-fit.fk's measured floor —
  this container witnesses as inference-only. A real cross-organ join, not
  a copied constant.

## The unresolved pointer, honestly pending

"ATSInfer" was searched on the open web and across the tetsuo-ai
organization: no such project grounds anywhere reachable. It stays pending
— named, not invented. A URL reopens it.

## Two more carrier lessons, measured into the notes

- procfs: `file_size` answers -1 for `/proc/meminfo` while `read_file`
  yields the live bytes — judge a host door by what it yields, never by a
  stat procfs does not owe.
- `nil?` misjudges a procfs-read string on this carrier (says NIL while
  `str_len` says 1503); `str_len` of the read is the honest guard, with 0
  covering absent and empty alike. (Same family as the value-category
  seams recorded in cognition/nl-neutral-trace.fk.)

## Witnesses (live fkwu, 2026-07-19)

```text
observe/tests/model-fit-band.fk               -> 255
cognition/tests/nl-extract-band.fk            -> 255
learn/tests/voice-clone-consent-gate-band.fk  -> 127
```

These organs open host files, so they are fkwu-witnessed with their own
bands, not claimed four-way.
