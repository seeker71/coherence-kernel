# LingBot-Map learned patch-weight slices

These two files are complete, byte-identical PyTorch ZIP local records from
Robbyant's released **balanced** `lingbot-map.pt` checkpoint:

| File | Tensor | Shape | Data bytes | Checkpoint byte range | SHA-256 |
|---|---|---:|---:|---:|---|
| `checkpoint-data7-record.bin` | `aggregator.patch_embed.patch_embed.proj.weight` | `[1024,3,14,14]` | 2,408,448 | 5,954,448…8,363,023 | `2c07e9f1d118d54358dc10eb56b16b8d4b81f3f0da11b2712133f1b8d1b54880` |
| `checkpoint-data8-record.bin` | `aggregator.patch_embed.patch_embed.proj.bias` | `[1024]` | 4,096 | 8,363,024…8,367,247 | `1bc851cacd9e6532372dafd9b4a3195ade615843e5c4d5d5bd6fd4bd33df94fe` |

Source: <https://huggingface.co/robbyant/lingbot-map>, pinned repository revision
`204754b72bb24f561f8d7e7e1e4e4cd9e809adf9`. The model card and upstream
repository identify LingBot-Map as Apache-2.0. The fetch script uses HTTP byte
ranges, verifies both complete-record digests, and never downloads or executes
PyTorch/Python.

The Form runtime validates each local header, payload length, central-directory
CRC, and float32 values before using the weights. The full weight tensor is
retained so the executable first-channel slice is not a hand-copied coefficient
fixture and later native channels can grow without changing provenance.
