# Receipt — Stone 9: the numeric tower (bignum / rational / complex) as composed cells (2026-06-29)

The numeric tower closes as PURE Form recipes composed over cells. No new kernel primitive —
`runtime/fkwu-uni.c` is untouched. bignum / rational / complex are theorems of "axiom-1 integers
under axiom-2 composition", proven on metal via `fkwu --src` and four-way (Go=Rust=TS=fkwu → 127).

## Axiom derivation — the tower IS axiom-1 + axiom-2, nothing bolted on

- **axiom-1 (integers as base)** gives the atom: the kernel's native machine integer, what every
  number in the tower resolves down to.
- **axiom-2 (composition)** gives the structure: a cell is a composition of children. The whole tower
  is that composition over those atoms:
  - **bignum** = a cell composing base-B digit-cells — a little-endian list of native ints, each in
    `[0, B)`, `B = 10000`. Arbitrary precision built ENTIRELY from composition, never a wider word.
  - **rational** = a pair-cell `(num den)` — two composed children.
  - **complex** = a pair-cell `(re im)` — two composed children.

So there is no numeric "type" in the kernel. There are integers (axiom-1) and there is composition
(axiom-2), and the tower is what they make together. The digit/pair structure is the axiom-2 tree; the
leaves are the axiom-1 integers.

## Core-grounded — zero special cases

`model/numeric-tower.fk` is built only on the generic surface every walker shares: `list / cons / head /
tail / len / nth` and the native int ops `add sub mul div mod eq lt gt`. The SAME reducer that walks any
recipe walks these. The base `B = 10000` is chosen so `digit*digit < 10^8` stays inside the native
integer for `bn-mul`, and so each digit's decimal is ≤ 4 chars. No `do`/`let` sequencing, no `append` —
only the pure-op surface (the witness is a nullary `(numeric-tower-check)` over nested defns, which
`fkwu --src` binds and walks natively).

## Observable — each op is an offer/ack over cells

`bn-add` OFFERS two digit-lists + a carry and ACKs the sum digit-list; every column is one offer→ack
step (`s = a0+b0+carry`; emit `mod s B`; recurse with carry `div s B`). `bn-mul` is the schoolbook
fold of single-digit scales, each place-shifted, summed via `bn-add`. `rat-reduce` OFFERS a pair and
ACKs the gcd-reduced pair (Euclid: `(a b) → (b a mod b)`). `cx-mul` OFFERS two pairs and ACKs a pair.
The carry/borrow/gcd recursion IS the trace the observe organ reads.

## Hard gate (`model/tests/numeric-tower-band.fk`) — verdict 127

```
FKWU=127  GO=127  RUST=127  TS=127      ->  fourth arm: four-way, 0 divergent
```

| bit | claim | proven |
|-----|-------|--------|
| 1   | `bn-add 99999999999999999999 + 1` → digit-list of `10^20` | structural `bn-eq` vs `(10^10)^2` |
| 2   | `bn-mul 99999999999999999999^2` → the exact number | `bn-eq` vs `(1 0 0 0 0 9998 9999 9999 9999 9999)` (base-10000 LE of `9999999999999999999800000000000000000001`) |
| 4   | `bn-cmp 10^20 > (10^20 − 1)` | `= 1` |
| 8   | `rat-reduce 4/8` → `1/2` | num=1 ∧ den=2 |
| 16  | `rat-add 1/2 + 1/3` → `5/6` | num=5 ∧ den=6 |
| 32  | `cx-mul (1+2i)(3+4i)` → `(−5, 10)` | re=−5 ∧ im=10 |
| 64  | `cx-add (1+2i)+(3+4i)` → `(4, 6)` | re=4 ∧ im=6 |

The verdict is an integer so string-rendering never affects the proof — every claim is a structural
digit-list / pair comparison built from list+int ops only. `99999999999999999999` (twenty nines, past
2^63 = 9223372036854775808) is built as `9999999999 * 10^10 + 9999999999` through the very recipes
under test; the expected results are constructed INDEPENDENTLY and compared structurally.

## The decimal lane — "its decimal", three-way honest

The gate also asks for the bignum's DECIMAL. `bn-decimal` renders a bignum to a native decimal string
(MS digit bare, lower digits zero-padded to 4 via `bn-pad4`). This needs `int_to_str` / `str_concat`.
The minimal **Rust/TS walkers carry no `int_to_str`** — so the DECIMAL lane is **three-way** by
op-coverage: proven on `fkwu --src` and the Go walker. This is an unsupported-op limitation of the two
minimal walkers, named here — NOT a divergence (every arithmetic op crosses four-way; the digit-list
witness above proves correctness without any string op).

```
$ ( cat model/numeric-tower.fk; echo '(bn-decimal (bn-add (big99) (bn-from-int 1)))' ) | go walker
  100000000000000000000          # 10^20, past 64-bit — exact

# decimal hard-gate as an integer verdict (str_eq internal):
  fkwu --src = 1     go = 1      # "100000000000000000000" matches

$ go walker  <bn-decimal 99999999999999999999^2>
  9999999999999999999800000000000000000001     # == python 99999999999999999999**2, exact
```

## Root-cause note — `fkwu --src` does not bind sequential `let` in a `do`

A first band draft used `(do (let a …) (let b …) … verdict)` and `fkwu --src` returned `0` while
Go/Rust/TS returned 127. This is NOT a recipe divergence: every individual op agreed four-way. The
`fkwu --src` source-runner does not carry sequential-`let`-binding scope inside a `do` (a known
source-runner limitation, an unsupported form — not a wrong answer). The fix is the right on-metal
shape anyway: the witness is a nullary `(numeric-tower-check)` defn over nested expressions and a small
`ntbk` helper — which `fkwu --src` binds and walks natively → **127 on all four including fkwu**. Same
lesson as the band-scope note: compute the witness in ONE form, bind descriptors as nullary defns.

## Edge robustness (fkwu --src)

`0+0 == 0` (canonical empty list); `0*99 == 0`; `x*0 == 0`; `9999+1 == 10000` (carry to a new digit);
`rat-reduce 0/5 → 0/1`. Zero is the empty digit-list; no leading-zero digit is ever kept (`bn-norm`),
so equal values share one shape (content-addressing).

## Honest floor

- **fkwu --src on metal (Mac): observed.** The full band crosses 127 on the c-bootstrapped `fkwu`
  `--src` source-runner — no Go-as-runtime, no flatten, no clang. The decimal lane observed on
  `fkwu --src` + Go.
- **Four-way: observed.** Go=Rust=TS=fkwu = 127 on the arithmetic + structural digit-list surface.
- **Decimal string lane: three-way** (fkwu --src + Go) — Rust/TS minimal walkers lack `int_to_str`;
  named, not hidden.
- **RTX / Android / iPhone --src receipts: pending** — the recipe is pure portable Form (list+int ops),
  so it carries to any `fkwu` build; those platform sessions are the pending rows.

This is a step ON the path to the native ideal: one engine (the recipe that proves four-way IS the
recipe that runs on metal), no parallel native impl, no new C primitive — the tower is a theorem of the
five axioms, not a host accident. It points at the north star — fewer special cases, the model's own
data as recipes — and away from nothing.

Source `model/numeric-tower.fk` sha256: `a8e7df13e5b1efc680e633b628f986829cc0ae6d50d88c2e7c287659ddc44bd8`
Band   `model/tests/numeric-tower-band.fk` sha256: `971f7a64e7f1c7801fdfeb211fdc51e11732f0c764bf33da26d9e506ffee61f0`
