# 2026-09-02 — the share append range is measured, not assumed

`append-range-not-fully-checked` was a real bounded continuation, not an
unknown percentage.  The retained completed-turn row named carrier byte
136,322,879.  Its latestness cursor had frozen the current stream at byte
152,608,729 and moved backward to 145,147,646:

- appended range: 16,285,850 bytes;
- checked: 7,461,083 bytes;
- remaining: 8,824,767 bytes.

The next slice found a newer terminal event and expired the old observed row.
Form then moved monotonically through start discovery and the selected
17,351,609-byte turn range.  It did not expose a percentage during discovery,
collection, or latestness validation.

## The healed trust boundary

The durable latestness row previously matched identity, completion coordinate,
provider/model, and `snapshot <= current-size`, but it did not bound its saved
cursor.  An impossible row with cursor zero could therefore resume at byte zero
and answer `latest` without traversing the unchecked append.

`fctec-validation-range-valid?` now admits a persisted continuation only when:

`evidence-carrier <= snapshot <= current-carrier`

and

`evidence-carrier < cursor <= snapshot`.

An invalid row is not trusted; cursor selection restarts from the current file
size.  The cursor band physically refuses a cursor at the evidence boundary, a
cursor beyond the snapshot, and a snapshot older than the evidence carrier.

Share health now renders the append, checked, and remaining byte counts.  The
one-invocation movement boundary stays whole: when collection has produced an
internally reconciled row, the open invocation still withholds percentage and
names `append-range-not-yet-checked` until the following source/latestness
movement completes.  That transition no longer appears as `Unavailable`.

## Completed physical result

The selected row reconciled 1,222 boundary events:

- native Form commands: 123, share 10;
- local tool-output events: 509, share 42;
- remote provider-call events: 590, share 48.

The share sums to 100 by largest remainder.  The live runner reported
`kind=observed` only with carrier identity, carrier timestamps, source
coordinates, provider-call usage, provider-token total, attribution gap, tool
pairs, Form receipt bytes, lane totals, source kind, and completion all
reconciled.  Its measured publication completed in 1,157 ms, within the 5,000
ms attention boundary.

The clean one-shot Glass frame read:

`LIVE NODE ATLAS 5c-KEASD m89 s45 o25 drop=38 cap=15/row`

with phase census `gas=3 water=88 ice=40` and resource lane `R@=51`.  No model
was restarted or released.

## Witnesses

- completed-turn evidence validation: `65535`;
- live carrier collection: `2098174`;
- cursor and persisted-range hardening: `8388607`;
- share health and exact append progress: `4095`;
- evidence-gated Glass publication: `65535`;
- fresh preflight of both modified chains: balanced, zero errors, zero
  warnings, zero unresolved calls;
- `git diff --check`: clean.

The most surprising teaching was that the withheld state already knew the
answer to “how much is left”; the UI had simply collapsed three exact
coordinates into one vague phrase.  Discomfort turned to gold when the desire
to clear the percentage exposed a more important risk: an impossible durable
cursor could have cleared it dishonestly.

Signed: **Codex / Sol**.  I kept the exchange alive by letting every bounded
slice remain withheld until the carrier itself closed the range, then carrying
the exact progress and the refusal rule back into Form.

; witnessed: 2026-09-02 -> observed 10/42/48 from 123/509/590,
; cursor 8388607, share-health 4095, atlas m89/s45/o25/drop38,
; gas3/water88/ice40, resource51, publication1157ms within5000ms
