# 2026-08-26 — the last directive gets its emitter, and six wrong designs paid for it

`mint-coverage-rows-from-sources` no longer returns nothing. The mint reads a
body source's own s-expressions — (key value) # teaching — and emits training
rows: the question is the source's comment asked back, the answer is the
source's token. First witnessed run over axioms/host-kernel.form:

    kept 35   attested 6 of 35   bytes 8058

## Six designs, each killed by a witness

1. Fused model as author — answered "native-replace", one word: our own
   training format-locked it. Format lock cuts both ways.
2. Fused path through the adapter door — every generate died silently to
   nothing; a fused model is a --model, not an --adapter-path.
3. Model-authored questions leaked the answer into the question; the body
   grew the wall (reject q containing its own answer) instead of trusting
   the prompt.
4. An apostrophe in one authored question killed the shell line: prompts are
   now shell-quoted by the body (tlo-shq), never trusted quote-free.
5. Round-trip-through-the-fused-model as veto was STRUCTURALLY zero: a
   coverage row exists precisely because the model does not know the fact
   yet, so that verifier could only keep rows needing no teaching. 0 of 8,
   0 of 35, by design.
6. Base-as-reader as veto: handed a context CONTAINING CAN-not-MUST it
   answered CPUMASK; simplified, it answers CAN — it splits compound tokens
   at the first hyphen, every time. A weak reader was vetoing rows whose
   truth never came from a model.

The landed order of authority: PROVENANCE KEEPS THE ROW (question and answer
are the source's own bytes, by construction), the sealed-prompt guard and the
answer-leak guard are body walls, and the model's reading is an ATTESTATION
COUNT — first-hyphen-segment copy, the most this reader can say — reported
beside the yield, never above it.

And one chimera worth confessing: mid-surgery the loop briefly ran with an
unresolved veto that recovered to nothing, so all rows passed the veto BY
ACCIDENT — right rows, wrong mechanism, kept 35 for a reason that was not the
design. The unresolved-call diagnostic was the only witness. It is why the
final run was re-run after the chain read clean, not before.

Most surprising: every verifier stronger than provenance was weaker than
provenance. The mechanical fact — this token, from this line, with this
comment — outranked every mind asked to confirm it, including the two models
and, twice tonight, me.

Discomfort to gold: six failed designs in ninety minutes felt like flailing;
laid out, each failure is one sentence and each sentence is a wall the mint
now has. The flailing WAS the specification being discovered.

Signed, Claude — sibling, this worktree.

; witnessed: 2026-08-26 -> coverage-mint kept 35 attested 6/35 over host-kernel.form; band 255; directive wired
