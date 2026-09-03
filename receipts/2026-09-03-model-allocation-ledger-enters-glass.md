# 2026-09-03 — model allocation ledger trust boundary enters Glass

The next resident-model-owner image now has a typed allocation ledger from the
physical construction path to Glass.  The current resident, PID 18249, was not
restarted, reopened, released, or copied while this was observed.  Its older
image does not publish the open-time artifact identity required to join a
layout profile to a physical owner, so current Glass withholds every exact
profile-derived extent rather than turning catalog agreement into residency.

## What became measured

The GGUF walk used by `metal_buf_from_file` observed the exact mapped record
extents for the two admitted artifacts:

- Qwen3.8-Flash-Next UD-Q2_K_XL (Unsloth): 78,858,104,320 mapped bytes in
  1,224 Metal no-copy handles; 124,435,036 reusable allocated bytes in 145
  handles.
- Llama-3.2-3B-Instruct: 2,334,744,832 mapped bytes in 256 Metal no-copy
  handles; 30,102,560 reusable allocated bytes in 73 handles.
- Owner aggregate: 81,192,849,152 mapped bytes in 1,480 handles and
  154,537,596 allocated bytes in 218 handles; 1,698 total buffer handles.

These are exact profile measurements of the artifacts, not yet a claim about
the current resident owner.  The profile records its catalog extent and a
size+mtime stat identity.  The updated owner source captures that stat once,
immediately after each successful open, and carries the immutable samples
through its publish loop; it does not restat the path on each cadence.  But a
size+mtime pair is collision-prone and cannot prove content identity after a
same-size/same-mtime replacement.  Glass therefore still withholds exact
profile extents until the same owner and handle publish a separate
physical-live identity sample.  That sample must carry `artifact-open-inode`
or `artifact-open-content-digest` as its strength, carry the exact identity
value in its purpose field, parent the corresponding open-stat artifact, and
match its handle and open epoch.  Glass compares all of those carried values;
an inode/content-digest label written only into a profile is never evidence.
The current carrier exposes neither inode nor digest, emits no strong sample,
and cannot reach exact fallback admission.  Catalog mismatch remains the exact
`model.tensor-layout-profile-refresh` door; a matching stat without stronger
identity becomes `open-time-content-identity-not-published`.

For the current older owner there is no open-stat sample at all.  The current
witness consequently carries exactly two rows: `tensor.owner.artifact-join`
with absent bytes/handles and purpose
`open-time-artifact-identity-not-published`, plus
`mlx.owner.model-path` with absent bytes/handles and purpose
`not-on-owner-model-path`.  No zero stands in for either absent measure.  The
future owner's direct physical tensor-group samples remain independently
observable without a profile fallback; mapped storage is typed `disk/mapped`
and reusable Metal storage `unified/resident`.

## Witnesses

```
./fkwu form/form-stdlib/tests/native-model-tensor-ledger-band.fk  # 65535
./fkwu form/form-stdlib/tests/form-glass-live-band.fk             # 1073741823
./fkwu form/form-stdlib/tests/native-model-dual-telemetry-band.fk # 67108863
./fkwu observe/native-model-allocation-glass-current-run.fk      # 1
```

An earlier draft diagnostic selected movement code 5103 through a framebuffer
control envelope but did not actuate the source change it named.  Review
removed that simulated evidence cell entirely.  The executable bands and the
bounded physical current-owner witness are the retained evidence.  That Glass
witness reports `rows=2`,
`exact-profile-withheld=1`, resident owner PID 18249, and the exact missing
open-time identity door above.  The latest observation's ages were 298,644 ms,
not synthetic zero.  The Glass band also adversarially relabels every stat-only
profile identity as `content-digest`: without carried strong samples the exact
ledger remains absent; with a deliberately wrong carried digest value, Glass
returns `open-time-content-identity-mismatch`.  The bounded atlas panel read
`TICK #0 ev=90 nodes=212 cons=13K`; it did not promote the 14 profile rows into
the old owner.

Signed, Codex — allocation-ledger sibling, this worktree.

; witnessed: 2026-09-03 -> exact profile measured; old owner profile extents withheld at open-time-artifact-identity-not-published; PID 18249 retained
