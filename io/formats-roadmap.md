# Communication formats — the honest read/write roadmap

The world communicates in markdown and raster; the body must read and write them to be in conversation.
Two distinct kinds of work, never conflated:

- **WRITE = a str_concat text-emitter** (the `form-glsl` / `svg-emit` pattern). Tractable, four-way today.
- **READ of text = a BMF cursor grammar-as-data** (the `tokenize-grammar` / `shell-grammar` pattern). One engine.
- **READ/WRITE of raster = a binary codec**, built deliberately keystone-first. The cursor's grammar model is the
  wrong tool for compressed binary; these are real codec builds.

## The ladder (by what each format actually requires)

| Format | Kind | What it needs | Status |
|--------|------|---------------|--------|
| `.svg` write | text emit | str_concat | **done** — `svg-emit` four-way (PR 3862) |
| `.md` write | text emit | str_concat | **building** — `md-emit` (heading/bold/link/item) |
| `.md` read | cursor grammar | headings/emphasis/lists/links as grammar rules | **next** — a markdown grammar on the one cursor (like `tokenize-grammar`) |
| `.svg` read | cursor grammar | XML/element grammar | follow-on |
| `.gif` | raster codec | **LZW** (simplest compression) + palette + frames | pending — LZW is the keystone |
| `.png` | raster codec | **DEFLATE/inflate** (zlib) + filters + chunks | pending — checksums `crc32` + `adler32` ALREADY in hand; DEFLATE is the missing keystone (Huffman + LZ77), shared with zlib, so it unlocks the most |
| `.jpg` | raster codec | **DCT + Huffman + quantization + YCbCr** | pending — the hardest, last |

## The keystone order for raster

**DEFLATE first.** It is the shared keystone (png + zlib + the hard half of the compression family), and the
body already holds the checksums that frame it (`crc32` for PNG chunks, `adler32` for the zlib wrapper). A minimal
`inflate` (fixed + dynamic Huffman, the LZ77 back-reference window) is the single highest-leverage codec to build,
four-way, before any container work. LZW (gif) is simpler and independent; DCT/Huffman (jpg) is its own large lift.

Self-presentation does NOT wait on this ladder — that path is SVG (vector, animatable, `svg-emit` + `self-image`).
This ladder is for **interop**: reading what humans send, writing what they read.
