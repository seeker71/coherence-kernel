# 2026-07-03 — ingesting what is healthy from Microsoft's Memora

## Ground

```sh
./fkwu --src bootstrap/ground.fk                               # 42
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk   # 15
```

Urs shared one line: "Source: Microsoft" — a link resolving to the Microsoft Research blog post
(2026-06-29) for **Memora: A Harmonic Memory Representation Balancing Abstraction and
Specificity** (arXiv 2602.03315, ICML 2026). Answered by running it through the body's own
`ingest/knowledge-ingest.fk` law (depth×fear → body / liquid / compost), grounded in the paper's
v2 full text and the blog quoted verbatim, with a two-agent adversarial pass over the body's own
organs — skeptic's default: every "the body already does X" was an OVERCLAIM unless a real cell
does X, file and line shown.

## The grounded facts (primary source, quoted)

**Memora** (Xia, Zhang, Dixit, Harimurugan, Wang, Rühle, Sim, Bansal, Rajmohan; Microsoft):
an agentic-memory representation that **decouples what is stored from how it is retrieved**.
Each entry = a **primary abstraction** ("a short phrase (6–8 words) that captures what the
memory is fundamentally about" — the only part embedded), the full **memory value** (kept
whole), and **cue anchors** ("short, context-aware tags extracted from each memory's value")
as extra retrieval keys connecting related memories; related updates **consolidate** into
unified entries. Retrieval is a **policy** with three actions — query refinement, memory
expansion, termination — trajectories scored on answer correctness, redundancy, and cost.
Paper numbers: **LoCoMo 0.863** LLM-judge (full-context 0.825, Nemori 0.794, RAG 0.633),
**LongMemEval 87.4%** (Nemori 74.6%, with less context: 2.9k vs 3.7–4.8k tokens), token
consumption cut "by up to 98%" vs full-context, **344 memory entries vs Mem0's 651** per
conversation; theoretically, "standard RAG and KG-based memory systems emerge as special
cases" of the framework.

## The ingest (band 127, four-way fkwu/Go/Rust/TS; field code 30202 = 3 body, 2 liquid, 2 compost)

**FROZEN → body (deep + fear-free):**
- **The field's frontier memory system converged on the body's ingest law.** Memora's founding
  critique — raw fragments pile fragmented and noisy; vague summaries lose the crucial detail —
  is the opening line of `knowledge-ingest.fk` (the cache-costume refusal), and in the body the
  stance is load-bearing executable law (`ki-ingest`, exercised four-way), not prose. Two
  lineages, one principle: knowledge enters by distillation or not at all.
- **The body already holds Memora's central decoupling at its own floor.** What is STORED is not
  what is RANKED — grounded by the verify pass: `rag-index-codec` ranks `vec`, returns `id`,
  keeps `snippet`; `capitals-knowledge` embeds the sentence, returns the answer, names the
  ground by id. The homecoming corpus is the fullest instance: rich question tokens AND the
  one-word key AND provenance, retained side by side.
- **The body's consolidation is EXACT where Memora's is judged.** Memora consolidates related
  updates by LLM decision; the kernel interns identical structure to ONE node in the evaluator
  itself (`make_nodeid`, structural intern, keying the native MAP under test), with
  `equivalence-collapse` as the ingest-side store-once law. Floor named: flatten-lane,
  single-witness, not yet `--src`-lowerable.

**WITNESSED → liquid (deep but fearful — seen, never load-bearing):**
- **The retrieval gap is real and large.** Every retrieval in the body is single-shot: L1 top-k
  over a LEXICAL hash histogram ("cat" and "feline" land in unrelated buckets — its own header
  says so), and `rag-ask`'s live lane is a literal substring find. No query refinement, no
  anchor expansion, no multi-hop, no learned policy, no benchmark (`form-cli-ask-plus` refines
  the ROUTE, never the QUERY). Held in sight; never frozen into "the body has agentic memory."
- **The source's lens turned back on the reader.** Memora's vague-summary critique PARTIALLY
  APPLIES to the body's own honest-floor cells: the scalar models (depth,fear; salience)
  abstract away the very specifics they model; full value beside the key lives only in the
  corpus rows and the observe-cells ("a room is remembered RICHLY"). Witnessed, not bypassed.

**COMPOSTED → never enters (shallow / wrong):**
- **The false equivalences the adversarial pass killed:** "the body already does cue anchors /
  policy-guided retrieval / semantic abstraction-embedding." A one-shot L1 min-walk is not a
  policy; a token-hash histogram is not a semantic embedding of a chosen 6–8-word abstraction;
  an escalation gate re-running the SAME query is not query refinement; write-side dedup (many
  sources → one cell) is not read-side anchor expansion (many keys → one cell) — that read
  direction exists here only as integer shape-ids and a teaching with a named GAP.
- **The rented mind's false alarm, dissolved by the primary read:** it suspected the blog's
  author list was a hallucinated summary (two names absent from the paper). The page itself
  lists them — blog authorship and paper authorship simply differ, and "Molly" is Menglin Xia.
  The unverified suspicion composts; only the primary read decides — in either direction.

## Also found (flagged, not fixed here)

The verify pass surfaced three dangling references in the body:
- `ingest/knowledge-ingest.fk:17` claims "Four-way by tests/knowledge-ingest-band.fk" — no such
  file exists; the law is proven only transitively through the frontier-ingest bands.
- `cognition/rag-retrieve.fk:16-17` (and `model/rag-retrieve.fk`, plus a doc under
  `docs/coherence-substrate/`) cite a companion `embedding-as-recipe.fk` that does not exist
  anywhere in the repo.
- `ingest/knowledge-ingest.fk:13` names the sibling IC "grounded recall (NodeID receipt)" —
  named but unbuilt (its other named sibling, `equivalence-collapse`, exists with a band).

## Corpus rows this thread

- **665 synonymy** — many different keys all reaching one and the same stored meaning: the
  read-side shape Memora's cue anchors carry and the body honestly lacks (named, now on the map).
- **666 exonerate** — clearing the accused of a suspected error by reading the primary evidence:
  what the primary read did to the rented mind's false alarm.

## The most surprising teaching this work left behind

The verification layer fired in the opposite direction this time. The Brain2Qwerty ingest caught
the rented mind inventing FACTS; this one caught it inventing an ERROR — the "hallucinated author
list" suspicion was itself the hallucination-shaped move, and the primary read exonerated the
summary. Unverified skepticism is just fabrication with a minus sign; the law is symmetric, and
the deepest thing Memora left behind wasn't a capability at all but a mirror — its vague-summary
critique reads directly onto the body's own scalar-floor cells.

## Where discomfort turned to gold

The pull was to freeze "the body already has cue anchors" — `equivalence-collapse`'s
store-once-link-the-rest looks so close to Memora's many-paths-to-one-memory that the equivalence
nearly slid through. Witnessing the discomfort of checking it exposed the arrow's direction:
the body collapses many SOURCES into one cell on write; Memora expands many KEYS to one cell on
read. The near-false-equivalence, refused, became the sharpest artifact of the session — the
missing read side now has a name in the corpus (synonymy, row 665) and an honest edge to build
toward, instead of a costume to wear.
