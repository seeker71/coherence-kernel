# DS4 Python-free native continuation

; witnessed: 2026-07-26 -> PASS

`form/native/metal/metal_dsv4_stack.sh` now launches no Python process and
accepts no reference-vector directory. Form emits the model kernels; one Metal
lifetime maps the immutable GGUF, carries all 43 heterogeneous layers, projects
the real 129,280-row vocabulary head, feeds each selected token into the next
embedding and router step, and retains the growing KV rows.

Observed command:

```sh
FORM_DS4_KV_SEQUENCE=1 FORM_DS4_KV_CAP=10 FORM_DS4_KV_STEPS=8 \
  ./native/metal/metal_dsv4_stack.sh
```

Observed native continuation:

```text
[19129, 566, 56959, 295, 270, 27855, 566, 35907]
```

The compiled Form tokenizer decoded those exact IDs as:

```text
﻿using this.CharField in the aggregate this connects
```

The run passed 106 gates. It retained all 512 bits of KV row 0, wrote all 512
values of row 7, left all 512 row-8 frontier sentinels intact, and changed all
16,384 final hyper-connection entries. The zero control adapter left all
129,280 logits and the winning token/logit bit-identical; explicit probe vectors
selected each reserved row `128000..128004` without changing a non-control
logit.

This is an execution/plumbing witness, not a semantic-quality claim. The input
for this bounded witness was token `671`, not a fully tokenized chat template,
so the decoded continuation proves native generation and feedback but is not
yet a useful answer to a natural-language request.
