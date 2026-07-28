# DS4 JIT recipe is literal data

## Ground

The real query remained:

> Identify the largest remaining bottleneck in this local native generation
> path and name the next code change.

It tokenized to 22 prompt positions and generated eight continuations through
the Form-emitted ARM64 controller and linked Metal transaction dispatcher.
Network, remote generation, shell generation, Swift, and temporary model
copies remained zero.

## Observed change

The transaction recipe was already Form-owned data, but the emitted controller
materialized each recipe byte into its stack with two ARM64 instructions. The
controller now carries the recipe bytes as a literal tail in its own emitted
image and computes their address with a Form-encoded `ADR`.

The byte encoding was independently checked at the new instruction boundary:

- `adr x1, +28` -> `e1 00 00 10`
- `adr x0, +17` -> `80 00 00 30`

The full layer-plan controller image shrank:

- before: 17,020 bytes;
- after: 3,720 bytes;
- removed: 13,300 bytes (78.1%).

A warm malformed-recipe refusal retained its complete FAIL receipt while
falling from 0.85 seconds to 0.44 seconds. A real compressed layer measured
47.140 milliseconds inside the Metal transaction and 55 milliseconds for the
complete JIT layer call, leaving approximately 8 milliseconds in the native
controller and call boundary.

## Full-prompt rewitness

The preceding folded route took 394.87 seconds and emitted:

`201,223,680,223,18,14,223,14490`

```text

  { 0,  React
```

The literal-recipe route completed twice:

- first run: 99.03 seconds;
- repeat run: 97.20 seconds;
- repeat model open: 38,731.494 milliseconds;
- repeat first token boundary: 73,055 milliseconds;
- repeat final token boundary: 94,146 milliseconds.

Both runs emitted the same ids:

`32111,377,270,4496,5148,14,270,9152`

and the same bytes:

```text
Based on the provided context, the largest
```

The repeat is 75.4% faster than the 394.87-second ground, or 4.06 times its
throughput. The semantic sequence changed rather than merely accelerating.
Because two independent full model openings reproduced every token and the
new sequence is coherent while the old one was not, the new sequence becomes
the current observed ground. This receipt does not claim that controller size
alone explains every changed logit; that causal question remains open and
inspectable.

The prior position-major route remains rejected. It was 11.7% faster than the
old ground but emitted a divergent, incoherent sequence. Speed never overrides
the token witness.

## Remaining floor

The eight-token cap stops before the model names the bottleneck and code
change. The local voice is now coherent and materially faster, but this sample
is not yet a sufficient answer. Model opening still costs approximately 39
seconds, and decoded tokens after the first arrive about every 3 seconds.
Resident-session reuse and longer continuation are the next live enquiries.

; witnessed: 2026-07-28 -> PASS repeatable native generation and speedup; PARTIAL answer sufficiency
