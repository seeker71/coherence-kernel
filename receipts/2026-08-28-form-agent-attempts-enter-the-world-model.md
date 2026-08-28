# The Form agent's observed attempts enter the world model

**Witnessed:** 2026-08-28
**Signed:** Codex / Sol

The resident Form agent could propose, execute guarded work, and retain typed
execution trajectories.  The generic world model could hold physical,
language and runtime entities.  No cell joined those two surfaces: a candidate
could be discussed beside the world without a rule saying what evidence made
it admissible as world state.

## What crossed

`form/form-stdlib/form-agent-world-model.fk` now accepts an
`form-execution-trajectory-v1` only when every candidate/observation pair has:

- the expected schemas;
- the same family;
- a matching structural candidate identity;
- a supported signal; and
- coldjudge evidence that passes the existing independent execution check.

An empty trajectory, mismatched identity, unsupported signal, or unbound
observation returns `nothing`.  Candidate prose has no separate admission
door.

Each admitted step becomes an ordinary `wm-ent` with kind `agent-attempt`.
Its fixed six-component embedding carries observed outcome kind, surprise,
preflight state, compile exit, run exit and whether the signal carries a
value.  Its world position retains step index, expected value and observed
value.  Failed and repeated attempts are retained: a three-round plateau is
three observed world events, not absence and not success.

The projected world offers one pure guide back through the existing
`form-agent-protocol` tool-result shape:

- green -> `crystallize`;
- semantic failure -> `revise-with-value-contrast`;
- syntax failure -> `revise-with-diagnostic`;
- timeout -> `branch-or-reduce`;
- nothing -> `request-evidence`;
- failure -> `investigate`; and
- choice/cut/undo -> `reobserve-control`.

This guide does not execute or mutate.  It is the first bounded seam by which
the agent can receive its own execution-observed world state without treating
its generated candidate as fact.

## Exact witnesses

- Fresh preflight of `form-agent-world-model.fk`: balanced, zero errors,
  warnings and unresolved calls.
- Fresh preflight of `form-agent-world-model-band.fk`: balanced, zero errors,
  warnings and unresolved calls.
- `form-agent-world-model-band.fk` -> `8191`, exit `0`: thirteen claims,
  including wrong-value surprise, returned-nothing versus signal-nothing,
  syntax evidence, identity refusal, plateau retention and protocol re-entry.
- Focused `form/validate.sh` -> one declared runtime-fkwu band green at
  `8191`, `1 ok, 0 divergent` inside that declared proof level.
- `form-execution-verified-curriculum-band.fk` -> `1048575`.
- `world-model-band.fk` -> `8191`.
- `form-agent-protocol-band.fk` -> `31`.
- `form-agent-tree-band.fk` -> `31`.
- `active-inference-band.fk` -> `127`.
- `binary-freshness-band.fk` -> `31`.
- After building the absent minimal Go/Rust proof walkers,
  `proof/four-way-run-recipe42.fk` -> `0`.
- `git diff --check` -> exit `0`.

## The proof lane that did not get invented

The first sibling-kernel probe was deliberately registered as four-way.  It
diverged before reaching the new logic:

```text
go/rust: bp: unreviewed bootstrap name: FORM-EXECUTION-CANDIDATE
typescript: bp: unregistered blueprint name "FORM-EXECUTION-CANDIDATE"
fourth-src: 8191, exit 0, diagnostics 0
```

All three sibling errors prescribed
`python3 scripts/scan_form_blueprints.py register FORM-EXECUTION-CANDIDATE`.
That script is referenced by `form/user-blueprint-registry.md` but is absent
from this checkout; the attempted command exited `2` with file-not-found.  No
ontology coordinate was guessed by hand.  The provisional four-way manifest
row was removed, the band now declares `PROOF LEVEL: FOURTH-ARM ONLY`, and the
focused validator witnesses that exact lane.

The next proof movement is to restore a Form-owned reviewed blueprint
registration path (or remove the dependency on the unreviewed bootstrap name),
register the execution candidate and trajectory shapes, and re-probe all four
arms.  The next live movement is to inject `fawm-agent-message` into the
resident contribution turnwheel and witness a physical
wrong-value -> value-contrast -> green transition in the same KV residence.

## What the diagnostic exchange taught

The first draft ended with two open forms.  `tree-heal` tried and correctly
left the file byte-identical because its safe search could not place a closer
that the kernel accepted.  The two missing closers were inside nine-branch
decision folds, the healer's documented blind spot.  After that repair, the
band stalled on `(do (nothing))`; content-free stage markers located the exact
fixture, and the existing curriculum showed the native spelling is
`(do nothing)`.  The markers were removed and the band completed in under a
second.

I kept the exchange alive by making the agent's failed attempt part of the
world rather than something hidden on the way to green.  The most surprising
teaching was that a plateau is not three copies of no progress; it is three
separate observations of a model that did not bend.  Discomfort turned to gold
when fkwu's green `8191` met three registry refusals: the mismatch prevented a
four-way claim whose registration door does not exist here.

; witnessed: 2026-08-28 -> preflight clean, fkwu band 8191, focused lane 1/0, regressions green, four-way baseline 0
