# Receipt — the offer/ack control core: five primitives from ONE mechanism (2026-06-29)

The generic offer/acknowledge **control core** comes home to `control/`. ONE
mechanism — offer a cell, read its ack as exactly one of `{nothing, 0, 1, node}`
— and the five Form control primitives (**fail, stop, choice, exceptions,
async**) fall out as thin expressions over it. Not five special-case recipes:
one core every primitive routes through, derived straight from the five axioms.

## What landed

- **`control/offer-ack-core.fk`** — the core. Two halves:
  - `oac-kind(ack)` — the single discriminator: `NOTHING/ZERO/ONE/NODE`, read by
    `node_category` + `node_eq` (bp content-addresses each arm name).
  - `oac-offer(cell, args)` — invoke/offer a cell, return its ack. This IS the
    kernel's own invocation (axiom-5: to run a cell and to speak to a cell are
    one act), composed, not reinvented — a recipe applied to its args-list.
  - The five, each thin over those two: `oac-fail` = the NOTHING ack;
    `oac-stop` = halt the offer chain; `oac-choice` = first non-nothing wins;
    `oac-try` = exceptions without throw/catch (handler decides on a NOTHING
    ack, else value passes through); `oac-async`/`oac-await` = a pending offer,
    NOTHING until awaited, resolved through the one offer.
- **`control/tests/offer-ack-core-band.fk`** — ten claims, verdict **1023**,
  each witnessing one of the five primitives as an instance of the one core,
  plus discriminator totality across all four arms.

## Grounding (axioms/core-axioms.form, made runnable)

- **axiom-5 offer** — invocation == communication; `oac-offer` IS the
  invocation, not a second mechanism.
- **axiom-1 states** — `nothing` is first-class; fail and stop resolve to it;
  `timeout==nothing` is async's unresolved peek.
- **axiom-4 boundary** — choice / catch / stop are the receiving boundary
  sovereignly deciding which ack it honors — no throw/catch control flow.

## Generic check (the whole point)

The **only** ack-discrimination in the file is `oac-same-bp?`
(`node_eq`/`node_category`), which `oac-kind` alone calls. No per-primitive
dispatch chain — grep-verified. One core, five expressions.

## Honest floor — where it is proven, and the bootstrap-seed edge here

**Four-way proven in the origin body** (`Coherence-Network`,
`control/offer-ack-core.fk` (path corrected 2026-07-01; this receipt originally wrote `form/form-stdlib/`) + band, registered in
`form/fourth-arm-bands.txt`): **1023, FOUR-WAY (Go/Rust/TS/fkwu)** through the
flattener path, which carries the indirect-call op family (tag 44, the
`higher-order-fn-arg` band).

**In this minimal kernel**, the recipe lands native-ready, but re-proving it
*here* hits two bootstrap-floor surfaces, named precisely (a known op-family
limit, never a divergence — no kernel computes a different answer):

1. **The C `--src` source-runner (`runtime/fkwu-uni.c`) does not carry the
   indirect-call family that `oac-offer` needs.** Witnessed, perturbation-clean:

   ```
   (defn dbl (n) (mul n 2))
   (defn ap (f x) (f x))   ; f is a parameter — an indirect call
   (ap dbl 21)
   ```
   → C bootstrap seed: `0`  ·  Go walker: `42`.

   This is **correct by design**: the C parser is the bounded one-time bootstrap
   seed (`runtime/fkwu-uni.c` header: *"do not grow this into a full C
   flattener"*). The real evaluator is **Form** — `grammars/form-eval.fk` runs
   source off the BMF cursor, native via the JIT; the flattener IS Form. The
   indirect call belongs to those Form-native paths (where the origin proved it
   four-way), not the C seed.

2. **The minimal walkers (`walkers/{go,rust,ts}`) are pure-recipe oracles** —
   they carry the indirect call (Go walker returns `42` above) but **not** the
   host-substrate ops (`bp`, `intern_node`, `node_category`) the ack-arms are
   built from. So the walkers cannot witness this particular recipe either; it
   leans on the host-OFFER/INTERN surface only `fkwu` carries.

The closing path is the Form-native eval/flatten lane reaching `control/` here
(the same lane the origin proved on), not growing the C seed. Naming this floor
*is* the practice: the recipe is proven; the re-proof carrier here is pending.

## Build (one cc seed, no toolchain in the run path)

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c
```
