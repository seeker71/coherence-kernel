# 2026-07-26 — DS4 BMF control-token allocation

## Observation

The model's own GGUF metadata was scanned by
`dsv4-control-token-probe.fk` through the native `fkwu` program-image
selector. No `--src`, sibling proof walker, external tokenizer, JSON, or
network lane participated.

The 129,280-row vocabulary contains:

- 127,997 normal tokens (type 1);
- 1,277 control tokens (type 3);
- 6 user-defined tokens (type 4);
- **zero unused tokens (type 5)**.

The type-3 bank includes numbered reserved pieces beginning at ID 128000.
Their artifact bytes are `<｜place▁holder▁no▁0｜>`,
`<｜place▁holder▁no▁1｜>`, and so on. Five rows are therefore aliased:

| BMF meaning | DS4 ID |
|---|---:|
| NIL | 128000 |
| CUT | 128001 |
| FAIL | 128002 |
| TIMEOUT | 128003 |
| CHOICE | 128004 |

`dsv4-control-tokens.fk` checks both type 3 and the exact stored piece bytes
before admitting the mapping. This does not resize or rewrite the base model.

## Honest semantic seam

Representability is complete; learned meaning is not implied. Reserved rows
already exist in all vocabulary-shaped structures, but their original
activations are not evidence that DS4 understands the BMF aliases.

The adapter must supervise:

- sparse input deltas for rows 128000–128004 of `token_embd.weight`
  (`[4096,129280]`, F16);
- sparse output-logit deltas for the same rows of `output.weight`
  (`[4096,129280]`, type 41), or an equivalent five-row logit adapter;
- transformer LoRA/delta capacity so surrounding language predicts and
  consumes the controls semantically;
- observation of the existing token-indexed routing structures
  `blk.0..2.ffn_gate_tid2eid.weight` (`[6,129280]`) and
  `dspark.*.markov_head.markov_w{1,2}` (`[256,129280]`). They need no shape
  extension, but their existing placeholder-row behavior must be measured and
  overridden by an adapter if it conflicts with the learned controls.

The base GGUF remains immutable. An adapter can be removed without changing
the artifact, and the alias manifest remains inspectable Form data.

## Corpus

`dsv4-control-training-corpus.fk` generates 750 original Form records:
120 training examples and 30 held-out semantic-boundary examples for each of
the five classes. Its native band returns 127 only when every class and split
is balanced.

; witnessed: 2026-07-26 -> reserved control aliases exact at ids 128000..128004; native manifest band 31; balanced corpus band 127
