# DS4 official four-token stream remains an open witness

; witnessed: 2026-07-28 -> PENDING

The producer's public code-completion fixture supplies this prompt:

> Complete the C statement with the next exact token only:
> return snprintf(buf, sizeof(buf), "%d", value

Its greedy continuation begins with three backticks, `c`, a newline, and
`return`.  The Form-owned vector
`form/native/metal/dsv4-official-short-code-stream.fk` opens the local model
once, prefills the 27-token chat prompt, and performs three more decode passes
against the resident KV state.  It returns `255` only for the producer's exact
four-token bytes.

The live route presently returns:

- token IDs: `201,9854,69,201`
- decoded bytes: `\n```c\n`
- expected token IDs: `9854,69,201,3916`
- finite logits at every step: `129280 / 129280`
- route: `form-arm64-jit + form-metal`
- membrane counts: `network=0 remote=0 shell=0 swift=0 temp=0`
- vector verdict: `0`

The sequence is structured but shifted: after the unexpected leading newline,
the next three generated tokens equal the producer's first three tokens.  The
framebuffer's first-step top eight are:

| rank | token ID | score |
|---:|---:|---:|
| 1 | 201 | 32.9505653 |
| 2 | 9854 | 28.721962 |
| 3 | 671 | 28.6442375 |
| 4 | 28986 | 28.1934929 |
| 5 | 9544 | 26.0224266 |
| 6 | 32111 | 25.9554596 |
| 7 | 66 | 25.781435 |
| 8 | 3916 | 25.3706074 |

The expected first token is therefore visible but is `4.2286033` logits behind
the newline.  This disproves a near-tie explanation.

The model artifact itself is not a version guess.  The local file is
`91,321,404,640` bytes and its live SHA-256 is
`000974720296f2cad17ac0525796f4bb9ceaac9f4015ed61af3fba445dfb1039`; both
exactly match the producer's published v5mx artifact.  Model identity is
therefore closed as the cause of this continuation divergence.

One producer-grounded repair was tested in this walk.  The upstream carrier
stores HC values as BF16 after initial broadcast and after every HC post
operation.  Form now performs the same round-to-nearest-even storage boundary.
The internal values changed but the four token IDs did not, so BF16 carrier
storage was a real semantic gap but is not the cause of this divergence.

The top-eight candidates and scores are now emitted by the Form-authored Metal
argmax kernel on every live step.  They are diagnostic observations, not host
reconstructed guesses.

Re-witness:

```sh
./fkwu-metal-transaction --src form/native/metal/dsv4-official-short-code-stream.fk
```

The next enquiry starts before argmax: compare the remaining prefill graph
operations with the producer at the first layer and position where their states
diverge.
