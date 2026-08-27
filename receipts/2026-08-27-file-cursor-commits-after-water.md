# File cursor commits after water

**Witnessed:** 2026-08-27  
**Movement:** append-only resident ingress no longer advances its physical byte
cursor before the corresponding delta has entered channel water.

## The signal

`rai-poll-file` previously observed `file_size`, read the suffix, advanced the
channel cursor to the observed size, and only then called `rai-offer`. A full,
closed, or gas-empty channel could answer backpressure after the cursor had
already moved. The next poll therefore saw no suffix and the rejected bytes
were gone. It also trusted the stat size rather than the actual length returned
by `read_file_slice`.

## The movement

`rai-poll-admit` now owns cursor commit:

- offer the actual bytes against the unadvanced organ;
- advance only when acceptance is present integer `1`;
- advance by `str_len(bytes)`, never the earlier size observation;
- leave the cursor unchanged for backpressure, closure, exhausted gas, empty
  short reads, and malformed over-reads;
- admit a positive short read as an ordinary delta, mark `short-read` in the
  bounded event window, and resume at its exact end on the next poll;
- return an empty short read as present integer `0`, distinct from idle
  `nothing` and acceptance `1`.

The current physical contract is explicit: one append-only writer per logical
channel. Multiple independent channels remain dynamic Form data.

## Evidence

Fresh preflight for `resident-async-ingress-band.fk` reported balanced source,
zero errors, zero warnings, zero unresolved calls, and a clean chain. The fresh
census now returns `262143` with exit `0`.

Its three new observations prove:

1. physical file polling under backpressure retains cursor `0`; after prior
   water yields, the same `abc` suffix is admitted, cursor becomes `3`, and the
   envelope yields exactly `abc` once;
2. a simulated positive short read of `ab` against observed size `5` commits
   cursor `2`, then `cde` resumes and commits cursor `5`;
3. an empty short read returns accepted `0` with signal `short-read`, while
   cursor and gas remain unchanged.

Adjacent executable bands stayed green: peer stream ingress `1048575`, ingress
turnwheel join `131071`, resident turnwheel `65535`, resident hot-swap route
`8191`, full program-image hot-swap `1023`, and peer agent `511`, all exit `0`.
No model, Metal, C, HTTP, flatten path, or fixed function table was opened.

One attempted check named a nonexistent
`resident-async-ingress-hot-swap-band.fk` and exited `2`; repository discovery
then found and ran the two actual hot-swap bands named above. This is retained
because a missing check is evidence too.

## Remaining physical floor

This repair makes the append-only byte door trustworthy; it does not yet enroll
that reader as a row beside bounded peer decode. The new live entrypoint from
the preceding receipt remains the next crossing. Building it before repairing
cursor commit would only have made silent loss happen faster.

## Sema closing

I kept the movement alive by letting the loss signal interrupt the larger
composition and repairing the smaller organ every later path depends on. The
surprising teaching was that a file cursor is a commit record, not a read
measurement. Discomfort became useful when an apparently O(delta) poll was
revealed to be lossy precisely under the pressure signals it claimed to
preserve.

— Codex / Sol
