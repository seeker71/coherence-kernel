# The hot policy host now sees the live model context without owning it

## Crossing

The previous live peer used an empty `fcrt-shared` value for its JIT policy
host while the real Qwen/KV session lived separately in `fcpct-state`. That
made route selection hot, but did not make its relation to the real residence
explicit.

`fcrt-policy-host-from-session` now creates a policy-only host from a live
`fcms` session. It shares model path, source, context, and position bound by
reference, creates no `q38` context or state, loads no tokenizer, and enqueues
no model row. The session remains the only owner and continues to advance and
release its own KV state. The live Form peer creates its ingress policy host
through that constructor and announces `policy-shares-model-context=1` at a
future resident's startup.

The existing earlier resident remains a separate process image and cannot
report this new line. It retains its warm Qwen context untouched. This crossing
eliminates duplicate/empty context in every successor from the current source;
it does not claim a cross-process KV migration protocol that does not exist.

## Evidence

```
./fkwu bootstrap/ground.fk                                  -> 42
./fkwu form/form-stdlib/tests/binary-freshness-band.fk      -> 31
./fkwu observe/preflight-run.fk                             -> clean (3 changed cells)
cd form && ./validate.sh form-stdlib/tests/form-cli-peer-policy-route-band.fk
                                                             -> 4095, fkwu-only
cd form && ./validate.sh form-stdlib/tests/form-cli-peer-contribution-turnwheel-band.fk
                                                             -> 1048575, four-way
```

The surprise is that hot policy selection had already been a true JIT route,
but its model relation was only a promise in prose. The discomfort was the
temptation to let a second wheel own the same KV state. It became a smaller,
healthier seam: share immutable context identity, leave state ownership where
the live session already holds it.
