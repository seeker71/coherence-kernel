# walkers — minimal four-way proof oracles

These are NOT the runtime. **fkwu** owns the native path (the JIT, the host-OS
surface, the Form→asm lowering, Metal). A walker here does exactly one thing: it is an
**independent** witness — its own lexer and its own tree-walking evaluator read a
`.fk` source and compute a value, so a shared parse/semantic bug fkwu's own paths
would miss has a second pair of eyes. That independence is the whole worth: one
kernel's lexer disagreeing with three is how a float-literal bug and an int64-width
literal bug were caught.

Keep them thin and shrinking. A walker never duplicates the JIT or the rich host
surface; it only confirms a recipe computes the same value four ways on the
**pure-recipe** surface.

## The three

- `go/main.go` (1,524 lines), `rust/src/main.rs` (1,331), `ts/main.ts` (1,678) —
  counted 2026-09-04. Each keeps ONLY the independent parse + eval core.

Surface covered: integer + int64 + float + string + bool literals; `add sub mul
div mod`; `eq ne lt le gt ge`; `if let do seq`; `defn` + user calls (tail-call
optimized); `and or not`; `head tail cons list nth empty len`; `str_concat
str_eq str_len str_find substring char_at int_to_str`; `value_eq`; `match`
(switch); plus the BMF s-expression lexer and the content-addressed intern. The
string floor is the narrow waist (`str_len` / `str_byte_at` / `byte_to_str` /
`str_concat`); everything above it is shared Form. `nothing` / `nothing?` are
fkwu natives the walkers do not bind — a band that measures them is fkwu-witnessed.

Build + run (a band is `core` + recipe + the band file, concatenated; bare
`import "path.fk"` declarations resolve recursively):

```
cd walkers/go && go build -o walker .
./walker core.fk recipe.fk band.fk      # prints the evaluated root value
```

The TS walker runs under `node --experimental-strip-types`; nothing prebuilt is
required. The Go and Rust walkers are build artifacts (`go build -o walker .` in
`go/`, `cargo build --release` in `rust/` → `form-walker-rust`); a fresh checkout
builds them first — the kernel reads an absent walker as a suspect one. The
kernel drives all three itself:

```
./fkwu proof/four-way-run-recipe42.fk   # -> 0 (FOUR-WAY; re-run 2026-09-04)
                                        # -> 2 (WALKER-SUSPECT) while a walker is unbuilt
```
