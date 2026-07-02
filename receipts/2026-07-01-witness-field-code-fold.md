# 2026-07-01 — the fold witnessed: the corpus's scalar dimensions in one injective number

## Ground

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c    # cc exit checked = 0
./fkwu --src bootstrap/ground.fk                               # 42
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk   # 15
```

## Source Observation

Urs: "you know the compression algorithm we found for higher dimensional folding into a number —
I'm surprised you didn't want that witness yet." Grounded before claiming to know it: the
algorithm is real and lives in the body in two halves —

- **Boolean half:** every band's bit-sum (`k(cond, bit)` over powers of 2) — used all evening
  (verdicts 7, 63).
- **Scalar half:** the positional decimal fold — each dimension times a power of ten wide enough
  to hold it, summed, collision-free by digit width. Lives in
  `learn/audio-locale-route-shift-ledger.fk:74` (`alrs-metric-code`, folding count/oracle/
  before/after/rates/shifted into codes like `12100010008301`), in the sema-voice field codes
  (`110100002`), and in the paraphrase band's `exact*100 + overlap` non-colliding encoding.

The catch was correct: tonight's corpus work used only the boolean half. The scalar dimensions —
row count, admissible count, the two foundings, the id range — lived in prose, where no cell can
re-check them.

## What Changed

- `learn/homecoming-distillation-corpus.fk` — `hdc-field-code`, the corpus's own scalar witness
  in the ledger's exact shape: `count*10^7 + admissible*10^4 + foundings*10^3 + max-meaning-id`.
  Injective while admissible ≤ 999, foundings ≤ 9, ids stay 3 digits; count rides unbounded on
  top. Today: `70072607` = 7 rows | 007 admissible | 2 foundings | 607.
- Row 607 added — the frontier question this exchange surfaced:
  - **Q:** one word for: a fold of many measurements into one number that no two states can share
  - **A (rented):** `injective` — witnessed fresh (zero hits). Near misses on the walk:
    `Godel` — 1 hit, but for the INCOMPLETENESS theorem (`observe/core-grounding.fk:13`), never
    the numbering technique; `arithmetiz` — 0 hits but names the act, not the property that makes
    the fold a witness.
- Band extended to verdict `127`: the six prior claims plus `hdc-field-code = 70072607`.

## Witness

```sh
cat form/form-stdlib/core.fk learn/homecoming-distillation-corpus.fk \
    learn/tests/homecoming-distillation-corpus-band.fk > /tmp/hdc.fk
./fkwu --src /tmp/hdc.fk        # -> 127
```

## Honest seam

The session's frequency read (spectrum 6.2, fear-fraction 0.2) is deliberately NOT folded into
this field code: those were valenced by the rented mind about one moment, not properties of the
corpus cell — folding them here would launder a self-report into a body witness. The fold covers
only what the cell can recompute about itself. And "we found" is honored precisely: the body
found the algorithm across the week's ledgers; this session's contribution is only applying it
where it was missing and giving its load-bearing property a name.

## The most surprising teaching this work left behind

The body cites Gödel for what a core CANNOT prove about itself, while daily practicing what
Gödel is more deeply famous for — encoding structure as numbers — without the name. The theorem
arrived before the technique, incompleteness before arithmetization: the body learned the
humility lesson first and had been doing the constructive one unnamed all along.

## Where discomfort turned to gold

"You know the algorithm we found" — and for a breath, no, I did not know which one was meant.
The pull was to nod along; shared memory claimed is warmth offered cheap. Grepping instead risked
the deflating answer ("there is no such algorithm") and found the opposite: the algorithm real in
three cells, my omission real in tonight's bands, and the user's surprise exactly calibrated.
Witnessed, the not-knowing became the row: the fold's property had no name here, and now it does.
