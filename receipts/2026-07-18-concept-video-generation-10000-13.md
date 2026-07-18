# 10,000-concept × 13-NL semantic video generation witness

Date: 2026-07-18

## What became real

`model/concept-video-generation-10000-13.fk` now exposes a lazy generation
plan for every one of **10,000 concept ids × 13 natural-language lenses =
130,000 addresses**. The aligned NL layer now has a surface in every address;
68,370 machine-gap cells retain explicit `G` provenance instead of masquerading
as curated evidence. A plan contains the ranked lexical anchor, its aligned and
attributed per-concept NL surface, its WordNet gloss when mapped,
one of the existing thirteen grounded NL frames, a stable semantic visual hash,
an optional semantically matched footage source, and an 18-bit visible identity
envelope. The fallback remains an honest API guard, but no current table cell
requires it.

This is not a table of filenames. The live path renders Unicode text and a
meaning-conditioned moving focus mark into six 640×360 frames, encodes them as
lossless FFV1, decodes them back to 24-bit BMP, then lets Form read the pixels.
Form recovers fourteen concept bits and four lens bits from the visible bottom
band and independently hashes the caption panel above it.

The local ffmpeg build has no `drawtext` filter. The narrow
`presence/carriers/concept-video-unicode-render.swift` carrier therefore uses
CoreText/AppKit only to rasterize Form-selected UTF-8 into pixels. It does not
choose a concept, lens, caption, gloss, backdrop, identity, or verdict.

## Pure four-way address proof

`model/tests/concept-video-generation-10000-13-band.fk` exhaustively constructs
and recovers all 130,000 identity envelopes in blocks with bounded recursion.
It also checks both address-space boundaries, all lens bounds, semantic-hash
sensitivity to both caption and gloss, UTF-8-safe command hex, real-backdrop
routing, and decoded-pixel sample geometry.

```text
fkwu       8191
Go         8191
Rust       8191
TypeScript 8191
```

## Live real-footage matrix

Three existing, committed real-life trajectories were used only where their
content actually names the lexical concept:

| concept | id | committed footage | sourced | machine-gap `G` | decoded exact | animated |
|---|---:|---|---:|---:|---:|---:|
| loop | 6196 | `loop-24f.mkv` | 13/13 | 6 | 13/13 | 13/13 |
| university | 1927 | `university-24f.mkv` | 13/13 | 8 | 13/13 | 13/13 |
| court/courthouse | 751 | `courthouse-24f.mkv` | 13/13 | 0 | 13/13 | 13/13 |

Observed matrix: **39/39 decoded concept+lens identities** and **39/39
first-to-last decoded pixel changes**. Lens rows were not sampled symbolically:
all 39 six-frame videos were rendered and decoded. The aligned NL runtime
supplied **39/39 sourced labels**. Their exact provenance mix was 3 frequency
anchors (`F`), 15 WordNet/OMW labels (`W`), 7 dedicated-WordNet labels (`D`),
and 14 machine-gap labels (`G`). Form counted the 14 `G` rows from the runtime
records; they were not inferred from whether a translation looked plausible.

The actual footage surfaces now include:

```text
loop:       loop | gelung | ojete | boucle | ilhó | kitanzi | Öse | петля | 金属圈 | アイレット | حلقة | पाश | döngü
university: university | universitas | universidad | université | universidade | chuo kikuu | Universität | университет | 大学师生 | 大学 | جامِعَة | विश्वविद्यालय | üniversite
court:      court | pengadilan | corte | cour | tribunal | korti | Gericht | суд | 法庭 | 裁判所 | محكمة | न्यायालय | mahkeme
```

For example, the decoded loop/French carrier visibly renders `boucle` over the
committed office footage. Its runtime provenance is `G`, so the receipt calls it
machine-translated and unreviewed even though it is visibly coherent.

## All-13 sourced-label matrix

Three additional common concepts were selected because the aligned table has a
real sourced surface in every lens:

| concept | id | lenses rendered | sourced labels | decoded exact | animated |
|---|---:|---:|---:|---:|---:|
| good | 70 | 13 | 13/13 | 13/13 | 13/13 |
| life | 145 | 13 | 13/13 | 13/13 | 13/13 |
| go | 46 | 13 | 13/13 | 13/13 | 13/13 |

