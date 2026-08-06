# DS4 43-layer stack re-witness

The live 91,321,404,640-byte DeepSeek-V4-Flash artifact completed all 43
heterogeneous layers through direct Metal.

The original all-at-once witness became driver-immobile because four full
Python oracle walks and the 86 GiB Metal mapping shared one lifetime. The
oracle outputs were retained separately, memory was released, and depth was
then increased as an observed sequence:

```text
1 layer  -> PASS 37 gates
2 layers -> PASS 69 gates
4 layers -> PASS 133 gates
8 layers -> PASS 209 gates
16 layers -> PASS 283 gates
32 layers -> PASS 405 gates
43 layers -> PASS 471 gates
43 layers + native exit head -> PASS 473 gates
```

The final run carried all four hyper-connection streams through every layer,
including hash routing, top-k routing, compressed RoPE, MXFP4, MXFP8 and
IQ2_XXS regimes. Wall time was 4.43 seconds at position 0 and 4.05 seconds at
position 7. Every one of 16,384 final HC entries changed across positions.

The exit head was then composed in the same Metal lifetime: HC no-weight RMS,
F16 head projection, sigmoid stream weights, four-stream collapse, output
RMSNorm, 129,280-row MXFP8 vocabulary projection, and greedy argmax. Both
positions emitted token id **19129**:

```text
pos 0 -> token_id=19129, logit=25.161182
pos 7 -> token_id=19129, logit=25.154716
```

The artifact's Form-native tokenizer decoded id 19129 to raw bytes
`[239,187,191,117,115,105,110,103]`, hexadecimal
`efbbbf7573696e67`: a UTF-8 BOM followed by `using`. The raw bytes are retained
because rendering the BOM away would alter the observed token.

This is a real first-token observation from one supplied token id at two
positions. It is not yet a natural-language request completion. The next
closure is the request tokenizer joined to an autoregressive KV-cache loop,
feeding each emitted id back until EOS or an explicit completion bound.

; witnessed: 2026-07-26 -> 43/43 direct-Metal DS4 layers + native exit head, 473 gates PASS, token 19129 / efbbbf7573696e67
