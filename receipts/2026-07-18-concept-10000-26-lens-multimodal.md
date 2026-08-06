# 10,000 concept anchors crossing 26 text lenses and live audio/video

This receipt is an observed rung toward the larger 10,000-concept multimodal
mind. It does **not** relabel symbolic carriers or bounded templates as learned
semantic generation.

## Ranked concept substrate

`model/concept-10000-ranked.dat` and
`model/concept-10000-lexical-index.dat` hold exactly 10,000 real English
lexemes from HermitDave/FrequencyWords OpenSubtitles2018, pinned at commit
`525f9b560de45753a5ea01069454e72e9aa541c6` under CC-BY-SA-4.0.

- ranked table: 300,000 bytes / 10,000 fixed-width rows, SHA-256
  `b790d53d43d2a651ed8e9e52deffc91805546abc012177dc98b8f7b33e9381e4`
- lexical index: 260,000 bytes / 10,000 sorted rows, SHA-256
  `f825944c54675fd7e4a94b2655fd9fd5fe2b3e4f6df6d3cc9a5653c24944dc4f`
- exact first and last anchors: `you` at ID 0 / count 28,787,591 and
  `caviar` at ID 9,999 / count 2,510
- live detections: `love` → 122, `world` → 227, `camera` → 959;
  `coherence-kernel` declines as absent

The source meaning is frequency-ranked lexical anchors, not 10,000
disambiguated word senses, definitions, or learned embeddings.

Proof:

```text
concept-10000-band: fkwu / Go / Rust / TypeScript = 127 / 127 / 127 / 127
concept-10000-live-witness: [300000,10000] + [260000,10000] + real rows
```

## Thirteen NL and thirteen PL lenses

The natural-language registry executes `en`, `id`, `es`, `fr`, `pt-br`, `sw`,
`de`, `ru`, `zh`, `ja`, `ar`, `hi`, and `tr`. “Top” means the first thirteen
non-pending NL seats in the repository locale registry on 2026-07-18;
population-ranking parity remains explicitly zero. Each seat detects bounded
presence/enquiry/affirmation/frame meanings and generates/reframes text.

The programming-language registry executes Python, JavaScript, TypeScript,
Java, C, C++, C#, Go, Rust, Ruby, PHP, Swift, and Kotlin. Each seat detects a
real source fixture and emits bounded add/subtract/multiply functions. Its
detectable generation carries a valid comment marker and satisfies
`detect(generate)==target` for every seat.

```text
nl-lenses-13-band: fkwu / Go / Rust / TypeScript = 8191 × 4
pl-lenses-13-band: fkwu / Go / Rust / TypeScript = 1048575 × 4
```

The PL capability vector remains `[1,1,0,0,0,0,0,0,0]`: lexical detection and
bounded generation exist; full parsing, AST round-trip, typechecking,
compilation, execution, arbitrary transpilation, and identifier escaping do
not. The NL cells are phrase-semantic lenses, not learned fluency or general
translation. Four NL rows remain `seed-pinned-unreviewed`.

## One joined concept crossing all 26 text lenses

`presence/concept-lenses-26-live.fk` detects `water` from the real table:

```text
id=377 rank=378 frequency=193014
13 NL generated rows, all re-detected as their source lens
13 PL generated rows, all re-detected as their source lens
26/26 rows carry concept id 377 and lens slot 0..12 in the same 18-bit code
verdict=127
```

The native fkwu runtime detects arbitrary generated NL frame detail with its
UTF-8 byte path. The minimal TypeScript proof walker documents a Latin-1
non-ASCII byte seam, so its four-way law remains on exact Unicode phrases;
that limitation is not hidden behind the runtime result.

## Actual audio and video generation plus sensing

`presence/concept-multimodal-live.fk` generated concept 4,242 / lens 7 as two
real files and sensed their identity back inside Form:

```text
audio: PCM s16le, mono, 8000 Hz, 1.5 s
encoded/recovered frequencies: 468 / 968 / 1440 Hz
recovered address: concept=4242 lens=7

video: lossless FFV1, 180×80, six frames
decoded BMP: 43,254 bytes
recovered address: concept=4242 lens=7
verdict=127
```

The audio signal is a three-band symbolic tone code, not multilingual speech.
The video is a visible colour-bit signal, not learned semantic footage of the
concept. They are genuine generated/sensed media and a common address membrane,
not substitutes for the semantic generators still owed.

`model/tests/concept-multimodal-full-live.fk` walked every concept ID and every
lens dimension without retaining a 130,000-row list:

```text
concepts exhausted: 10000, checksum 972438
lenses exhausted at both concept edges: 13
valid address space: 130000
audio/video code parity: 1
semantic generation parity: 0
```

The pure codec is independently witnessed `127/127/127/127`.

## Runtime seam exposed

The temporary C seed currently ignores `host-exec`’s supplied stdin argument.
The first PPM video carrier therefore received zero bytes. No C growth was
added. Form now emits a bounded ffmpeg colour-filter command instead, then
decodes the lossless result and owns pixel recovery. The untrue stdin claim is
left visible as a separate runtime repair, not silently relied upon.

## Remaining work orders toward the actual goal

1. Replace lexical anchors with disambiguated concepts carrying definitions,
   relations, embeddings, and multimodal training evidence.
2. Give all 10,000 concepts reviewed lexical surfaces in all thirteen NL
   lenses, followed by held-out detection and learned generation.
3. Add programming-language identifier escaping, parsers, AST round-trips,
   typechecking, execution, and concept-to-program task generation.
4. Replace tone codes with intelligible multilingual speech generation and
   native acoustic detection across all thirteen NL seats.
5. Replace colour codes with semantic image/video generation and learned
   concept detection, joined to the LingBot world model.
6. Execute and score the full semantic 10,000 × 13 NL × 13 PL matrix. Its
   completion parity remains `0` today.
