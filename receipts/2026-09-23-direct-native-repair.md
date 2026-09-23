# Apply the native repair where it arrives

Codex, 2026-09-23. This advances native execution on the original response
enquiry. Qwen3.8-27B Q8 supplied a guarded replacement for caller-writable
`answer.md`. Form refused it solely because the current role was `repair`.
The next native reply diagnosed that role restriction and requested a return
to implementation. The saved action already contained the proposed work.

## Same action, same documents, same checks

The [native replay](artifacts/2026-09-23-direct-native-repair/repair-edit-reobserve.bml)
reconstructs the 515-word candidate and its failed caller check, then applies
the [exact native action](artifacts/2026-09-23-direct-native-repair/95004-1790144688822-1.txt).
This isolates the affected boundary; it does not replay the whole conversation
or change the still-running model's state. Request and action hashes bind both
readings.

| Observation | Before | After |
|---|---:|---:|
| Words | 515 | 492 |
| Applied document change | 0 | 1 |
| Native tool calls, including immediate verification | 0 | 2 |
| Automatic caller checks | 0 | 1 |
| Counted reply transitions | 1 | 1 |
| Required word range passed | 0 | 0 |

The [before](artifacts/2026-09-23-direct-native-repair/repair-edit-before.json)
and [after](artifacts/2026-09-23-direct-native-repair/repair-edit-after.json)
retain the refusal and new counterexample. The [492-word answer](artifacts/2026-09-23-direct-native-repair/repair-edit-after.json.answer.txt)
remains Qwen's text. It is still overlong and its claim that traceability changes
felt resonance remains an interpretation. This establishes a usable repair
boundary, not answer parity or completed review.

## What changed

Code-mode repair accepts `edit`, create-only `write`, and `repair-bml` on
caller-writable paths. A successful document change records the actual action
and prior failure, explicitly without inventing a model diagnosis. The original
source checker runs immediately. Failure returns to repair; success awaits
task completion, review and final verification. Failed or unchanged edits stay
failed. Review mode and non-writable documents retain their authority.

The checks exposed two defects in the first implementation. Internal movement
counted three model turns for one received action; the final path counts one.
Repeated verification compared source hashes as part of the counterexample,
allowing changed bytes with the same failed result to evade replanning. It now
compares the actual exit/stdout/stderr while retaining full source identities.
A repeated counterexample still requires a revised plan before further edits.

## Checks and adverse evidence

- `form-run ./fkwu form/form-stdlib/tests/form-cli-code-policy-band.fk` →
  **65535**, including direct repair, immediate checks, source guards, role
  authority, provenance, reply counting and repeated-failure routing.
- Repair band **262143**, request band **255**, session band **31**; exit 0.
- All four preflights and the complete source-backed CLI preflight clean.
- `form-run ./fkwu gate/drift-gates-run.bml` → **8191**, exit 0; no kernel change.
- `git diff --check` clean.

The helper's initial preflight failed with `UNBALANCED parens, depth -1`, then
an unresolved `read_stdin`; these were repaired with balanced syntax and the
existing `read_line` primitive. Empty-stdin preflight then exposed the absent
destination; a local default repaired it. Policy checks failed first at
`repair edit uses one model transition and still awaits completion and review`,
then at `failed repair retains changed candidate and repeated caller counterexample`.
Both failures drove the repairs above; the checks were preserved.

No additional model generation or provider call was used for this replay.
Codex authored the policy change; Qwen authored the replayed document change.
The preceding completed coordinator turn cost **5,163,518 tokens**, including
**4,940,928 cached input tokens**, as retained in the affine-grid receipt. This
open turn has no completed total. Its share reading remains declared, percentage
withheld while carrier append validation is incomplete. Cost parity is open.

Closing instruments: glass first frame **30 ms**; intentional interruption
after observation returned 130. Counsel reports **0 orphans** and **11/12 lanes
unobserved** with no standing hearth. The native guide reports 0 Python
implementations, 2 invocation candidates and 0 unread files. The verified
teaching is offered under session `native-arrival-bootstrap`, event
`2026-09-23-direct-native-repair`, retained as
`ea096be13751dec6c90788fbf40f81af00ea0e294620c7391f0a0f70faf7601a`;
its training and later use remain to be observed.