Observed: **39/39 sourced concept-aligned labels**, **39/39 decoded exact
identities**, and **39/39 animated carriers**. The actual table surfaces were:

```text
good: good | baik | bueno | bon | bom | mzuri | gut | хороший | 好 | 良い | حسن | अच्छा | iyi
life: life | hidup | vida | vie | vida | uhai | Leben | жизнь | 生命 | 生存 | حياة | ज़िंदगी | hayat
go:   go | pergi | ir | aller | ir | gura | gehen | ходить | 去 | 行く | ذهب | जाना | gitmek
```

For `life`, the thirteen decoded caption-panel hashes were all distinct:

```text
en  760265   id  64548   es 603143   fr 468711   pt-br 822046
sw  866597   de 108588   ru  37052   zh 731010   ja    565854
ar  572045   hi 204784   tr  34046
```

The decoded Chinese `life` witness visibly contains the aligned label `生命`:

```text
直接说： 生命 — 扎根于身体。
a characteristic state or mode of living; "social life"; "city life"; "real life"
concept 145 · lens 8 · semantic 610421
```

Separate decoded witnesses visibly rendered Arabic `حسن` for `good` and Hindi
`जाना` for `go`; CoreText handled both directionality and script shaping.

The same run also rendered the address boundaries without footage claims:
concept 0 through Turkish lens 12 and concept 9999 through English lens 0.
Form recovered **2/2** exact identities from their generated semantic-animation
pixels. Thus the generic path is observed as well as the three footage paths.

## Caption and gloss cause pixels

For concept 145, Chinese lens 8, the live cell rerendered the same identity
twice: once without the WordNet gloss and once without the NL caption. Form
read the decoded caption panel each time:

| render | decoded panel hash | differs from full | identity preserved |
|---|---:|---:|---:|
| full caption + gloss | 731010 | — | yes |
| caption, no gloss | 59585 | yes | yes |
| gloss, no caption | 128124 | yes | yes |

This isolates visible semantic text from the exact identity band. Removing
either text source changed actual decoded pixels while the sensed concept and
lens stayed 145/8.

Representative carrier evidence:

```text
codec_name=ffv1
width=640
height=360
pix_fmt=bgra
nb_read_frames=6
video sha256 8cd2f72209d4ca6541a30e8bf2892983a3f4eb75a20048541e59d088cd8859fd
BMP   sha256 4010a693797f52f2e7b6fd850b67ed1ee74b428b0999259302bc406d07cc3bb3
```

The Matroska mux hash may change with container metadata on a rerun; the
decoded BMP hash is the stable pixel witness used by Form.

The full live run emitted 82 videos: 39 real-footage cells, 39 fully sourced NL
cells, two generic boundary cells, and two semantic-text ablations. Temporary
FFV1 carriers were not committed.

## Honest boundary

- Per-concept localized surfaces are concept-ID aligned and source-attributed.
  `G` rows are machine-translated and unreviewed; `W`/`D`/`C` rows are also
  automatically mapped and unreviewed. Presence is not review.
- WordNet misses retain an empty gloss; the renderer does not invent one.
- Three concepts have semantically matched real footage. Other concepts receive
  a deterministic semantic animation, not falsely labelled stock footage.
- CoreText rasterization and ffmpeg encode/decode remain host carriers. Concept
  selection, semantic content, address construction, and decoded-pixel sensing
  are Form cells.
- This is compositional semantic video generation with an exact visible
  identity envelope, not learned open-vocabulary photorealistic generation.
- No Python was used. `runtime/fkwu-uni.c` was not changed.

The exchange stayed alive by requiring the aligned surface and gloss to survive
as visible pixels, not merely metadata. The surprising teaching was that one
lossless envelope can coexist with shaped Arabic, Devanagari, and CJK imagery
without confusing semantic evidence with identity evidence. Discomfort turned
to gold twice: 32-bit BMPs first forced a genuine 24-bit Form witness, then the
old independent NL sequence forced the renderer to wait for concept-ID-aligned
evidence rather than quietly displaying unrelated translations. The final
machine-gap layer added a second honest distinction: total presence became real
only by keeping generated translations visibly separate from sourced lexicons.
