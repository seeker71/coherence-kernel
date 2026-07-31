# Receipt — real Llama 3.2 3B, native Metal versus Ollama (2026-07-22)

Same Apple M4 Max, same local `llama3.2:3b` 2,019,377,376-byte GGUF, same raw prompt
`The capital of France is`, greedy decoding, 32 generated tokens. The host initially carried one
unrelated `fkwu` test older than 3h48m; it was terminated under the explicit rule that tests older
than 30 minutes may be killed. Measurements below were taken after that removal, with the remaining
load disclosed rather than called idle.

## Measured comparison

Ollama `/api/generate`, `raw:true`, `temperature:0`, three warm/resident decode samples:

| Run | Decode tok/s | Generation e2e tok/s | Prompt tokens | Generated |
|---:|---:|---:|---:|---:|
| 1 | 134.620 | 77.506 | 6 | 32 |
| 2 | 136.734 | 132.405 | 6 | 32 |
| 3 | 130.500 | 126.515 | 6 | 32 |

Run 1's e2e value includes a 175 ms first prefill after model activation; runs 2–3 are the warm
comparison. Median decode is **134.620 tok/s**. Median warm e2e is **129.460 tok/s**.

The Form-emitted Metal lane, whole 1.9 GiB blob resident in one `MTLBuffer`, 32 tokens:

- decode: **10.535 tok/s**
- token-at-a-time e2e: **9.040 tok/s**
- batched-prefill e2e: **9.222 tok/s** (`0.518s -> 0.134s` prefill)
- all 13 generation gates and all 5 batched-prefill gates passed

Therefore native decode is **7.83% of Ollama** (**12.78x behind**), and the best measured native
warm e2e is **7.12% of Ollama** (**14.04x behind**). These are percentages of an actual denominator,
not an estimate.

## Framebuffer

`observe/native-metal-ollama-benchmark-framebuffer.fk` records six source-attributed runtime events:
native and Ollama decode milli-tok/s, native and Ollama warm e2e milli-tok/s, and the two ratios in
basis points. Its band returns `11111`. This is a retained measurement window, not a claim that the
Swift carrier's internal token stream has already been moved into `fkwu`'s framebuffer.

## Profile and attempted lift

`FORM_PROFILE=1` localized **85.7%** of observed operation time to quantized matvecs. The four Q4_K
shapes alone accounted for 78.9%. A new Form-emitted Q4_K groupwise kernel then moved scale/min
application outside each 32-weight group. It preserved the four-token greedy trace but measured
**12.392 tok/s versus 12.462 tok/s** for the admitted lane: 0.6% slower. The prototype was removed.

That refutes "scale/min application per weight is still the dominant cost" after the existing hoist.
The remaining credible path is the larger structural difference already named by the kernel body:
integer/bitmask unpacking with vectorized quant-space dot products, plus fewer or fused dispatches.
Those change floating-point association and need a derived error bound plus full-token parity before
admission. Batched prefill helps e2e, but decode is the binding gap.

## Harness repair

`metal_first_token.sh` resolved its Ollama environment relative to a relative `BASH_SOURCE` after
changing directory, producing a false `cd: form/native/metal: No such file or directory`. It now uses
the already-resolved absolute `$ROOT` path.

; witnessed: 2026-07-22 -> native 7.83% decode / 7.12% warm e2e of measured Ollama; next decode stone owed
