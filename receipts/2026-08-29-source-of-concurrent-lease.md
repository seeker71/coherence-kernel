# Source discovery no longer shares one mutable shard file

## Crossing

`source-of.fk` is the Form organ that discovers definitions from the source
tree instead of consulting a held function table. Its scan is necessarily a
host membrane crossing today because the kernel's `host-exec` stdout has a
bounded payload; the full rows therefore travel through a file and the
completion token travels through stdout.

That file was formerly the fixed path `/tmp/sof-shard.txt`. Two independent
residents could overwrite it between scan and read, then each could structure
the other's source evidence. The organ now asks `mktemp` for a one-use
`/tmp/sof-shard.*` lease. Form accepts a completion only when it begins at the
exact `ZDONE:` boundary, stays under that lease prefix, and contains only the
conservative path alphabet. It reads the exact leased file once and releases
that exact lease before returning. A caller still supplies only source
directories; it cannot nominate the read or release path.

This removes the shared mutable seat without turning the computed index into a
curated map. The result of lookup remains one source, `choice` with all births,
or `nothing`.

## Evidence

```
./fkwu bootstrap/ground.fk                              -> 42
./fkwu form/form-stdlib/tests/binary-freshness-band.fk  -> 31
preflight source-of.fk                                  -> clean
preflight source-of-band.fk                             -> clean
./fkwu form/form-stdlib/tests/source-of-band.fk         -> 127
two concurrent source-of bands                          -> 127, 127; both exit 0
find /tmp -name 'sof-shard.*' after both complete       -> 0 active leases
```

The full tree scan is still a seconds-scale cold observation, so this receipt
does not call it a hot per-token operation. The next composition is to retain
an epoch-bound computed index inside a future resident and give it a direct
source-symbol action: then a request can discover a definition and its
plurality in milliseconds without a rescan, while an epoch change deliberately
creates a new observation.

The surprising teaching is that `choice` has a physical side: preserving
multiple definition births also requires each concurrent observer to retain
its own evidence. The discomfort was the convenient global temporary file; it
became a small, observable lease boundary instead of a hidden race.
