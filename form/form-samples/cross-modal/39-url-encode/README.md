# 39-url-encode — RFC 3986 percent-encoding in Form, computed from kernel primitives

> *"we want just primitives in the kernel. and form native code to
> host native assembly using JIT to have generic cross kernel
> functions with host native performance"*  — Urs

## What walked

```
$ ./validate.sh form-stdlib/url-encode.fk form-samples/cross-modal/39-url-encode/url-encode.fk
  ✓  url-encode.fk+url-encode.fk     → enc-hello-ok: 1
                                       enc-path-ok: 1
                                       enc-utf8-ok: 1
                                       round-trip-ok: 1
                                       4
  1 ok, 0 divergent — kernels agree on every sample.
```

Three sibling kernels (Go, Rust, TypeScript) each ran URL percent-encoding
from the Form recipe **only** — no URL-encode native exists in any kernel.
The recipe in `form-stdlib/url-encode.fk` composes the per-byte
conditional pass-through-or-escape from small-int primitives plus
`byte_to_str` / `str_concat`, and produces RFC-3986-correct outputs for
every canonical shape three-way.

- `url-encode("hello world")` = **"hello%20world"** ✓ (space escapes)
- `url-encode("a/b?c=d")` = **"a%2Fb%3Fc%3Dd"** ✓ (URI reserved chars escape)
- `url-encode("héllo")` = **"h%C3%A9llo"** ✓ (UTF-8 multibyte: é = 0xC3 0xA9)
- `decode(encode(bs)) = bs` ✓ (round-trip identity on all three inputs)

**Final verdict: 4** — every canonical vector matches in every kernel.

## The unreserved set (RFC 3986 §2.3)

A byte passes through unescaped iff it is one of:

| Range | Chars | Codes |
|-------|-------|-------|
| `A`–`Z` | upper-case letters | 65–90 |
| `a`–`z` | lower-case letters | 97–122 |
| `0`–`9` | digits | 48–57 |
| `-` `.` `_` `~` | four punctuation marks | 45, 46, 95, 126 |

Every other byte (including space, `/`, `?`, `=`, `&`, `#`, and all
bytes ≥ 128) becomes `%XX` where XX is the **uppercase** hex of the
byte value. UTF-8-encoded text passes through naturally — the 0xC3
0xA9 bytes of "é" become `%C3%A9`, never interpreted as a single
codepoint.

RFC 3986 §2.1 says decoders MUST accept both hex cases. Decode in
this recipe handles `%c3%a9` and `%C3%A9` identically (both → byte
sequence [195, 169]). Encode always emits uppercase, matching
Python urllib, JS `encodeURIComponent`, and Java `URLEncoder`.

## Why byte-lists, not string literals

Multibyte string literals diverge across sibling kernels at the
parser level:

- Rust's source unescape lifts each input byte to a Unicode codepoint
  (`byte as char`), then UTF-8-encodes that codepoint. Byte 0xC3 becomes
  char U+00C3 ("Ã"), which UTF-8-encodes back to two bytes 0xC3 0x83.
  So `"héllo"` (6 bytes on disk) becomes 8 bytes in the parsed string.
- TypeScript stores strings as UTF-16 code units. `"héllo".length` = 5,
  `charCodeAt(1)` = 233 (the codepoint), not 195 (the first UTF-8 byte).
- Go preserves the raw bytes as-is. `len("héllo")` = 6, `s[1]` = 0xC3.

The only fully-portable carrier of arbitrary bytes is a **byte-list**.
So `url-encode` takes a byte-list. For ASCII strings the sample walks
the literal via `substring`/`ord` (which agrees across kernels for
ASCII bytes < 128); for multibyte text we spell the UTF-8 byte sequence
directly: `(list 104 195 169 108 108 111)` for "héllo".

## Recipe behavior

The Form recipe computes percent-encoding
from primitives. Validates four canonical vectors three-way.

## The Form recipe shape

`form-stdlib/url-encode.fk` carries:

```
(defn url-unreserved? (b)
    (if (and (ge b 65) (le b 90))   true     ; A-Z
        (if (and (ge b 97) (le b 122)) true  ; a-z
            (if (and (ge b 48) (le b 57)) true ; 0-9
                (if (eq b 45) true           ; '-'
                    (if (eq b 46) true       ; '.'
                        (if (eq b 95) true   ; '_'
                            (if (eq b 126) true ; '~'
                                false))))))))

(defn url-byte-to-escape (b)
    (str_concat "%"
        (str_concat (url-nibble-to-hex (div b 16))
                    (url-nibble-to-hex (mod b 16)))))

(defn url-encode (bytes)
    (url-encode-loop bytes ""))
```

The hex nibble helper maps `0..9` → ASCII `'0'..'9'` (48–57) and `10..15`
→ ASCII `'A'..'F'` (65–70). All other arithmetic is small-int; the
recipe stays within the kernel's bitwise and integer surface.

## Cross-refs

- [`form-stdlib/url-encode.fk`](../../../form-stdlib/url-encode.fk) — the canonical recipe
- [`form-stdlib/tests/url-encode-band.fk`](../../../form-stdlib/tests/url-encode-band.fk) — sibling-witness band (16 checks)
- 38-hex — unconditional hex envelope (every byte → 2 chars)
- 30-base64 — Base64 from the same string/bitwise primitives
- 34-http-parse — where URL-encoded query strings travel

## Native execution

Run Form source with `./fkwu path.fk` or `./fkwu path.bml` from the
repository root. See [native JIT routing](../../../../docs/native-jit-routing.md)
for the current compiler, emission and dispatch witnesses. This sample's
result checks establish behavior; performance and native entry coverage need
measurements of the executed workload.
