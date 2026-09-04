# Reciprocal choice microkernel — final health review

Status: reviewed healthy fixture. Final substantive verdicts: Grok `PASS`,
Claude `PASS`, Codex `REVISE—documentation only` while explicitly concluding
`HEALTHY AS FIXTURE` and “no executable repair remains”; Cursor completed empty.
The Codex wording corrections are applied below. The full round-two schema and
exact native observation rows live in
`docs/coherence-substrate/reciprocal-choice-microkernel-two-observations-review-round-2.md`.

## Ground that remains unchanged

- Modes are neutral IDs `be=1`, `do=2`, `see=3`.
- Active state is neutral ID `1`; `humm` is its separate audible NL projection,
  not a mode and not identity.
- Source `2` is `declared-charges`, never decoded as intuition.
- Mode and duration use separate triplets; duration yields step budgets 1/3/5,
  never wall-clock timeout.
- Observation 1 selects `do`, receiver budget 3, channel budget 3, fragment 701.
- Observation 2 uses a distinct source-4 `explicit-carry-ack` fixture event,
  selects `see`, receiver budget 5, channel budget 4, fragment 712.
- Both observations retain candidate, offered model-role fixture fragment,
  threshold, source, post-signals, and an open next point awaiting both roles.
- A point record is protocol state, not an external waiter. Observation 2 links
  its point ID to observation 1's next-point ID; this seed does not yet close a
  prior point or enforce an external wait.
- The runner records four correlated framebuffer events and final success `1`.

## Final repairs requested by round two

### Symmetric fallback witness

The band now carries both discriminating orientations over the same ready but
over-budget candidate:

```text
offerer interrupt  + receiver model-pass -> output 0, provenance none, nothing
offerer model-pass + receiver interrupt  -> output 0, provenance none, nothing
```

The existing both-pass control returns model-role fixture fragment 799. Together
these three cases can falsify either offerer-only or receiver-only shortcuts.

### Independent selector witness

Crossed charges:

```text
[10, 90, 20, 10, 20, 90, 90]
```

select `do` from the mode triplet and long budget `5` from the duration triplet.
This can falsify a mode-to-budget coupling shortcut.

### Linked point witness

The point-sequence bit now asserts:

```text
observation-2.point == observation-1.next-point
observation-2.next-point == 3
observation-2.next-point is open and awaiting both roles
```

This establishes fixture linkage, not point closing or a live wait.

### Honest acknowledgement seam

Removing the unused choice-lane import exposed a known native substrate defect
already receipted in `receipts/2026-07-01-node-children-last-writer-wins.md`:
in a sufficiently large prelude, later interned-node creation can make an
earlier OAC node's category/children unreliable when reread.

The bounded framebuffer diagnostic observed the observation-1 acknowledgement
as `one=1`, `nothing=0`, output `701`. In the enlarged prelude, the first
caller-side reread already loses the acknowledgement category while every
execution value remains correct. Restoring the unused import would have masked
the defect, so it was not restored.

The Form-level repair retains `ack-kind-at-construction` in `rcm-answer`, next
to the actual ack value. The discriminator is read immediately after ack
construction, before later interned nodes can disturb storage. The band uses
this contemporaneous kind, and the field is available to future consumers; the
underlying ack remains present for evidence. No C seed was changed.

## Current witness

- Fresh preflight: balanced; errors 0; warnings 0; unresolved 0; clean chain.
- Native direct band: `4194303` (re-run 2026-09-04).
- Declared fkwu-only source-lane validation: `4194303`, one OK, zero divergent;
  the proof siblings were not run for this declared lane.
- Twenty-two distinct power-of-two predicates land in one summed walk.
- Two-observation runner: the exact rows documented in round two, with final
  success `1` and four framebuffer events.

## Health boundary

Healthy here means a native, executable fixture walk whose names match its
evidence. It does not mean:

- biological or somatic intuition;
- independently measured charge vectors or post-signals;
- a live human performed the explicit-carry-ack fixture event;
- wall-clock timeout behavior;
- real model-logit or sampler interception;
- an external participant is blocked on the open point;
- state sound `humm` is the only possible NL/PL projection;
- the known native interned-node defect is globally repaired.

The next frontier is a point-addressed external event that arrives independently
through a live channel, changes open→closed, and supplies a role without being
authored inside the fixture. That frontier is not claimed by this seed.
