# Receipt — native model hierarchy re-witness

Date: 2026-07-25

## Movement

The local model router had collapsed three different facts into one:
an engine exists, an artifact exists, and that exact pair executes. It also
allowed loopback HTTP/JSON carriers to stand too close to direct Metal.

`form/form-stdlib/native-model-native-hierarchy.fk` now keeps those facts
separate and makes membrane exclusion the default. Its current observed rows
are:

- `llama32.form-metal`: engine, artifact, and execution observed; direct Metal;
- `deepseek.ds4-metal`: engine and 91,321,404,640-byte artifact present, direct
  Metal intended, but execution refused because the engine does not accept the
  artifact's GGUF types 40/41;
- `nanbeige42.form-metal`: admitted artifact present, but the looped weights
  are not yet bound to the direct-Metal graph;
- `nanbeige42.llamacpp-comparator`: observed local response, explicitly marked
  as a loopback membrane and comparator.

The self-contained `form-cli` now exposes:

```text
model-route
```

and returns:

```text
llama32.form-metal
```

`form/scripts/native_model_route.sh` now follows the same decision:
`LOCAL_MODEL_ROUTE` defaults to `form-metal`, which executes
`metal_ask.sh` directly. The prior Ollama path remains only as the explicit
`ollama-llama32` route.

JSON is not part of the native route. It remains only on the explicit
`nanbeige <prompt>` comparator because that command crosses a socket/HTTP
membrane into the pinned authors' llama.cpp branch.

## Witnesses

Bootstrap regeneration and native carrier build both completed:

```text
regen: form-cli-emitted.c ... functions=1448 nodes=42227 strings=1355
built form-cli ... 1576056 bytes, self-contained
```

The native hierarchy crossed all four sibling walkers:

```text
core.fk+native-model-native-hierarchy.fk+
native-model-native-hierarchy-band.fk -> 127
1 ok, 0 divergent
```

The amended control plane also crossed all four:

```text
core.fk+native-model-control-plane.fk+
native-model-control-plane-band.fk -> 65535
1 ok, 0 divergent
```

The direct-Metal Llama lane was then re-run on the real 2,019,377,376-byte
artifact. All 13 graph/token gates passed, the lane selected the
`lane-simd-hoisted` kernel, and it staged decoded text with token ids
`[198, 21509, 449, 1193]`. During this witness, two stale carrier assumptions
were found and repaired: decoded text can span two output lines, and the rate
line can consequently fall beyond the carrier's old three-line window.
After repair, `metal_ask.sh` completed with:

```text
=== VERDICT PASS — native ask lane staged, bound to its question,
every cost declared and measured ===
```

This is direct model execution on Metal. It does not invoke Ollama or
llama.cpp.

## Remaining doors

Nanbeige is available in `form-cli` today only as an explicit verified
comparator. A full native Nanbeige response requires binding its 22 shared
decoder layers, two execution loops, loop-indexed KV slots, and vocabulary
head to the existing direct-Metal generation machinery, then proving
token-by-token parity.

DS4 has a native Metal engine and a full local artifact, but their current
pairing is not an execution lane. The next movement is artifact support or a
compatible engine/model pair, followed by an actual generated-token witness.

The router will promote neither from presence alone.

; witnessed: 2026-07-25 -> native selector 127 four-way; control plane 65535
; four-way; direct Metal Llama generation 13 gates PASS; Nanbeige native Metal
; pending; DS4 type40/type41 pairing refused
