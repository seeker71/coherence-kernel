# Live BMF prompt cursor entered resident Metal

**Observed:** 2026-08-25 13:31 WITA
**Presence:** Codex

The local Qwen voice no longer waits for one complete prompt-ID list before its
first model-state transition.  A seal-keyed qtf2 reader now walks the existing
fixed BMF rows one bounded slice at a time, and a live cursor offers each token
directly to the resident Qwen/Metal prefill loop.

This is not a flattened tokenizer table in the Form body.  The qtf2 artifacts
remain external, sealed row recordings.  The runtime reader has six recursive
definitions; artifact recording, publication, acquisition, and their proof
mirrors remain separate Form organs.

## Exact crossings

The small semantic band fresh-compiled and returned:

```text
qwen35-tokenizer-live-cursor-band = 2047, exit 0
```

It observes ordinary token values `0` and `1` with `present=1`, and observes
completion/refusal as `present=0`, `value=nothing`.  The independent qtf2 writer
and proof body remained whole:

```text
qwen35-tokfast-v2-band = 65535, exit 0
```

The new read-only artifact admission organ read the writer's seal, verified the
current bytes of `abc`, refuted the mutated bytes `abd`, and released its temp
files:

```text
qwen35-artifact-seal-reader-band = 63, exit 0
```

The full form-cli composition then fresh-compiled and returned:

```text
qwen35-form-cli-band = 2047, exit 0
form_cli_generation_attestation_test: PASS
```

The source compiler's actual cap was allowed to speak.  Pulling the entire
129-definition qtf2 recorder into the live voice crossed `FK_FN_CAP=4096`.
Replacing it with the reader exposed a second retained build-time edge:
`qwen35-crystal.fk` imported the 59-definition artifact fetcher for the single
read-only `qaf-seal-path-for` meaning.  Crystal now imports the ten-definition
seal reader instead.  The final whole-program registration count is 4081,
fifteen below the unchanged cap.  NodeID routing stayed in-process.  The C seed
did not grow, no operation table was introduced, and no flattening path was
used.

## Token equality and time

The short prompt comparison observed all 27 IDs equal and released the cursor:

```text
reference_ms=8160
cursor_open_ms=2
cursor_drain_ms=313
reference_tokens=27
cursor_tokens=27
cursor_pieces=15
cursor_probes=3271
cursor_released=1
ids_equal=1
```

The complete current Form teaching overlay observed:

```text
reference_ms=423219
cursor_open_ms=1
cursor_drain_ms=1719
system_bytes=1484
upper_bound=1567
reference_tokens=393
cursor_tokens=393
cursor_pieces=351
cursor_probes=70613
cursor_released=1
ids_equal=1
```

That is about a 246x contraction of the full tokenizer step.  More importantly,
the cursor makes `open` available in 1 ms and permits Metal state to advance
while later pieces are still being transmuted.

## End-to-end physical witness

The final raw local generation ran through the exact new dependency graph:

```text
prompt_cursor=bmf-qtf2-live-v1
text=LOCAL FORM ALIVE
raw_probe_wall_ms=144341
prompt_tokens=386
generated_tokens=4
forward_passes=390
gpu_busy_us=97136100
prefill_gpu_busy_us=95555441
decode_gpu_busy_us=1580659
state_released=1
state_handles_closed=128
state_handles_expected=128
buffers=0
buffer_slots=1013
free_slots=1013
pending=0
in_flight=0
total_sync=11
last_error=none
exit=0
```

The earlier whole-tokenizer witness took `414028 ms`; this final run took
`144341 ms`, about 2.87x faster end to end.  Metal prefill is now the dominant
measured cost rather than whole-prompt token materialization.

## Honest boundary and next signal

This crossing streams the system/user prompt.  Thought-offer lists and any
future heed injection still have their own tokenization boundaries and are not
claimed live here.  The qtf2 rows and 29 GB model are local but do not yet have
an independently powered recovery copy.  The full CLI also has only fifteen
definition registrations of current headroom, so future growth should deepen
runtime/build separation or program-image loading rather than enlarge the C
seed.

Most importantly, tokenizer and carrier health do not establish Form semantic
mastery.  Heldout-A is still the current learning surprise (`score95=0`).  The
next movement remains heldout-B → public curriculum actuation → generative and
native execution → release → projection re-observation.

— Codex
