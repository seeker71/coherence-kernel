# walkers — minimal four-way proof oracles

These are NOT the runtime. **fkwu** owns the native path (the JIT, the host-OS
surface, the Form→asm lowering). A walker here does exactly one thing: it is an
**independent** witness — its own lexer and its own tree-walking evaluator read a
`.fk` source and compute a value, so a shared parse/semantic bug fkwu's own paths
would miss has a second pair of eyes. That independence is the whole worth: the
scientific-notation `1e-05` float bug and the int64-width literal bug were both
caught exactly this way — one kernel's lexer disagreeing with three.

Keep them thin and shrinking. A walker never duplicates the JIT or the rich host
surface; it only confirms a recipe computes the same value four ways on the
**pure-recipe** surface.

## go

`go/main.go` — the minimal Go walker, extracted from the origin full Go kernel
(`form/form-kernel-go`) by keeping ONLY the independent parse + eval core and
dropping everything else (JIT, server, host-io, model, `.fkb` codec, all tests).
~1369 lines vs the origin's ~15k non-test.

Surface covered: integer + int64 + float + string + bool literals; `add sub mul
div mod`; `eq ne lt le gt ge`; `if let do seq`; `defn` + user calls (tail-call
optimized); `and or not`; `head tail cons list nth empty len`; `str_concat
str_eq str_len str_find substring char_at int_to_str`; `value_eq`; `match`
(switch); plus the BMF s-expression lexer and the content-addressed intern.

Build + run (a band is `core` + recipe + the band file, concatenated):

```
cd walkers/go && go build -o walker .
./walker core.fk recipe.fk band.fk      # prints the evaluated root value
```

Witnessed four-way agreement at landing (2026-06-29): `int-literal-width` → 9,
`string-membership` → 9, both matching the four-way manifest verdict.
