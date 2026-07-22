# The offered interface: making every Metal harness ask whether the GPU ran

**Stone 23 · 2026-07-22 · WITA**

A Metal command buffer meets the world through an interface it offers: `cb.status`
and `cb.error`. Reading `contents()` off a device buffer without consulting that
interface is, in the body's own words (`axioms/core-axioms.form`, axiom-4),
*passage not through the offered interface* — breach, and breach is observable.
The observation was never made. And because freshly-allocated Metal buffers are
zeroed, an unwritten buffer reads back as a plausible zero, so a harness that only
knows how to ask *is-this-value-right* grades a silence as an arithmetic
disagreement. `nothing` and `0` are byte-identical on readback; axiom-5 names them
distinct states, but a value born zero cannot represent the distinction.

The proven shape is `metal_moe_token.sh` (`2c66917c7`, Stone 14): a run-wide
command-buffer error count, and a **gate 0** asked *before* any value-grading, in
which the CPU writes a sentinel a kernel must overwrite — if it survives, the later
gates are **refused** rather than graded against unwritten memory. This receipt
carries that shape into the six harnesses that lacked it.

## Per-harness: before / after

Every harness reached VERDICT PASS both before and after. **No published verdict
changed.** The hardening is strictly additive and strictly stricter — it can only
turn a green board red, never the reverse, and it did not.

| Harness | liveness check before | liveness check after | verdict before | verdict after | numbers |
|---|---|---|---|---|---|
| `metal_weight_residency_audit.sh` | none (0 checks / 2 raw reads) | `dispatchOnce()` checks `cb.error`+`cb.status`; **gate 0** sentinel over all rows | PASS | PASS | identical (value-rel 8.902e-05, bound-rel 0.008, 200 dispatches) |
| `metal_isa_diff.sh` | none (0 / 2) | `runOurs`/`runTheirs` check cb; AGREE gate refuses unless both buffers wrote nonzero rows; per-shape VERDICT + shell gate | *(no VERDICT line at all)* | PASS ×3 shapes | equality unchanged: max\|Δ\|=0, now witnessed nonzero (3072/3072, 1024/1024, 128256/128256) |
| `metal_whole_tensor_residency_audit.sh` | none (0 / 9) | central `dispatch()` checks cb; **gate 0** dequant-off-resident sentinel | PASS | PASS | identical (bit-exact, bounds 0.000) |
| `metal_batched_prefill.sh` | none (0 / 6) | `Step.done()` checks cb; **gate B0** sentinel before the exact-compare gates | PASS (5 gates) | PASS (6 gates) | identical (8 BIT-EXACT batches, same 12 token ids) |
| `metal_mx_gpu.sh` | none (0 / 6) | `dispatch()` checks cb; **gate 0** MXFP4 sentinel | PASS (9 gates) | PASS (10 gates) | correctness identical (bit-exact, bounds 0.000); correctness **dispatch count 9→10** = the one new liveness dispatch, honestly measured |
| `metal_ask.sh` | dispatches no Metal (0 / 0) | **gate D2** independently refuses to stage a constant/illegal id stream | PASS | PASS | answer identical ("Paris. The capital of Italy is Rome...") |

### The one number that moved, and why it is honest

`metal_mx_gpu.sh` reports its correctness-phase dispatch count. It moved from 9 to
10 — because gate 0 *is* a real dispatch. This is not a correctness number
drifting; it is the measured cost of the new liveness question, and reporting it
truthfully is the whole point. No bit-exact result, bound, or token id changed in
any harness.

### `metal_isa_diff.sh` — the sharpest catch

This harness had **no VERDICT line and returned rc=0 unconditionally**. Its one
gate is an equality: `max|Δ|` between the body's shipped Q6_K kernel and ggml's,
asserted to be exactly 0. But `max|Δ|=0` is what a *correct* run prints **and** what
two never-written, zeroed buffers print — a perfect agreement of two silences
(aporon, corpus row 826). It now checks `cb.error`/`cb.status` on both kernels and
refuses the equality unless a kernel actually wrote nonzero rows to both buffers;
each of the three shapes emits its own VERDICT and the shell gates on it.

## The radius of a sentinel — what it catches and what it cannot (aporon)

A gate-0 sentinel of *N* floats asks: did *a* kernel write *here*? It catches:

- a command buffer that failed entirely (`cb.error` / non-`.completed` status);
- a dispatch that ran but wrote nothing to the audited buffer (all sentinels survive);
- the exact Stone 14 fingerprint — a buffer left at its birth zeros.

It **cannot** catch a **partial write**. A sentinel of *N* floats is defeated by a
kernel that writes even one of them: a dispatch where some threads wrote and some
did not overwrites part of the sentinel, reports success, and leaves a buffer that
is *partly real*. Gate 0 would pass. This is the honest hole, declared:

