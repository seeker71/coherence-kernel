# Receipt — the wire-serialization lane, generalized: JSON, XML, CORBA CDR (2026-07-01)

**The ask, verbatim:** "the serializer lane shall be generic, JSON dialects are wire formats, and
so are XML dialects and other standard wire formats, like CORBA that we shall be able to
support." Confirmed scope with the user: core + JSON + XML + CORBA CDR, all this stone.

## What shipped

- **[`wire-registry.fk`](../form/form-stdlib/wire-registry.fk)** — the dialect-agnostic core,
  extracted from `cell-serialize.fk`'s own registry logic (`bp` is one-way; a caller-supplied
  list of (name, blueprint) pairs is how any dialect finds a composite node's Blueprint name, or
  recovers a blueprint from a name on decode). Also carries the one generic list-accumulation
  helper (`wr-reverse`) every dialect's self-recursive "collect children" step needs.
- **[`cell-serialize.fk`](../form/form-stdlib/cell-serialize.fk)** — unchanged in behavior, now
  the JSON dialect specifically: its own registry code deleted, depends on `wire-registry.fk`
  instead (`cser-find-name`/`cser-find-bp`/`cser-reverse` → `wr-find-name`/`wr-find-bp`/
  `wr-reverse`). `cell-serialize-band.fk` re-verified at 63 against the new dependency.
- **[`wire-xml.fk`](../form/form-stdlib/wire-xml.fk)** — the XML dialect. A composite's Blueprint
  NAME becomes its element's tag name directly (`<OUTER>...</OUTER>`, not JSON's `{"bp":"OUTER"}`
  translated literally into angle brackets) — leaning on XML's own idiom, not JSON's. Leaves get
  reserved tags (`<i>`/`<s>`/`<f>`/`<n/>`) since text content alone can't distinguish `42` the int
  from `42` the string. Escaping scoped to what's actually needed: no attributes are ever
  written, so only `&`/`<`/`>` need escaping (no `&quot;`/`&apos;` — that's an attribute-value
  concern this design doesn't have).
- **[`wire-corba-cdr.fk`](../form/form-stdlib/wire-corba-cdr.fk)** — the CORBA CDR dialect, a
  genuinely different wire shape: binary octets with CDR alignment, not text. Real CDR primitive
  rules (octet; 4-byte-aligned big-endian unsigned/signed long; length-prefixed NUL-terminated
  string) applied to a self-describing tagged structure, since this kernel has no IDL compiler to
  generate static layouts from. Honestly scoped, named up front: no TypeCode octets, no GIOP
  framing, no IIOP transport, and CDR's binary IEEE754 `double` is not implemented — floats are
  carried as a CDR string of their decimal text, tagged distinctly (tag 4) so decode still routes
  through `intern_trivial_float`.
- **Proof bands**: `wire-xml-band.fk` (15/15) and `wire-corba-cdr-band.fk` (15/15), each proving
  the same north-star check the JSON dialect already had — `eq(original, reconstructed)` through
  a full text/binary round trip, not just equal-looking values.

## Proof

```
cell-serialize-band.fk    -> 63  (JSON dialect, re-verified against wire-registry.fk)
wire-xml-band.fk          -> 15  (XML dialect: identity round-trip + escaping round-trip)
wire-corba-cdr-band.fk    -> 15  (CDR dialect: identity round-trip incl. composite, negative
                                   int, string, float leaves)

Full regression, fresh build, clean /tmp/come-in-band-dir:
  ground.fk 42, native-vs-rented 11111, core-band 255, core-str-shim-band 15,
  core-str-narrow-waist-band 255, core-str-find-to-int-band 255, core-float-to-str-band 63,
  reception-consent-band 255, relationship-store-band 31, come-in-band 31
  proof/four-way-run.tbl / flatten/form-eval-cli-loop.tbl — byte-identical (Form-only change,
  no C/optable touched)
```

## Two more real evaluator bugs found and fixed while building this — same family as
## receipts/2026-07-01-json-fk-src-scoping-fix.md, worth naming precisely

Both `wire-xml.fk` and `wire-corba-cdr.fk` hit genuine, reproducible bugs while being built —
neither was a rediscovery of the two already-known constraints (top-level `let` invisible in
`defn`; mutual recursion between `defn`s never resolving). Both are now fixed by construction, not
worked around:

1. **Self-recursion is only safe as a true tail call.** `wire-xml.fk`'s first `wx-parse` draft
   computed a child, made a FURTHER self-recursive call for its siblings, and only then combined
   both results (`(list (cons (wx-pv first) (wx-pv rest)) ...)`) — this crashed (SIGBUS) on any
   non-empty children list, even though the exact same self-recursive-function-with-a-mode-tag
   shape that fixed `json.fk`'s root cause #2 was already in use. Root cause: self-recursion here
   behaves like a loop, not a real stack-frame-per-call — holding a node value live across a
   FURTHER self-call corrupts it. Fix: rewrote the children-parsing mode to be strictly
   tail-recursive with an accumulator (`(cdr-parse 2 ... (cons (wx-pv nxt) acc))`, returning only
   at the very end) — exactly the shape `cser-parse` in `cell-serialize.fk` already, unknowingly,
   had right. `wire-corba-cdr.fk` was written with this constraint already understood, and needed
   it doubly: CDR alignment is itself position-dependent, so its `cdr-emit` threads an explicit
   `buf` accumulator through even the ENCODE side (unlike JSON/XML's plain string-concatenation
   emit, which has no absolute-position dependency and turned out to tolerate non-tail combination
   fine — confirmed directly, not assumed, since `wx-emit`'s sibling-list branch IS non-tail and
   works).
2. **A mundane off-by-one, not an evaluator bug — named so it isn't mistaken for one.** A separate
   `wire-xml.fk` crash after fixing #1 traced to `(add end (add 4 (str_len name)))` when skipping
   a leaf's own closing tag (`"</NAME>"` is `2 + len(name) + 1 = 3 + len(name)` bytes, not `4 +
   len(name)`) — a simple arithmetic mistake, caught by the same bisection discipline, not another
   deep runtime quirk. Worth naming explicitly: not every crash found while building this was a
   new interpreter constraint, and conflating the two would be its own kind of dishonesty.

## Not attempted here (named, not silently assumed complete)

- ~~`json.fk` itself remains unfixed — root cause #3 from~~ **Corrected later the same day:**
  `json.fk` is genuinely fixed now, and "root cause #3" was a misdiagnosis, not a real fourth
  bug — see `2026-07-01-json-fk-actually-fixed.md`. Original text left below unedited.
- `json.fk` itself remains unfixed — root cause #3 from
  `2026-07-01-json-fk-src-scoping-fix.md` (self-recursion in one `if`-branch corrupting sibling
  branches that never take it) is a DIFFERENT, deeper bug than either evaluator issue found and
  fixed in this stone, and still needs its own C-level investigation.
- CDR's IEEE754 `double` binary encoding, GIOP request/reply framing, IIOP transport, TypeCode
  octets — real CORBA scope beyond "the CDR primitive encoding rules, honestly."
- `cell-serialize.fk` task #12 (still pending, inherited by every dialect that reuses its leaf
  model): `null` decodes as int `0` in every dialect here, not a distinct value — no `node_type`
  tag 4 constructor available in Form without depending on `json.fk`'s `intern_node_at`-based one.
