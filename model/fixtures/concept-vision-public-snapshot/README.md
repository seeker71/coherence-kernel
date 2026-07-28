# Public real-photograph snapshot

This directory freezes 24 decoded photographs from the first 50 rows returned
by Wikimedia Commons' `Category:Quality images` API on
`2026-07-18T06:52:00Z`. `SOURCE-SNAPSHOT.json` records the exact API endpoint,
response identity, page IDs, source pages, authors, licenses, derivative URLs,
pixel dimensions, byte sizes, and SHA-256 values. `PROVENANCE.tsv` is the flat
offline audit view. Licenses are per row; attribution remains with each author.

Selection happened **after** a content-only Apple Vision sweep of all 50 API
rows. The 24/24 original target count is therefore an admitted content corpus,
not held-out accuracy. The selection deliberately spans 24 domains and retains
nearby claim misses. No source title, caption, author, domain, target, or
expected ID is passed to the classifier. It sees only numeric local paths
`0001.jpg` through `0024.jpg`.

`MODEL-OUTPUTS.tsv` preserves all 20 raw output rows for each original,
bottom-occluded, and center-cropped image: 72 streams and 1,440 observations.
`capture-model-outputs.sh` reproduces those streams; `verify-model-outputs.sh`
requires byte-identical output on the current host. The model is Apple's
OS-rented `VNClassifyImageRequest` on macOS 26.3.1 build 25D771280a, arm64.
The learned weight bytes and weight hash are not exposed by that API.

Offline source-byte verification:

```sh
./fetch.sh --verify-only
```

Network reproduction from the exact derivative URLs:

```sh
./fetch.sh
```

Acquisition failures are not erased. `ACQUISITION-ATTEMPTS.tsv` records the
rejected COCO HTTPS attempt, Commons search rate limit, and the first malformed
ffmpeg command before the corrected successful run. No insecure TLS bypass was
used.
