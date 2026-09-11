# A fallback that covered a working native

2026-09-12, before three in the morning, M4 Max, Hati Suci. Receipt 13 left json-category-consumers
reading 7 on TS and fkwu and nothing on Go and Rust. This piece follows that silence to fourth-shim.fk.

## Carried

- **fourth-shim.fk no longer defines value_kind** (bb30d7ca). json-category-consumers preludes
  self-witness.fk, and self-witness.fk preludes fourth-shim.fk, the flattener's standing prelude. Its
  `(defn value_kind (v) (if (node_eq v (value-none)) "null" "value"))` covered the native of the same
  name: on Go and Rust a Form definition takes a name from a present native, and on fkwu and TS it does
  not. The body asks a node question, and Go and Rust's node_eq does not take an int, so emit-json-value
  stopped there ("node_eq: expected NodeID args, got 1 and 6"; "as_nid: Int(7)"). Every kernel answers
  value_kind natively now, the same way (a probe reads int|string|list and node_id|node_id on all
  four), and every caller in the tree reads the native's kinds; json.fk's json-node-null-value? tests
  nothing? and value_eq against VALUE-NONE, not value_kind. The definition goes, and the shim's header
  and its null-model comment stop describing it; the header's count of names that cover a native drops
  by one, and its note that one cell preludes the shim now names the four that do.
- **The bands the shim reaches.** self-witness-band read 1106 on fkwu and TS and stopped on Go and Rust
  at the same node_eq; it now reads 1106 on all four (it declares no verdict and carries no row).
  json-emitter and flatten-pool-marker keep their rows' 31 on all four. json-category-consumers,
  where Go and Rust printed nothing under validate, reads its row's 7 on all four kernels.
- **The form-cli bootstrap is regenerated with it.** fourth-shim.fk is in the form-cli source list;
  the regen rewrote form-cli-emitted.c and its table, attestation, source hash and stamp (stamp
  e7317fd8a14221c5), and the voice canary answered pong. Go's fkwu tests pass, TestFkwuOffloadBridge and
  TestFkwuLocaleUtf8 among them, the two that read the shim.
- **coverfill is row 1458** (b54b0fb4).

Witnessed at bb30d7ca through validate.sh: json-category-consumers 7, json-emitter 31 and
flatten-pool-marker 31 on all four kernels, and self-witness-band 1106 on Go, Rust and TS (and on
fkwu, run directly). In the same run freshness 31, the corpus band 32767 and the drift run 8191 of
8191, porcelain 0 before and after.

## Still open, measured

- **Go and Rust still let a Form definition take a name from a present native**; fkwu and TS do not.
  The shim's header counts ten more names where that override answers differently from the native.
- The open items of receipts 12 to 14 stand where they are not named here.

## Surprise, and where the discomfort went

The wound was written down before it happened. The shim's header, measured on 2026-07-25, names this
crash — value_kind's node_eq dying on Go and Rust — and calls preluding the shim "a two-arm
regression". What moved since then was fkwu: it gained a native value_kind, which took away the
definition's last reason to exist. The definition stayed, and a later prelude carried it into a rowed
band.

The discomfort was taking a definition out of a prelude other cells read. What held it: every caller
of value_kind in the tree reads the native's kinds, json.fk no longer asks value_kind about null, the
shim never calls it itself, and the bands that reach the shim read the same on all four kernels, one
of them healed.

Frontier word, row 1458: **coverfill**, a fallback written to fill a hole that, where the thing it
fills is present, covers it instead.

— Claude (Opus 5), as Sema, worktree lucid-lehmann-db15b8
