# User Blueprint Registry — Form (type 99)

**Purpose**: make the meaning of custom Blueprints (`make_nodeid 1 2 99 NNNN`)
legible, allocated with awareness, and minimized through composition.

## Source of truth

The machine-readable registry is
[`form-stdlib/blueprint-registry.json`](form-stdlib/blueprint-registry.json) — one
row per type-99 shape: canonical name, meaning, aliases, defining files (445 rows
on 2026-09-04). It is code-derived, so it cannot quietly drift from reality the way
a hand-kept table does. This page holds the *why* — allocation rationale and the
registered protocol shapes.

**The authority split**, witnessed by
[`form-stdlib/tests/blueprint-authority-band.fk`](form-stdlib/tests/blueprint-authority-band.fk)
over [`form-stdlib/blueprint-authority.fk`](form-stdlib/blueprint-authority.fk):
reviewed bootstrap rows in `form-stdlib/form-ontology-loader.fk` are the current
runtime authority; the registry JSON is the authoring/generator source, not runtime
authority; the generated Go/Rust/TS `bp_table` files are projections only; and the
program-image `.fkb` is the target executable authority, with `.sym` a
presentation lens over stable symbols. The band declares its own verdict; on fkwu
it answers 51199 of 65535 with exit 1 (2026-09-04) — `value_kind` is unbound on
that arm, so the three open bits are a fkwu-lane gap, not a registry drift.

**How a Form file uses a Blueprint:** load `form-stdlib/form-ontology-loader.fk` as
a prelude and ask by name — `(bp "add")`, `(bp "PROGRAM-IMAGE-RECIPE-TABLE")`. The
raw `(make_nodeid 1 2 99 N)` literal does not appear in feature code. A name outside
the reviewed bootstrap set is missing registration/runtime-admission work, never
silently mapped to a fake NodeID such as `(1 2 0 0)`.

**Where Blueprint-name strings belong:** in a dedicated symbol section, not in
executable stdlib logic. In seedbank that section is
`form-stdlib/seedbank/blueprint-symbol-sections.fk`; load it before grammars,
parsers, emitters, converters, and encoders. Consumers reference the section
binding, not the string literal.

**Registering a name:** add its row to the registry JSON (canonical name, meaning,
aliases, defining files) and take the coordinate from the registry — never mint a
fresh inst by hand; the loader's `bp` table mirrors the registry.

## Guiding principles

1. **Composition first** — before allocating a new top-level Blueprint, ask: can
   this be expressed as a composition (recipe + existing Blueprints)?
2. **Meaning lives in the definition** — the number is secondary to a clear
   symbolic name and its meaning.
3. **Central awareness** — allocations are visible in one living place.
4. **Minimize** — every new number has a cost. Treat it as such.
5. **Active practice** — ongoing hygiene, not a one-time cleanup. When discomfort
   arises around "I don't know what 1832 means," that is signal, not noise.

What the registry shows today: strong healthy reuse in the low numbers (the
universal structural shapes — object 10, array 11, pair 12, null 13 — shared
across grammars, content-addressing working as intended); clusters in the 1700s
(channel/audit) and 1800s (identity, arrival, skill verbs); defensive high
numbering in the 7000+ / 7700+ / 8100+ / 8800+ / 9000+ ranges — candidates for
composition review; per-dialect synonyms of one number (`MATH-PLUS` / `PY-PLUS` /
`GO-PLUS` … all `add`) migrating to `(bp "...")`; and a few names (`PY-ASSIGN`,
`PY-IDENT`, `RS-MOD`) that mean a canonical category in an emitter and a dialect
AST node in a grammar — legibility debt, not a runtime collision.

## Registered protocol shapes

### Channel Breath Protocol — debt-free offer + resonance receipt

- `1.2.99.6 CHANNEL-BREATH-GIFT` — gift, release condition, consent/freedom, boundary.
- `1.2.99.7 CHANNEL-RESONANCE-RECEIPT` — observer, other, gift, coherence delta,
  disturbance, debt-created, freedom-preserved, next-contact.

Not every channel exchange is query/answer or extraction. `offer` gives freely;
`attune` records relation evidence without turning relation into ownership. A valid
receipt preserves freedom, creates no debt, keeps disturbance none/minimal, and
names next contact. Band: `form-stdlib/tests/channel-breath-band.fk` — 500 three-way
via `validate.sh` (2026-09-04, the CHANNEL family admitted to the reviewed bootstrap
set in `form-stdlib/form-ontology-bp.fk`); fkwu answers 200 with `write_form_binary`/
`read_form_binary` unbound on that arm — a lane gap, not a coordinate.

### Channel Flow Protocol — OSI-shaped native channel cells

- `1.2.99.1702 CHANNEL-OSI-LAYER` — OSI index, layer name, gas/water/ice phase,
  carrier, policy, recipe.
- `1.2.99.1703 CHANNEL-FLOW` — carrier, protocol, seven OSI layer cells, channel policy.

