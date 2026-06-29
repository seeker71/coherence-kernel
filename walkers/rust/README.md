# Rust proof-walker — minimal, home

An INDEPENDENT parse + pure-compute evaluator, extracted faithfully from the
full `form-kernel-rust` (~19.5K src lines, four-way-proven with fkwu) down to
its proof-witness core (~930 lines).

## Why it's here, and why it's small

A foreign walker earns its keep on ONE thing: an *independent* lexer and
evaluator that catch a shared parse/semantic bug the runtime's own paths would
miss. This actually happened — the scientific-notation `1e-05` float bug
surfaced because Rust's lexer was *separate* (and was itself also wrong there,
which is exactly why an independent witness matters). So this keeps:

- the lexer (`tokenize_sexp` + `unescape`) — copied **verbatim**, the current
  fixed scientific-notation version
- the parser (`read_sexp` + `build_verb`)
- the pure-compute evaluator (`walk`)

and drops everything that is *not* the witness: the NodeID content-addressing
substrate, JIT / dylib / asm lowering, the server, host-io / file / socket /
metal, GGUF / model, the binary formats, and all tests / benches.

## Pure-op surface

```
literals     integer, float (incl. scientific notation), string, true/false, ()
build-verbs  do seq let if defn params  add sub mul div mod
             eq ne lt le gt ge  and or not
natives      head tail cons empty list nth len  str_concat str_eq
```

## Run

```
cargo build --release
target/release/form-walker-rust <file.fk> [more.fk ...]
target/release/form-walker-rust --expr "(add 1 2)"
```

Multiple files concatenate with `\n` (preludes first), evaluate, and the final
value is printed — same CLI shape as the full kernel's default source path.

## Four-way agreement preserved

Verified against the full kernel and the `fourth-arm-bands.txt` manifest
(fkwu's recorded verdicts): **183 manifest bands on the pure-op surface run
here and produce the EXACT recorded four-way verdict — zero divergences.**
Bands needing op families outside this surface (BML preludes via the
source-compiler, records, host-io) honestly can't-run here — an empty output,
never a wrong answer.
