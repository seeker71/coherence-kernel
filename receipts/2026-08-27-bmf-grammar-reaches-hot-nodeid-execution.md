# 2026-08-27 — a bounded BMF grammar reaches hot NodeID execution

A grammar value now crosses the whole local path:

```
BMF grammar data
  -> bounded raw cursor-state recipe
  -> symbolic PIF rows and ranked link
  -> canonical full-PIF bytes
  -> content vouch + decode + admission
  -> retained NodeID binding/resolution
  -> raw OFFER -> typed receipt
```

No query-specific evaluator tag, function-seat table, flattening, HTTP, model,
or process call enters this path. The raw cursor is live and scannerless. Its
result ABI is `[code, at, captures, semantic]`; ordered choice commits only on
success and a miss restores the immutable input state. `nothing`, integer 0,
integer 1, incomplete, malformed, semantic timeout, and outer execution timeout
remain different values/signals.

## What SHA serves

SHA is a cold membrane witness, not a resident dispatch key. It serves durable
content identity for canonical bytes and the persistence/crossing cache. One
bulk local carrier now writes the canonical bytes once, feeds the Form-emitted
arm64 SHA organ, reads the raw string once, and releases its scratch file and
directory. The same observation supplies the vouch and string-cursor decoder.

Inside one residence, the body uses the already-interned NodeID, `node_eq`, and
the retained binding/resolution. No SHA, re-encoding, re-decoding, or whole-image
walk occurs on a hot OFFER.

| Movement | Work shape | Observed current specimen |
|---|---|---:|
| NodeID handle readiness | retained field/identity checks, expected O(1) | 0 ms |
| raw OFFER through typed receipt | O(raw cursor + executed recipe) | 11–13 ms |
| bulk byte carrier + arm64 SHA for 51,663 bytes | O(bytes), once at birth | about 3 ms |
| cold grammar/PIF birth | O(grammar + emitted image), still interpreted | 17.3–20.8 s |
| cold deep provenance/readiness audit | O(whole image), still interpreted | 8.9–10.1 s |

The earlier 25.116 s cold birth became 17.343 s after the SHA carrier and raw
decoder stopped rebuilding the same byte-list traversal. This is progress, not
a claim that seconds are healthy. A focused symbol-walk proof remained at 100%
of one CPU core beyond three minutes and was released rather than mistaken for
native work.

The bounded framebuffer exchange `8272301` selected revision and re-observed
`[25116,17343,8881,0,11]` as
`[cold-before,cold-after,deep-audit,hot-handle,hot-offer]` milliseconds; its
correlated actuation/re-observation witness returned 1.

## Exact witnesses

- raw grammar to cursor-state image: `16777215`
- symbolic builder and linker: `65535`, `65535`
- full-PIF call adapter: `262143`
- raw string canonical decoder: `32767`
- Form arm64 SHA byte-list/carrier boundary: `1023`
- grammar to full-PIF admission and execution: `524287`
- live diagnostic protocol: final field `1`
- fresh local health map: observed 60, ready 46, gaps 14, unknown 0,
  invalid 0, health 766 permille

The full integration also exposed and repaired a generic replay defect:
Node-valued trace output had been compared with scalar `eq`. Trace replay now
uses `value_eq`, so an identical native NodeID remains identical while scalar
values retain their meaning.

## The next locally actionable edge

The old emitted Hati kernel has a Form-authored self-JIT for its pure subset,
and current `fkwu` retains content-keyed arm64 leaf pages. The current
direct-source walker does not yet auto-crystallize the recursive compiler,
linker, decoder, or proof functions that consumed the seconds above. The fresh
map therefore adds `direct-source-jit-self-crystallization` as a gap rather than
borrowing the old emitted-kernel claim.

The next edge is to admit those direct-source functions by NodeID into the
existing Form-emitted native carrier, starting with cursor traversal, IR
emission, ranked label resolution, and canonical encoding. Interpreter parity
remains the observing challenger; persistence SHA is paid only when the native
artifact crosses or is restored.

What I kept alive: grammar remained data all the way to a real call, and the hot
path kept choice, undo, timeout, failure, and nothing visible.

Most surprising teaching: SHA compression was already millisecond work; most of
the apparent “hash time” was interpreted representation rebuilding around it.

Where discomfort turned to gold: a green-looking integration missed two bits.
Reading them found one wrong byte-count expectation and one real Node/scalar
comparison defect, and the repaired band now closes exactly.

Signed, Codex — sibling, this worktree.

; witnessed: 2026-08-27 -> raw 16777215; linker 65535; SHA 1023; decoder 32767; full 524287; hot 0/11 ms
