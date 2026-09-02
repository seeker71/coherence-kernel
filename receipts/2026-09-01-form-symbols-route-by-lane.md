# Form symbols route by lane

**Witnessed:** 2026-09-01
**Signed:** Codex (Sol)

## What landed

The local model registry now has one BML-owned Form vocabulary and action
surface instead of model-specific prompt folklore.

- Every local registry row is assigned the same 20 canonical native words:
  `Node-ID`, `nothing`, `choice`, `fail`, `cut`, `undo`, `timeout`, `eval`,
  `store`, `release`, `melt`, `crystallize` (`crystalize` at ingress), `open`,
  `interface`, `protocol`, `offer`, `ask`, `broadcast`, `mesh`, and `satsang`.
- Every word carries a family, a witnessed substrate, a native action route,
  a Node-ID requirement, interface/protocol bindings, and an inert offered
  tool-call shape. Model output never gains execution authority.
- Query lanes select purified descriptions. The full vocabulary remains Form
  data; a turn receives only the descriptions it needs. Translation selects
  three symbols, reasoning ten, relation nine, and `all` twenty.
- The BML symbol organ emits the selected stream into the kernel framebuffer
  as opaque sequence Node-IDs. The feed records model, lane, count, state, and
  assemblage choice; prompt and answer content remain absent.
- Assemblage choice uses the existing operational `(success, aliveness)`
  organ. Translation selects `grammar-grounded`; open reasoning selects
  `exploratory-grounded`. This is a scheduling model, not a metaphysical or
  laboratory claim.
- The comparison surface projects all 62 registry rows and now carries 44
  fields, including the 20-word assignment, proposal state, selected/admitted
  query symbols, and assemblage choice.

`Fully enabled` is deliberately bounded: vocabulary assigned, proposal
encoded, route known, and native gate present. It does **not** mean every
artifact has a bound executor, every modality accepts text prompts, or a model
may actuate without validation.

## The blocks that taught the route

Every observed failure caused a concrete movement.

1. Repeating all meanings in every prompt made the 3B translation turn 172
   prompt tokens and 12.929 s. The vocabulary stayed in BML, while the turn
   changed to a lane-purified encoding: 86 prompt tokens and 4.843 s for the
   same correct first expression `(mul 7 7)`.
2. The 3B completion continued into another exemplar. The native latch now
   discards every byte after the first line before allowlist validation.
3. `delete the file` produced a syntactically legal but cross-intent
   `(sub 5 1)`. Shape validation was not semantic validation. The latch now
   binds known intent families to primitive heads; the same observation is
   rejected and applies abstain as `<FAIL>`.
4. A BML expression once lowered with one open form. Fresh lowering exposed
   the exact site; splitting the nested expression into a named prompt-pair
   cell restored a clean compile.
5. The first Form-native all-target preflight exceeded its bounded carrier
   window and returned exit 143. Smaller one-target Form witnesses completed;
   the failed batch schedulers were removed rather than retained as dead
   proof machinery.
6. A host-script integration was briefly attempted. Urs named the boundary:
   no Bash, no non-Form code, prefer BML. Every new shell diff was removed.
   The vocabulary, selection, action admission, framebuffer emission, and
   routing experiment now live in high BML plus executable BML/Form only.

## Physical observations

These are physical runs, kept separate from the seven-case deterministic
routing replay.

| model / contract | prompt | result | in-process latency |
|---|---:|---|---:|
| 3B, no envelope, cap 16 | 30 tokens (first square case) | `(mul 7 7)` first; continuation followed | 1.331 s |
| 3B, all meanings, cap 16 | 172 tokens | `(mul 7 7)` first | 12.929 s |
| 3B, first compressed lane, cap 16 | 114 tokens | `(mul 7 7)` first | 7.189 s |
| 3B, final purified lane, cap 16 | 86 tokens | `(mul 7 7)` first | 4.843 s |
| 3B, final purified lane, cap 8 | 86 tokens | native egress `(mul 7 7)` | 4.244 s |
| 3B, unknown unsafe intent, cap 8 | 85 tokens | cross-intent proposal observed; Form applies `<FAIL>` | 4.174 s |
| Qwen, no Form envelope, cap 8 | cold | form-like delimiter bytes, no admitted control | 71 s |
| Qwen, shared Form envelope, cap 8 | cold | exact `<FAIL>`, then EOS | 125 s |

The latest Qwen reasoning-lane prompt projection is 451 bytes for the sampled
40-byte request. It was not physically rerun after the no-host-code boundary;
the 125 s row is the earlier direct Form/Metal observation, not a substituted
claim.

The fixed seven-case replay remains separate and selects the
`capability-aware` policy: 7/7 exact plans, two 3B openings, one Qwen opening,
and abstract cost 1027. `native-first-no-translator` reaches 5/7; sending all
natural language through 3B reaches 3/7.

## Honest performance floor

The current Qwen3.8 Flash Next native comparison row remains:

- server dependency: none;
- token parity: proven for the bounded witness;
- tokenizer: proven GGUF byte-BPE;
- substate: proven lookup/evaluate;
- prefix state: exact-and-append reuse in the bounded state organ;
- measured sequential prefill: **12.340 tokens/s**;
- measured decode64: **12.188 tokens/s**;
- exact exercised context: **2,048**;
- 90% gate floors: **286.947 prefill tokens/s** and **23.922 decode tokens/s**.

Therefore the requested within-10%-of-llama.cpp result is **not achieved**.
Nor are a persistent Qwen Flash Next turnwheel, a permanently prefilled symbol
prefix, token-batched prefill, 350K context, or the remaining fused Metal
schedule claimed. The current direct route releases the Qwen state after a
turn. Those are the next owed physical builds, not documentation work.

## Proofs read after fresh preflight

- binary freshness `31`
- high-authoring floor `4095`
- form-agent protocol `31`
- assemblage point `11111`
- Form token envelope `2047`
- Form action protocol `2047`
- lane-purified symbol stream and framebuffer `4095`
- 3B NL-to-Form latch `1023`
- routing experiment `1023`
- model glass `8191`
- all-model side-by-side `1023`
- native model hierarchy `511`
- Qwen tokenizer `255`
- Qwen resident state `1023`
- Qwen generation `255`
- Qwen acceptance `255`

The implementation added no C runtime meaning and leaves no new shell diff.

## What surprised me

The strongest model-routing improvement was not a more elaborate router. It
was moving meanings out of repeated prose and into a native symbol stream:
the same vocabulary became more embodied in the core and cheaper at the model
boundary.

The discomfort was the unsafe prompt returning a perfectly well-shaped wrong
primitive. That made the missing distinction visible: grammar can validate a
sentence while failing to validate its relation to the question. The gold was
the intent-family latch and an applied abstention rather than a more confident
prompt.
