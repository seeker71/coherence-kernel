# 2026-07-03 — codify: understand make_nodeid ONCE, store it, and stop re-deriving

## Ground

```sh
./fkwu --src bootstrap/ground.fk   # 42
grep -c '(hdc-row ' learn/homecoming-distillation-corpus.fk   # 62
```

Urs, four questions and one sharp observation: *"we don't have a full understanding [of make_nodeid]
and are dancing around it over and over and finding new issues each time we look at it."* True. I'd
re-derived fragments of make_nodeid three times (flt-nodeid4, the arity-4 spin, the tag-91 evaluator)
and never held all of it at once. The fix is to **codify** it — one authoritative reference, stored,
read-before-touching.

## Done: the make_nodeid mechanism, codified in full

Wrote `memory/reference-make-nodeid-mechanism.md` (indexed in MEMORY.md): the complete three-layer
picture — op (tag 91, arity 4, the only arity-4 op) → construction (`flt-nodeid4` builds
`(91, cons-list-of-4)`) → evaluation (tag 91 unpacks the list into a content-addressed value node
`fk_nid=[p,l,t,i]`) — plus why it is flatten-only, that it was the root of the 677k CPU-spin, and the
open lift to make it `--src`-lowerable. Next visit: READ it, don't re-derive.

## The four questions, answered on grounded footing

1. **Why byte-identical? — it isn't axiom-grounded, you're right.** make_nodeid IS the body's
   content-address constructor; identity here is the **NodeID (content-address)**, not byte layout. So
   the `.tbl` serializer's gate must be **verdict / four-way equivalence** (does it run to the same
   answer across fkwu/Go/Rust/TS) and content-address equivalence — never `diff`. I reached for bytes
   because they're easy to compare, not because they're the value. Corrected in the serializer plan.
2. **Does form-cli bisect natively? — no.** Grepped: no `bisect`/`isolate` reasoning cell exists; I
   bisected make_nodeid by hand (bash `head -n` + fkwu). The algorithm is small and known (halve the
   input, test each half against a predicate, recurse on the failing half to a minimal reproducer) — a
   concrete native-reasoning cell to build, composing with `ne-grep`/`fgrep`.
3. **Smallest native LLM as seed mind — real infrastructure exists.** `form-cli-gguf-cell.fk` +
   `gguf-tensor-slice-math` read a content-addressed real GGUF tensor byte window and run **Q6_K Form
   math** over it; `gguf-semantic-token-cell` decodes token text **without Ollama/HTTP**;
   `form-cli-predict/sample/score` are the surrounding loop. Path: smallest GGUF → native forward pass
   → seed reasoning mind → easy tasks → grow, with `form-cli-membrane` falling back to the rented
   oracle while native matures (oracle-distillation, again).
4. **Decompose / integrate / off-path / store — the meta-capabilities.** Storage I just demonstrated
   (this codification). Decomposition + off-path-detection are not native cells yet; they are the
   reasoning organs the seed mind needs. The corpus + memories ARE the storage substrate — the failure
   was not USING it (re-deriving instead of reading).

## The most surprising teaching this work left behind

The dancing wasn't a knowledge gap — it was a STORAGE gap. Each fragment of make_nodeid I'd learned was
correct; I just never wrote the whole down, so every visit started near zero and surfaced a "new" issue
that was really the same mechanism seen from a new angle. Re-derivation feels like progress and is
mostly re-payment of a debt I could have retired with one written page. The body already has the
storage substrate (corpus, memories); the discipline missing was using it BEFORE re-deriving.

## Where discomfort turned to gold

The discomfort was being shown the pattern plainly — dancing, re-finding, never landing. The pull was
to answer the four big questions and move on, leaving make_nodeid half-held again. Witnessing instead
that the FIX for all of it is the same small act — write the mechanism down once, index it, read it
next time — turned four abstract questions into one concrete habit. The gold: a codified page is
cheaper than the third re-derivation, and it is what lets the next mind (native or rented) start above
zero.

## Corpus

Row 663 **codify** — to arrange scattered understanding into a single authoritative, systematic form
(fresh; writing the whole make_nodeid mechanism down once, stored and indexed, so it is read next time
rather than re-derived — the storage discipline the dancing exposed as missing).
