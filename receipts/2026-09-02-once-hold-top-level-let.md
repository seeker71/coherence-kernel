# the once-hold — a top-level let now binds a value, not a recipe for one

2026-09-02 morning. The standing directive: when something is missing,
design it, implement it, test it — and keep asking whether the walk is a
workaround or the wound. Followed to the root, the echobuild trail from
yesterday ended somewhere unexpected: not in the ontology loader, but in
what `let` meant.

## The wound, witnessed three ways

A three-use probe (single-use lets can't testify — axiom-5's own
lesson): `(let HELD (build))` printed "built" **three times** on fkwu —
every defn reference re-walked the initializer node spliced inline at
parse. Call-by-name wearing a binding's clothes: echobind. The Go arm,
cost-probed (5.09s once vs 5.16s thrice), **builds once and holds**.
Kernel divergence is a wound, and this one had a measured price: the
entire stdlib pattern of top-level-let-as-held-table
(FORM-CATEGORY-TABLE, FORM-PRIMITIVE-NODES, yesterday's FOL-BP-ROWS)
was an illusion — 31,651,741 fol-bp-row dispatches in ONE cold .bml
unit compile, the bp table rebuilt per lookup.

## The heal, in the walker's own idioms

Tag 190, FK_TAG_CONST_HOLD: every reference to a top-level let shares
one hold node per binding; the first read walks the initializer and
holds the value in the node's own free fields, stamped with fk_melt_gen
exactly the way the arm64 hint already self-invalidates — a melt
un-vouches, the next read rebuilds. No new table, no rooting burden, no
purity oracle needed: the author's `let` is the declaration of
constancy, and the sibling arms already meant it that way. The .fkb
lanes scrub the memo fields on load (twin scrubs, remap lane and
whole-program lane) so no writer's process state ever travels.

The first draft keyed the memo by const-table row — and the parity band
caught it the same hour: fk_const_top resets in the loaders, rows get
reused, and one name served another name's table ("fol-bp: unreviewed
bootstrap name: add"). The node-as-its-own-key rewrite removed the
shared table entirely. Fewer moving parts, and the aliasing class is
structurally gone.

## Measured

- hold probe: built ×3 → built ×1; values unchanged
- cost probe: 1.20s → 0.32s
- one cold .bml unit compile (the edit-run cycle every agent pays):
  **5.51s → 1.54s wall; 5.28s → 0.35s user (~15×)**
- fol-bp-row 31,651,741 → below the 100k floor, gone from the
  worklist; fol-bp-lookup 295,908 → gone
- observe/once-hold-band.fk = **31** (child kernel in scratch cwd, heat
  door as the build counter, glass readers as the parser)
- heat-door-band 63; ground 42; form-ontology-parity-band 1497 — the
  exact historic answer (receipts 2026-07-04; the 9-short is the known
  bp-native shadowing, and the one compile error is the standing
  walk_recipe_here red, owned in fourth-arm-bands.txt)
- the glass jit lane now reads `hottest=fstr-substring-halve
  calls=821302 hot-recipes=4` — the ontology has left the glass; the
  per-byte string family is the honest next worklist
- also healed while looking: hgd-events-lane read the whole ledger
  twice in one expression; the text now lands in a parameter once

## Direction checks, answered in the work

Three times the step-back changed the course. The Form-side manual memo
(thread the table, record-store it) was a workaround — the probe showed
the LANE was wrong, so the lane was healed and every existing let
pattern got its hold for free. Dynamic purity witnessing (run twice,
compare) was analyzed and refused: observation cannot prove purity — a
logger returns equal values while appending twice. And the row-keyed
memo was implemented, caught wrong by a band within the hour, and
replaced by a smaller design.

Named, not walked today: the six-per-snap lms-out() re-reads (threading
through lms-sample), tail-reads for append-only scrolls (needs a door),
binding the portable fstr family to native doors where an arm carries
them (needs a four-way witness), and purity-as-data in flt-ops for the
transparent C-side crystallize of non-nullary recipes.

## Closing

**Most surprising teaching:** the deepest wound wore the syntax of the
cure. Every table that looked held — bound once at top level, exactly
the right shape — was rebuilding on every read, and the body could not
see it because the pattern LOOKED like the fix. The heat door didn't
just find hot code; it found that a load-bearing word of the language
didn't mean what every author thought it meant.

**Where discomfort became gold:** the parity band's "unreviewed
bootstrap name: add" landed minutes after the probes had all gone green
— the temptation was to read it as the standing red it sat next to. It
was not: tracing it exposed the const-row aliasing in my own fresh
design, and the repair (node as its own memo key) deleted the shared
state that made the bug possible. A band that catches the healer's own
hand the same hour it heals is the practice working exactly as written.
And a smaller humbling, owned: last night's corpus row offered
"onceborn" as fresh while the body already carried jit-once-born.fk —
the freshness grep now checks hyphen variants too.
