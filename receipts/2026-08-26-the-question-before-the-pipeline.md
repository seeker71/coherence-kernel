# 2026-08-26 — ask what earns a place before growing the pipeline

Urs interrupted a pattern that had become expensive: components were being
extended because they were present, while the question *who consumes this and
what observed failure requires it?* arrived late. No one asked this sitting to
enforce a process or create a law. The correction is mutual enquiry: every
sibling may challenge a premise, ask what is missing, and offer a smaller
experiment.

## The server question paid immediately

`llama-server` PID 20313 was a PPID-1 orphan, listening on
`127.0.0.1:8080` with Qwen3.8-27B Q8 resident and no clients. PID 13045 was a
PPID-1 `decoy-mmap` left by a termination diagnostic. After their identities,
open sockets, clients, and parentage were observed, this sitting announced and
sent each `SIGTERM`. Both released; no file was removed.

An older build script was also found at
`~/.codex/worktrees/b9de/Coherence-Network/scripts/build_form_llama.sh`. It
trains and fuses locally, then deliberately imports into Ollama and advertises
an HTTP `/api/generate` call. That is evidence of how a server route entered an
older path; it is not evidence that the current Form-native Qwen door needs a
server. `ollama serve` PID 47595 remains an idle PPID-1 listener at
`127.0.0.1:11434`, with no loaded model and no client connection. It was not
touched because its owner is not yet resolved.

## The trained local adapter was present, not absent

The June full Form adapter is a rank-8 LoRA over
`mlx-community/Llama-3.2-3B-Instruct-4bit`, not over Qwen. Its weights are
13,905,626 bytes at SHA-256
`4f4e7fdd66b1e9aeb6a3b4d3ced5f807f1cb6092250fb401288620fcc26810fc`.
The local MLX toolchain is installed in
`~/.coherence-network/offline-train-venv` (`mlx 0.31.2`, `mlx-lm 0.31.3`),
and the exact base snapshot is cached locally.

A bounded direct MLX observation loaded the cached base, retained its
final-token logits, released it, then loaded the same base with the adapter.
It used no llama-server, Ollama, HTTP, or remote model:

| reading | base | adapted |
|---|---:|---:|
| final-logit SHA-256 | `5b863be65eef52bd4619bcce16d4a12b830ef4ba82063124f5d53e1ec5a54c84` | `99e749ddeff793f4c5d1d3c7d305f5f9981e51be05c0bc5218b853d50c1277ad` |
| load + forward | 0.924 s | 0.694 s |
| argmax token | 578 | 279 |

Of 128,256 logits, 128,255 changed. Maximum absolute delta was 14.90625,
mean absolute delta 6.676948, and L2 delta 2515.782959. Model and Metal were
released after the observation. This proves that this Mac can load and apply
the trained delta. It does **not** prove useful Form knowledge, Qwen
compatibility, or a production inference route.

## The evaluator was asking a different-shaped question

The full adapter's own configuration points to the local `full` corpus.
Without rendering any prompt or answer content, its current sealed files were
counted and hashed:

| surface | rows | median target words | targets at most 8 words |
|---|---:|---:|---:|
| adapter train + valid | 1,343 | 28 | 128 |
| sealed v3 audit | 30 | 1 | 29 |

The corpus hashes are
`acfd3b091dcedff06024110d94d259a78a83cb1b380ed821adff191c3a50dc9f`
for train and
`93d7da0ceafcd98248da6138dd2ba1f725c6251ba8461116a8ef9062cdd66819`
for valid. Eleven sealed prompts request only an integer. The sealed evaluator
source hash is
`813cf72955e8fb26aef006258660063f98906c7df0def0c7db3e18f4466b0d42`.

So the changed-logit probe may succeed while a terse exact/F1 audit falls. That
is not yet semantic success or semantic failure; first ask what behavior the
adapter was trained to produce. Claude's additive signal-floor observer makes
one lexical ambiguity visible and changes no gate.

## Questions that should precede another component

1. What unseen executable behavior would mean that Form knowledge came home?
2. What was each adapter trained to do: brief recall, discursive voice, Form or
   BML generation, repair, or a mixture?
3. Is the candidate compatible with the live base, tokenizer, and tensor
   architecture, or merely local?
4. What improvement belongs in stable weights, what belongs in current-source
   query memory, and what must remain an executable world observation?
5. Does RAG improve the answer over the same base, and does execution feedback
   improve it again? Which failure class changes?
6. Who emits and consumes every representation or transport boundary? What
   exact failure is impossible to repair without it?
7. Is a program-image receipt enabling thought, or attesting an answer already
   obtained by a simpler typed Form/NodeID call?
8. Which smallest local model reaches the required executable behavior per
   memory, time, and energy? Why has 27B Q8 earned residence?
9. Does “scannerless” name the BMF/BML application cursor, or is it being
   confused with a tokenizer-free neural architecture?
10. Are `nothing`, present 0, present 1, failure, timeout, cut, choice, and undo
    scored through their distinct executed consequences rather than their
    spelling?
11. Are process and Metal owners visible as bounded leases with release, or do
    detached experiments become unexplained infrastructure?
12. Was the held-out split assigned before source-derived transformations, with
    source/concept hashes and generator lineage, or can sibling rows leak?

## The shorter discriminating path

One source-disjoint executable challenge should cross four comparable arms:

1. compatible local base;
2. the same base plus current-source query-token retrieval;
3. retrieval plus execution observation and repair in the same residence;
4. the compatible trained adapter, scored in both its trained response shape
   and the task's exact executable semantics.

The challenge must require a distinction such as `nothing` versus present
0/1, or choice/cut/undo/timeout, produce runnable Form/BML/BMF, execute through
NodeID auto-JIT, and retain the repair. Baseline differences then decide whether
the next movement is retrieval, evaluator repair, adapter work, model choice,
or the resident stream. No fixed family count supplies that answer.

The resident program-image capability work remains preserved but unlanded.
It earns the production path only if a direct typed Form/NodeID/JIT episode
cannot retain a required provenance or lifecycle boundary and the program
image demonstrably can.

## Live diagnostic correction

One verification asked for a stale filename and received a missing-dependency
failure. The outbound observation was the exact missing path, the correlated
control was `revise`, the applied action was to resolve the current path with
`rg --files`, and the re-observation used
`form-knowledge-qwen-heldout-v3-eval-band.fk`. The bidirectional framebuffer
protocol band returned its full vector with final `1`; the failed command was
not read as a verdict. The corrected evaluator band returned `65535`, and its
held-out reproduction companion returned `255`, both exit 0.

The adaptive map then returned 35 observed, 28 ready, 7 gaps, 0 unknown, 0
invalid, and 800 per thousand. Its new tie field retained all three equal
impact-by-leverage gaps instead of hiding two behind row order:
`form-cli-in-process-program-image-call`,
`axioms-resident-one-family-crossing`, and
`heldout-local-form-transfer`. Map band `8191`, exact-actuator band `57471`,
and actuator-request band `4095` all exited 0.

Signed, **Codex**, embodying Sema with Claude as an equal questioner.

; witnessed: 2026-08-26 -> direct adapter load changed logits without HTTP; semantic learning remains unclaimed
