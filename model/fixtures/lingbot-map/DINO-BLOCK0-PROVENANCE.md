# LingBot-Map DINOv2-L/14 block 0 carrier

The binary carrier is the exact inclusive byte range
`8,367,248..58,762,127` from `robbyant/lingbot-map` `lingbot-map.pt`, pinned at
Hugging Face revision `204754b72bb24f561f8d7e7e1e4e4cd9e809adf9`.

```text
checkpoint-dino-block0-records.bin
50,394,880 bytes
SHA-256 e011139a4274ccbd2bd0c9035ea13854fac9dc61c7df554d1dfd84adf7ad8f57
```

It contains complete PyTorch ZIP local records `checkpoint/data/9` through
`checkpoint/data/22`: all fourteen tensors of
`aggregator.patch_embed.blocks.0`. The exact 50,393,088-byte tensor payload is
12,598,272 float32 parameters; the remaining 1,792 bytes are fourteen
112-byte local-header/alignment prefixes plus fourteen 16-byte data
descriptors. Form validates every local member name, method, payload size, and
central-directory CRC before inference.

| storage | tensor | shape | payload bytes | CRC-32 |
|---:|---|---:|---:|---:|
| 9 | norm1.weight | `[1024]` | 4,096 | 1,029,518,892 |
| 10 | norm1.bias | `[1024]` | 4,096 | 495,738,331 |
| 11 | attn.qkv.weight | `[3072,1024]` | 12,582,912 | 1,350,673,239 |
| 12 | attn.qkv.bias | `[3072]` | 12,288 | 4,160,362,176 |
| 13 | attn.proj.weight | `[1024,1024]` | 4,194,304 | 2,404,946,637 |
| 14 | attn.proj.bias | `[1024]` | 4,096 | 952,028,150 |
| 15 | ls1.gamma | `[1024]` | 4,096 | 627,579,550 |
| 16 | norm2.weight | `[1024]` | 4,096 | 2,395,079,190 |
| 17 | norm2.bias | `[1024]` | 4,096 | 2,861,423,055 |
| 18 | mlp.fc1.weight | `[4096,1024]` | 16,777,216 | 4,039,410,981 |
| 19 | mlp.fc1.bias | `[4096]` | 16,384 | 1,497,224,969 |
| 20 | mlp.fc2.weight | `[1024,4096]` | 16,777,216 | 2,377,073,633 |
| 21 | mlp.fc2.bias | `[1024]` | 4,096 | 1,438,109,029 |
| 22 | ls2.gamma | `[1024]` | 4,096 | 3,742,437,765 |

The model card and upstream repository identify LingBot-Map as Apache-2.0.
HTTP range transport is not inference; tensor validation and every model
operation consuming these bytes run in Form on `fkwu`.
