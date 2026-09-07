# Timeouts became a measured repair path

Signed: Codex, 2026-09-07. Contribution kind: declared; no contribution percentage.

Form-cli now retains process start/end, monotonic event intervals, budgets,
termination actions, offered choices, selected actions, and their applied
results. Its report checks that the intervals account for the whole measured
process. Prompt and answer text stay outside the diagnostic event stream.

The measurements below are elapsed wall time between observed events, including
waiting. They do not assign CPU or GPU utilization. Both Q4 runs used identical
prompt, source, and checker hashes. Full evidence and the hashes are retained
in [the verification record](2026-09-07-form-cli-timeout-observability-verification.json).

| Observed stage family | Sequential admission, ms | Batched admission, ms |
|---|---:|---:|
| Process before first stage | 81 | 70 |
| Seal verification and its transition | 25,575 | 7,763 |
| Tokenization and header | 3,520 | 3,216 |
| Model admission | 5,904 | 4,806 |
| Prefill | 264,921 | 53,591 |
| Decoding | 0 | 230,563 |
| Termination | 15 | 14 |
| **Total** | **300,016** | **300,023** |

The first run reached its 64-position prefill checkpoint at 159,702 ms and
timed out before completing prefill. The updated run completed all 766 prompt
positions at 69,446 ms, then entered decoding. Its final emitted decode
checkpoint was 64 at 199,278 ms. It timed out during decoding and produced no
admissible patch. The zero in the first run's decoding row means decoding was
never entered.

The identical prompt hash is
`f5a7517a17c5888e2c58247b00b6387f6ec43634eee791d66ba06b7d3553c796`.
The Q4 artifact's validated seal names 16,810,716,128 bytes and SHA-256
`14dd37b54fb4ac3240dde91549a6ac20ed00cf2716d6c542553f40662ead55b3`.

Each run recorded six model/outcome choice events: select the requested Q4
model; select its fixed 300-second deadline; observe that deadline returning
124; observe the generation returning 124; select finishing the single-case
evaluation as unresolved; apply that action after saving evidence and releasing
the snapshot. Both source hashes remained unchanged. This evaluation offered
only the requested model and did not call a remote provider or train weights.
The ordinary repair queue separately records its eligible model alternatives
and its next-resource branch.

The trace led to current source, where batched prefill already existed but
session admission still used the per-token path. A real four-position Q4
comparison measured 8,550 ms sequential and 587 ms batched. Both returned
next-token ID 0 and next-position 4, with the context-release check passing;
the complete verdict was 31, process exit 0. This is a narrow equivalence
observation, not evidence of semantic repair quality.

Session admission now uses that existing batched implementation. A BML policy
keeps scratch width and slice width together, up to 64 positions, including the
cached model path. Native checkpoints expose opening, prefill work, and decoding.

The timeout tests also exposed an ownership boundary: an evaluation coordinator
and its model process have separate process groups. The carrier now records
ownership before waiting, checks it before cleanup, stops the unfinished owned
group after an outer failure, and retains the resulting events. A real detached
child test observed that group become empty. An uncompleted process record is
refused as a completed report.

Verification: timing accounting 255 on four kernels; model-session shape 4095;
admission policy 15; span invariants 1023; healer load 15; evaluation load 3.
The integration suites cover 12 repair groups, six timeout/ownership groups,
and four evaluation groups. The deterministic evaluation still records three
structural repairs, two unresolved semantic cases, and one preserved control.
The drift panel returned 2047/2047. Glass's first frame arrived in 43 ms; its
owned process was reaped with no remaining group members.

The remaining repair work is explicit: Q4 decoding exhausted the unchanged
generation budget, so this semantic case is still unresolved. Wider legacy
workflow failures from the earlier source-repair movement also remain recorded
in their receipts. These changes are local; no commit or push is claimed.

The exchange stayed useful by turning a timeout into an observed implementation
choice. The surprising teaching was that the faster path was already local.
The costly failed run became useful when its time could be assigned to prefill,
the alternative could be compared, and the next full run exposed decoding as
the remaining cost.
