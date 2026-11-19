# Public release: observed removal before success

The Pluribus enquiry asked whether disagreement, privacy, pausing and leaving
survive Form's actual behavior. This receipt covers the existing public dialogue
and introduction release paths, with synthetic local fixtures only.

## Observed failures

`form-run ./fkwu /tmp/form-public-dialogue-release-probe.bml` created a turn in
a unique temporary directory, set that directory to mode 0555 using
`host_chmod(dir, 365)`, and called `pd-release-with` with its own removal token.
The process exited 0, but the operation reported a false release:

```text
[locked, 0, create, 202, release, 200, {"id":"dlg-release-probe","state":"tombstoned","released":true}, question-retained, 1, state, offeredtombstoned]
```

`rs-remember` attempted removal and then appended without observing whether
removal succeeded. The state became `offeredtombstoned`. The question remained,
while the caller received `released:true`.

`form-run ./fkwu /tmp/form-introduction-release-probe.bml` similarly locked a
temporary introduction directory, then called `cp-retract`:

```text
[status, 200, remaining, 1]
```

## Repair and re-observation

The shared relationship store now uses native truncating writes. Its obsolete
byte conversion and remove-then-append implementation have been removed.
Public release checks the tombstone and the absence of every content field
before reporting success. An incomplete operation returns 503,
`released:false`, `release-incomplete`, remaining field names, and a retry
instruction. The same token retains recovery authority even when an interrupted
write left the state field absent. Public reads hide content once the tombstone
has been written, including while physical field removal is incomplete.

The same public fixture then observed:

```text
release, 503, {"id":"dlg-release-probe","state":"tombstoned","released":false,"error":"release-incomplete","remaining_fields":["question","point","locale"],"retry":"Restore storage access and retry with the same removal capability."}
[retry, 200, {"id":"dlg-release-probe","state":"tombstoned","released":true}, question-retained, 0]
```

Introduction retraction now verifies absence, with the actual result:

```text
[status, 503, reason, Service Unavailable, remaining, 1]
[retry, 200, remaining, 0]
```

Both probes restored permissions and removed their fixtures. No live service
was opened or interrupted. Release establishes absence from these named stores;
it does not establish erasure of copies held elsewhere.

## Verification and fixture care

The existing public dialogue band now exercises a nonempty directory blocking
field removal, same-token recovery after a missing state field, and repeated
release. This failure injection also works when mode bits would be bypassed.

The introduction band initially returned `110011111`: it cleaned and checked a
temporary circle while its handlers wrote `plugin/circle`. The live handler now
accepts an explicit circle internally; both introduction and visitor bands pass
their temporary store through that same flow. Reserved `witness-*` fixture rows
from the initial run were removed from the living store, and both bands now clean
their actual temporary stores. The unused path-selection helper was removed.

The public contract band initially returned `191` because it expected public-write
instructions in a manifest whose current hosted Action is read-only. Its check
now preserves that read-only scope while separately checking the local API.

Observed commands, all prefixed with `form-run ./fkwu`, and final verdicts:

- `control/tests/backtrack-band.fk`: `255` (zero ends the choice walk).
- `form/form-stdlib/tests/offer-ack-core-band.fk`: `32767`.
- `plugin/tests/public-dialogue-band.fk`: `16383`.
- `plugin/tests/public-dialogue-contract-band.fk`: `255`.
- `plugin/tests/introduction-band.fk`: `111111111`.
- `plugin/tests/visitor-ledger-band.fk`: `1111111111`.
- `plugin/tests/chatgpt-plugin-band.fk`: `111111111`.
- `plugin/tests/public-dialogue-restart-witness.fk`, three fresh processes:
  `111`, `1111`, `11111`, with fixture cleanup in the third.

Source changes triggered native cache admission failure followed by the offered
rebuild and successful re-admission. Those warnings were retained; each listed
observation exited 0. `form-run git diff --check` passed.
