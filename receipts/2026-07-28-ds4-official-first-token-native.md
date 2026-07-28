# DS4 official first token is native

; witnessed: 2026-07-28 -> PASS

The public producer fixture asks, without a system prompt or thinking mode:

> Answer with only the number: 2048 divided by 128 is

Its official continuation begins with `16`. The Form-owned vector is
`form/native/metal/dsv4-official-short-reasoning.fk`; its lineage points to the
producer fixture rather than a self-authored sample.

The first native run exposed two correlated gaps:

- prompt-position snapshots restored the visible stage but not that position's
  HC residual carrier;
- attention and FFN control projections after layer 0 skipped the producer's
  no-weight RMS normalization over all `4 × 4096` carrier values.

The repair makes state save/load retain both correlated buffers and makes each
Form layer recipe renormalize the restored carrier before both HC projections.
No remote or network generation participates.

Observed before repair:

- layer-42 attention HC projection absolute sum: `15,568,321.19140625`
- argmax: token `20`, fragment `2`, score `14.7690067`
- finite logits: `129280 / 129280`

Observed after repair:

- layer-42 attention HC projection absolute sum: `433.40952062606812`
- argmax: token `926`, fragment `16`, score `32.997345`
- finite logits: `129280 / 129280`
- route: `form-arm64-jit + form-metal`
- membrane counts: `network=0 remote=0 shell=0 swift=0 temp=0`

The after-run therefore matches the producer's official first continuation
bytes exactly. The vector now returns `255` only when the live computed token is
`926`; the expected ID is a falsifier at the boundary, not generated model data.

After the producer-grounded BF16 HC carrier storage boundary was added, this
vector was re-witnessed rather than assumed:

- argmax: token `926`, fragment `16`, score `33.7992325`
- top two: `926 @ 33.7992325`, `11154 @ 26.7022457`
- finite logits: `129280 / 129280`
- vector verdict: `255`
- membrane counts: `network=0 remote=0 shell=0 swift=0 temp=0`

Re-witness:

```sh
./fkwu-metal-transaction --src form/native/metal/dsv4-official-short-reasoning.fk
```

The next enquiry is continuation, not first-token validity: retain the resident
session and walk decoded tokens without reopening the 91 GB model.