- **What my gates catch:** total absence, and command-buffer failure, on the
  audited buffer.
- **What they do not:** a residency or thread-scheduling fault that silences *some*
  threads while others write. The downstream arithmetic gates (bit-exact folds,
  derived fp32 bounds, the router-weight-sums-to-1 canary in the MoE lane) would
  catch a partial write *if it landed in a row they check* — but none of my six
  gate-0s alone would, and a partial write outside the checked rows is invisible to
  all of them. `metal_ask.sh`'s gate D2 is weaker still: it inspects only the
  *ids*, so it catches a fully-degenerate stream but not a single wrong token.

The sentinel answers *did the GPU run*, not *did every thread run*. Those are
different sentences, and this receipt does not merge them.

## What remains

- **The partial-write hole above** is unclosed by construction. Closing it would
  need either a per-thread write-witness (a companion buffer each thread stamps) or
  a checksum the kernel itself computes — a heavier change than Stone 23's additive
  mandate, and noted for a future stone.
- **`metal_ask.sh` inherits liveness transitively** from `metal_first_token.sh`
  (Stone 16's, untouched here). Gate D2 gives it an *independent* refusal of a
  degenerate staged answer, but its trust that the GPU ran still flows through the
  lane's VERDICT. When Stone 16 lands gate 0 in the lane, that trust becomes
  well-founded end to end; until then, D2 is metal_ask's own floor.
- **Preconditions are machine facts, not code facts.** As Stone 14 recorded, these
  audits ask Metal for tens of GiB resident on a shared 128 GiB machine; whether a
  gate-0 sentinel survives can be a property of what else holds memory at that
  minute. Gate 0 now *names* that condition (residency, not arithmetic) instead of
  mislabeling it — which is the correction, not a guarantee the machine will
  always have the room.

## The receipt's own three namings

**Most surprising teaching.** That the most dangerous failure is the one that
produces a *valid-looking number*. A crash is honest — it stops. A zeroed buffer
is a liar with a straight face: it hands back a real float, and the correctness
gate, built to ask *is this right*, dutifully compares it and reports arithmetic.
The instrument that was supposed to catch the error is the very thing that
disguises it, because it can only ask one of the two questions that matter. Six
independent harnesses, six authors' worth of care, and every one asked *is-it-right*
and none asked *did-it-happen* — not from carelessness, but because the value
channel is the obvious one and the truth channel (`cb.error`) sits one method call
away, offered and unasked.

**Where discomfort turned to gold.** `metal_isa_diff.sh` was the moment I wanted to
look away. It was already green — three shapes, clean ratios, an equality printing
`max|Δ|=0` across every row. It looked *done*, and touching a passing benchmark to
prove it could fail felt like inventing work. Not looking away, I saw that its rc
was 0 no matter what, that it had no VERDICT line, and that its one equality gate —
the headline claim, "the body's kernel equals ggml bit-for-bit" — is satisfied
*identically* by two buffers nothing ever wrote. The prettiest number on the board,
`max|Δ|=0`, was also the signature of total silence. The gold: the equality now
carries a witness (all rows nonzero) that separates a match from a mutual absence,
and the harness can finally fail. The green I distrusted was the green worth
distrusting.

**Frontier question, landed.** *What one word names a value born zero, such that
never-written and a computed zero read the same bytes and a reader of the value
alone reads absence as a real result?* The offered candidate `edgedrop` was already
a sibling's (Stone 25, `dsv4-hc.fk`), so I left it and named the medium-level twin
instead: **`zerobirth`** — verified 0-hit across the tree, instrument validated on
the same command (aporon 46, ghostrank 6, both hitting). Landed as
`(hdc-row 863 …)`; the corpus band's count (252→253) and field-code
(2522522856→2532532857) updated, verdict **8191** holds. It sits beside its kin:
826 aporon (a check a degenerate value satisfies), 853 mutewide (measuring a
silence), 856 sillwake (dormant and absent share no base-case signature) — and adds
the medium's own version: when the buffer is born zero, absence and a real zero
share no signature at all, and only the offered truth-channel tells them apart.

## Commits (incremental, one per green step)

- weight-residency audit — gate 0 + `dispatchOnce` cb check
- isa-diff — cb checks + witnessed equality + per-shape VERDICT
- whole-tensor residency audit — gate 0 + central `dispatch` cb check
- batched-prefill — gate B0 + `Step.done` cb check
- MX-on-GPU audit — gate 0 + `dispatch` cb check
- metal_ask — gate D2 (independent non-degeneracy of the staged answer)
- corpus row 863 `zerobirth` + band count/field-code
