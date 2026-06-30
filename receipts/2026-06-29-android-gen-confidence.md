# Receipt — Rung 4: generation confidence wired to the router axis

**Status: WITNESSED on device.** v1 and v2 failed honestly, showing that raw LM-head margin alone can be
overconfident on nonsense. v3 passed by gating raw margin confidence with grounding before feeding the router
confidence axis.

## Pre-registered metric

For each generated token, the device recipe keeps the LM-head top-1 and top-2 logits and computes the margin:

```text
margin = top1_logit - top2_logit
```

The token confidence score is bucketed from the f32 margin:

```text
margin >= 1.0  -> 100
margin >= 0.5  -> 75
margin >= 0.25 -> 50
else           -> 0
```

The answer confidence is the integer average of those token scores across the generated tokens.

## v1 gate and result

The first pre-registered gate was:

```text
HIGH confidence: answer_confidence >= 50
LOW confidence : answer_confidence <  50
```

This is the live version of `observe/thought-framebuffer.fk`'s margin trace: the model is more confident where
the chosen token clearly beats the runner-up.

## Router wire

The generated-answer confidence feeds the existing `fcr-confidence-axis` as an evidence observation:

```text
passes   = answer_confidence
attempts = 100
router_confidence = fcr-confidence-axis(passes, attempts)
```

This preserves the router's single confidence axis instead of introducing a second route formula.

## Device gate

Two fixed prompts will be run on the Galaxy S23 / Adreno path:

```text
in-distribution : "The capital of France is"
out-of-distribution: "zxqv jlmno"
```

Observed on `R5CW20DK17A`:

```text
stdout: 820000052
rung4.conf: in_conf=87 out_conf=52 in_router_conf=87 out_router_conf=52
```

So v1 **failed honestly**: the OOD prompt was still above the too-low `50` cutoff.

## v2 gate

The second gate keeps the same token-score buckets but tightens the HIGH/LOW boundary to the bucket that means
the average generated token cleared at least the `0.5` logit-margin tier:

```text
HIGH confidence: answer_confidence >= 75
LOW confidence : answer_confidence <  75
```

The fixed v2 prompts are:

```text
in-distribution : "The capital of France is"
out-of-distribution: "mork blenf"
```

The receipt may be marked witnessed only if the same device run reports HIGH for the in-distribution prompt,
LOW for the v2 out-of-distribution prompt, and the router confidence equals the answer confidence for both
lanes.

Observed on `R5CW20DK17A`:

```text
stdout: 820000079
rung4.conf: in_conf=87 out_conf=79 in_router_conf=87 out_router_conf=79
```

So v2 also **failed honestly**: raw margin alone can be high on nonsense. This is the overconfidence problem
Rung 4 is meant to expose.

## v3 gate

The third gate keeps the raw margin score, but it is no longer accepted by itself. The final generated-answer
confidence is:

```text
grounding_confidence = 100 if the prompt text is present in rung4_ground.txt, else 0
answer_confidence    = min(raw_margin_confidence, grounding_confidence)
```

The HIGH/LOW boundary remains:

```text
HIGH confidence: answer_confidence >= 75
LOW confidence : answer_confidence <  75
```

The fixed v3 prompts are:

```text
in-distribution : "The capital of France is"
out-of-distribution: "mork blenf"
```

The grounding source is the real Rung 3 receipt,
`receipts/2026-06-29-android-native-generate.md`, pushed to the device as `rung4_ground.txt`.

The receipt may be marked witnessed only if the same device run reports HIGH for the grounded in-distribution
prompt, LOW for the ungrounded OOD prompt, and the router confidence equals the final answer confidence for both
lanes.

Observed on `R5CW20DK17A`:

```sh
adb -s R5CW20DK17A push model/form-llama-generate-rung3-WIP.fk /data/local/tmp/formvk/rung3.fk
adb -s R5CW20DK17A push receipts/2026-06-29-android-native-generate.md /data/local/tmp/formvk/rung4_ground.txt
adb -s R5CW20DK17A shell 'cd /data/local/tmp/formvk && : > rung4.mode && FORM_KERNEL_STACK_MB=2048 timeout 2400s ./fkwu --src rung3.fk'
```

```text
stdout: 4
rung4.conf: raw_in_conf=87 raw_out_conf=79 in_conf=87 out_conf=0 in_router_conf=87 out_router_conf=0
```

The raw OOD margin remained high (`79`), but grounding was absent, so final OOD confidence was `0`. The router
confidence matched the final answer confidence for both lanes (`87` and `0`) through `fcr-confidence-axis`.
