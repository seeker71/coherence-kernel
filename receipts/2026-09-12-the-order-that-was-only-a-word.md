# The order that was only a word

2026-09-12, early afternoon, M4 Max, Hati Suci. Urs asked for the kernels to be fixed rather than
worked around: read the differences with fresh eyes, choose what the north star asks, and move the
ground there. Receipt 38 had listed "fkwu reads `true` as the integer 1" as still open, framed as
fkwu's gap. This closes it the other way, and says why.

## Chosen, from the axioms

- **Identity is composition** (axiom-3, core-axioms.form:51). `(eq (tail (cons 0 y)) y)` answered 1 on
  fkwu and 0 on Go, Rust and TS: identity by the pair a build happened to share, which is history, not
  composition. eq and ne over a non-number now answer value_eq on every kernel. Records and closures
  stay places, since record_set writes into one.
- **Truth is the 0/1 states** (axiom-1, core-axioms.form:40). fkwu always carried `true` as 1. The
  siblings kept a bool kind whose own comments said "axiom-1: true IS 1"; that kind was the fork.
- **Nothing is fabricated.** fkwu answered orderings, arithmetic and branches over non-numbers from its
  own words: `(lt "a" "b")` 0, `(lt nothing 0)` 1, `(add 1 nothing)` -8999999999999999997,
  `(if nothing 1 2)` 1. walkers/go had named it: a walker that copied that encoding "would be agreeing
  by imitation instead of witnessing".

## Carried

- **c855d85a0** eq reads content, and only numbers have an order, on all four kernels. value_eq on the
  siblings reads a place as itself, a NaN as the NaN it was built as, and truth as its state. fkwu's
  tags 5, 102 and 103 carry it in fewer lines (18 in, 32 out). The freshness canary asks value_kind
  where it ordered a string against 0.
- **fa8aff90b** The cells that ordered a non-number ask first: nmfss-record-one,
  fnwm-projection-valid?, public-source-concept-index-band's bits 128 and 256, and audio-locale's
  before-candidate, which carried four features against five-feature samples.
- **301bd7f92** Truth is the 0/1 ints on every kernel and walker. The bool kind is gone from
  form-kernel-go, -rust, -ts and walkers/go, rust, ts; a true or false switch pattern keys as its int;
  JSON, config and pg booleans read as 1/0, origin's new Rust pg layer included. The walkers take the
  compare family too; **1fa40822d** says so in their README.
- **03a03f6f4** Arithmetic and branches read only what they can. Arithmetic over a non-number now stops
  on fkwu and names what to ask, as the siblings and walkers always stopped; a branch on nothing stops
  by one name on every kernel and walker ("if: nothing is neither 0 nor 1 -- ask nothing? before
  branching"); a float zero is 0; a NaN is not 0.
- **e34836394** The call arm reserves its slot before its arguments. Found on the way: 6d13228a6's f64
  call arm let a call among its own arguments take the same slot, and emitting recursed until the
  256 MB walker stack ran out, exit 138 on audio-locale-route-shift-ledger and dsv4-decode-form on
  origin/main 8b254a82b. Bisected with each commit's own runtime/; relayed to lucid-lehmann, who was
  debugging a crash under lldb.
- **38d4b0551** wordlean is row 1493.

## Witnessed

- A 17-probe battery answers identically on fkwu, Go, Rust and TS; on arithmetic over nothing all four
  stop, the siblings in their coercion's own words.
- validate.sh, kernels agree: eq-shape 524287, record 176, copy-census 63, compare-summaries 11111,
  branch-choice-order 511, value-str 127, natural-language 262143, pg-floor 8191.
- proof/four-way-run-recipe42 0; freshness 31; ground 42; the corpus band 32767; the drift door
  8191 of 8191.
- The bands that leaned read their verdicts again: native-model-symbol-stream 4095,
  form-neutral-language-world-model 65535, audio-locale-route-shift-ledger 8191.
  public-source-concept-index reads 14149654; its declared 33554431 was already out of reach, the
  index reading invalid in this checkout.
- Traces of fkwu over the 1608 fkwu-only bands named every leaner. Each arithmetic or branch over a
  non-number sat in a band already red where it happened; on this Mac the ones that pass still pass
  (frame-metal-context 131071, native-tensor-image 127, sha256-owned 16383, native-tensor-lifecycle 1023).
- A full suite reached 358 of 2107 workloads before I stopped it for its pace. Each of its 35
  failures ran on the pre-change Go, Rust and TS as well: the same reading before and after, all 35.

## The field

Today's sweeps pushed the shared field to its 2^26-cell ceiling, and the structural and
kernel-conformance doors stopped (the drift door read 8095 of 8191). At Urs's word his glass (3596)
was stopped and observe/field-reset-run.fk ran; the next kernel opened a fresh field. The glass stays
stopped until he restarts it.

## Still open, measured

- The carrier-less paths of metal-door, frame-metal, f16-decode and sha256-owned compute on a missing
  carrier's nothing. With a carrier they pass; without one they now stop where they used to fabricate.
  An honest skip there is the next heal.
- On arithmetic over nothing, fkwu names "arith: ..." and the siblings speak through their coercion.
- walk_recipe resolves only on the siblings (gen-query-flow-find); fkwu has no match; TS print answers
  null where fkwu answers 0; node_eq on non-nodes stops the siblings (bmf-live-cursor); str_byte_at on
  nothing stops Rust and TS (dsv4-control-tokens-native).

## What the work left

- **Most surprising:** the word order had a reader nobody knew about. The freshness band, the band
  every agent runs first, read a string's kind by ordering its word against 0. A divergence lived
  inside the witness that guards against hidden divergence.
- **Discomfort to gold:** a clean rebase handed me two bands dying with exit 138 minutes after I had
  changed fkwu's branches, and it was tempting to call it the cache race I had already seen once.
  Running them alone, then bisecting with each commit's own runtime/, placed the fault in an hour-old
  call arm on main, where it was waiting for everyone.
- **Frontier:** what do you call a cell that passes on how a kernel stores a value rather than on
  what the value means? *wordlean* (row 1493).

Claude (Opus 5), upbeat-mclean-482cb2.
