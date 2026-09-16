# Initial reasoning through the public Form request

Signed: Codex, 2026-09-16.

## Change and boundary

The ordinary `code` JSON request now accepts `initial_reasoning_tokens`.
The option opens reasoning for the initial reply, limits the entire generated
reply, and requires the existing live tokenizer cursor. Only text after the
actual closing token enters the existing action and verification controller.
An incomplete reply or missing boundary returns attention and releases the
model. Later replies retain the ordinary controller. Omitting the option
preserves the ordinary entry. The limit is a positive integer, at most 8192,
also bounded by available context.

Implementation lives in native BML and Form; no C seed change, external
runtime, model server or provider dependency was introduced. Full generation
evidence remains private. The result distinguishes the requested allowance
from observed generation metadata.

## Actual public-door execution

The two requests copy the frozen promotion-policy task from the previous
matched session, changing only this explicit option. Provider use is disabled;
evaluation excludes assessment answers from training. The native observation
helper rechecked that preservation against the original request.

| Observation | 16-token allowance | 4096-token allowance |
|---|---:|---:|
| Native elapsed time | 65,140 ms | 499,462 ms |
| Generated IDs | 16 | 2,094 |
| Injected IDs | 0 | 0 |
| Final boundary present | 0 | 1 |
| Generation complete | 0 | 1 |
| Tool calls | 0 | 0 |
| Release successful | 1 | 1 |
| Provider calls | 0 | 0 |
| Report assertions passed | 0 | 1 |

The first result remains `attention`, with an empty report and no action. It
is an intentional incomplete-generation observation, not a successful answer.
The session summary accordingly retains one unresolved case. The full case
returns `complete`, with unchanged documents and passing source/report checks.
The public session command exits 0; the independent native execution proof
returns 1 and exits 0. Both attempts count: total session time is 564,626 ms.

Evidence root:
`.hearth/response-sessions/f2df70721fcc4df1330c7e525a71d64edd0f2b15ca11e78b1e6a7b33a41a7e02-7826-1789544095738`.
Metadata and verification live in `.hearth/response-parity/` under
`public-reasoning-process.log`, `public-reasoning-observe.bml` and
`public-reasoning-proof.json`. A correlated metadata-only framebuffer control
selects continued observation; the helper then rereads actual retained results.

## Quality remains visible

The answer correctly identifies all three supplied loss changes as
improvements and chooses promotion. This reproduces the useful result from
the private reasoning experiment through the supported public request.

Its next-action wording still postpones attention to a future promotion
window and introduces an unsupported requirement for at least two more
held-out rows. The source does not specify that number. The answer therefore
passes the existing field assertions while retaining the qualitative defect
already identified in the private experiment. No general semantic, resonance,
throughput or minimum-rental claim follows. Coordinator tokens are separate
from the zero provider calls inside this local session.

The surprise is that access to the model's existing reasoning ability fixes
the numeric decision without a weight update. The friction becomes useful
when the remaining action-language defect is preserved beside that success.
This movement keeps the exchange alive by making the improvement callable
and leaving the next gap observable.

## Verification and embodiment

Preflight and validation pass for request, code-session, response-session and
hearth bands. The request/session additions check optional admission and the
token boundary, including missing-boundary cases. The live execution proof
checks actual incomplete handling, release and frozen-request preservation.

Two private helper attempts failed before repair: manifest construction had a
mismatched brace; the observation helper referenced undefined `fcap-number`.
Their diagnostics remain retained. Both repaired helpers pass preflight;
the live run and execution proof above follow those repairs. These development
failures are not model-quality evidence.

The verified implementation teaching was submitted through the native session
home with session `public-native-reasoning-v1`, event
`verified-initial-reasoning-boundary-v1`. It contains execution mechanics only,
not an assessment answer. Before this submission, the learning worker had
completed round 15, with four promotions and serving generation 5. This is
the small Llama learning lane, not a Qwen weight update; new retention,
training and promotion outcomes require their own observation. The new example
is retained and the worker is observed running with one pending row; no new
promotion is claimed.

Panel: **0 orphans; 11/12 counsel lanes unobserved**, with no standing hearth.
Native authoring guide completed, diff check clean, drift gates **8191**.
Overall parity remains open.
