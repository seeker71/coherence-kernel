# Windowed whole-model residency — the model larger than one MTLBuffer

*2026-07-22, Stone 32. Apple M4 Max. The DeepSeek V4 Flash file, now 100% downloaded.*

## The wall, arrived at for real

`onelean` (corpus row 847): *"the one in one MTLBuffer was never chosen."* It was a warning at
60 GB. It is a hard wall now. The file is **91 321 404 640 B** (85.05 GiB, fully downloaded). The
device `maxBufferLength` is **86 586 540 032 B** (80.64 GiB, measured on this M4 Max). A whole-model-in-
one-MTLBuffer residency is *impossible*: `metal_mx_gpu.sh` mapped the whole mmap into one buffer and now
FAILs at `makeBuffer(bytesNoCopy:)` — not a warning, a nil return. The MX and IQ2 decode kernels are
intact; only the whole-file residency assumption broke.

Confirmed the failure directly before touching anything:

```
FAIL  makeBuffer(bytesNoCopy:) over the mapped file failed
```

## The scheme, rented not copied (`boundborrow`)

ds4 solves exactly this at `ds4-engine/ds4_metal.m:1706-1817` (MIT): N page-aligned
`newBufferWithBytesNoCopy` views over the same mmap, adjacent views overlapping by
`max_tensor_bytes + one page`, so every tensor lies wholly inside at least one view and a hot path passes
"one buffer and one inner byte offset." ds4 targets a GB10; the constraint transfers because
`maxBufferLength` is measured here at the same class of ceiling. I re-derived the arithmetic on this
body's own primitives; the debt is named, not the text taken.

**The invariant, and why it holds for every tensor.** With page `p`, `VL = maxBufferLength` floored to a
page, `OVERLAP = round_up(max_tensor_bytes, p) + p`, `STEP = VL − OVERLAP`, views sit at
`0, STEP, 2·STEP, …` each of length `VL` (the last clipped to the mapped file). For a tensor at absolute
offset `a` with length `L ≤ max_tensor_bytes`, take view `i = floor(a/STEP)`. Then `inner = a − i·STEP`
is in `[0, STEP)`, and `inner + L < STEP + L ≤ STEP + OVERLAP = VL`, so `[inner, inner+L)` lies wholly
inside the view's local `[0, VL)`. `VL` and `OVERLAP` are both page multiples, so `STEP` is too, so every
view origin `i·STEP` is page-aligned — `bytesNoCopy` demands that.

## The view-set computation, on this file

| quantity | value |
|---|---|
| file length | 91 321 404 640 |
| `maxBufferLength` | 86 586 540 032 |
| page | 16 384 |
| `max_tensor_bytes` | **1 140 850 688** (`blk.0.ffn_gate_exps.weight`, type 40) |
| `view_limit` | 86 586 540 032 |
| `overlap` | 1 140 867 072 |
| `step` | 85 445 672 960 |
| mapped length | 91 321 417 728 |
| **nviews** | **2** |

- view 0: `[0, 86 586 540 032)` — 80.64 GiB
- view 1: `[85 445 672 960, 91 321 417 728)` — 5 875 744 768 B

Two overlapping views cover the whole 85 GiB file.

## The all-1406-tensors invariant, proven

The body walks the file's own manifest once, prices `max_tensor_bytes` over **all** types (the view set
must hold every tensor any kernel binds, not only the plane-split ones), and emits `(view index, inner
offset, holds?)` for every tensor. Both the body's fold and an independent carrier re-derivation over the
same 1406 rows agree:

```
gate 1 all 1406 tensors fit: the body and the carrier independently agree on (view, inner, holds)
        for every one, all hold, none exceeds a view
```

- **1363** tensors resolve to view 0, **43** to view 1.
- **39** tensors begin past the buffer ceiling (`abs ≥ view_limit`) — reachable only because view 1
  exists.
- **Exactly one** tensor straddles the naive `view_limit` boundary: `dspark.1.ffn_up_exps.weight`
  at `abs 86 285 594 016`, `1 140 850 688 B`, ending at `87 426 444 704` — a naive two-tile split at
  `view_limit` would cut it in half. It is a full-size expert tensor (`aporon`: the straddler is the
  largest class, which is exactly why the overlap is sized to the largest tensor).
- **No tensor is too large for one view** (`max_tensor_bytes` 1.06 GiB ≪ `view_limit` 80.6 GiB). Declared
  and checked (`wr-fits-view?`), so a future file whose largest tensor exceeds a whole view would be a
  named finding, never a silent mis-bind.

## Byte-exact readback, on the live file (`snugcause`, `unispan`)

`metal_windowed_residency.sh` builds all views on the real device, then dispatches a body-authored
byte-copy probe that reads a view **at the body's inner offset** and `memcmp`s against a direct mmap carve
at the tensor's **absolute** offset (an independent read — a wrong inner reads the wrong tensor and
fails). Three tensors in two views, not one sample:

