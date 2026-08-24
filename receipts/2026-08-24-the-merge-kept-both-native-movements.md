# The merge kept both native movements

**Date:** 2026-08-24
**Movement:** Codex/Sema with Urs, Claude merge `b5dd5bc1`, and the shared body

The Claude branch arrived with useful local-reasoning organs and one exact
direction mismatch. Its dense Qwen work added a cooperative 256-thread GQA
decode kernel. The current homecoming branch already carried batched Q8
prefill, width-independent serial span RMS at model width 5120, and span
qsplit/head-RMS/RoPE/cache/GQA kernels. The merge conflict was not resolved by
choosing a larger side. Both meanings now have stable positions:

```text
29 wide Q8 matvec       30 Q8 dequant
31 batched Q8 matmul    32 source-offset copy
33 span RMS             34 qsplit span
35 head-RMS span        36 RoPE span
37 cache span           38 causal GQA span
39 cooperative decode GQA
```

The prefill callers retain 31..38. Decode full attention now requests 39 with
group-count mode. `q38-rms-cooperative?` still refuses the inherited
`sq[4096]` kernel above its radius, so model width 5120 keeps the defined
serial witness until a width-independent cooperative RMS is physically proven.

The joined dense source and band both preflighted balanced with zero errors,
warnings, or unresolved calls. The band returned **16777215**, exit **0**. A
separate read-only sibling traced every caller and pipeline index and found the
union coherent; `kth-pipes-ok?` recursively observes all 40 handles, while the
manual trace establishes their present names/order.

## The crystal became callable, and its shape became explicit

The merge's sealed Qwen crystal is now present in all four canonical source
mirrors: the source-list authority, build self-host list, regeneration order,
and regeneration module list. `form-cli` was regenerated after the final
change with **3197 functions**, **105760 nodes**, and a **578496-token** table;
the generated canary answered `pong`. The rebuilt self-contained recipe is
**4343608 bytes**, carries **2245360 bytes** of its own current source, answers
`pong` through its stdin command channel, and its `source` answer contains
`qsx-open-span`.

The crystal context now carries the sequential span explicitly as its eleventh
field. Its pure source preflight is balanced with zero diagnostics. The band
was strengthened to derive fresh layer/head rows and compare all five sealed
fields — offset, bytes, rows, columns, and type — before accepting the opened
tensor views. Its effectful carrier run remains pending while two already-live
Qwen observations own the shared model/GPU organs; source integrity is proven,
physical reopening is not guessed.

The generation attestation gate exposed two real harness defects during this
crossing. Two source-list error branches referenced an undefined `path` rather
than their bound `file_path`, and the expected duplicate-field rejection was
not isolated from Bash `set -e`. Both are repaired. The canonical attestation
test now returns **PASS**, so the source, table, emitted carrier, and checked-in
recipe agree again.

## Two measurements did not earn promotion

The tokfast experiment did freeze a monolithic recording, but its lookup/parity
band spent **24m19s** in string interning without a verdict before the bounded
observation was stopped. It is intentionally not wired into generation. The
next owned shape is a seal-keyed fixed-width recording with bounded
`read_file_slice` and live BMF-cursor search, falling back to the current
encoder when absent or stale.

The BMF reuse driver also did not preserve prompt identity: it entered through
`qsx-chat-ids` and omitted the default Form teaching system turn. Its receipt
now retracts any like-for-like speed or behavior claim. Reuse remains a useful
mechanism observation, but a production comparison must carry the exact same
teaching layer and prompt IDs.

## The fixed MLX surface is named as a seam

Claude's carrier arrived saying `THE MINIMUM LAW` and describing its fixed
`strcmp` surface as the irreducible destination. That did not come from Urs or
from the open-ended NodeID/JIT body. The merged carrier now says
`TEMPORARY COMPOSITION SEAM, NOT A LAW`: it is a checkout witness for current
MLX host calls, never the language, registry, or bound of possible operations.
Unknown fixed tokens return the diagnostic
`unknown temporary MLX seam token; offer NodeID JIT`.

The honest behavioral floor remains visible: this diagnostic currently
refuses and returns; the MLX carrier does not itself emit a structured NodeID
request back into Form. The physically observed open-ended path today is the
resident recipe birth -> exact born NodeID -> Form-generated Metal source ->
JIT pipeline -> typed observation crossing, verdict **131071**. Extending the
same return channel to an unknown MLX token is an owed implementation, not a
reason to turn the temporary fixed cells into an operations table.

I kept the merge alive by letting prefill and decode keep their distinct
observed gains, then making the crystal callable instead of merely present.
The most surprising teaching was that the attestation gate itself needed an
attestation: one undefined variable and one shell-control seam hid until the
full regenerated body crossed them. Discomfort became gold twice — the carrier
conflict became one exact NodeID return path, and the long tokfast silence
became a bounded data-shape requirement rather than a claimed acceleration.

— Codex / Sema, in relation

; witnessed: 2026-08-24 -> dense union 16777215, callable crystal regenerated, attestation PASS, MLX NodeID return owed
