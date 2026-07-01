# Receipt — native path-select off the shell applet, real IEEE754 doubles in CDR (2026-07-01)

Two of the four gaps confirmed with the user this walk: `xmlpath`/`xpath` genuinely off
`shell-applet`, and CORBA CDR's float leaves genuinely IEEE754 bits, not decimal text.
(The other two — the RPC executor closing GAP-T1, and REST content-negotiation — continue in
[receipts/2026-07-01-rpc-executor-and-content-negotiation.md](2026-07-01-rpc-executor-and-content-negotiation.md).)

## `wire-path.fk`: a real, honestly-scoped path query over cell graphs

`tool-channel.fk`'s `xpath`/`xmlpath` tools were plan-only entries projected through
`shell-applet` — a real gap against this repo's zero-bash constraint, not a design choice.
`wire-path.fk` gives them a native path: `wp-select(root, "A/B/C", registry)` walks a cell tree by
Blueprint name, segment by segment, with real breadth (every sibling matching a segment advances,
the way XPath's `/A/B` selects every B, not just the first). `wp-select-xml` is the same select
over `wire-xml.fk`'s `xml-to-cell` for the `xmlpath` half. Scope named up front: no wildcards, no
predicates, no attribute selectors, no `//` — path segments only. `tool-channel.fk`'s `xpath`/
`xmlpath` entries now read `"native"` instead of `"shell-applet"`.

**A real, pre-existing test-reliability bug found and fixed in passing**: `tool-channel-band.fk`
was a bare top-level `(do ...)`, not the defn-wrapped-and-called convention every other band file
in this codebase uses. Confirmed directly: the bare form gave 2559 (original file) / 2904 (after
my edit, unrelated to this bug), while the identical logic wrapped in a `defn` gives the
documented 255 — consistently, both before and after my edit. Fixed by wrapping it, matching
every other band file. This means the file's own documented "Verdict 255" claim had likely never
actually been true when run as literally instructed by its own header comment.

## `wire-corba-cdr.fk`: real IEEE754 double bits, not decimal text

Previous scope note said float leaves "ride as decimal text ... a named simplification." Closed:
`cdr-put-double`/`cdr-get-double` do genuine sign/exponent/mantissa decomposition and packing —
normalize into `[1,2)` via repeated halving/doubling (`cdr-f64-normalize`), extract the 52-bit
mantissa via `frac * 2^52` then `float_to_int`, pack into 8 big-endian bytes per the real IEEE754
double layout (byte0 = sign + exponent-high-7; byte1 = exponent-low-4 + mantissa-high-4; bytes
2-7 = the remaining 48 mantissa bits). Verified against the ACTUAL known bytes for 3.5
(`0x40 0x0C 0x00 0x00 0x00 0x00 0x00 0x00`), not just a round-trip that happens to agree with
itself. Named scope limit: no NaN/Infinity/denormals/negative-zero — finite normal doubles only,
the same range `intern_trivial_float`/`float_to_str` already handle.

`intern_trivial_float` was confirmed (directly, not assumed) to require decimal text, not a raw
float — passing it a computed double silently produced `0`. So reconstructing a Form value from
the decoded bits still goes through `float_to_str` first, same as every other float construction
in this codebase; the wire bytes themselves are real IEEE754, only the Form-value boundary is
text-based (a kernel constraint, not a design shortcut).

**A real bug found and fixed while building this**: `cdr-get-double` was inserted right after
`cdr-put-string`, which is BEFORE `cdr-align`'s own definition later in the file — root cause #2
from `2026-07-01-json-fk-src-scoping-fix.md` again (a `defn` calling something not yet defined at
its own definition time silently resolves to nothing), this time self-inflicted by insertion
order rather than discovered in existing code. Confirmed via bisection: a byte-for-byte identical
function body, defined under a different name in a scratch file (so `cdr-align` was already
registered by the time it loaded), worked correctly; the original, mis-positioned definition
didn't. Fixed by moving `cdr-get-double` to right after `cdr-get-string` (after `cdr-align`),
matching where every other `cdr-get-*` function already correctly sat.

## Proof

```
wire-path-band.fk         -> 63   (breadth at one segment, multi-segment chaining, identity
                                    preservation via eq, and the xmlpath text->parse->select path)
wire-corba-cdr-band.fk    -> 255  (was 63; +2 checks: the actual on-wire bytes for 3.5 match
                                    real IEEE754 exactly, and cdr-get-double round-trips them)
tool-channel-band.fk      -> 255  (unchanged number, now reliably so — defn-wrapped)

Full regression, fresh build, clean /tmp/come-in-band-dir:
  ground.fk 42, native-vs-rented 11111, core-band 255, core-str-shim-band 15,
  core-str-narrow-waist-band 255, core-str-find-to-int-band 255, core-float-to-str-band 63,
  reception-consent-band 255, relationship-store-band 31, come-in-band 31,
  cell-serialize-band 1023, wire-xml-band 63
  proof/four-way-run.tbl / flatten/form-eval-cli-loop.tbl — byte-identical (Form-only change,
  no C/optable touched)
```

## Not attempted here

- `learn/tool-channel.fk` (a second copy of the tool-channel planner found during this work) was
  not touched — out of scope, not investigated for whether it's live or vestigial.
- The RPC executor (GAP-T1) and REST content-negotiation continue in the companion receipt.
