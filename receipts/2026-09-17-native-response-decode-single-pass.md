# Response decoding walks the vocabulary once

Signed: Codex. This changes Form's executing response path, with the existing
C-bootstrap and no new runtime dependency or C change.

## Observation, repair and byte identity

Model-session generation and ordinary Form generation still decoded each
output ID through an independent `gmt-token-off` vocabulary walk. The encoding
indexes did not remove that output-side traversal. A native probe with 64
fixed public IDs measured **4,344 / 4,349 ms** for reference decoding and
**198 / 198 ms** for a single pass. Both outputs matched byte for byte.

`bml/form-token-decode-batch.bml` now carries that pass. It walks only through
the highest requested ID, decodes only requested pieces, and reconstructs the
original sequence including repeated and empty pieces. The map is local to
the call; it stores no complete vocabulary or persistent index. A traversal
estimate keeps scalar lookup when batching does not save enough repeated
work. IDs are integers inside the source vocabulary, with 0 and 1 present.
The reference byte alphabet and rendering are preserved. Token prediction,
model state and generated-ID accounting are unchanged.

Both `form-cli-model-session.fk` and the ordinary generator's six text-decoding
call sites now use this native implementation.

## Retained-report replay

The prior review's retained final text was re-encoded through the existing
native tokenizer. These **747 IDs** are re-encoded text, not claimed as the
original generated sequence. The private evidence retains them explicitly.
Reference-first and batch-first runs gave:

| Order | Reference decoding | Batch decoding | Output |
| --- | ---: | ---: | --- |
| Reference first | 3,945 ms | 168 ms | exact match |
| Batch first | 3,969 ms | 167 ms | exact match |

All **3,040 output bytes** also match the retained input text, SHA-256
`fa4f4b6de74913d939e6e30c06fbbd57615fcf5bb366971dca4257556146dfa9`.
This is about 24 times faster decoding. The roughly 3.8 seconds saved are
under 1% of that earlier **557,050 ms** whole run; a whole-session speedup of
24 times is not established. No model or provider generation ran in this replay.
Evidence: `.hearth/response-parity/decode-batch-replay-v1`.

## Actual native session

A fresh Qwen Q8 session used the changed production decoder, with a public
short explanatory prompt and a 128-token allowance. It consumed **36** prompt
tokens, generated **74**, stopped normally and released successfully. Decoding
those exact generated IDs through the reference returned identical bytes.
The actual IDs and output remain private under
`.hearth/response-parity/decode-batch-session-v1`.

- Admission and prefill: **30,152 ms**.
- Generation and changed decoding: **13,312 ms**.
- GPU busy time during generation: **10,949,853 microseconds**, a subset of
  the generation interval.
- Additional reference decoding for the comparison: **532 ms**.
- Provider subprocesses and training during the session: **0**.

This establishes live integration and output preservation. It is one small
session, not a quality comparison or an isolated GPU optimization. Admission
reuse and GPU execution are the larger remaining costs in this sample.

## Checks, failures and learning

The existing model-session band passes **4095**, including real in-memory
GGUF decoding of reordered, repeated, empty and non-ASCII pieces; raw byte
expectations; IDs 0 and 1; invalid-ID planning; and two distinct vocabularies
that cannot share cached pieces. Reference handling of out-of-radius special
tokens remains explicitly compared, without claiming improved Unicode support.
Existing final-stage budget, ordinary answer reserve and generation report
bands pass **1**, **127** and **16777215**. Fresh preflights are clean.
Drift gates pass **8191**, binary freshness **31**, whitespace checks clean.

The first private probe used unresolved `max`; the replay helper first used
unresolved `fcap-int-json`. The compile-only checks refused both. Direct Form
comparison and explicit native JSON-node construction repaired those helpers
before either measurement ran. Neither failure was accepted as evidence.

Native guide: Python implementations **0**, invocation candidates **2**,
unread files **0**. Counsel: orphans **0**, **11/12** lanes unobserved because
no hearth stands. Glass's first frame reports `dt=0/50ms`; its owned viewer
was closed. The prior procedural learner completed round **77**, pending
**0**, promotions **4**, with its serving generation unchanged. That Llama
learner was terminal before the Qwen session started.

After Qwen released, verified decoder procedure was retained under event
`single-pass-response-decode-verified-v1`, session
`codex-native-response-parity-2026-09-17`. Its worker launched; completion or
promotion of that new teaching is not yet claimed. Evaluated replies and
desired answers were excluded.

The completed-turn reader reports the preceding coordinating turn
`01a0af21-764f-7a91-84c2-5619b59c2e7b`: **40** model calls,
**4,261,124** input tokens including **4,157,568** cached and **103,556**
uncached, **17,529** output including **5,344** reasoning, plus **25,148**
explicitly unattributed tokens. Reconciled total: **4,303,801**. This excludes
the current open turn and separate provider processes. Native-run rental
counts do not erase this substantial coordination cost.

The useful surprise was that the output lookup still repeated work after
input tokenization had gained an index. The harder correction was scope:
a large component speedup saves only a small part of the whole response.
Keeping both readings visible lets the next step address the larger costs.
Overall response quality, frequency/resonance and minimal total rental remain
open; this movement embodies a measured output-preserving improvement.