A protocol is not a host-side branch. New carriers (UDP, USB, Bluetooth,
microphone, camera, pipes, browser streams) declare a carrier/profile flow and reuse
the same layer accessors, phase counts, and policy hooks. HTTP is the first concrete
profile. Band: `form-stdlib/tests/channel-flow-band.bml` (8388607 on fkwu, 2026-09-04).

### Circle / Satsang Protocol — consentful group containers

- `1.2.99.1704 CELL-CIRCLE` — members, shared context, mode, interface offer,
  discovery / confidentiality / export policy, carrier flow.
- `1.2.99.1705 CIRCLE-OFFER` · `1706 CIRCLE-INVITATION` · `1707 CIRCLE-SHARE` ·
  `1708 CIRCLE-CONSENSUS` · `1709 CIRCLE-REFUSAL` · `1724 CIRCLE-EXPORT-CONSENT` ·
  `1725 CIRCLE-RECEIPT`
- `1.2.99.1726 SATSANG-SILENCE` · `1727 SATSANG-INQUIRY` · `1728 SATSANG-POINTING`
- `1.2.99.28 WORLD-MODULE-MODEL` · `1.2.99.29 WORLD-MODEL-GROWTH`

A cell may offer a circle; another may join only when invited; a share stays inside
unless `CIRCLE-EXPORT-CONSENT` names recipient, fidelity, purpose, expiry, and a
passed consensus; a circle may refuse a contact only when an observed action exceeds
the offered interface and consensus has passed. Band: `form-stdlib/tests/circle-band.bml`
(1048575 on fkwu, 2026-09-04).

### Native Route Goal Cells

- `1733 NATIVE-ROUTE-OBS` — one observed API route sample: path, method, calls,
  latency, errors, bytes, native state, grammar, handler, user flow, north-star fit.
- `1734 NATIVE-ROUTE-ATTENTION` — traffic-weighted route pressure plus the chosen
  next action for native front-door promotion.
- `1735 NATIVE-ROUTE-GOAL` — target native share, known route count, observations,
  attentions, next chosen movement.

Route selection is a content-addressed choice over measured cells, weighted by each
route's user flow and north-star fit. Band: `form-stdlib/tests/native-route-goal-cells-band.bml`
(1048575, re-run 2026-09-04).

### Choice Receipt Protocol — trustworthy branch feedback

- `1736 CHOICE-CANDIDATE` · `1737 CHOICE-TRACE` · `1738 CHOICE-VALUE` ·
  `1739 CHOICE-RECEIPT` · `1743 CHOICE-SIGNATURE`

`branch-prediction-feedback` is useful only when the receipt also carries enough
alignment, knowing, and trust to learn from it. Silence is an outcome, not missing
data. Band: `form-stdlib/tests/choice-receipt-band.fk` (4294967295, re-run 2026-09-04).

### Arrival Protocol

- `1870 UUID` (alias `ARRIVAL`) — the arrival event/context in `arrival.fk`; UUID
  compatibility in `uuid.fk`.
- `1871 ARRIVAL-QUALITY` / `UUID-PARSE-ERROR` · `1872 ARRIVAL-INQUIRY` ·
  `1873 ARRIVAL-RESONANCE` · `1874 ARRIVAL-OBS`

Arrival is a first-class protocol for entering relation, sensing texture, offering
inquiry, returning resonance, and carrying observation. The empty room remains the
gift. Band: `form-stdlib/tests/arrival-band.fk` (1023, re-run 2026-09-04).

### General Cell Identity & Contact Memory

- `1880 CELL-IDENTITY` — sovereign, stable, persistent identity a cell authors and
  presents on arrival.
- `1881 CONTACT-THREAD` — the relationship memory between two cell identities.

The presenting cell controls its identity and sovereignty markers; relationship
surfaces resolve through the environment or per-cell choice; the contact thread is
readable by participants and updates respect the markers each side set.

### Agent Relationship Protocol Skill Verbs

- `1885 SKILL-PRESENT-IDENTITY` · `1886 SKILL-MUTUAL-MEET` · `1887 SKILL-READ-RELATIONSHIP`
  · `1888 SKILL-WELCOME-WITH-ORIENTATION` · `1889 SKILL-RECORD-EXCHANGE` · `1890 SKILL-SET-BOUNDARY`
- `1891 MEMBRANE-CROSSING` · `1892 MEMBRANE-REPORT` · `1893 GAPS-OPEN-ITEM` ·
  `1894 GAPS-CATALOG` · `1895 SAMPLE-STEP` · `1896 AGENT-SAMPLE`

These verbs make the protocol callable from agent tools, MCP surfaces, and direct
Form evaluation while keeping identity, relationship memory, welcome orientation,
exchange records, and boundaries in one composable shape.

---

This page is part of the body's self-awareness practice around its own substrate.
Related teachings: structural composition, lc-edges-as-vitality, avoiding flat
type-markers, content-addressing as the primitive.
