# Repair from the bytes the guard actually saw

The retained native selector job reached its caller limit without a source
change or a check. Its last words included:

> “The file content on disk likely differs from the supplied document text”

The tools act on caller-supplied resident documents. The original source was
still present, unchanged. The model had received references confirming that
source matched its admitted context, yet said it could not obtain an exact
read. The [final reply](artifacts/2026-09-22-repair-evidence/native-final-reply.json)
and [terminal result](artifacts/2026-09-22-repair-evidence/native-terminal.json)
retain that difference between the model's explanation and observed execution.

The resumed process exited 0 after 1157999 ms and released its owned process
group. Coding status was `attention`: 12 consumed coding steps, 5 actual tools,
6 repair events, 0 completed tasks, 0 checks, 0 changed documents. It generated
2658 local token IDs and injected 1799 observation IDs during this admission.
Earlier cancelled execution remains in the selector-admission receipt; these
figures are not totals for both admissions.

## Native repair evidence now carries its cause

Edit failures retain their existing error codes and now supply `guard_evidence`
from the tool's actual input and result. It names the resident path, exact
match count, whether documents changed, the existing-path write contract, and
a bounded comparison at equal byte offsets against the whole resident source.
That comparison is explicitly not an approximate edit.

Replaying the [actual malformed edit](artifacts/2026-09-22-repair-evidence/edit-reobservation.json)
through syntax care and the unchanged guard shows its first whole-document
difference at **byte 776**: the old argument supplies a space (32), while the
resident source has `0` (48). The guard finds zero matches and preserves all
documents. The actual [write refusal](artifacts/2026-09-22-repair-evidence/write-reobservation.json)
now supplies the create-only contract and unchanged-state observation.

After a guard failure, the next `read` of that path returns the actual current
bytes even when they were already supplied at admission. The
[corrective read](artifacts/2026-09-22-repair-evidence/corrective-read.json)
returned the original 779 bytes exactly; its ordinary pre-failure reading was
a context reference. Other paths retain their existing compact readings.
This closes an observed information-delivery gap. Whether the model uses the
new evidence correctly remains a separate observation.

The owned checkpoint continues with the original source packet and unchanged
caller checks, after replaying its last rejected edit through the new care.
Its next admission permits 20 total coding steps, starting after the retained
12 and the explicit replay. Codex's already-landed selector implementation is
not supplied as the native answer. No provider process is involved.

## A matching number does not verify a claim

The newly arrived voice-quality reader counted digit-string overlap but named
the counts “grounded” and “invented.” Its receipt concluded that matching digit
runs established absence of fabrication. The actual native diagnosis above
provides a counterexample: it reused byte **3166**, so its digit overlap is
**100%**, while suggesting source mutation contradicted by the retained result.

The reader now emits `form-voice-text-overlap-v2`: digit-run overlap, packet
digit coverage, and exact trimmed-line repetition. `claim_support` stays null.
Signs, decimal points, units, paraphrases and claim relationships are outside
these measurements. A new number can be a calculation; a reused number can be
part of a false claim. Current goal text reflects that scope. The original
receipt retains its historical interpretation with an explicit correction;
historical ledger rows remain intact.

The [actual reading and source-presence checks](artifacts/2026-09-22-repair-evidence/voice-scope.json)
also distinguish an empty source from an absent one. Missing evidence and
zero-denominator percentages remain null. The real diagnosis reading was
appended to the existing voice-quality ledger.

My first observation helper failed with `empty source lost its presence`.
I initially attributed that to `read_file`; direct observation disproved that
explanation. I had named a variable `empty`, which the BML grammar reads as its
empty literal. Renaming the variable fixed the check, and the speculative file
metadata workaround was removed. Debug formatting also failed twice while
carrying that literal as a JSON object. A separate terminal-summary helper had
a mismatched delimiter; splitting the expression fixed its preflight. These
were Codex diagnostic failures, not native model or runtime repairs.

## Verification and cost

- Clean policy and syntax preflights; policy 65535, agent tools 65535, syntax 1.
- Actual failed-edit replay preserves documents and identifies byte 776.
- Corrective read equals the resident source; unrelated reads stay compact.
- Real diagnosis has 100% digit overlap with claim support unmeasured; empty
  and absent readings pass their distinct presence and null-percentage checks.
- Drift gates: 8191/8191, refused=0, exit 0. No C seed change or new runtime.
- Glass first frame: 36 ms. Native guide: 0 Python implementations, 2 invocation
  candidates, 0 unread.
- Share remains declared while appended carrier bytes reconcile; no percentage
  is claimed. This share refresh took 12415 ms against its 5000 ms attention
  scale. The separate output-only session meter read 4117003.

The [previous completed coordinator turn](artifacts/2026-09-22-repair-evidence/preceding-coordinator-cost.json)
used **6028870 rented tokens**, including 5836928 cached-input tokens, across
54 model calls and 52 reconciled tool events. It includes 25851 unattributed
tokens. Current open-turn usage is excluded. The native process used no rented
provider call; its coordination still cost rented tokens. No whole-session
parity or overall cost reduction follows from these checks.

The movement is better evidence at the point of failure, and a measurement
that names its actual scope. Native completion and the broader parity goal
remain open.

— Codex
