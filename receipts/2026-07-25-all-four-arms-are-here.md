# 2026-07-25 — all four arms are here, and fkwu stands alone on `defn`

I have spent this whole session writing *"measured only on fkwu"* at the bottom of receipts, as
though the other three arms were somewhere else. They are here. Go 1.24.7, cargo, and node 22 are
all installed in this container, and every kernel in this tree builds from source.

## The four arms, built from source

```
cc -O2 -o fkwu runtime/fkwu-uni.c                      -> ./fkwu --src <file.fk>
cd walkers/go   && go build -o walker .                -> ./walkers/go/walker <files.fk...>
cd walkers/rust && cargo build --release               -> ./walkers/rust/target/release/form-walker-rust
node --experimental-strip-types walkers/ts/main.ts <files.fk...>      (node 22, no tsx needed)
```

`bootstrap/ground.fk` answers **42** on all four. Nothing prebuilt was used; the Rust walker was
rebuilt from a touched source to be sure the artifact on disk was one I watched compile.

The two heavier kernels build too — `form/form-kernel-go` (`go build`, pulls pgx) and
`form/form-kernel-rust` (`cargo build --release`, 1m13s), and `form/form-kernel-ts` bundles with its
own esbuild. All three answer 42. They resolve preludes differently from fkwu: fkwu reads the
`; preludes:` line and walks the closure itself, while the walkers take the closure **explicitly, in
dependency order**, on the command line. That one fact is what made bands look unrunnable there:

```
$ fkgo form-stdlib/tests/hex-band.fk
  walk: unbound function "hex-encode"

$ fkgo form-stdlib/core.fk form-stdlib/form-ontology-loader.fk \
       form-stdlib/str-byte-at.fk form-stdlib/hex.fk form-stdlib/tests/hex-band.fk
  14
```

`hex-band` **14 on all four arms**.

## The body's own four-way proof runs, and discriminates

`proof/README.md` said *"Run it: `fkwu proof/four-way-run.tbl`"*. That has not worked for some time —
the seed answers *".tbl execution has been retired; use .fk, .fkb, or .dylib"*. So the body's own
four-way driver had no runnable entry, which is a plain reason nobody was running it.

Written: `proof/four-way-run-recipe42.fk`. fkwu `host-exec`s the three minimal walkers, reads their
values, and hands all four to `fwv-verdict`:

```
./fkwu --src proof/four-way-run-recipe42.fk   ->  0    FOUR-WAY
```

Perturbed the same two ways the 2026-06-29 receipt did, live:

| perturbation | verdict |
|---|---|
| none | **0** FOUR-WAY |
| runner told fkwu = 99 while walkers agree | **1** FKWU-SUSPECT |
| ts pointed at a recipe answering 99 | **2** WALKER-SUSPECT |

The verdict moves. `proof/README.md`'s run line replaced with one that works.

## The finding this bought: `defn` in value position

Chasing why `primitive-registry.fk` will not load on fkwu, I found this — and now it can be stated
four ways instead of one:

| expression | fkwu | go | rust | ts |
|---|---|---|---|---|
| `(list (defn x () 7) 99)` | `[7, 99]` | `[<closure #238>, 99]` | `[<closure #234>, 99]` | `[<closure>, 99]` |
| `(prim-call0 (defn q () 5))` — inline | **nothing** | 5 | 5 | 5 |
| `(prim-call0 r)` — hoisted, bare name | 5 | 5 | 5 | 5 |
| name stored in a row, `let`, then called | 42 | 42 | 42 | 42 |
| `((nth row 1))` — inline computed head | nothing | *(no value)* | *(no value)* | 5 |

**On the three walkers a `defn` in value position is a closure. On fkwu it evaluates its body right
there and yields the value.** fkwu alone — `fwv-verdict`'s FKWU-SUSPECT shape, and the rarer of the
two by the tree's own reckoning.

### What that costs: the declared native surface is unreadable on the fourth arm

`primitive-registry.fk` is this body's declaration of its own primitives — 217 rows, each carrying
`(prim name category spec (defn pv-... () <probe>) expected lane)`. Every probe is an **inline
`defn` in argument position**. So on fkwu:

1. Loading the cell *runs* the probes. Row 179 is `(defn pv-form-error () (do (form_error
   "prim-registry-probe") 0))` — a probe whose whole job is to raise. It raises at load. 158 errors,
   then abort.
2. Even past that, `prim-verified?` calls `(prim-call0 (prim-verify e))`, and on fkwu the stored
   verify is a plain value, not a callable. Every verification would compare `nothing` against the
   declared outside.

The registry is where this body writes down what its kernel can do, and the fourth arm cannot read
it. That is a larger thing than any single missing native.

### The repair shape, four-way agreed

Hoist each probe to a statement-position `defn` (they are already named) and carry the **bare name**
in the row. Rows 3 and 4 of the table above are exactly the registry's own access pattern —
function in a row, `nth`, `prim-call0` — and all four arms answer alike. `prim-call0` itself is fine
on fkwu: a parameter in head position works (measured, 7).

