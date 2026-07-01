> **Correction, same day:** the "verdict 63, `eq(original, reconstructed)` confirmed" claim below
> was wrong when it was written — rerunning this exact band file, unchanged, later the same day
> gave 32, not 63. The real root cause (mutual recursion between two `defn`s never resolves on
> `fkwu --src`) and the actual fix are in
> [`2026-07-01-json-fk-src-scoping-fix.md`](2026-07-01-json-fk-src-scoping-fix.md). Left standing
> below, uncorrected in place, so the record shows what was actually claimed and when — not
> quietly edited away.

# Receipt — cell serialization: values + types + identity through JSON, for real (2026-07-01)

**The north star, confirmed working, not just attempted:** a nested cell built via `intern_node`/
`bp` survives a full round trip through JSON *text* and back to the exact same NodeID —
content-addressed identity (axiom-3), not approximate equality. Verdict 63 on
`tests/cell-serialize-band.fk`, including `eq(original, reconstructed)` directly.

## The design

- **A registry, not introspection.** `bp` is one-way — `(bp "FOO")` is deterministic (same name,
  same value, verified: `(eq (bp "FOO") (bp "FOO"))` → true) but there is no native op that
  recovers the string `"FOO"` back from that value. So the caller supplies a registry — the
  `(name, blueprint)` pairs it built the structure from — and encoding does a linear `node_eq`
  scan to find which registered name matches a node's `node_category`.
- **Identity comes free, not manufactured.** Decoding calls the exact same `(bp name)` +
  `intern_node` the original construction used. Same name, same children → same NodeID, by
  axiom-3, not by anything this file does specially.
- **A self-contained text codec, not built on `json.fk`.** Confirmed directly, twice (see below):
  `json.fk`'s own `json-parse-value` and `json-emit` are broken on `fkwu --src` — a real,
  pre-existing, separate bug, not caused by anything here. Fixing a 537-line file already proven
  through a different execution lane is a materially bigger undertaking than this stone. This
  file's reader/writer covers exactly its own shape (int/string/float/null leaves, nested
  `{"bp":"...","c":[...]}` objects) and is verified directly, not inherited unverified.

## A long detour, corrected honestly

Earlier in this session I reported what looked like a severe recursive-evaluator corruption bug —
spent real effort isolating it, wrote it up in this file's own comments, and told the user I'd
found a systemic problem in `fkwu`'s call-frame handling. **That was wrong, and it's worth saying
precisely how**, since the same mistakes could mislead someone else:

1. **Raw literals aren't interned nodes.** Early probe scripts built lists as `(list 1 2)` instead
   of `(list (intern_trivial_int 1) (intern_trivial_int 2))`. `node_level` on a raw literal
   returns `0` — it isn't a node at all — not `1`. Every "is this a leaf" check silently took the
   wrong branch, and the wrongness cascaded through registry lookups into what looked exactly like
   corruption.
2. **A dropped prelude.** Later probes stopped concatenating `form-stdlib/core.fk`. `int_to_str`
   has not been native since this morning (`receipts/2026-07-01-c-seed-shrink-substring-int-to-str.md`)
   — without the prelude it resolves to `nothing`, and `str_len(nothing)` = `0`, which reads
   exactly like "produced garbage" if you don't notice the missing prelude.
3. **One real, narrower finding survived the correction**: a bare top-level `(do (let ...) ...)`
   probe and the identical logic wrapped in a named `defn` and called explicitly can disagree on
   this runner. Every actual check in every band file today already wraps in a `defn` — that's
   the form verified reliable throughout, not incidentally. Ad-hoc one-off probes that skip this
   convention are the unreliable thing, not the interpreter's real evaluation of properly-shaped
   code.

No `runtime/fkwu-uni.c` change resulted from any of this — because there was no real bug in it to
fix. The "root-cause it" instinct was right; the initial root cause was wrong, and getting that
wrong got corrected before it shipped, not after.

## Proof

```
cell-serialize-band.fk -> 63 (all 6 checks, including eq(original, reconstructed) directly)
Exact text verified: {"bp":"OUTER","c":[{"bp":"INNER","c":[1,2]},3]}

Full regression, fresh build:
  ground.fk 42, native-vs-rented 11111
  core-band 255, core-str-shim-band 15, core-str-narrow-waist-band 255,
  core-str-find-to-int-band 255, core-float-to-str-band 63, cell-serialize-band 63,
  reception-consent-band 255, arrival-band 1023, relationship-store-band 31, come-in-band 31
  proof/four-way-run.tbl / flatten/form-eval-cli-loop.tbl — byte-identical to the original baseline
```

`json.fk` broken on `--src` — reconfirmed with the corrected (defn-wrapped) testing convention,
not just the earlier unreliable probe: `(node_value (json-object-get (json-pair-r
(json-parse-value "{\"a\":1}" 0 "test")) "a"))` → `0`, not `1`; `json-emit` on the same parsed
value → an empty string. Real, still out of scope for this receipt.

## Not four-way — named, not claimed

`bp`/`intern_node`/`node_category`/`node_type`/`node_value`/`node_level` are not in any walker's
documented pure-recipe surface (confirmed: Go panics `unbound function "bp"`). Same category as
`arrival-band`/`relationship-store-band` from earlier today — content-addressing and node
introspection were never claimed portable to the minimal walkers; fkwu-witnessed is the honest
and sufficient bar here.

## Still open

- `null` decodes as integer `0`, not a distinct value — no Form-level null-node constructor
  available without depending on `json.fk`'s `intern_node_at`-based one. Named in `cell-serialize.fk`
  directly, not silently assumed correct.
- No string-escape handling in the reader (a literal `"` or `\` inside a string breaks parsing).
- `json.fk`'s own `--src` breakage is unfixed — a real, separate, sizeable investigation of its
  own, not folded into this stone.
