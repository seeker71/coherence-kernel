# Receipt — STONE S8: native indexed ARRAY + content-addressed MAP (2026-06-29)

**S8 — the two oldest data primitives come home, as a Form recipe layer over the EXISTING kernel
core.** No change to `runtime/fkwu-uni.c`'s reducer: the recipe composes only ops the kernel already
exposes to `--src`. Array = axiom-2 (indexed children of a cell); MAP = axiom-3 (content-addressing
IS the hash — same content → same `make_nodeid` node-id → the key).

- Recipe: [`substrate/native-structures.fk`](../substrate/native-structures.fk)
- Band:   [`substrate/tests/native-structures-band.fk`](../substrate/tests/native-structures-band.fk)

## Axiom derivation

- **ARRAY = axiom-2.** A cell composes indexed children; an array IS that cell, read/written by
  position. `a-get i` is the i-th child; `a-set i v` is the same composition with one child
  re-pointed (axiom-3: nothing referenced is overwritten, a new shape is composed). The reachable
  indexed-children carrier from `--src` is the kernel cons-list (`cons`/`nth`/`head`/`tail`).
- **MAP = axiom-3.** Content-addressing IS identity, so a key's content-hash node-id is its address:
  `make_nodeid (list 0 0 0 k)` → same content yields the same node, and lookup is match-by-content
  (`node_eq`). The content-hash is the hash; the map is keyed by what a key IS. `make_nodeid` /
  `node_eq` are the kernel's own content-address ops (optags 91 / 80), used, not added.
- **OBSERVABLE = axiom-5/1.** `a-get` / `m-get` are offers; the ack is a present value on a hit and
  `nothing` (axiom-1) on a miss. `(nothing? (m-get m absent))` is true — the declined ack the
  observer reads, not an error.

## HARD GATE — `--src`, witnessed native on mac arm64

`cc -O2 -o fkwu runtime/fkwu-uni.c` ; `( cat substrate/native-structures.fk; cat substrate/tests/native-structures-band.fk ) | fkwu --src -`

```
ARRAY  a-set(a-make 6 0, 4, 99): a-get(4)=99, a-get(0)=0 (untouched), a-len=6
MAP    7→1 then 3→55 then 7→2 (re-put 7: content-key shadows, latest=2):
         m-get(7)=2   (get-after-put, shadowed)
         m-get(3)=55  (distinct content-key, no collision)
         nothing?(m-get(5)) = 1   (MISSING key acks nothing)

witness = 99 + 6·10⁵ + 2·10³ + 55·10⁷ + 1·10⁹ = 1550602099   ✓  (fkwu --src)
```

Isolated miss-observable check (array OOB → nothing, map miss → nothing, map hit → not-nothing):
`11` ✓ — `(nothing? (a-get arr 9))`=1, `(nothing? (m-get mp 5))`=1, hit is present (0).

## Four-way (where the recipe is value-pure)

**ARRAY side crosses FOUR-WAY.** Value-pure over `cons/nth/head/tail/len/empty/if/eq/le/arith`:

```
arr-witness = a-get(a-set(a-make 6 0, 4, 99), 4) + a-len·10⁵
  fkwu = 600099   go = 600099   rust = 600099   ts = 600099     ✓ four-way
```

**MAP side is fkwu-native only** — an UNSUPPORTED OP, not a divergence. The map key uses the
content-address op `make_nodeid`, which the sibling walkers do not produce: `walkers/go` →
`unbound function "make_nodeid"`; `walkers/rust` and `walkers/ts` error the same way (ts has
`node_eq` but no `make_nodeid`). The op FAMILY (content-address) is absent on the siblings; the
axiom-3 keying is exact on fkwu. `3-kernel only` is honest here, with the op named:
**content-address op `make_nodeid` absent on go/rust/ts walkers.** Exposing `make_nodeid` to the
siblings closes the gap; the recipe shape is unchanged.

## HONEST FLOOR — O(1) and the TCB-follow stone

S8 asked for O(1). The recipe is **O(i)** today, and the reason is named, not papered over:

- **Array** rides the cons-list (the only indexed carrier reachable from `--src`): `a-get`/`a-set`
  are O(i). O(1) needs a **mutable indexed buffer slot** (`ll-buffer`/`fk_buf` get/set) exposed to
  the source layer. The kernel HAS `fk_buf` internally, but **no op-name in `fkwu-optable.h` binds
  it for `--src`** — so it is not source-reachable. **TCB-follow stone: expose the indexed buffer
  slot (and the record constructor) to `--src`.** After that, `a-get`/`a-set` become O(1) with this
  same recipe shape — cons-list → buffer is a carrier swap, the axiom-2 indexing is identical.
- **Map** lookup is an O(n) association scan for the same reason. The kernel's native record
  (optags 64/65/66/67) is already a content-addressed store, but its **constructor (tag 64) has no
  op-name in `fkwu-optable.h`**, so an empty map cannot be bootstrapped from `--src`
  (`record_set` on `(nothing)` returns 0). The **axiom-3 KEYING is already exact and proven** here
  (`make_nodeid` content-hash = the key); O(1)-by-content lands the moment the constructor is
  reachable. Same TCB-follow stone.

## Standard receipt

| field | state |
|-------|-------|
| body | array + content-addressed map as a Form recipe over the kernel core (no reducer change) |
| c-bootstrap | observed — `cc -O2 fkwu` + `--src`, mac arm64 |
| toolchain-free | observed — `--src` only, no go/rust/clang/bash/python in the run |
| platforms.mac | observed (1550602099 native; array 600099 four-way) |
| platforms.windows | pending |
| platforms.android | pending |
| honest-floor | O(i) not O(1) — buffer-slot + record-constructor `--src` exposure is the named TCB-follow stone; map content-addressing fkwu-native (siblings lack `make_nodeid`) |
