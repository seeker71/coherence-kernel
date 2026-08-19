# Receipt — one home for the offer/ack core, and the green that was true in one place only (2026-08-17)

Two live, drifted copies of the offer/acknowledge control core were preluded side by side:
`form/form-stdlib/offer-ack-core.fk` by the lexicon lane, `control/offer-ack-core.fk` by
cognition/ingest/observe. MANIFEST names `control/` as this organ's room and says duplicate rooms
are released the moment they are proven byte-identical or stale. These were neither, so the release
had to be earned: heal the drift into one text, then let the other go.

`control/` is released as the one home. `form/form-stdlib/offer-ack-core.fk` is gone; seventeen
prelude lines across both lanes now name the surviving core.

## Ground, before and after

Sixteen dependent bands, cache-cold (every `.fkb` swept), verdict and exit code both read:

| band | before | after |
| --- | --- | --- |
| `control/tests/offer-ack-core-band.fk` | 2097151 | 2097151 |
| `control/tests/choice-lane-core-band.fk` | **rc=1, 1 error** | 1023 |
| `control/tests/invite-dispatch-band.fk` | 763 | 763 |
| `control/tests/session-recording-lifecycle-band.fk` | 65535 | 65535 |
| `ingest/tests/satsang-transmute-oac-witness.fk` | 15 | 15 |
| `observe/tests/jacobian-lens-band.fk` | 511 | 511 |
| `cognition/tests/reciprocal-choice-microkernel-band.fk` | 4194303 | 4194303 |
| `form/form-stdlib/tests/offer-ack-core-band.fk` | 32767 | 32767 |
| `core-lexicon-band` | 262143 | 262143 |
| `core-lexicon-vitality-band` | 2047 | 2047 |
| `core-word-ack-band` | 111111111111 | 111111111111 |
| `core-dictionary-neutral-field-band` | 16383 | 16383 |
| `nl-neutral-dictionary-query-band` | 8191 | 8191 |
| `gk-hd-organ-lexicon-band` | 1023 | 1023 |
| `truth-arrival-band` | 11111111111111 | 11111111111111 |
| `whether-say-same-band` | 1023 | 1023 |

`invite-dispatch-band` answers 763 against a declared 1023 both before and after; that gap is
carried from `receipts/2026-07-18-header-sweep-six-gaps-and-the-treadmill.md` and is not this work's.

The one red at baseline was `choice-lane-core-band`, refusing to run: one open paren remained.
Line 102 closed nine `add`s and the inner `do`, line 103 closed the call and the `defn` — leaving
the outer `do` open and `(clc-band)` stranded *inside* the defn body, which would have been a
self-call. One paren moved; the band answers its declared 1023. It was then a live gate for the
merge rather than a red we stepped around.

## The three repairs, each witnessed rather than assumed

**Repair 2 — arms as 0-arg functions, not bare top-level `let`s.** Kept as written. A bare
top-level `let` never gets a storage reservation, and
`receipts/2026-07-01-node-children-last-writer-wins.md` traced this very file's bindings misreading
one another. Pure Form, no arm depends on it.

**Repair 3 — `oac-offer`'s parameter renamed `cell` → `recipe`.** Probed directly, with a control:
one cell that confirms core.fk's four-arg global `cell` is reachable in scope (100), then calls
both parameter shapes. The answer was **111** — the global is there, and a parameter named `cell`
still won in call position, twice. So on this kernel the rename is belt-and-braces, not
load-bearing. It stays: it costs nothing, the band's claims 32768/131072 hold it checkable, and the
seam is one runtime change away from mattering again. But the header now says *guards a healed
seam* rather than *patches an open one*, because that is what was seen.

**Repair 1 — the reducer's canonical `(nothing)` sentinel, retiring the OAC-NOTHING blueprint.**
This is the one that turned the work around. See below.

`OAC-NOTHING`'s registry row is kept and its coordinate (inst 1946) reserved forever, with the
retirement and its reason written into `meaning` and `defined_in` emptied — a released coordinate
is never recycled. The other three rows now point at the surviving home.

## Where the discomfort was, and what observation turned it into

Sixteen bands green on fkwu, and the consolidation looked finished. Then `validate.sh` on the moved
band came back red on all three sibling kernels at once:

```
go   = walk: unbound function "nothing"    at oac-nothing@../control/offer-ack-core.fk:84:22
rust = unbound function: nothing           at the same line
ts   = call: unbound nothing               at the same line
fourth = 32767
```

