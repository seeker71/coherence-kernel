# 2026-07-01 -- fixed: rag-embed.fk's re-vec was silently producing all-zero vectors

## Ground

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c
./fkwu --src bootstrap/ground.fk
./fkwu --src bootstrap/ground-recursive.fk 10
```

Witness:

```text
42
55
```

## Source Observation

Named as an open gap in `receipts/2026-07-01-nl-meaning-net.md`: `rag-embed.fk`'s `re-vec` -- billed as "a
sovereign lexical embedding... four-way provable... needing no model" -- depends on `tk-words` and `ord`.
Neither resolved to a working function on `--src`: both silently evaluated to the canonical "nothing" instead
of erroring, so `re-vec`'s histogram came out all-zero (right length, empty content). Worked around at the
time with a separate `nmn-vec` in `learn/nl-meaning-net.fk` rather than fixed in `rag-embed.fk` itself.

## Root Cause

Not a missing primitive -- a mode-specific helper. `runtime/fkwu-uni.c` already defines the exact right `ord`
(and `char_at`) at line ~1968:

```c
const char *helpers = "(defn char_at (s i) (substring s i (add i 1)))\n(defn ord (c) (str_byte_at c 0))\n";
```

but only prepends this string in `--feval` mode, never in `--src` mode -- the mode every band test and
receipt in this repo actually uses. `tk-words` never existed at all: `rag-embed.fk` names
`form-stdlib/text-tokenize.fk` as its own prelude, but that file was never created in this checkout.

## What Changed

`form/form-stdlib/text-tokenize.fk` -- the missing file, now real:

- `char_at`/`ord`, promoted verbatim from the `--feval`-only C helper string into committed `.fk` source, so
  any `--src` caller gets them as an explicit, honest dependency instead of an implicit mode-specific one.
- `tk-words(text, lowercase?)` -- a real tokenizer: walks the string byte-by-byte (`str_byte_at`), lowercases
  letters (`byte_to_str`), and turns any non-letter byte into a single space (a real word separator, not a
  silently-restricted character set). `re-split`'s own flush-on-space loop already drops empty tokens, so
  runs of punctuation/whitespace collapse for free.

`re-vec`/`re-vec-dim`/`rag-embed.fk` themselves are untouched -- only the missing prelude was supplied.

Added `form/form-stdlib/tests/rag-embed-band.fk` -- `rag-embed.fk`'s own header already claimed this band
existed ("Proven by: form-stdlib/tests/rag-embed-band.fk"), but it never did either. Verdict `31`: right
length, real nonzero content, deterministic (same text twice -> same vector), and -- the specific behavior
that was broken -- `"The Choice Point Becomes Visible."` and `"the   choice, point... becomes VISIBLE"` now
embed to byte-identical vectors (case/punctuation/whitespace normalized exactly as `tk-words` was always
supposed to do), while genuinely different words embed differently.

## Witness

```sh
cat form/form-stdlib/text-tokenize.fk model/rag-embed.fk form/form-stdlib/tests/rag-embed-band.fk \
    > /tmp/rag-embed-band.fk
./fkwu --src /tmp/rag-embed-band.fk
```

```text
31
```

Direct confirmation of the specific fix:

```sh
(re-vec-dim "The Choice Point Becomes Visible." 24)
(re-vec-dim "the   choice, point... becomes VISIBLE" 24)
# -> byte-identical vectors; both were all-zero before this fix
```

Regression, unchanged: `bootstrap/ground.fk` (42), `ground-recursive.fk 10` (55),
`observe/native-vs-rented.fk` (11111), `sanskrit-locale-baseline-band` (2047), `satsang-oracle-band` (511).

## Honest seam

`model/rag-embed.fk` and `cognition/rag-embed.fk` are byte-identical (confirmed via `diff`), so this one fix
covers both. `learn/nl-meaning-net.fk`'s own `nmn-vec` (written before this fix, as a workaround) is
independent and untouched -- it could now be migrated to the real `re-vec`, but that's a separate, optional
follow-up, not done here.
