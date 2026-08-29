# Direct source policy action keeps the resident model untouched

## Crossing

`source-knowledge` is now a fixed Form adapter selected by the resident JIT
policy, rather than a Qwen-mediated peer branch. Its exact entry is
`fcpdsa-run(session, task-kind, task-text, source-ctx, call-id, provenance)`.
The caller creates `source-ctx` once in `fcpct-live-new` from the resolved body
root, source artifact, local-ready bit, and frame bound. Task bytes and policy
images cannot supply any of those values.

The action first refuses a wrong kind, malformed surface, or absent context.
Otherwise it performs exactly one strict source effect,
`fknqr-public-strict-executor(source-ctx, fcnkd-surface(task-text))`, passes the
returned result into the retained current-answer PIF, and admits the existing
source-world tool message only for a returned resident value of `1`. The input
session crosses unchanged. Its typed cursor carries raw strict result, PIF
receipt, and optional `ap-msg`; the peer ABI keeps the integer resident value
beside that cursor.

`fcpct-run-head-peer` chooses this branch before `fcpa-observe-task`. The legacy
generic source branch now refuses without its caller-held source context rather
than opening the former model-mediated source path.

## Durable observation

The normal single `file_append_bytes` commit now holds, in order, the policy
selection frame, a content-free `<|form:source-world|>` terminal, and the
ordinary contribution frame. The terminal contains task/call/provenance,
source/request coordinates, path and current source identity, counts and PIF
phases, but never source or answer bytes. A failed append retains the exact
typed stage; only a successful append marks the task seen. There is no second
resident world store: the structured message stays in the staged cursor and the
atomic terminal is the durable reconstruction material.

The separate bootstrap-seed repair and its >128 source-closure witness are
recorded in [`2026-08-29-source-loader-dependency-growth.md`](2026-08-29-source-loader-dependency-growth.md).

## Evidence

```
./fkwu bootstrap/ground.fk                                      -> 42
./fkwu form/form-stdlib/tests/binary-freshness-band.fk          -> 31
cd form && ./validate.sh form-stdlib/tests/form-cli-peer-direct-source-action-band.fk
                                                                  -> 255, four-way
cd form && ./validate.sh form-stdlib/tests/form-cli-peer-contribution-turnwheel-band.fk
                                                                  -> 1048575, four-way
cd form && ./validate.sh form-stdlib/tests/form-cli-peer-policy-route-band.fk
                                                                  -> 1023, fkwu-only JIT lane
./fkwu form/form-stdlib/form-cli-peer-direct-source-action-live-run.fk
                                                                  -> value / lookup=1 /
                                                                     PIF value=1 /
                                                                     world=1 /
                                                                     mutation=0
```

The surprising part was that the direct PIF/world result already fits the peer
ABI's existing integer `value` seat; only the structured world message needed
to remain beside it. The discomfort was the apparently small 128-file C
collector: it became a visible native source-door limit as soon as the Form
action owned the full retained-answer closure. I kept the crossing alive by
removing that limit and making the source action, its refusal, and its durable
admission independently observable.
