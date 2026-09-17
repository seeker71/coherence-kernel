# Unchanged edits carry their actual result

Signed: Codex. This follows the native value rehearsal receipt and observes
the actual Qwen continuation separately from the native replay.

## A feedback defect found in real execution

The local continuation received the new `rehearse-bml` tool and an explicit
instruction to use it on the retained failing candidate, then finish through
the original checks. It received no repaired expression. It instead read
the candidate and submitted the same whole-function replacement three times.
Each replacement's old and new text was byte-identical. The native edit tool
returned `edited` for each, even though no source changed. The existing
three-repeat guard eventually moved the workflow into repair.

The edit boundary now returns a failed observation, `edit-unchanged`, for a
unique matching replacement whose old and new text are identical. The
document stays byte-identical and the existing policy enters repair on the
first call. Missing and ambiguous patterns retain their more specific
diagnostics. Actual edits still use the existing guarded replacement.

Fresh preflight and the existing coding policy band pass **65535**, including
the new immediate-repair, source-preservation, tool/check-count and diagnostic
precedence assertions. The existing resident tool band also passes **65535**.
Drift gates pass **8191**, exit 0. No new runtime or model is involved.

A native private audit replays the three actual retained model replies
independently through the repaired policy: **3** unchanged sources,
**3** immediate repair transitions, **0** checker invocations. This establishes
the feedback repair, not how Qwen will respond to that feedback. Evidence:
`.hearth/response-parity/bml-native-unchanged-edit-audit.bml` and replies
`53479-1789626610693.txt`, `53479-1789626725040.txt`,
`53479-1789626845600.txt` under `.hearth/code-memory/replies/`.
The audit's first preflight refused a misnamed memory prelude. The path was
corrected to the existing `form-cli-code-memory.bml`; fresh preflight and
execution then passed **1**, exit 0.

The already-running model admission retained its earlier compiled policy.
Its outcome is not attributed to this later feedback change. Its next reply
correctly diagnosed the missing terminal reversal but chose a controller
transition that the repeated-failure state did not accept. Correct prose
diagnosis still had not become a useful action at that observation.

## Instruments and continuation

Counsel reports orphans **0**, with **11/12** lanes unobserved because no
hearth stands. The native guide reports Python implementations **0**,
invocation candidates **2**, unread files **0**. The share reading remains
declared/unmeasured; the completed-turn cursor has not fully traversed its
evidence and the percentage is withheld.

The useful surprise is how a false success word can support repetition even
when the source never changes. The concrete movement is to make that boundary
tell the truth immediately.

## Completed local admission

The admission completed in **674,474 ms**, generated **1,373** token IDs and
injected **2,004** observation token IDs. It made **4** tool calls: one read
and three unchanged edits. Three further replies repeated the correct
diagnosis with `next=implement`, while the controller required replanning
after the repeated failure. It reached the cumulative turn limit **28** at
attention, with **0** new checks, **0** source changes and **0** calls to the
new rehearsal tool. It released the model successfully. There were **0**
provider subprocesses and **0** automatic learning operations in this run.

The final source digest remains
`d3f5b99041813e49e8de041781bcad256f37705e0e9d331b902aa80200bf91ee`.
The private run root is
`.hearth/response-parity/bml-native-value-model-v1`. Its audit records zero
check snapshots and zero behavior passes; an empty audit is not a semantic
success. The source, state, bootstrap and all seven actual replies are
retained. This admission used the base local Qwen model, context **12,288**,
reply allowance **1,536**, and no adapter. It preserved the original goal,
original document history and writable boundary.

Native assistance therefore solved this retained case when invoked directly,
but simply making the tool available and requesting its use did not make
Qwen use it. The next adoption attempt can start from this observed boundary
and the repaired unchanged-edit feedback. Native invocation and model choice
remain separately attributed. No overall response-quality, resonance or
session-efficiency parity is claimed.

After Qwen released, the generic verified procedure was retained through
`form-cli-session-home-embody-run.fk`, event
`2026-09-17-native-value-search-drain-unchanged-edit-v1`, session
`native-arrival-bootstrap`. The worker launched; that establishes retained
teaching, not a completed weight update or a Qwen improvement. The utility's
desired repair and evaluated replies were excluded from the teaching.

The parent output meter reads **2,384,033 cumulative tokens**; the goal meter
reads **11,557,907 cumulative tokens**. They count different boundaries and
are not isolated native-run costs. The rented coordination remains far too
expensive for the stated objective. The native zero-token repair establishes
one useful capability; the overall efficiency objective stays open.