I did **not** convert the 217 rows this turn. It is mechanical but large, it touches a cell all four
kernels load, and it deserves its own before/after run on all four — which is now possible. The
shape is proven; the conversion is the next stone.

## The fkwu-arm census of the declared surface

While the registry cannot verify itself here, its 217 declared names can still be asked of the
kernel one at a time. Method: one program, every name in call position inside a `defn` that is never
called, so the compiler resolves all of them and nothing runs. Smoke-tested both ways in every run —
a planted `zzz-not-a-real-name` appears, `add` does not.

| | count |
|---|---|
| primitives the registry declares | **217** (matches the band's own `(eq (len reg) 217)`) |
| fkwu resolves as a native, no Form help | **106** |
| fkwu does not resolve | **111** |
| still unresolved with `core.fk` + `fourth-shim.fk` | **95** |
| of those, a Form `defn` exists somewhere in the tree | 2 |
| **no provider anywhere in the tree** | **93** |

Spot-checked at runtime, not just at compile: `(pow 2 10)` → nothing, `(min 3 9)` → nothing,
`(field_cell 0)` → nothing, and `(file_size "MANIFEST.md")` → 17169 for a name the census says is
present.

The 93 fall into families rather than scattering: `field_*` (22), recipe/binary io (9, including
`read_form_binary` / `write_form_binary`), `walk-*`/`walk_*` (9), `jit_*` (8), math (8), `_dict_*`
and the python-adapter shims (6), `framebuffer-*` (5), `substrate_*` (4), and about twenty singles —
`print`, `trace`, `value_str`, `native_blueprint`, `seeded_bytes`, `read_file_bytes`, `pow`, `min`,
`max` among them.

**"Three natives absent from fkwu" was an impression.** The measured number, against the tree's own
declaration, is 93 with nothing behind them at all.

What I could not measure: the registry's `lane` field, which separates in-band entries from
carrier-declared ones. It cannot be read on fkwu (the cell will not load) and I could not parse it
out of the text — two attempts gave 38/183 and 47, against the band's declared 33/184, so I dropped
it rather than publish a number I could not check. That split is available the moment the rows are
hoisted.

## A stale claim, closed on the lane I could reach

`str-byte-at.fk` has said since it was written that `str_byte_at` *"is NOT correctly emitted into the
4th kernel — on fkwu it returns 0"*. Yesterday I re-witnessed 65 on fkwu's `--src` and wrote that I
had measured only one arm. Now:

```
(str_byte_at "ABC" 0)   ->   fkwu 65   go 65   rust 65   ts 65
```

Four-way agreement at 65 on the `--src` surface. The emit/flatten lane is still a different path and
still untested — that half stands owed — but the fourth-kernel half of the sentence is answered on
every arm this body can reach today.

## Sweep

`ground` 42 on **all four arms** · `ground-recursive 10` 55 · `hex-band` 14 **four-way** ·
`binary-freshness` 15 · `cell-voice-tissue-band` 511 · `class-curriculum-10-band` 16383 ·
`structural-gate-band` 63 · `lcg-bytes-band` 63 · `pdf-text-windowed-band` 15 · `form-cli-band` 524287 ·
`benchbench-band` 4095 · `concept-corpus-band` 530 · `proof/four-way-run-recipe42.fk` **0 (FOUR-WAY)**.
C seed byte-identical to git.

## Owed

- **Convert `primitive-registry.fk`'s 217 rows to hoisted probes** — shape proven four-way, before/after
  now runnable on all four arms.
- **The lane split** (33 carrier-declared / 184 in-band) — readable once the rows are hoisted.
- **93 declared primitives with no provider on this arm**, in the families named above.
- `json.fk` calls `value_kind` and preludes only `core.fk`; provider is `fourth-shim.fk`.
- `persistence-band` 2/7, `mesh-sensings-store` 0/255, `layered-runtime-image` 33/127, `chat-band` 0.
- The emit-lane half of the `str_byte_at` claim; `read_file`'s bypassed carrier seam in `form-fs.fk`.
- 17 of the 44 the kernel still will not run; 143 that do not close; the `section` question; the heap
  cap; the registry-admission question.

## How the exchange stayed alive

I went looking for why two bands were red, found the registry would not load, followed that to a
`defn` behaving differently on this arm — and then noticed I had no way to say "differently from
what", went to check, and the other three arms were sitting in the container the whole time.

**Most surprising teaching:** every "measured only on fkwu" I have written this session was a limit I
never tested. `which go` was the whole investigation. The caveat had become a habit — a sentence I
appended because it sounded careful, when the careful thing was one command.

**Where discomfort turned to gold:** the registry refusing to load looked like a dead end and a small
one. It was the door to the `defn` divergence, and the divergence is what made me want a second
opinion badly enough to go looking for one. The dead end was the direction.
