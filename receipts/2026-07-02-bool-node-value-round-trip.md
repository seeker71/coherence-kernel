# Receipt — booleans made real across the whole wire lane (2026-07-02, overnight)

Urs, before sleep: *"what can you do in the next 8 hours that you can show me in the morning?"*
Honest scope first — I don't run continuously overnight; I work in a turn and leave it standing.
So: one complete, verified thing, every gate green, waiting for you. This is it.

## The bug (real, and older than tonight)

A trivial boolean serialized as **`null` in every wire dialect** — JSON, XML, and CDR alike —
for the entire life of the wire-serialization lane. Root cause in the C seed: `node_value`
(tag 49) returned, for a bool node, the *interning sentinel* stored in `fk_nval` (the value that
makes `true` and `false` distinct interned nodes) instead of the boolean itself. So no Form code
could read a bool's value; every dialect's leaf-emit fell through to its null case. `json.fk`
(the standalone JSON codec) had worked around it by comparing node *identity*
(`eq node (intern_trivial_bool true)`), but the wire lane's own JSON dialect (`cell-serialize.fk`),
XML, and CDR had no such workaround — they simply lost the value.

## The fix (fundamental, then per-dialect)

1. **C seed — `runtime/fkwu-uni.c`, `node_value` tag 49:** for a bool node (`node_type` 3),
   return the boolean `nid[3]` tagged (`<<1`) instead of the sentinel. Now
   `(node_value b)` equals the `true`/`false` literal, and `(if (node_value b) …)` dispatches
   correctly. Verified with an emit-shaped band: 15/15, including the exact case the old
   `json.fk` comment doubted (false must be *falsy*).
2. **`cell-serialize.fk` (JSON dialect):** bool leaf → `true`/`false`; parser reads `t`/`f`.
3. **`wire-xml.fk`:** bool leaf → `<b>true</b>`/`<b>false</b>`; parser reads `<b>`.
4. **`wire-corba-cdr.fk`:** bool → CDR tag 5 + one octet (real CDR has a boolean primitive);
   decoder reads tag 5. 
5. **`json.fk`:** left as-is (its identity form is proven and needs nothing from node_value);
   its now-stale "value slot is truthy for both bools" comment corrected in place — true of the
   pre-fix runtime, no longer true of current source.

## Proof (every gate, on the freshly-built and installed `./fkwu`)

```
new wire-bool-band.fk -> 255  (JSON/XML exact text + identity round-trip; CDR identity
  round-trip; true-cell and false-cell stay distinct through the binary wire)
node_value emit-shaped rig -> 15/15

fresh build 0 errors; ground 42; binary-freshness canary 15
four-way-run.tbl -> 0 ; form-eval-cli-loop.tbl -> 0
proof/four-way-run.tbl + flatten/form-eval-cli-loop.tbl -> BYTE-IDENTICAL to baseline
full regression unchanged: core 255, str-shim 15, narrow-waist 255, find-to-int 255,
  float-to-str 63, json 1023, cell-serialize 1023, wire-xml 63, wire-cdr 255, wire-path 63,
  wire-rpc 15, http-negotiate 127, reception-consent 255, hdc-corpus 127
```

## Most surprising teaching — the penumbra (row 620)

The fix **edited the C seed**, yet the four-way proof stayed `0` and both `.tbl` artifacts stayed
byte-identical. Not luck: booleans live *outside* the pure-recipe surface the Go/Rust/TS walkers
prove (`node_value`/`intern_trivial_bool` are fkwu-only), so the defect sat in the region the
proof lights only *partially* — fkwu-witnessed, never four-way. The fresh word for that
partial-shadow zone is **penumbra**: a proof's umbra is where nothing can hide; its penumbra is
where a real bug can sit lit-enough-to-run, dark-enough-to-miss. This bug lived in the penumbra
for the whole life of the wire lane. The teaching: a green four-way is not a clean bill of
health — it is a clean bill *for the umbra*. The penumbra needs its own witness bands, which is
exactly what `wire-bool-band` now is.

## Where the divergence turned to gold (stated functionally)

Two forks, taken honestly rather than smoothly. First: my initial bool test returned `5` and the
fluent read was "the fix failed" — the honest branch was to check *why*, and it was my own
assertion confusing the integer literal `2` (tagged value 4) with the tagged bool `2`; the fix
was correct, my test was confused. Naming that stopped a good fix from being discarded on a
mis-read. Second: `json.fk`'s own comment stated flatly that node_value is "truthy for BOTH
bools" — the fluent branch was to trust the comment and route around node_value forever; the
honest branch was to *test the claim on current source*, where it is no longer true. The gold is
the same rule tonight kept teaching in every register: verify the claim against the running
body, not against the confident sentence — mine or the code's.

## Not attempted

- Four-way for bools: the walkers do not carry `intern_trivial_bool`/`node_value`; extending them
  is a separate stone. Named honestly, as every fkwu-witnessed organ here is.
- `json.fk`'s identity-form bool emit was not rewritten to node_value — working code left working.
