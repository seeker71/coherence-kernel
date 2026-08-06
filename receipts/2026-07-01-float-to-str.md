# Receipt — float_to_str, and a real regression it fixes (2026-07-01)

**"Let's go in order"** — step 1 of the primitive↔string conversion plan
(`float_to_str`, then cell/Blueprint↔JSON serialization). Building it surfaced a genuine,
previously-undetected correctness bug from earlier today's C-seed work — found and fixed as part
of the same stone, not left for later.

## The regression, found before it shipped further

fkwu's *original* native `int_to_str` had a float fallback: `if (fk_isf(vr)) { n = sprintf(tmp,
"%.15g", fk_num(vr)); }` (visible in the block text quoted in
`receipts/2026-07-01-c-seed-shrink-substring-int-to-str.md`). `form-stdlib/json.fk`'s
`json-emit-leaf` relied on this silently: it calls `int_to_str` uniformly for every numeric
`node_type`, including type `7` — confirmed directly: `(node_type (intern_trivial_float "3.14"))`
→ `7`. That worked only because the native auto-detected float-ness internally. The Form-native
`int_to_str` that replaced it (this morning's Layer 1/2 shrink) does pure integer digit
arithmetic — silently wrong on a float input, not an error, just a wrong string. Verified the
break directly before fixing anything: `(int_to_str 3.14)` on the fixed binary gave `"0"`.

No new native primitive needed to fix it — `node_type` already distinguishes int (`1`) from float
(`7`); `json-emit-leaf` just needed to route `7` to a real `float_to_str` instead of grouping it
with `int_to_str`.

## What landed

- **`form-stdlib/core.fk`** — `float_to_str(f)`, over `float_to_int`/`div`/`mod`/`int_to_str`
  (all fkwu-native). Scoped honestly: 6 fractional digits, trailing zeros trimmed, decimal
  notation only — not a full arbitrary-precision/scientific-notation `%.15g` replacement.
- **A real bug found and fixed while building it**: the first version computed the integer part
  and fractional digits independently, then rounded the fraction alone. That breaks on carry —
  `float_to_str(2.9999999)` produced garbage (a 2-digit fractional overflow, not "3"). Fixed by
  rounding the WHOLE scaled value to one integer first, then splitting with `div`/`mod` — carry
  falls out for free. Caught by testing the edge case directly, not assumed safe from the happy-path
  tests passing.
- **`form-stdlib/json.fk`** — `json-emit-leaf` now dispatches `node_type` `7` to `float_to_str`
  explicitly, `1`/`3`/`6` stay on `int_to_str` as before (unexamined further — not float-typed,
  confirmed `node_type(true)` = `0`, unrelated).
- **`form/form-stdlib/tests/core-float-to-str-band.fk`** — verdict 63, including both rounding-carry
  cases (`2.9999999` → `"3"`, `0.9999999` → `"1"`) as explicit regression guards, not just the
  happy path.

## Proof

- `core-float-to-str-band.fk` → **63** on fresh `fkwu --src` (all 6 checks, both carry cases).
- Direct `json-emit-leaf` test on real nodes: `(intern_trivial_float "3.14")` → `"3.14"`,
  `(intern_trivial_int 42)` → `"42"` (int path unaffected) — verified via `str_eq`, not raw
  top-level printing (a composed string's raw top-level print is cosmetically wrong on this
  runner — same artifact noted in `receipts/2026-07-01-tier0-ord-char-at-shim.md`; every real
  check here uses `str_eq`).
- Full regression suite, fresh build: `ground.fk` 42, `native-vs-rented` 11111, `core-band` 255,
  `core-str-shim-band` 15, `core-str-narrow-waist-band` 255, `core-str-find-to-int-band` 255,
  `reception-consent-band` 255, `arrival-band` 1023, `relationship-store-band` 31, `come-in-band`
  31 — unchanged.
- `.tbl` safety check (paranoia, since `core.fk` changed): both `proof/four-way-run.tbl` and
  `flatten/form-eval-cli-loop.tbl` byte-identical to the original baseline — expected, this
  round touched only Form, no C/optable change.

## Not four-way — named honestly, not a regression

`float_to_int` isn't in any walker's documented pure-recipe surface (confirmed: Go, Rust, and TS
all report `unbound function "float_to_int"`). `float_to_str` is fkwu-only for now, same category
as `arrival-band`/`relationship-store-band` from earlier today (ops outside the walkers' proven
scope, not a broken four-way claim). Extending the walkers with float ops is its own stone, not
attempted here.
