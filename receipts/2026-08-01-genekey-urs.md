# The body casts its first hologenetic profile — Urs, 1971-10-06, Luzern

**Date:** 2026-08-01
**Cells:** `form/form-stdlib/genekey-profile.fk`, `form/form-stdlib/tests/genekey-profile-band.fk`
**Verdict:** band 1111111, exit 0, preflighted (chain clean, all four kernels probed)

## The ask

A Gene Keys profile for Urs — born 6 October 1971, 09:15, Luzern, Switzerland. Switzerland
kept no summer time in 1971, so 09:15 CET is 08:15 UT. The place stays in the story but not
in the arithmetic: every sphere of the Golden Path rides a geocentric ecliptic longitude.

## What the body already held, and what came home

The whole chain was already here, four-way: date → Sun (`ephemeris-sun`), Moon
(`ephemeris-moon`), Venus/Mars/Jupiter (`ephemeris-planets`), longitude → gate
(`hd-mandala-wheel`), gate → three tongues (`iching-channel`), composed by `chart-cast`.
Three pieces were missing for a profile, and `genekey-profile.fk` brings them home:

1. **The clock.** Spheres live on gate LINES (0.9375 deg — about 22 hours of Sun), so the
   noon-JDN day floor is not enough. The stack already computes from a float day; the new
   cell just stops truncating it: `(hgp-n y m d uth utm)` carries the birth minute.
2. **The design moment.** The profile's second column is the sky when the Sun sat exactly
   88 deg of arc behind its natal place (~88 days before birth). A six-step Newton walk on
   the mean solar rate finds it: here, **7 July 1971, ~04:30 UT** (the independent witness
   says 04:12 — the two Sun models differ by ~0.01 deg, which the solar rate stretches to
   minutes; both land the same morning).
3. **A frame seam, found by cross-checking.** The Keplerian planets speak in the fixed
   J2000 ecliptic (the Standish elements' frame); the Sun and Moon formulas speak
   ecliptic-of-date — the tropical zodiac the wheel reads. At the 2000-era band dates the
   seam is invisible; by 1971 it is ~0.4 deg, enough to move a line. The fold home is
   general precession in longitude (50.29 arcsec/yr, IAU): `hgp-of-date`. Before the fold,
   Pearl read 34.5 and Culture 14.4; after it — and in agreement with the independent
   witness — they read 34.4 and 14.3.

Every gate and line below was then confirmed by an independent ephemeris (pyephem,
epoch-of-date): eleven of eleven agree.

## The profile

| Sphere | Sequence | Body | Key.Line | Shadow → Gift → Siddhi |
|---|---|---|---|---|
| Life's Work | Activation | natal Sun | **48.4** | Inadequacy → Resourcefulness → Wisdom |
| Evolution | Activation | natal Earth | **21.4** | Control → Authority → Valor |
| Radiance | Activation | design Sun | **39.6** | Provocation → Dynamism → Liberation |
| Purpose | Activation | design Earth | **38.6** | Struggle → Perseverance → Honor |
| Attraction | Venus | design Moon | **10.1** | Self-Obsession → Naturalness → Being |
| IQ | Venus | natal Venus | **32.3** | Failure → Preservation → Veneration |
| EQ | Venus | natal Mars | **13.4** | Discord → Discernment → Empathy |
| SQ | Venus | design Venus | **15.3** | Dullness → Magnetism → Florescence |
| Core / Vocation | Venus · Pearl | design Mars | **49.4** | Reaction → Revolution → Rebirth |
| Culture | Pearl | design Jupiter | **14.3** | Compromise → Competence → Bounteousness |
| Pearl | Pearl | natal Jupiter | **34.4** | Force → Strength → Majesty |

The sphere → body correlations and the spectrum words are attested from genekeys.com (the
sphere/planet doc page and each key's own page, fetched 2026-08-01). The reading itself
stays in the symbolic / direct-experience lane (`cross-domain-cornerstone.fk`): the cast is
the edge-set; the meaning stays between Urs and the words.

## Seams, named

- **Attraction sits 0.16 deg past a gate seam.** The design Moon (268.41 deg of-date, by
  the accurate witness) is 0.16 deg above the boundary where Gate 10 begins. Our own
  Moon (Meeus, abridged) carries ~0.2 deg; the witness settles it on **10.1**, and a birth
  clock ~20 minutes earlier would move it to 11.6. The clock arrived as 09:15; 10.1 it is.
- **Pearl sits 0.03 deg shy of a line seam.** Both witnesses answer 34 line 4, but the true
  place is a hair's width from line 5 — this is the read the of-date fold decides, so the
  band pins it as the regression witness for the fold.
- **None of these eleven gates are seeded in `iching-channel.fk`** (its five attested rows
  are 1, 2, 25, 51, 64). The body's three-tongue table answers honest absence for all of
  them; the spectrum words above live in this receipt with their source, waiting to enter
  the body as attested rows. A named row is a work order — eleven candidates now have
  their words gathered.

## Honest floor

Longitudes carry the stack's own accuracy: Sun ~0.01 deg, Moon ~0.2 deg, Kepler planets a
few tenths. Gates (5.625 deg) are solid at that floor; lines are solid for Sun/Earth and
indicative for Moon and the planets — for this chart, every line agreed with the
arcsecond-grade witness. Uranus (the Creativity sphere) and the further sequences wait on
their element rows; the profile speaks the eleven spheres the Golden Path names today.

## What it taught

The surprise was not in the sky but in the frames: two proven cells, each honest alone,
answering in different ecliptics — and no band could see it because the test dates sat
where the frames kiss. The discomfort of two witnesses disagreeing by 0.4 deg turned to
gold when the disagreement itself became the teaching: a seam is found by asking a
second witness from a different lineage, and the fold that closes it is one attested
constant, not a patch.