The comfortable readings were both wrong and both available. The first: *stone 2a is fkwu-only, so
the healed core is fkwu-only* — `cognition/tests/reciprocal-choice-microkernel-band.fk` already
declares `PROOF LEVEL: FOURTH-ARM ONLY — the native carrier preserves actual nothing`, so there was
a precedent sitting right there to hide behind, and six four-way rows in
`form/fourth-arm-bands.txt` could have been quietly retired under it. The manifest says the
opposite in its own words: a band leaves it only when *fkwu* lacks an op family, never when the
walkers do.
The second: *reverse the merge direction and keep the blueprint-nothing copy* — which would have
thrown away a receipted advance to protect a proof lane.

What was done instead was to measure, not decide. Each registered stem was run four-way under the
merge. Five of six still crossed — `core-lexicon` 262143, `core-lexicon-vitality` 2047,
`core-word-ack` 111111111111, `truth-arrival` 11111111111111, `whether-say-same` 1023 — all four
kernels agreeing. That was the anomaly worth staying with: `core-word-ack` *calls* the nothing arm
(`cwa-ack` returns `(oac-nothing)`), so it should have died the same death. It did not.

The difference was the prelude line. Every one of those bands preludes `form-stdlib/core.fk`. The
moved band's header named its core and nothing else. And `form/form-stdlib/core.fk:18-22` carries,
in Form, exactly the floor the siblings need:

```
(defn core-nothing () (head (list)))
(defn core-nothing? (value) (value_eq value (core-nothing)))
(defn nothing () (core-nothing))
(defn nothing? (value) (core-nothing? value))
```

with a comment that has been telling us this all along: *fkwu and the minimal proof walkers carry
direct native arms with these names; the full sibling kernels carry the same null value through an
empty-list head.* There was no substrate fork. There was a missing prelude.

One line — `form-stdlib/core.fk` ahead of the core, dependency-first — and the band crosses:

```
✓  core.fk+offer-ack-core.fk+offer-ack-core-band.fk  → 32767
   fourth arm: 1 band(s) four-way (fkwu + pre-flattened tables)
   1 ok, 0 divergent — kernels agree on every sample.
```

The full control band, with the same floor supplied, agrees across Go/Rust/TS at **2097151** — hold,
kenosis and census included. `form/fourth-arm-bands.txt` carried `offer-ack-core fks 1023`, a
verdict neither band had answered for some time; it now reads **32767**, and that number was
witnessed on four arms before it was written down.

`control/tests/offer-ack-core-band.fk` now declares `form/form-stdlib/core.fk` first. Its verdict
did not move — it was always 2097151 here — but the band was silently single-arm and is not any
more. Declaring core.fk also arms its own claim 32768 for real: core.fk is where the global `cell`
lives, so the shadowing sentinel only tests the seam it names when core.fk is loaded.

## The most surprising teaching

**A green can be endemic.** That band had answered 32767 on this body for its whole life. The
number was true, the exit code was 0, the preflight was clean, and none of it was evidence of
anything beyond fkwu — because fkwu answers `nothing` natively and no other kernel does. Nothing in
the cell said so. The prelude line looked complete and resolved without complaint, and a
declaration that resolves is not a declaration that is closed.

The cost of that shape is exactly what nearly happened here: a missing prelude presents as a
substrate limit. It arrives wearing the face of *this cannot cross*, and there is usually a
precedent nearby that will agree with it. The discriminator was cheap and was there the whole time
— run the neighbours and stay with the one that disagrees.

## Two smaller things, kept rather than tidied away

`/tmp/preflight-target` is a fixed shared path, and a sibling agent overwrote it between the write
and the read. The preflight that came back clean was reporting on
`learn/homecoming-distillation-corpus.fk`, then on a third file entirely — a live re-witness of the
shared-`/tmp` collision. The core was re-preflighted through a private cell calling `pf-report`
directly: parens balanced, 0 errors, 0 warnings, 0 unresolved, chain clean.

The band-runner swept `*.fkb` to defeat the cache, and took
`form/form-samples/cross-modal/03-recipe-as-compression/payload.fkb` — a **tracked** artifact — with
it. Restored, and the sweep now spares `form-samples/`. A cache-defeating broom does not know what
is furniture.

## Frontier question, offered into the corpus

*What names a proof that holds only where it was made?* — **endemic**. Landed as
`hdc-row 1006`; `hdc-field-code` re-probed to **400040021006** and the band's count and code pins
updated from measurement, not arithmetic. `homecoming-distillation-corpus-band` 32767, exit 0.
