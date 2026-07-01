# Receipt — CORBA-DII RPC executor (GAP-T1), REST content-negotiation (2026-07-01)

The last two of the four gaps confirmed with the user this walk. (The first two — native
`xpath`/`xmlpath`, real IEEE754 doubles — are in
[receipts/2026-07-01-path-select-and-ieee754-double.md](2026-07-01-path-select-and-ieee754-double.md).)

## `wire-rpc.fk`: a real executor closing GAP-T1

`docs/coherence-substrate/tool-grammar.form` names GAP-T1: `tool-channel.fk` plans the
`kernel:call` protocol's `call` tool, but — by its own header — "never executes a host process,
reads a file, sends HTTP, or invokes a kernel route." Its own text: "the remaining runtime work
is the per-protocol executor behind the offered channel, not a Python carrier that owns tool
semantics." `wire-rpc.fk` is that executor: `rpc-call(op, args, registry)` marshals `(op, args)`
into a real CDR request buffer, unmarshals it, dispatches by operation name to a small, real,
pure-Form operation table (`echo`, `count-children`, `negate`, plus an honest "no such operation"
result for anything else — never a crash, never a shell command), marshals a genuine result cell
into a CDR response buffer, and unmarshals it back. Every arrow in that chain is a real wire
crossing (verified: the operation table is only ever reached through `rpc-unmarshal-request`,
never called directly from the test), matching a CORBA DII (Dynamic Invocation Interface) call
by name with no static IDL stub — the shape this kernel's lack of an IDL compiler already forced
`wire-corba-cdr.fk` toward. Scope named, inherited from `wire-corba-cdr.fk`: no GIOP framing, no
IIOP transport, no real cross-process boundary.

## `http-negotiate.fk`: real REST content-negotiation

Closes: "Content-Type is a hardcoded `application/json` on every response — no Accept/
Content-Type-aware dispatch exists." `hn-negotiate(accept-header, available-types)` parses a real
Accept header (comma-separated media ranges, `;q=` weights, `*/*` and `type/*` wildcards, a
missing/empty header defaulting to the server's first preference, an unmatched header falling
back to the same rather than silently producing nothing) and picks the best available
representation — verified against seven real scenarios including two where a specific q-value
ordering flips which of two acceptable types wins, not just the trivial exact-match case.

**A real, checkout-wide limitation discovered while trying to wire this in, not assumed**:
`kernel-http.fk`/`http-adapter.fk` (the actual HTTP request/response value types) are
BML-authored (`section [form.bml] { def ... }`), and this checkout has **no working BML
flatten/lowering pipeline at all** — confirmed by direct investigation (a dedicated research
pass, not a guess): `flatten/SEED-DROP.md` names the BML-to-table path blocked, the proven-bodies
registry lists the whole `kernel-http`/`http-*` stack as depending on `.tbl` files that don't
exist in this checkout, and concatenating `http-adapter-band.fk` (or `http-serve-band.fk`, a
second, unrelated test — confirming this is checkout-wide, not file-specific) with its BML
preludes and running `fkwu --src` gives `0`, not the documented verdict, both before and after
any change here. `receipts/2026-06-30-http-stdlib-import.md` already named this: the whole
BML HTTP stack was "imported with their old proven evidence... not re-proven here through the
direct fkwu --src runner."

Given that, the responsible move was NOT to modify BML code I have zero ability to run or verify
in this environment and call it done. `http-negotiate.fk`'s algorithm is built independently,
fully self-contained, fully verified via bare `fkwu --src` (real bytes in, real Accept-header
parsing, real q-value comparison, real answer out — not a stub). Wiring it into
`http-adapter.fk` is a small, mechanical BML addition (`kh-ok-negotiated`, mirroring the existing
`kh-ok-json`'s shape exactly) that inherits the file's own pre-existing "not re-proven here"
status — named explicitly in both files' headers, not silently left for someone else to discover.
`http-adapter-band.fk` was deliberately NOT given a new check for this: adding an assertion I
cannot actually run and claiming a verdict number for it would be a false claim, the same mistake
already corrected once this session (`2026-07-01-json-fk-src-scoping-fix.md`'s "verdict 63" note).

## Two mundane paren mistakes, named so they aren't mistaken for evaluator bugs

Building `http-negotiate.fk`, a naive comment-stripping check (`sed 's/;.*$//'`) flagged a false
imbalance — the file's own Accept-header logic legitimately contains `";"` and `","` as
**string literals** (parsing HTTP's own delimiter characters), which a comment-stripper that
doesn't understand quoting misreads as a comment start mid-line. Chased that down with a
string-aware checker before touching any code — the file was correct as originally written; an
earlier "fix" attempted against the false signal was reverted. `tests/http-negotiate-band.fk`
had one real off-by-one (a leftover extra closing paren from copying a 6-check band's ending
onto a 7-check band) — a genuine, mundane paren-count mistake, not a runtime quirk.

## Proof

```
wire-rpc-band.fk           -> 15   (echo/count-children/negate/unknown-op, each through the
                                     full marshal -> dispatch -> marshal round trip)
http-negotiate-band.fk     -> 127  (no-Accept default, exact match, unsupported-type fallback,
                                     two q-value-ordering cases, both wildcard forms)

Full regression, fresh build, clean /tmp/come-in-band-dir:
  ground.fk 42, native-vs-rented 11111, core-band 255, core-str-shim-band 15,
  core-str-narrow-waist-band 255, core-str-find-to-int-band 255, core-float-to-str-band 63,
  reception-consent-band 255, relationship-store-band 31, come-in-band 31,
  cell-serialize-band 1023, wire-xml-band 63, wire-corba-cdr-band 255, wire-path-band 63,
  tool-channel-band 255
  proof/four-way-run.tbl / flatten/form-eval-cli-loop.tbl — byte-identical (Form-only change,
  no C/optable touched)
```

## Not attempted here

- `wire-rpc.fk`'s operation table is demonstrative (3 real operations), not a production catalog
  — adding a real one is one `if str_eq` branch, named directly in the file's own header.
- The actual BML wiring in `http-adapter.fk` (`kh-ok-negotiated`) is unverifiable in this
  checkout — fixing the BML flatten pipeline itself is real, separate, sizeable infrastructure
  work, out of scope here and not attempted.
- ~~`json.fk`'s root cause #3~~ — **corrected later the same day**: `json.fk` is genuinely fixed
  now, root cause #3 was a misdiagnosis, see `2026-07-01-json-fk-actually-fixed.md`. CDR's
  GIOP/IIOP/TypeCode scope is unchanged from prior receipts (a real, deliberate scope boundary,
  not a bug).
