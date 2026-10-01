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

- `go/main.go`, `rust/src/main.rs`, `ts/main.ts`. Each keeps ONLY the independent
  parse + eval core.

A witness that answers by other laws than the kernels witnesses nothing, so each
walker holds the laws of `docs/kernel-interface.md` its surface reaches: integers
are 63-bit two's complement, literals included (law 1); integer `div`/`mod` by zero
stops (law 2); float `mod` truncates (law 5); a float renders the one way
(`1e-05`, `1e+06`, `-0`) and a closure prints `<closure>` (law 9); a float is not
an index; `make_nodeid` keeps the native node word's range law. The reader reads
as fkwu's does: `\n \t \r \" \\` are the escapes and any other backslash stands for
itself; a `.` after the digits makes a float (`5.` is 5.0). `bp` is not a walker
native: its one meaning is the Form resolution in `form/form-stdlib/form-ontology-bp.fk`.

Call heads read as fkwu reads them: a head fkwu reserves (its op rows, rewrite
rows and control forms; `gate/reserved-heads.bml` writes the list beside each
walker) answers as the primitive, or as its shared-Form recipe here, under any
local binding of its spelling; every other head reads the nearest local binding
first.

Surface covered: integer + int64 + float + string literals, and true/false as the
ints 1/0 (axiom-1); `add sub mul
div mod`; `eq ne lt le gt ge`; `if let do`, a three-form `(let name value body)`
binding the name over its body alone, as fkwu reads it; `defn` + user calls (tail-call
optimized); `and or not`; `head tail cons list nth empty len`; `str_concat
str_eq str_len str_find substring char_at int_to_str`; `value_eq`; plus the BMF
s-expression lexer and the content-addressed intern. The
string floor is the narrow waist (`str_len` / `str_byte_at` / `byte_to_str` /
`str_concat`); everything above it is shared Form. `nothing` / `nothing?` are
fkwu natives the walkers do not bind — a band that measures them is fkwu-witnessed.

A walker reads the plain Form files named on argv, joins them in order with one
newline and walks the result as one unit — the contract every sibling kernel keeps.
It follows no directive and lowers nothing; fkwu hands it a unit's whole closure:

```
cd walkers/go && go build -o walker . && cd ../..
./fkwu --closure band.fk band-closure.fk   # from the repo root: one plain-Form file
walkers/go/walker band-closure.fk          # prints the evaluated root value
```

The TS walker runs under `node --experimental-strip-types`; nothing prebuilt is
required. The Go and Rust walkers are build artifacts (`go build -o walker .` in
`go/`, `cargo build --release` in `rust/` → `form-walker-rust`); a fresh checkout
builds them first — the kernel reads an absent walker as a suspect one. The
kernel drives all three itself:

```
./fkwu proof/four-way-run-recipe42.fk   # -> 0 (FOUR-WAY) with the three walkers built
                                        # -> 2 (WALKER-SUSPECT) while a walker is unbuilt
```

The cell hands fkwu's leg in as the constant 42 over a recipe of `(add 40 2)`, so its
fkwu leg cannot fail; the leg that can is the walkers'.