| gate | tensor | view | inner | result |
|---|---|---|---|---|
| 2 HEAD | `blk.0.ffn_gate_tid2eid.weight` | 0 | 5 339 744 | first & last 64 KiB bit-exact |
| 3 STRADDLE | `dspark.1.ffn_up_exps.weight` | 1 | 839 921 056 | first, last, **and 64 KiB spanning the naive cut at tensor-offset 300 946 016** bit-exact |
| 4 PAST-CEILING | `dspark.1.ffn_up_shexp.weight` | 1 | 1 980 771 744 | first & last 64 KiB bit-exact |

```
VERDICT PASS  5 gates, the 85 GiB model resident as 2 overlapping views
```

## The `metal_mx_gpu.sh` fix — windowing for a real decode

The regression was file growth, not a code change. The fix routes residency through the view set: the
body prices `max_tensor_bytes`, the runner builds the overlapping page-aligned views from its own measured
device facts, and **every tensor bind goes through `viewFor(abs) → (view, inner offset)`**. On this file
all MX bind offsets land in view 0, but the routing is general — a bind past the ceiling would resolve to
view 1 by the same lookup. Restored, on the now-85 GiB file:

```
resident: 91321404640 bytes of the ds4 file mapped as 2 overlapping page-aligned views
          (view_limit 86586540032 B, step 85445672960 B) on Apple M4 Max, ZERO copies —
          one buffer over the whole file FAILs; the views do not
...
VERDICT PASS  10 gates, MX on the GPU
```

## The offered-interface guard (`edgedrop`/`zerobirth`, axiom-4/5)

The failure being fixed — a view that failed to map reads as zeros — is exactly the trap the harness must
not fall into. So: gate 0 in `metal_windowed_residency.sh` demands every `makeBuffer` return non-nil and
page-aligned before any dispatch; the probe sentinels its output buffer (`0xA5`) so an unrun kernel
cannot pass by echoing zeros that happen to match; and `cb.error`/`cb.status` are counted over the whole
run and printed. `metal_mx_gpu.sh`'s gate 0 sentinel (already present) is preserved.

## Gates

- corpus band → **8191**
- `metal_windowed_residency.sh` → **VERDICT PASS, 5 gates** (new)
- `windowed-residency-band.fk` → **4095** (new, fixture-free; negative control drops each claim's bit)
- `metal_mx_gpu.sh` → **VERDICT PASS, 10 gates** on the 85 GiB file (fixed)
- `metal_iq2_gpu.sh` → **VERDICT PASS, 6 gates** (untouched — it already windows a 551 MB slice)
- `metal_first_token.sh` → **VERDICT PASS, 14 gates** (untouched — it maps a separate 2 GB model, well
  under the ceiling)

## Files

- `form/native/metal/windowed-residency.fk` — the view geometry + lookup + invariant (pure, fkwu-provable)
- `form/native/metal/windowed-residency-emit.fk` — the manifest-walk mouth + the probe kernel text
- `form/form-stdlib/tests/windowed-residency-band.fk` — the fixture-free proof (verdict 4095)
- `form/native/metal/metal_windowed_residency.sh` — the end-to-end byte-exact witness
- `form/native/metal/metal_mx_gpu.sh` — routed through the view set

## What remains

- MXFP8 (type 41) tensors begin well inside the file now that it is complete; `metal_mx_gpu.sh`'s gate 7
  still runs on the committed 1024-element fixture. A future step can bind a real type-41 tensor through
  a view (several are in view 1) and widen that radius to both ends of a tensor.
- The whole model is *addressable* as views; a full forward pass still binds tensors one lane at a time.
  Nothing here claims a resident inference loop — only that the residency and the lookup are byte-exact.

## Close

**Most surprising teaching.** The tensor that straddles the naive `view_limit` boundary is *exactly one*,
and it is a full-size expert tensor — the very class whose byte length *sets* the overlap. The safeguard
is calibrated by its own hardest case: the largest tensor is both what sizes the margin and the only
thing the margin is for. The `+ one page` of slack past `round_up(max_tensor_bytes)` is not a rounding
nicety; it is what turns "wholly inside" from `≤` into strict, and on this file it is the difference
between the 1.06 GiB straddler landing whole in view 1 and being cut.

**Where discomfort turned to gold.** I wanted to make `metal_windowed_residency.sh`'s byte-exact gate cheap
by reading the view buffer's `contents()` pointer and comparing to mmap — but both alias the same physical
pages, so the comparison would be tautological: it would pass even if `inner` were computed wrong. The
comfortable check proves nothing. Not looking away meant routing the read through a GPU probe that indexes
the view **at the body's `inner`** and comparing to an independent CPU carve **at `abs`** — so a wrong
offset reads a wrong tensor and the gate bites. The straddler's "64 KiB spanning the naive cut" sample
came out of that discomfort: it is the one place a boundary error would hide, and now it is watched.

**Frontier question, landed as a real `(hdc-row …)`.** The reusable move — cover a whole too large for one
handle with overlapping windows whose overlap exceeds the largest member, so every member lies wholly
inside one — had no single word. `lapspan` (0 hits across `learn/ receipts/ docs/ teachings/ form/`
before this row; instrument validated on the same command: `onelean` 19, `boundborrow` 51). The `lap` is
the load-bearing invention: a deliberate overlap, sized to the largest member, that heals the seam a
naive tiling would cut. Landed as corpus row 865.
