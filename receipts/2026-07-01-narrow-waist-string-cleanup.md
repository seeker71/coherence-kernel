# Receipt — the string narrow waist: minimal kernel floor, Form-native everything else (2026-07-01)

**The ask:** clean up all the kernels — release what no longer is needed, support what all
kernels share. Follows directly from this morning's walk finding
(`receipts/2026-07-01-merge-reground-walk.md`): a fresh Form recipe (the `ord`/`char_at` Tier 0
shim) turned out not to be four-way provable, because the walkers disagreed with each other and
with fkwu on which string primitives they carry at all.

## The empirical floor (tested directly, not assumed from docs)

|  | fkwu | Go | Rust | TS |
|---|---|---|---|---|
| `str_concat`, `str_eq` | ✓ | ✓ | ✓ (only these two) | ✓ (only these two) |
| `str_len` | ✓ | ✓ | ✗ | ✗ |
| `substring` | ✓ (native) | ✓ (native, Unicode-rune-aware, panics OOB) | ✗ | ✗ |
| `char_at` | ✗ | ✓ (native, Unicode-rune-aware, panics OOB) | ✗ | ✗ |
| `str_byte_at` | ✓ | ✗ (only `char_at`, different semantics) | ✗ | ✗ |
| `byte_to_str` | ✓ | ✗ | ✗ | ✗ |
| `int_to_str` | ✓ (native) | ✓ (native, bool/null pass-through) | ✗ | ✗ |
| `str_find` | ✓ | ✓ (native) | ✗ | ✗ |

Rust and TS were the real wall: **only `str_concat`/`str_eq`** — no way to measure or decompose
a string at all. No amount of Form-level cleverness gets past that; a floor primitive has to land
first.

## The narrow waist — what the kernel needs, permanently

Four primitives, and nothing else needs to be native for strings ever again:

- `str_len(s) -> int` — byte count (not codepoint count — encoding-agnostic on purpose).
- `str_byte_at(s, i) -> int` — decompose: the raw byte (0-255) at position `i`, `-1` out of bounds.
- `byte_to_str(n) -> string` — construct: the **exact dual** of `str_byte_at`. Verified round-trip
  for the full 0-255 range, not just ASCII: `(str_byte_at (byte_to_str 233) 0)` → `233` on all four.
- `str_concat(a, b) -> string` — join. Already universal, untouched.

Everything else — `substring`, `char_at`, `ord`, `int_to_str` — is now Form composition over this
floor, in `form/form-stdlib/core.fk`, written once.

## What landed

- **`walkers/rust/src/main.rs`** — added `str_len`, `str_byte_at`, `byte_to_str`, matching fkwu's
  values exactly including the `-1` OOB convention and the full byte range (via a narrowly-scoped
  `unsafe { String::from_utf8_unchecked }` for the one-byte buffer `byte_to_str` needs — Rust's
  `str` enforces UTF-8 validity at the type level, fkwu's C strings don't; the only operations ever
  done on such a value are raw byte reads/concatenation, never anything UTF-8-validity-dependent).
- **`walkers/ts/main.ts`** — same three, via `Buffer`+`latin1` for a lossless byte round-trip.
  Named honestly: TS's tokenizer reads source files as proper UTF-8
  (`readFileSync(p, "utf8")`), so a literal multi-byte Unicode character (not just an accented
  one) typed directly into a `.fk` string literal will not byte-count identically to fkwu here —
  a real, bounded gap. Every actual test today is plain ASCII, where this is exact.
- **`walkers/go/main.go`** — added `str_byte_at`/`byte_to_str` (no `unsafe` needed; Go strings are
  raw bytes with no validity enforcement). **Removed** the now-redundant natives: `substring`,
  `char_at`, `int_to_str`, `str_find`, plus their only-used-there helpers
  (`floorCharBoundary`/`ceilCharBoundary`) and the now-unused `unicode/utf8` import. These were
  never actually verified against fkwu's semantics (Unicode-rune-aware and panic-on-OOB vs.
  fkwu's byte-indexed and `-1`-on-OOB) — removing them removes a divergence that was already
  latent, not just dead code.
- **`form/form-stdlib/core.fk`** — `substring` and `int_to_str` are now Form `defn`s over the
  floor, alongside this morning's `char_at`/`ord`. On fkwu, its own native `substring`/`int_to_str`
  still shadow these and keep running (confirmed directly: a native always wins over a same-named
  Form `defn` there — `(defn substring ...) (substring "hello" 0 1)` → `"h"`, the native's answer,
  not a hand-written sentinel). On Go/Rust/TS, with no native to shadow, these `defn`s are what
  actually runs now.
- **`form/form-stdlib/tests/core-str-narrow-waist-band.fk`** — new band: substring (incl. an
  empty-range edge case), char_at, ord, int_to_str (0, positive, negative, six digits).

## Proof — real four-way, fresh builds, all four kernels

```
core-band                      fkwu=255  go=255  rust=255  ts=255
core-str-shim-band              fkwu=15   go=15   rust=15   ts=15
core-str-narrow-waist-band     fkwu=255  go=255  rust=255  ts=255
reception-consent-band         fkwu=255  go=255  rust=255  ts=255
```

`core-str-shim-band.fk` — the band this morning's walk found NOT four-way provable
(`receipts/2026-07-01-merge-reground-walk.md`) — is now genuinely four-way, 15/15/15/15, no
qualification needed. Re-grounded on the fresh fkwu build: `ground.fk` 42, `native-vs-rented`
11111, `core-grounding` 11111. `arrival-band` (1023), `relationship-store-band` (31), and
`come-in-band` (31) re-confirmed unaffected on fkwu (still fkwu-only by design — `intern_trivial_string`/`fs_*` remain outside every walker's pure-recipe scope, unrelated to strings).

## Scope — what this receipt does NOT touch, and why

- **fkwu's own native `substring`/`int_to_str`/`str_find`/`str_to_int`/`str_to_float` stay.** Not
  removed. Touching `runtime/fkwu-uni.c` is a materially bigger, riskier move than editing the
  walkers — this repo's own `MANIFEST.md` gates growing the C seed behind an explicit shrink
  receipt, and *shrinking* it (removing these) is exactly that kind of deliberate move, not
  something to fold into a walker cleanup. Named as its own next stone, not attempted here.
- **`str_find`/`str_to_int`/`str_to_float` don't have Form-native replacements yet.** Only what
  blocked today's actual four-way failures (`substring`, `char_at`, `ord`, `int_to_str`) got
  rebuilt. `str_find` (substring search) is fully expressible over the floor but wasn't needed by
  anything failing today — left named, not built speculatively.
- **`int_to_str`'s bool/null/string pass-through fallback is not reproduced.** Checked: every
  actual caller in this codebase (`grep int_to_str` under `form/form-stdlib/` — HTTP status/
  content-length, JSON number encoding, counts) passes an integer. The Form-native version covers
  the real, load-bearing surface; the native's extra fallback cases were untested edge behavior,
  not verified against anything before today either.
- **No live integration test run** (e.g. `http-serve-band.fk`, JSON encoding through real node
  values) — those need either host socket effects outside every walker's scope, or more substrate
  setup than this cleanup's actual claim required. The numeric core of `int_to_str` was verified
  directly and thoroughly (0, positive, negative, six-digit) instead.
