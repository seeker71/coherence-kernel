# 10,000 × 13 text round-trip and held-out lexical witness

The exhaustive Form gate calls the indexed detector for every one of the
130,000 committed concept/locale cells. It observed 130,000 expected IDs,
zero failures, 85,394 unique cells, 44,606 collision-bearing cells, and 220,504
candidate entries. Source states and combined primary-WordNet plus attributed
Wiktionary-morphology-overlay sense states matched the pinned indexes for every
cell. The overlay contributes 1,073 method-3 anchors: the combined semantic
surface is 8,444 mapped anchors and 1,556 honest misses, so the 13 lenses contain
20,228 unmapped, 23,946 unique-sense, and 85,826 ambiguous-sense cells. The
compact operational proof is 2,001,008 bytes: a
12-byte cell index and complete `u16` candidate stream. The source manifest
records whole-file SHA-256 hashes for both artifacts and the pinned NL, source,
primary semantic, and overlay semantic inputs;
those hashes are reproducibility metadata, not runtime verification. The former
35 MB duplicated TSV and the earlier length-only per-cell hash artifact are not
part of the proof.

Concrete live rows span operational domains rather than placeholder phrases:

| Lens | Input fragment | Expected ID/surface | Complete candidates |
|---|---|---|---:|
| en | road crew closed the street because of black ice | 629 / street | 1 |
| id | petugas memeriksa pintu darurat | 296 / pintu | 1 |
| es | el autobús llegó tarde por una avería | 1102 / autobús | 1 |
| fr | le médecin a vérifié la dose | 370 / médecin | 1 |
| pt-br | desliguei o fogão antes de sair | 6600 / fogão | 1 |
| sw | wahudumu waligawa maji baada ya bomba kupasuka | 377 / maji | 1 |
| de | ein Baum blockierte die Landstraße | 1071 / Baum | 1 |
| ru | предупреждение пришло на телефон | 332 / телефон | 1 |
| zh | 老师把书和作业发给了学生 | 571 / 书 | 1 |
| ja | 川の岸が浸水した | 916 / 岸 | 1 |
| ar | وزعت فرق الطوارئ ماء نظيفًا | 377 / ماء | 1 |
| hi | बच्चा दवा की अगली खुराक लेगा | 468 / बच्चा | 1 |
| tr | ambulans hastane acil servisine geldi | 628 / hastane | 1 |

Collision probes preserve rather than hide ambiguity: Indonesian `ya` returns
17 IDs, Spanish `Señor` 9, French `ouais` 13, Portuguese `ah` 17, Swahili
`kupiga kelele` 17, German `schlagen` 9, Russian `да` 16, Chinese `开始` 12,
Japanese `ああ` 17, Arabic `رائع` 15, Hindi `चिल्लाना` 13, and Turkish `evet`
17. English `street` is the one-candidate control. For all 13 groups the compact
index candidate IDs are compared with the original full 10,000-row Form scan.

The held-out gate is separate from self-round-trip. Its 13 practical positive
and 13 unrelated negative sentences are passed to the full 10,000-row detector
without supplying an expected surface. The 13 ambiguity/control probes are a
separate, explicitly prompted literal-surface check used to compare the compact
index against brute-force equality lookup. Success means exact, boundary-aware
lexical occurrence and complete surface-equality recovery. It is committed data
lookup, not learned or native inference, and does **not** establish syntax,
entailment, reference resolution, translation quality, or language understanding.

Witness commands:

```sh
./fkwu --src cognition/tests/concept-text-roundtrip-10000-13-live-band.fk
./fkwu --src cognition/tests/concept-text-roundtrip-10000-13-heldout-live-band.fk
```

Most surprising: translated surfaces create large, structured collision fields
even where English is unique. Discomfort turned to gold when review exposed an
unverified hash claim: removing it left a smaller proof whose runtime assertions
say exactly what the Form walk observes.
