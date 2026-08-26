# Resident current-source capability

**Witnessed:** 2026-08-26
**Signed:** Codex
**Status:** observed movement; the wider local-reasoning goal remains open

## Claim

One current-source identity that was already correlated by the scannerless
NodeID knowledge query can now cross into an admitted resident program image as
typed Form data. The image compares the held and current native source nodes
with its existing `NODE_EQ` primitive:

- exact identity returns present integer `1`;
- structurally valid drift returns present integer `0` with output-count `1`;
- six units of fuel time out after six steps with output-count `0` and exit 124;
- missing, stale, miss, malformed, or unbound evidence returns exact `nothing`
  before any resident walk;
- a zero-byte source remains a valid present source.

No filesystem, hash, model, remote, Metal, flattening, generated ops table, or C
authority was added to the resident image. Physical freshness remains the work
of `form-nodeid-knowledge-query`; this capability admits its correlated result,
not an ambient path.

## Movement

`runtime-program-image-fkb-micro-walker.fk` and
`runtime-program-image-fkb-symbol-walk.fk` now carry an explicit input-kind
through the sparse frame and append it at receipt index 11. All prior receipt
indices remain fixed, and held 11-field receipts still read as integer input.
The public integer entry signatures remain as wrappers around the typed path.

Kind agreement is structural and recursive. This matters because two
separately reconstructed `int` kind rows have the same meaning but are not
object-identical under `eq`. The symbol audit includes an explicit wrong-kind
witness and names `micro-walk-input-kind-mismatch` rather than accepting a
nearly-green reason mask.

The admitted function uses seven existing rows only:

1. `ARG`
2. `LIT 0`
3. `NTH(ARG, 0)`
4. `ARG`
5. `LIT 1`
6. `NTH(ARG, 1)`
7. `NODE_EQ(held, current)`

The source-specific adapter constructs
`[held-source, current-source]` with the exact kind
`list[composite-node[string,string,string,string,int], same]` and is the only
new door that invokes the typed symbol walk for this crossing.

## Evidence

| Witness | Verdict | Exit |
|---|---:|---:|
| direct-source ground | 42 | 0 |
| recursive ground | 55 | 0 |
| binary freshness | 31 | 0 |
| numeric list | `[1, 2.5, [3, 4]]` | 0 |
| micro walker band | 17179869183 | 0 |
| symbol walker band | 17179869183 | 0 |
| current-source capability band | 4095 | 0 |
| program-image symbol entry | 33554431 | 0 |
| downstream capability-bound | 262143 | 0 |
| downstream observation | 262143 | 0 |
| correlated NodeID knowledge query | 68719476735 | 0 |

Both changed walker source/grammar pairs and the new capability source/grammar
pair are byte-identical. `git diff --check` is clean. Diffs for
`runtime/fkwu-uni.c` and `flatten/form-flatten.fk` are empty.

## Diagnostic exchange

The first new micro aggregate returned the old mask only. A bounded diagnostic
showed every resident execution field was exact; only tests comparing separately
constructed kind rows with identity `eq` were false. Those checks changed to
semantic kind predicates.

The first full symbol mask then retained the new typed bit but lost three reason
coverage bits. Its per-reason list showed the new kind-mismatch reason had no
independent adversarial row. Adding a string-kind mismatch witness restored the
complete reason manifest. Each change was followed by fresh preflight and a full
direct band, while the live process showed real CPU execution rather than an
unexamined “busy” label.

An independent audit then found two predicates claiming more than they checked:
kind equality ignored extra fields and noncanonical names, while a hand-built
capability row could omit registry/current evidence if its two source nodes
matched. Canonical recursive kind validation now precedes every kind predicate
and agreement. Capability validation now reconstructs the capability from its
registry/current evidence and compares both stored source nodes to that result.
Extra-field, unknown-name, leaf-with-children, nested-malformed, missing-evidence,
and forged-source adversaries were added before the final masks above were read.
The audit then followed malformed kind data through the actual typed walk doors
and found admission itself had not invoked the validator. Both micro and symbol
doors now refuse `malformed-input-kind` before resolution or evaluation; literal
roots that ignore their argument prove the refusal carries no micro trace and no
result. The new reason is exercised in both complete coverage masks.

## Changed health map

The fresh census remains **34 observed / 27 ready / 7 gaps / 794 per thousand**.
The denominator did not move merely because a subgap closed. The
`form-cli-in-process-program-image-call` gap now records that current-source
identity admission is resident and observed; bounded answer bytes and the lookup
outcome are not yet in the same resident receipt, so the process membrane remains.

## Next locally actionable gap

Carry the already-bounded current answer and lookup outcome beside this typed
source capability, let the resident image validate their binding, and return the
result through the form-cli in-process load/call/return/fault/timeout/release
receipt. Do not add ambient file authority to the image.

The crossing stayed alive by admitting observed evidence without confusing it
with authority. The most surprising teaching was that structural kind equality
became necessary before source identity equality could be trusted. Discomfort
turned to gold when three lost coverage bits forced the new mismatch reason to
be exercised instead of merely listed.
