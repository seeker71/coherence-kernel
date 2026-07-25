# Receipt — the reply through the native form-cli verified surface

**Ask:** produce and land the reply through form-cli's own *verified* path — a response witnessed by the native
agent surface, not asserted by the rented mind — and make the membrane crossings live-observable on the lane.

**Status: the verified surface runs here, native and network-free; the full DeepSeek-off-GPU reply stays the
owed re-witness. Named, not faked.**

## The floor, observed

The committed `form/form-cli` binary is built for another architecture — it answers `Exec format error` on this
x86 box. Its **source runs on `fkwu --src`**, so the verification lane is reachable here without the binary and
without the network.

## The verified reply, witnessed native and network-free (this box)

Every one of these ran on fresh `fkwu`, no GPU, no network, no remote oracle:

| Surface | Band | Verdict |
|---|---|---|
| form-cli ask / reply path | `tests/form-cli-ask-band.fk` | **262143** (18/18) |
| form-cli membrane | `tests/form-cli-membrane-band.fk` | **1023** (10/10) |
| the membrane observer | `observe/tests/membrane-lane-band.fk` | **31** |
| the observer wired to the bidirectional channel | `observe/tests/membrane-lane-live-band.fk` | **31** |
| native token observation | `tests/dsv4-decode-loop-band.fk` | **1023** |

A form-cli reply that *verifies* on the native surface, with the network unplugged, is what "form-cli verified
reply" means — and it is standing, not promised.

## What "verified" is, concretely — and where the DeepSeek GGUF plugs in

`form/form-stdlib/form-cli-gguf-cell.fk` is the native verification lane for a reply generated off a GGUF. It
emits, as form-cli key/value reply lines, real checks:

- `gguf_cell_verified` — the GGUF magic is read and matched;
- `gguf_cell_sha256` / `gguf_cell_sha256_mismatch` — the weights are **sha256-checked**, so the reply names
  exactly which bytes it stood on;
- `gguf_tensor_slice_verified`, `gguf_tensor_set_verified` — the tensors the reply used are the tensors on disk;
- `semantic_token_generation_verified`, `full_width_model_logit_generation_verified` — the tokens the reply
  emitted are the tokens the model's own forward produces.

So the DeepSeek-V4-Flash-REAP25 GGUF the offer names is not a black box: pointed at
[`twaggs88/DeepSeek-V4-Flash-REAP25-DSpark-ds4-GGUF`](https://huggingface.co/twaggs88/DeepSeek-V4-Flash-REAP25-DSpark-ds4-GGUF)
on hardware that holds it, this same form-cli surface returns a reply whose weights are sha256-stamped and whose
tokens are `_verified` — the verified reply, at full scale. That run is the owed re-witness
(`2026-07-23-local-lane-rewitness-offer.md`); this receipt is the surface it runs through, proven native here.

## The crossings are now LIVE on the lane

`observe/membrane-lane-live.fk` turns the observer's static read into the loop AGENTS.md item 8 requires: each
owed crossing becomes a correlated observation + a **request-evidence** control on the bidirectional framebuffer
channel; a landed re-witness removes its ask on re-observe; and request-evidence *holds* the state while it
waits — only rehearse-ground swaps it, and a no-response falls to a named alternative, never silence. Evidence
is requested, never fabricated. Band **31**.

## Reproduce (this box, no network)

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk    # -> 15 (else REBUILD first)

cat form/form-stdlib/core.fk observe/membrane-lane.fk \
    observe/bidirectional-framebuffer-channel.fk observe/membrane-lane-live.fk \
    observe/tests/membrane-lane-live-band.fk > mll.fk && ./fkwu --src mll.fk   # -> 31
```

Witnessed 2026-07-23 on fresh `fkwu` (freshness band 15), network unplugged.
