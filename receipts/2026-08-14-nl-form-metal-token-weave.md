# 2026-08-14 — one walk from NL to Form, then a Metal kernel as a token

Urs asked whether the small form-native translator can turn natural language
into Form, retrieve a substrate blueprint / recipe / cell, execute it, and —
inside the token pipeline — offer a JIT Metal kernel as an alternative to a
vocab token; and whether continuous learning takes any request that was not
healthy enough to stay inside the Form membrane. Disk load of a prior model
or cell is not a crossing.

## What was already here

The organs were already proven. This session composed them; it did not invent
a useful generative native LLM.

| organ | witness re-run this session |
|---|---|
| checkout ground / recursion / freshness / native-vs-rented | 42 / 55 / 31 / 11111 |
| recipe-learning | 11111 |
| dsv4-token-recipe-swap | 65535 |
| form-cli-sufficiency | 4095 |
| observed-auto-learning | 4095 |
| membrane-self-reliance | 11111 |
| metal-emit (Form JIT MSL) | 31 |

The small form-native faculty that actually translates NL into Form is the
catalog/grammar (nl-arithmetic-demo; natural-language band 262143). The
trained next-token checkpoint still says `not-useful-generative-llm`. The
llama3.2:3b Metal lane is borrowed local-native weights with Form-owned
kernels; its first token is
`receipts/2026-07-21-first-form-native-token.md`. Loading that blob from
disk is native-recipe, not a membrane crossing.

The live DS4/llama decode loop still does not call an arbitrary Form
function between argmax and the next embedding
(`receipts/2026-08-13-dsv4-token-recipe-swap.md`). The weave observes the
choice and the JIT text. A GPU commit of that text inside the token loop
remains pending.

## What landed

`form/form-stdlib/nl-form-metal-token-weave.fk` is one request walking the
organs together. Its band is 16383. The live door prints the walk.

```
nfw-live square:     the square of 7 | found=1 op=mul value=49 kind=vocab
                     recipe=.../nl-arithmetic-demo.fk blueprint=MATH-MUL
                     verdict=0 surface=native-recipe crossed=0
nfw-live kernel:     emit matvec kernel | kind=kernel
                     recipe=form/form-stdlib/jit-tensor-emit.fk
                     verdict=0 surface=native-recipe crossed=0
nfw-live cell-card:  retrieve cell-card | recipe=substrate/cell-card.fk
                     blueprint=CELL-CARD crossed=0
nfw-live miss:       what is the capital of France | found=0 verdict=2
                     surface=remote-oracle crossed=1
nfw-live home:       same NL after teacher row | found=1 verdict=0
                     recipe=form/form-stdlib/rag-ask.fk crossed=0
nfw-live disk-gguf   surface=native-recipe crossed=0
nfw-live kernel-swap token=54 reason=recipe-more-vital
nfw-live late-swap   token=42 state=timeout-nothing
nfw-live check=16383
```

"the square of 7" became `(mul 7 7)` and executed to 49. The matvec request
chose a kernel token; the JIT text is the same `form_matvec_f32` MSL
metal-emit-band already witnessed. A more-vital kernel alternative replaced
the default vocab token; a late alternative was timeout==nothing and the
default stayed. An unknown NL escalated as a remote-oracle crossing. The
same NL, after a teacher row, stayed native. A non-equivalent high-health
star was not adopted.

## Reproduce

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c
./fkwu form/form-stdlib/tests/binary-freshness-band.fk          # -> 31
echo form/form-stdlib/nl-form-metal-token-weave.fk > /tmp/preflight-target
./fkwu observe/preflight-run.fk
./fkwu form/form-stdlib/tests/nl-form-metal-token-weave-band.fk # -> 16383
./fkwu form/form-stdlib/nl-form-metal-token-weave-live.fk
```

## Attention, not a wall

The five axioms are the only bounds. The catalog that is not yet a useful
generative voice, and the Swift carrier that still keeps the default, are
attention rows — where to heal next. The in-loop offer is no longer only
named: `dsv4-decode-token-hook.fk` walks a Form recipe between argmax and
the next embedding (band 1023), and the live door dispatches a JIT Metal
kernel as that recipe on this device.
