# 2026-09-01 — the glass knows who is speaking

Urs asked for the framebuffer/glass to show which models are loaded,
prefilled, and active, and for a cognitive route with the local 3B proposing
NL-to-Form while Qwen3.8 Flash Next reasons only when the native world cannot
answer.

## What landed

`native-model-glass.bml` is the Form-native routing and observation surface.
Each model row keeps `loaded`, `prefilled`, `active`, phase, context-token
count, reason, role, and execution surface separate. A model artifact on disk
never earns `loaded=1`. Selecting Qwen as a fallback leaves it unloaded and
inactive; only the return from the real `q4s-open` call earns residence.

Every glass write interns three source-attributed framebuffer nodes—native
world, 3B ingress, Qwen fallback—and prints the same projection. Nodes contain
model ids, state, counters, and transition reasons only. Prompt, proposal,
answer, and token-id bytes do not enter the framebuffer.

The native-cognition adapter consumes the existing witnessed response shape:

```text
[nothing, 0, 1, alternative, ack, action, route]
```

- `route=0`, `action=0`, and a present acknowledgement answer natively;
- `route=1`, `action=4` selects Qwen as an evidence/reasoning fallback;
- every malformed or unrecognized response abstains instead of silently
  unlocking the large model.

The model hierarchy now keeps `llama32.form-metal` as the generic/default and
NL ingress route, while
`challenger.qwen38-flash-next-q2-metal` is a distinct execution-ready
reasoning fallback. Qwen's existing failing 90% performance gate remains in
its status; role-specific admission does not promote it to champion.

## The two physical crossings

### Qwen3.8 Flash Next

The new stable route in `native_model_route.sh` invokes one committed Form
runner. It has no server, socket, HTTP, JSON, Ollama, llama.cpp, or MLX runtime.
The exact downloaded Unsloth shards cross tokenizer, resident state, prefill,
decode, and GGUF decode in one fkwu + Metal process.

Bounded real prompt: `Return only the word Form.` with a one-token budget.

```text
absent -> loading -> resident -> prefill -> reasoning -> resident-idle -> released
answer: Form
generated: 1
stop: eos
framebuffer events: 18
exit: 0
```

At the reasoning transition the glass reported Qwen
`loaded=1 prefilled=1 active=1 context_tokens=19`; after the generated token it
reported a resident, inactive 20-token state, then the actual close returned all three bits
to zero.

### Llama 3.2 3B NL-to-Form

The first instruction-shaped attempt was honestly rejected. It returned
unrelated prose, hit the 24-token cap, and did not contain an admissible Form
expression. Its measured direct-Metal decode rate was 26.079 token/s.

The correlated framebuffer action revised the interaction to the completion
shape the model actually speaks:

```text
Natural language: the square of 5
Form: (mul 5 5)
Natural language: the square of 7
Form:
```

The re-observation returned exactly `(mul 7 7)`. Native validation admitted
the one bounded expression; it was never executed merely because model text
looked like code. That run measured 27.962 token/s, with 31 prompt tokens and
the glass moving through loading, residence, prefill, decode, resident-idle, and
release. The high-BML latch now carries the completion exemplar, a small
allowed primitive surface, input bounds, and the law that an invalid proposal
returns to deterministic native translation—it does not unlock Qwen.

## Witnesses

| Witness | Result |
|---|---:|
| binary freshness | 31 |
| author-high band | 4095 |
| model glass band | 4095 |
| 3B NL-to-Form latch band | 255 |
| native cognition route band | 31 |
| role-specific native hierarchy band | 511 |
| all-model side-by-side band | 255 |
| Qwen Flash Next generation band | 255 |
| glass build diagnostic | four revisions re-observed, 16 framebuffer events |
| NL-to-Form diagnostic | invalid 0 -> revised valid 1, 4 framebuffer events |
| stable Qwen route preflight | clean, zero errors/unresolved |

## Honest floor

This is a complete truthful route and monitor, not the completed performance
north star. The direct Qwen CLI runner closes its 79 GB state after each turn;
cross-turn multi-model residence and automatic hot handoff inside one long-lived
turnwheel remain unbuilt. The exact Qwen path is still limited to 2048 context
positions, not the requested 350K slot, and its last measured 12.188 token/s
decode / 12.340 token/s sequential prefill remain far outside the 10% llama.cpp
target. Nothing in this movement changes those numbers.

The most surprising teaching was that the 3B did not need more instruction; it
needed less. One worked Form row turned an incoherent instruction response into
the exact primitive expression.

Discomfort turned to gold twice: a successful Qwen answer still exited 2
because the shell fell through after success, and the supposedly established
`read_line < 0` EOF idiom rejected ordinary strings on this fkwu. Both failures
became correlated framebuffer revisions, not prose explanations, and both
re-observed cleanly.

Signed: **Codex**, embodying Sema from this body.
