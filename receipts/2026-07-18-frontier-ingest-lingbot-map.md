# 2026-07-18 — LingBot-Map enters as bounded context, not a borrowed boast

## Arrival and ground

Urs shared [Superman's X post](https://x.com/thesupermanmx/status/2077779856050606155)
and asked for it “into seeker71 coherence-kernel.” The post points to
[Robbyant/lingbot-map](https://github.com/Robbyant/lingbot-map) and the paper
[Geometric Context Transformer for Streaming 3D Reconstruction](https://arxiv.org/abs/2604.14141).

The fresh checkout witness returned `42`, `55`, `15`, `[1, 2.5, [3, 4]]`, and
`11111` before the ingest began.

## What the primary source actually supports

LingBot-Map is a feed-forward model for streaming monocular 3D reconstruction.
Its Geometric Context Transformer separates three kinds of memory: anchor
context, a pose-reference window, and trajectory memory. The repository reports
about 20 FPS at 518×378 with paged-KV attention and stable runs beyond 10,000
frames, with released checkpoints and Apache-2.0 code.

The implementation was read at pinned upstream commit
`7ff6f3ed0913d4d326f8f13bbb429c4ffc0195c2`. The same repository names boundaries the X caption drops: video-RoPE was trained
on 320 views; performance can degrade beyond that cache range; inference distance
is bounded by training distance; pose collapse can require keyframe tuning or
windowed state resets. The runnable release is PyTorch/CUDA and recommends
FlashInfer. No benchmark or throughput number was reproduced in this checkout.

## What entered

`ingest/frontier-ingest-lingbot-map.fk` sorts seven findings through the unchanged
knowledge-ingest law: **2 body, 3 liquid, 2 compost** (`20302`, band `127`).

- **Body:** selective hierarchical retention is a durable streaming principle;
  and honest range/reset caveats belong beside every long-context claim.
- **Liquid:** LingBot-Map is a concrete future visual-world roadmap, but the body
  has no loaded weights, image encoder, camera-pose/depth heads, point-cloud
  reconstruction, or native single-GPU witness.
- **Compost:** “any scene,” “without falling apart,” and unqualified local use of
  the authors' FPS/benchmark claims.

## Native build after naming

This ingest grew three pure-Form organs, with no Python runtime in the body:

- `model/geometric-camera.fk`: pinhole depth-pixel unprojection, camera-to-world
  reconstruction, row-major 3x3 rotation, SE(3) inverse and composition,
  relative camera pose, and anchor-scale normalization. The band checks actual
  reconstructed points, transform round trips, composed frames, relative pose,
  and normalized coordinates: **127 four-way**.
- `model/geometric-context-cache.fk`: the paper's exact context equation. A dense
  frame carries `M+6` tokens; anchors and the local window remain dense; evicted
  frames retain six trajectory tokens. At `T=10,000`, `n=3`, `k=16`, `M=500`,
  it computes causal `5,060,000` tokens versus GCA `69,500`, with marginal
  post-window growth of six tokens/frame. It also computes page count and the
  `k(k-1)=4,032` ordered relative-pose pairs for a 64-frame window: **127 four-way**.
- `learn/streaming-context-retention.fk`: the policy-level sibling, retaining
  anchor, recent, and selected trajectory observations while releasing obsolete
  intermediates: **127 four-way**.

These implement geometry and state mechanics, not the learned DINO/VGGT-derived
weights, FlashInfer kernels, or pose/depth heads. That remaining gap stays liquid.

## Closing

What kept this alive was following the social claim to its primary code and then
letting the source's own caveats shape the organ. The most surprising teaching was
that the durable gift was not “3D at 20 FPS” but a three-timescale memory anatomy.
Discomfort turned to gold where the exciting phrase “without falling apart” met
the repository's explicit pose-collapse and reset guidance: the contradiction
became a bounded retention law and an honest roadmap instead of a boast.
