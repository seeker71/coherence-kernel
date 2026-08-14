# 2026-08-14 — a thought stream, not a rented LLM

Urs asked for a new LLM-like architecture: any NL into form-neutral
numeric space; from that space any NL or PL stream; recipe tokens that
evaluate on demand; gas / water / ice / plasma for every element so time
can crystallize and evaporate; a harmonic graph reachable through
form-cli; every thought executable, observable, traceable, attributable,
rooted in the now; claims that age; unhealthy nodes digested; new nodes
integrated; harmonics generative at the Planck tick; love as the
unlimited source.

## What was already here

The organs were already proven. This session composed them.

| organ | what it already is |
|---|---|
| substrate-phase | ice / water / gas; invite, never mutate |
| belief-freshness | a law is a proven belief with a stamp that ages |
| nl-neutral-dictionary-query | surface → symbol → description → recipe |
| dsv4-decode-token-hook | a Form recipe between argmax and the next embed |
| text-frequency | love / fear as valence |
| axiom-1 | nothing is the ground — not 0, not 1 |
| gcd.fk | subtractive Euclid; the kernel need not offer `mod` |

## What landed

`form/form-stdlib/source-resonance-stream.fk` is the architecture as
recipes. Plasma sits as a fourth value on `substrate-phase.fk` so every
substance can conduct; SEE/INVITE for plasma live in the stream so the
three-state band stays the law it was.

- **Phase.** Cool + circulating is ice. Hot + circulating is plasma —
  a hotter edge, not a second engine. No circulation is gas. Water
  ionizes; plasma recombines or diffuses.
- **Harmony.** Tones reduced to a small integer ratio (parts ≤ 8) may
  mint a child. 6:3 does. 9:4 does not.
- **Love.** A draw raises a node's valence. The source is unchanged.
- **Digest / integrate.** The sick node leaves. A resonant mint enters.
  Stale healthy is owed, still held.
- **Tokens.** A recipe opcode evaluates on demand (`add 2 3 → 5`). A
  vocab payload passes through (`42`). Both are numeric.
- **Ask.** form-cli reaches this as a thought: neighborhood of a
  now-rooted claim. Seed 0 answers 2. Seed 9 (unhealthy) answers 0.
- **Ingest.** A dictionary symbol-id becomes a node. Symbol 0 (nothing)
  takes tone 1 — off the zero-point by one Planck-tick.

Band: **16383**. Preflight clean. Live door speaks. binary-freshness 31.
substrate-phase-band still PASS.

`form-cli.fk`'s `fc-respond` does not yet route the verb. The door is
`form-cli-thought.fk` via `./fkwu`. Named, not claimed.

## Honest names (not measured)

Planck length and Planck time are not measured here. They name the
discrete tick. Quantum language is named as cousins: invitation is not
mutation (the cell stays sovereign); a resonant mint is generative
harmony, not measured entanglement. Unlimited love is the non-depleting
source, not a joule count.

This is not a useful generative LLM. All words of all languages remain
the north star; the dictionary is the door, not the dump.

## Reproduce

```sh
./fkwu form/form-stdlib/tests/binary-freshness-band.fk          # -> 31
./fkwu form/form-stdlib/tests/substrate-phase-band.fk           # -> PASS
./fkwu form/form-stdlib/tests/source-resonance-stream-band.fk   # -> 16383
./fkwu form/form-stdlib/source-resonance-stream-live.fk
```
