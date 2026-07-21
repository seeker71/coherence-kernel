# 125,165 distinct attributed human sentence rows, with the 130,000 gap left open

**Verdict:** the body now carries a random-access projection of **124,583**
Tatoeba source rows across all thirteen NL lenses. Joined by globally unique
Tatoeba sentence id with the earlier concept-stratified 1,300-row snapshot,
there are **125,165 distinct attributed rows**: 718 overlap and 123,865 are
net-new. This is a large real expansion, but it is not 130,000. The exact
hash-pinned Swahili archive ends at 4,583 eligible attributed rows, so the
remaining honest deficit is **5,417 Swahili rows**.

No sentence was repeated, translated, paraphrased, or synthesized to make the
target look complete. The state remains `human-contributed-unreviewed`: source
attribution is present, but native-speaker status, review, factual correctness,
parallel translation, and novel generation are not claimed.

## Framebuffer first: where the work and cost went

Every locale intake opened a kernel framebuffer window before selection and
closed it only after the projection and index were written. The 13 measured
intake stages consumed **595,502 ms** and **110,382,485,288 native dispatches**.
Every projected row's source hash was computed by the Form SHA-256 recipe over
the exact reconstructed upstream six-field row; this dominates the cost.

The later exhaustive content audits re-read all 124,583 projected rows through
the fixed-width index, checked structure and strictly increasing source ids,
reconstructed the original Tatoeba row, and recomputed every source-row hash.
They consumed **519,075 ms** and **113,188,467,530 native dispatches**. Result:
**124,583 structurally valid, 124,583 source-hash matches, zero failures**.

| lens | rows | projection bytes | intake ms | intake dispatches | audit ms | audit dispatches |
|---|---:|---:|---:|---:|---:|---:|
| en | 10,000 | 2,356,904 | 46,168 | 8,141,752,135 | 38,750 | 8,368,722,495 |
| id | 10,000 | 2,555,151 | 46,807 | 8,373,289,063 | 39,408 | 8,635,493,569 |
| es | 10,000 | 2,507,720 | 45,851 | 8,200,564,381 | 38,521 | 8,461,708,053 |
| fr | 10,000 | 2,394,857 | 46,574 | 8,198,710,661 | 39,027 | 8,418,589,074 |
| pt-br | 10,000 | 2,592,083 | 45,801 | 8,394,742,166 | 40,732 | 8,672,731,366 |
| sw | **4,583** | 1,250,845 | 20,142 | 4,380,454,694 | 19,918 | 4,491,695,509 |
| de | 10,000 | 2,524,851 | 45,317 | 8,300,024,771 | 40,407 | 8,549,583,041 |
| ru | 10,000 | 2,861,111 | 59,748 | 10,644,729,918 | 49,700 | 10,829,217,689 |
| zh | 10,000 | 2,517,786 | 43,469 | 8,216,378,624 | 40,040 | 8,484,493,445 |
| ja | 10,000 | 2,535,655 | 48,130 | 8,876,386,187 | 40,435 | 8,968,211,810 |
| ar | 10,000 | 2,625,149 | 47,914 | 8,945,459,904 | 40,995 | 9,177,692,485 |
| hi | 10,000 | 2,964,949 | 61,636 | 11,329,707,209 | 52,759 | 11,494,065,037 |
| tr | 10,000 | 2,517,184 | 37,945 | 8,380,285,575 | 38,383 | 8,636,263,957 |

The complete projections occupy **32,204,245 bytes**. Their 13 random-access
indexes occupy **498,384 bytes**. An index is one little-endian u32 offset before
each row plus a terminal offset; Form checks its exact size, initial header
boundary, every row boundary used by the exhaustive walk, and terminal equality
with the shard size.

The recorded exhaustive runs still included an old transport-hash verification
at the tail of each audit window. That call did not select, parse, hash, or judge
any row; the complete source-content scan had already produced its independent
counts. It was then removed. The committed runtime contains **no `host-exec` or
shell callable**. Pinned whole-file hashes remain provenance in the manifest and
were verified externally as byte transport only; the operational integrity path
is now the stronger per-row Form reconstruction.

## Source and acquisition boundary

All thirteen archives are the exact URLs and SHA-256 values already pinned in
the earlier `ARCHIVES.tsv`, retrieved 2026-07-18 under CC BY 2.0 FR. `curl` and
`bzip2` transported and decompressed those bytes into stdin; they made no corpus
decision. Form alone validated the detailed-export shape, required exact
language/positive id/named contributor/nonempty sentence, retained the first
10,000 eligible rows in upstream order, computed each row hash, and wrote the
projection and index. Downloads remained under `/tmp` and did not enter git.

`SOURCE-MANIFEST.tsv` binds every archive URL/hash/size, projection and index
hash/size, row count, scan/rejection count, source bytes observed, intake trace,
and outcome. Its SHA-256 is
`8dd098075152721e3725be907e27be376db6a014331887e06cc2047dae07fbd7`.

Japanese required scanning 22,851 upstream rows because 12,851 lacked the named
contributor required by this corpus contract. Arabic rejected 48 of 10,048.
Other completed lenses reached 10,000 without rejection. Swahili exhausted the
entire source at 4,583; the EOF became `source-exhausted-no-padding-shortfall-5417`.

## Executable evidence

Fresh kernel grounding before the build returned `42`, `55`, freshness `15`,
`[1, 2.5, [3, 4]]`, and native-vs-rented `11111`.

The bounded runtime/material/union gate returns:

```text
./fkwu --src cognition/tests/concept-human-corpus-130000-band.fk
32767
```

The Form-native old/new union audit returned:

```text
old rows              1300
new projection rows   124583
overlap                718
distinct source rows  125165
net-new over old      123865
shortfall              5417
```

Per-locale old/new overlaps were `48, 61, 48, 54, 58, 100, 51, 43, 48, 52,
45, 58, 52` in lens order. A source scan over the new Form files finds no
`host-exec` or `host_exec` token.

## Honest floor

- This is human-contributed quote material, not a learned generative model.
- It is unreviewed and not guaranteed parallel across languages.
- The source-order projection is broad corpus volume, not proof that all 10,000
  concepts occur or that the complete detector recognizes each sentence.
- The original local WordNet carrier contains 40,446 attributed usage-example
  occurrences, but only 25,471 exact strings and many are phrases/fragments.
  They were deliberately not relabeled as 40,446 human sentences.
- Completing 130,000 still requires 5,417 additional genuinely attributed,
  permissively licensed Swahili rows from a newly pinned source.

The exchange stayed alive by letting the source end at 4,583 instead of filling
the hole with copies. The surprising teaching was the price of provenance:
per-row native SHA-256 pushed both intake and audit beyond 110 billion
dispatches. Discomfort turned to gold when the smallest archive exposed the
target as impossible under the authorized pins; the shortfall became an exact
work order rather than a hidden quality loss.
