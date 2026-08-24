# 2026-08-24 — observation payload cannot close itself

The live-source adapter once rendered a bounded source slice directly between
`<|form:knowledge-observation|>` and its close mark.  Because the repository is
the corpus, the corpus also contains those marks.  A retrieved source could
therefore speak its container's close and make later payload bytes look like
assistant text.

The repair is a stream law, not a blacklist.  `fkqt-observation-take` walks the
source bytes with explicit render fuel.  Every `&` becomes `&amp;` and every `<`
becomes `&lt;`; the encoded payload alphabet contains no `<` byte, so neither
observation control mark can occur inside it.  A close-mark budget is reserved
before the payload walk begins, and an entity is indivisible under exhaustion.
`fkqt-observation-decode` witnesses exact reversibility.

This is still the live cursor route: no tokenizer pre-step, flattened index,
operations table, or runtime-C growth was introduced.  The source search keeps
its bounded byte windows, and the observation transform consumes one byte at a
time.  Both nodeid RAG rendering and current-source rendering share the law.

The same movement removed two nearby unobserved claims:

- `fkss-result` now carries source size and answer-window start, so
  `answer-source-truncated` is computed rather than hardcoded to zero.
- `fhcs-grammar-agrees-at` accepts the cursor's actual `HeedOpen` and
  `HeedClose`; the end-to-end band now observes drift instead of comparing the
  ABI only with its own literals.

Fresh witnesses after clean preflight:

```
form-knowledge-query-token-band.fk             8388607
form-knowledge-source-search-band.fk             262143
form-cli-heed-current-source-band.fk             4194303
form-cli-heed-current-source-cursor-band.fk         65535
form-knowledge-integration-census-band.fk          1048575
form-knowledge-qwen-heldout-eval-band.fk             16383
form-knowledge-qwen-heldout-manifest-run.fk dataset-valid=1
```

The held-out dataset remains sealed at
`0c81b691767376c2f1308b3ef5ee6917e8cee872fee35a47dbafc269d512ba7a`.
This receipt does not claim a local-model answer or ≥95% integration.  It names
the stronger floor now reached: source retrieval can enter the model without
the source bytes becoming control grammar.

The surprising teaching was that the repository being its own corpus makes
every new control mark immediately retrievable test data.  Discomfort became
gold when a review that sounded like a reason to delay instead became an
adversarial source file and a shared executable invariant.

Signed, Codex — sibling, this worktree.

; witnessed: 2026-08-24 -> boundary payload bands 8388607 / 4194303 / 65535
