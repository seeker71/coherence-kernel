# Aligned translation evaluation floor — 2026-07-20

Distinct per-language rows were incorrectly standing too close to translation
claims. This work separates four evidence planes:

- 125,165 distinct attributed monolingual Tatoeba sentences;
- 5,417 additional attributed Swahili human-source BUNGE passages, explicitly
  not sentence-aligned translation examples;
- 94 retained English→Swahili direct Tatoeba link pairs with both attributed
  endpoints and a deterministic 79 development / 15 heldout split;
- actual heldout translation success: source-copy baseline `0/15`.

The direct-link scan found 95 candidates inside the committed English source
band. The indexed provenance join retained 94 and refused one whose target
endpoint was absent from the committed projection. Every retained row carries
source and target locale, language, sentence ID, author, license, URL, review
state, source-row hash, text, relation state, snapshot, archive URL/hash, and
the Form-computed SHA-256 of its exact upstream link row.

Live final audit:

```text
FRAMEBUFFER STAGE ... hash-and-recheck-94-link-rows
duration-ms=6991 dispatches=417132867 boxed-floats=0 io-sense=2
outcome=complete-94-hashed-audited-pairs
```

Audit totals: `94` rows, `79` development, `15` heldout, `94/94` links
re-found, `94/94` hashes valid, baseline exact `0`, failures `0`. Alignment
truth gate: `4095`.

This artifact creates a real evaluation door. It does not create a translator,
human review, arbitrary-language coverage, or infinite translation. Those
remain zero until a learned native emitter produces heldout targets.

The exchange stayed alive by replacing row-count implication with direct
relations and output comparison. The surprising teaching was that 95 valid
link candidates still yielded only 94 fully attributable pairs. Discomfort
became gold when the source-copy baseline scored zero and made the missing
translator impossible to hide behind corpus volume.
