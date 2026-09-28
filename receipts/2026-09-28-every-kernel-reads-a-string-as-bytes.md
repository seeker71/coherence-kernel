# Every kernel reads a string as bytes

fkwu is the gold: its string is a byte buffer. Go's already was, so Go needed
only `byte_to_str` (one raw byte, not a code point) and `char_at` (core.fk's
`(substring s i (add i 1))`, one byte). TS and Rust held text.

TS now holds a Form string as a byte string, one code unit per byte. Sources
are read as latin1, so a literal `"Ω"` is the bytes 206 169; `byte-host.ts`
is the one place text meets bytes (paths, URLs, SQL leave as UTF-8 text; names
and bodies from the host arrive as bytes; file contents cross raw). Rust holds
a `Bstr` (shared bytes): a Rust `String` is its UTF-8 bytes, so text becomes a
`Bstr` byte for byte, and what a `str` could not hold is held too. Text is
asked for only where text is meant; `as_str` stops, catchably, on bytes that
are not text. FORMBIN2 keeps its contract: artifact strings are UTF-8 text,
validated, then held as bytes.

Readings, all four arms: wav-emit 4095 (Rust and TS answered 2049, so the band
was fkwu-only), string-boundary 8, byte-waist 255, substring-one-meaning 4095,
str-find-one-meaning 8191, nl-many 268435455, pdf-text-file 15, file-bytes
127; kernel conformance whole; every TS and Rust unit test and the browser
proof pass.

The cursor also reads BML's operators now: the spaced operators by the source
compiler's precedence, `!`, the four comparison casts and `??`, lowered NodeID
for NodeID with the source compiler (seven texts pinned in
form-bml-cursor-full-band). The prefix engine reads infix, and a `<` that opens
no generic call ends a chain as the parser does.

**Surprise.** A string that wraps an expression the reader never asked for is
still a change of meaning: the `??` rule's wrapper around every operand moved
the parse band's count and hid the canonical final `0;` from the prefix-choice
reader. The template that answers an operand as itself when no `??` follows
keeps the tree exactly as it was.

**Discomfort to gold.** Mid-regeneration I edited `cache.fk`, a sealed source,
and the bundle's seal would have refused a second time. Setting the edit aside
until the seal closed, then batching every sealed edit before one regeneration,
turned a race into a rhythm.
