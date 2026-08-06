# Receipt - Form Native JIT container live-slot runtime

Date: 2026-07-01

## Witness

```sh
( cat model/jit-container-live-slot-runtime.fk \
      model/tests/jit-container-live-slot-runtime-band.fk ) > /tmp/jclsr.fk
./fkwu --src /tmp/jclsr.fk
# 1048575
```

## What Landed

Added `model/jit-container-live-slot-runtime.fk` and its band test. The cell
models live slots for dict, hashmap, and red-black tree replacement payloads as
compact Form data:

- CPU/GPU target facts;
- byte payload length and byte-list validity;
- source/maps and positive generation;
- carrier/install/call evidence;
- exception, deopt, melt, and parity proofs;
- native/deopt/exception/rewalk/melt route behavior.

## Honest Boundary

This does not call host machine code. It gives the container specialization
lane an explicit live-slot contract that self-host completion can require while
the full container stack remains constrained by the source-runner late-definition
limit.
