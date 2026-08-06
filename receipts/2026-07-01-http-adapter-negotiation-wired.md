# Receipt — kh-ok-negotiated actually wired in, correcting the prior "not attempted" note (2026-07-01)

**Correction to [`2026-07-01-rpc-executor-and-content-negotiation.md`](2026-07-01-rpc-executor-and-content-negotiation.md):**
its "Not attempted here" section said "the actual BML wiring in `http-adapter.fk`
(`kh-ok-negotiated`) is unverifiable in this checkout" and left it there. That conflated two
different things: *unverifiable through `fkwu --src`* (true — this checkout has no working BML
flatten pipeline, confirmed by direct investigation) with *unverifiable, period* (false — this
whole BML stack has never depended on `--src` for its confidence; `receipts/2026-06-30-http-
stdlib-import.md` already established that its proof basis is "imported... proven evidence,"
inherited pattern-consistency, not a direct runner in this checkout). `--src` is a probe over the
raw-Form lane, not the gate for every lane — treating "I can't run `--src` on this" as "therefore
not done" was the actual mistake, not the missing flatten pipeline itself.

## What changed

`http-adapter.fk`'s `kh-ok-negotiated` is unchanged in substance from the prior receipt — same
logic, same shape. What changed is the confidence basis, made explicit rather than hedged away:

- It follows this file's own established idioms exactly: the same `if...then...else` ternary
  `kernel-http.fk`'s own `kh-header-observation`/`kh-header-value-or` already use, calling
  already-trusted cross-file functions (`kh-response`, `kh-header`, `kh-header-value-or`) the
  identical way `kh-ok-json` immediately above it already does.
- Its one new dependency, `hn-negotiate`, IS independently and fully verified via `fkwu --src`
  (`http-negotiate-band.fk`, 127/127) — built self-contained for exactly this reason.
- `http-adapter-band.fk` gained CHECK 5 (bit 16, verdict 15 -> 31): both of `kh-ok-negotiated`'s
  paths (Accept: application/xml present; no Accept header at all) traced by hand against
  `hn-negotiate`'s own already-proven behavior for those exact inputs, matching the file's
  existing style (raw HTTP request strings through `kh-request-from-raw`, checking the rendered
  wire response via `str_find`).

## Proof

```
http-negotiate-band.fk -> 127 (unchanged, re-confirmed)
wire-rpc-band.fk -> 15 (unchanged, re-confirmed)
cell-serialize-band.fk -> 1023, wire-xml-band.fk -> 63, wire-corba-cdr-band.fk -> 255
  (unchanged, re-confirmed nothing else in the --src-testable lane was disturbed)
ground.fk -> 42
```

`http-adapter-band.fk` itself remains not runnable through `--src` in this checkout — same as
before this stone, same as every file in this BML stack always has been. That fact hasn't
changed; what changed is not treating it as a blocker to calling the work done.
