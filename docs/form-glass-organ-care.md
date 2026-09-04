# Glass organ care

Run the native Glass care reading directly through the body:

```sh
./fkwu observe/form-glass-organ-care-run.fk
```

It reads the declared local/offline health census and emits one short report,
plus one source-attributed framebuffer event for each census member.  The
current shape is intentionally human-readable:

```text
glass-organ-care
scope declared-local-offline-census
coverage complete
organs 62
healthy 48
asking 14
attention-received 0
attention-applied 0
unobserved 0
all-held 0
next concept-digest-evidence-and-bucket-image
```

`scope` matters.  This is an exact statement about the live declared census,
not a claim that every `.fk` or `.bml` file is a healthy organ.  Glass permits
`all-held 1` only when coverage is complete and every census member is either
freshly `healthy`, has an exact `attention-received` receipt, or has an exact
`attention-applied` receipt.  Quiet and missing readings remain `unobserved`.

An attention receipt is a pure Form value carrying five matching facts:
organ identity, its evidence identity, its requested move, the receipt stage
(`received` or `applied`), and a receipt identity.  It matches only the same
organ/evidence/move triple.  It says a request was received or applied; it
does **not** say the organ healed.  Only a new positive organ reading restores
`healthy`.

The reusable authority is
`form/form-stdlib/bml/form-glass-organ-care.bml`.  A cell that already carries
its own Form values calls `fgoc-attention` and passes the returned value into
`fgoc-status` or `fgoc-all-held?`; it never opens a host process, file, socket,
or network path.  The runner deliberately supplies no attention receipts, so
it cannot fabricate care that has not occurred.

The current census adapter is `observe/form-local-offline-health-pulse.fk`.
To widen the word “all,” add an organ to that explicit census with its source,
evidence, requested move, and fresh health reading; do not infer membership
from source-file presence.
