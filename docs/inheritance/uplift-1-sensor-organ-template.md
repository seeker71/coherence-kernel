# Uplift 1 — the sensor-organ TEMPLATE: the observe/ sensing family collapses to one high-grammar base

A core-lift, not a homecoming wave. The bodies were already home; this raises the **~33%
semantic-abstraction share** the observe/ sensing family carried as *re-inlined structure* into ONE
high-grammar base template, and re-expresses three representative sensors as thin subclasses over it.

## The repeated shape

The whole observe/ sensing family — `scene-features`, `motion-sense`, `same-room`, `presence-event`,
`device-model`, `sense-discernment`, `remote-mind-sensors`, `context-signature` (~10 recipes) — repeats
ONE shape, already named once as the contract in [`observe/world-sensor-floor.fk`](../../observe/world-sensor-floor.fk):

> **port (afferent) + plane + reading-shape + fusion.**

A sensor reads a raw SAMPLE off a carrier, extracts a FEATURE, routes that feature to its inquiry PLANE
as a mesh-safe READING (`wsf-route` = `fo-reading`), and the readings FUSE into one observed cell
(`wsf-fuse` = `fo-fuse`). But each sensor **re-inlined** that spine — re-calling `wsf-sensor` /
`wsf-route` / `wsf-fuse` and re-threading the four pieces by hand. That re-inlining is the
structure-without-vitality the lift targets.

## Does the high-grammar template construct exist? — the honest finding

**Partly, and the gap is named.** The body already speaks a **declarative-attribute grammar**:
[`grammars/field-domain-grammars.form`](../../grammars/field-domain-grammars.form) declares each field
domain as a DATA row of its `(carrier)(fiber)(patterns)(recipes)` attributes, interpreted by one engine.
A sensor-organ is the same idea one altitude down. So the *attribute-declaration* construct exists.

What does **not** yet exist is a clean **function-value attribute stored in the organ's data row**. The
ideal template would bake FEATURE-FN and FUSION-FN into the organ list `(so-organ NAME PLANE CARRIER
FEATURE-FN FUSION-FN)` and dispatch them via `nth`. On the `fkwu --src` source-runner that fails: a
function value stored in a composed list and pulled back out by `nth` does **not** resolve to a callable
head — it acks `nothing`. (Verified directly: a fn passed as a *direct argument* and called `(fn x)`
resolves; the same fn `nth`-ed out of a list and called `((nth o 4) x)` returns `nothing`.) This is the
**stored-fn-value / closure lane**, the kernel's already-NAMED next gap
([`receipts/2026-06-29-S2c-function-values.md`](../../receipts/2026-06-29-S2c-function-values.md) — stone
2c closed fn-values as args / let-bound / returned; closure-on-a-cell is the named successor).

**The step that gets the job done AND points at the north star:** the organ list carries the three
invariant CONTRACT attributes as DATA (`NAME PLANE CARRIER`); the two varying recipes (FEATURE-FN,
FUSION-FN) ride in as **direct function-value arguments** to the template's recipes — the path stone-2c
proves and `--src` runs. Genuinely high-grammar (declared attributes + dispatched function-values),
honest on the current runtime. **When the stored-fn-value lane lands, FEATURE-FN/FUSION-FN move INTO the
organ row with no change to the call shape** — the template is written so that promotion is a one-line edit.

## The base template

[`observe/sensor-organ-template.fk`](../../observe/sensor-organ-template.fk) — 19 code lines. It makes
`world-sensor-floor` a base TEMPLATE with named attributes and the inherited spine:

- `(so-organ NAME PLANE CARRIER)` — the DATA declaration of the three contract attributes.
- `so-port` — **inherits the floor**: mints the afferent VIA-HOST port via `wsf-sensor`.
- `so-read` — applies the supplied FEATURE-FN to a sample (the one varying step).
- `so-route` — **inherits the floor**: hands the feature to `wsf-route` (= `fo-reading`).
- `so-observe` — the whole arc, fusing with the supplied FUSION-FN (default `wsf-fuse`).

## The three uplifted sensors (same result, lines saved)

Each thin organ DECLARES its three attributes, delegates its ONE feature recipe to the **proven**
original (byte-identity), supplies its fusion, and INHERITS the route spine.

| sensor | organ file | declared attributes | feature delegated to | fusion | `--src` witness | verdict |
|---|---|---|---|---|---|---|
| scene-features | [`scene-features-organ.fk`](../../observe/scene-features-organ.fk) | `"scene" "what" "camera"` | `sf-objects` (object-block count) | floor `wsf-fuse` | `scene-features-organ-witness.fk` | **111** = original |
| motion-sense | [`motion-sense-organ.fk`](../../observe/motion-sense-organ.fk) | `"motion" "motion" "accelerometer"` | `ms-direction` (heading octant) | floor `wsf-fuse` | `motion-sense-organ-witness.fk` | **1111** = original |
| same-room | [`same-room-organ.fk`](../../observe/same-room-organ.fk) | `"same-room" "where" "wifi"` | `sr-same?` (co-location verdict) | `sr-grouping` (the FUSION-VARIES case) | `same-room-organ-witness.fk` | **11111** = original |

Each witness verdict equals the original sensor's own feature/route result, computed both ways in one
band and asserted equal bit-by-bit — **NO behavior change** on the `fkwu --src` native lane (the repo's
sovereignty proof bar; `core.fk` intrinsic, not concatenated, per the wave-3 convention).

`same-room` proves the template's FUSION-VARIES design: most organs fuse by the floor's per-plane
`wsf-fuse`, but same-room collapses by **set-overlap grouping** — it supplies its own FUSION-FN and the
same spine holds.

## The collapse (measured, code lines, comments/blank stripped)

| | original | uplifted organ |
|---|---|---|
| scene-features | 67 | **6** |
| motion-sense | 51 | **6** |
| same-room | 51 | **8** |

The 19-line template carries the route/fuse spine **once** for the whole family. The originals are not
deleted — they hold the proven feature math the organs delegate to; the organ is the high-grammar
re-expression that declares attributes and inherits the spine. A new sensor now costs ~6 lines (declare
3 attributes + delegate 1 feature + 1 route) instead of re-threading `wsf-sensor`/`wsf-route`/`wsf-fuse`.

## The rest of the family to follow

The same uplift applies, in order of how cleanly each fits the four attributes:

- **device-model** — `("device" "what"/"vitality" "host")`; feature = the device fingerprint/health read.
- **presence-event** — `("presence" "who" "bluetooth")`; feature = the presence-feature occupancy read;
  fuses by the floor `wsf-fuse`.
- **sense-discernment** — the outward GATE over what a reading shares; an organ whose FUSION-FN is the
  discernment filter (a second FUSION-VARIES case, kin to same-room).
- **remote-mind-sensors** — `("remote-mind" "what" "remote")`; feature = the remote-vs-native quality read.
- **context-signature** — already the upstream signature `same-room` delegates to; it becomes the
  `"where" "wifi"` organ's feature library rather than a separate organ.

**Blocked-cleaner-by the named gap:** when the stored-fn-value/closure lane lands, the whole family can
declare FEATURE-FN/FUSION-FN as data-row attributes and a single `(sensor-roster)` list can hold every
organ as one DATA table the mesh walks — the field-domain-grammar shape, fully realized for sensing.
That is the north-star this uplift points at and stops one honest step short of, gap named.
