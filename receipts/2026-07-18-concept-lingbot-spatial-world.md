# Real video content and LingBot geometry meet in one world movement

Date: 2026-07-18

## What now runs together

The earlier organs were individually real but stopped one seam short: visual
content produced concept IDs, while the native-resolution LingBot routes
produced trajectories, depth, clouds, and object rows. This movement observes
each route through both paths and joins only after both have independently
finished:

1. four held-out BMP frames enter the pretrained classifier as pixels only;
2. Form composes raw labels and accepts or abstains without receiving the scene
   name, expected ID, caption, or identity envelope;
3. the 24-frame LingBot path visits every source pixel, emits 49×4 ViT frames,
   streams the 683-row GCA cache, composes 24 poses, predicts 1,472 depths, and
   creates 1,472 cloud/native-object rows;
4. an accepted concept is persisted through `cwm-persist-detection` at the
   final observed camera viewpoint.

The expected ID is consulted only by the acceptance witness after detection.
It does not enter the classifier or `cvcs-detect-labels`.

## Live results

| route content | detected ID | visual score / margin | path length | final camera viewpoint | depth/cloud/object rows | combined rows | verdict |
|---|---:|---:|---:|---|---:|---:|---:|
| office | 493 | 34 / 34 | 6.556486 | `[0.524292, 0.865418, 0.333075]` | 1,472 / 1,472 / 1,472 | 1,473 | 255 |
| university | 1927 | 10 / 10 | 5.013107 | `[0.229986, 0.847898, 0.332139]` | 1,472 / 1,472 / 1,472 | 1,473 | 255 |
| courthouse | 9066 | 12 / 12 | 7.000985 | `[-0.135076, -0.101276, -0.806413]` | 1,472 / 1,472 / 1,472 | 1,473 | 255 |

Each concept entity matched its exact 14-bit persistent cell, has persistence
2, and carries the exact final viewpoint vector produced by its own geometry
run. Each underlying native-resolution scene independently scores 1023.

Executable doors:

```text
presence/concept-lingbot-spatial-world-office.fk
presence/concept-lingbot-spatial-world-university.fk
presence/concept-lingbot-spatial-world-courthouse.fk
presence/concept-lingbot-spatial-world-examples.fk
```

## Gates

```text
model/tests/concept-spatial-world-band.fk
  fkwu 255   Go 255   Rust 255   TypeScript 255

presence/tests/concept-lingbot-spatial-world-live-band.fk
  fkwu 255
```

The pure band also proves that an abstained visual detection creates no concept
entity. No Python ran and `runtime/fkwu-uni.c` was not changed.

## Honest floor

- The concept position is the observation viewpoint, not a semantic bounding
  box, segmentation mask, or inferred object center inside the point cloud.
- The visual decision remains the three-concept Form evidence vocabulary over
  host-pretrained labels; it is not 10,000-class visual parity.
- The native ViT/GCA state and released camera token are operational, but the
  complete learned LingBot graph and metric depth scale remain parity 0.
- Geometry is classical relative reconstruction and may drift.

What kept this alive was refusing to call two adjacent world outputs an
integration. The surprising teaching was that the content and geometry runs
could remain independent all the way to the final persisted entity. Discomfort
turned to gold when “position” was narrowed to its honest meaning here: the
camera viewpoint that observed the concept, not an invented courthouse center.
