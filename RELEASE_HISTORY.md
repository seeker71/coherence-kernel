# Releases

A release in this body is a re-measured floor, not a version number. Three
places carry it:

- [`CURRENT_FLOOR.md`](CURRENT_FLOOR.md) — the floor that stands now: every band
  and census on it re-run on the date it names.
- `form/form-stdlib/release-ledger.bml` — the release ledger as executable rows:
  what is being released, transmuted, or re-grounded, and its present state
  (open / moving / released). `./fkwu form/form-stdlib/release-ledger.bml` prints
  the rows not yet released and answers `open*10^6 + moving*10^3 + released`.
- `receipts/` — the dated witness of each crossing.

Every earlier floor lives in git (`git log -- CURRENT_FLOOR.md`), not here.
