# Receipt — the RICH sense recipes run natively on the android fkwu via the flatten path (2026-06-29)

**The correction this closes.** The android sense app (`com.coherence.sense`/`LiveSenseActivity`, device
R5CW20DK17A) labelled who/what/where "pending flatten" while running only `(if (le 50 luma) 1 0)` for
presence. That framing was wrong: flatten is proven and observed (the working `loop-table` IS a flattened
`form-eval-full` recipe already on the phone). The rich recipes were never blocked by flatten — they simply
had not been flattened WITH the eval driver as preludes and pushed. Done now.

## What was flattened, and how (the reproducible command)

The Form flattener (`form-flatten.fk`) executed by the `bin-go` bootstrap (the flattener's executor only,
never the runtime) flattens `live-observe-cli.fk` WITH the sense recipes as preludes into the
`observe-table` fkwu walks:

```
bin-go  <FOURTH_CHAIN: minimal-surface hati-os-kernel host-io-fs-fkwu-emit fkc-table-serialize
                       hati-os-kernel-emit form-parse form-flatten fourth-shim>
  '(print (fks-table-file
            (flt-band-sources-fns (list (read_file "fourth-shim.fk") (read_file "core.fk")
                (read_file "input-stream.fk") (read_file "presence-feature.fk")
                (read_file "scene-features.fk") (read_file "spatial-fusion.fk")
                (read_file "confidence-weighted-vote.fk") (read_file "mesh-sense-7w.fk")
                (read_file "fused-observation.fk") (read_file "live-observe.fk"))
              (read_file "live-observe-cli.fk"))
            (flt-band-sources-pool ...)))'
  -> observe-table.txt  (58602 bytes)
```

Wired into `experiments/coherence-sense-android/build-android-fkwu.sh` (`flatten_observe_table`) so the
table regenerates from source on every build; the emitted table then runs on fkwu with no Go on device.

## The recipe — camera-native fused observation

`live-observe.fk` composes the proven camera organs over ONE downsampled NxN luminance grid:
- `present` <- `pf-present?` (presence-feature: lower-center occupancy variance above a wall baseline)
- `where`   <- `sf-objects` (scene-features: the coarse blocks whose detail clears a floor) → `fo-fuse`
- `who`/`what` — un-witnessed on a camera-only frame (no face-match, no microphone). Named, never faked.

Proven four-way (Go, Rust, TS, fkwu) at `validate.sh`: **`live-observe` band -> 127, 0 divergent**
(`form/form-stdlib/tests/live-observe-band.fk`; manifest `fourth-arm-bands.txt`).

## Witnessed native — mac, then android metal

`live-observe-cli` over a staged luminance grid, on the c-bootstrapped fkwu (`cc` on mac, NDK clang for
android arm64), no go/rust/clang/python/node in the run:

```
input: 100 100 100 100  100 100 100 100  100 10 250 100  100 250 10 100   4 20 30   (a present frame)
  mac fkwu     -> 1 120 2 2 2     (present=1 occupancy=120 where-count=2 where-first=2 fused-where=2)
  android fkwu -> 1 120 2 2 2     (device R5CW20DK17A — toolchain scan: none found)

input: 100 100 100 100  100 100 100 100  100 100 100 100  100 100 100 100   4 20 30  (a vacant frame)
  mac fkwu     -> 0 0 0 -1 0      (present=0 occupancy=0 nowhere placed)
  android fkwu -> 0 0 0 -1 0
```

The five planes are byte-identical mac↔android — the same flattened table, the same numeric walk.

## The app — every plane native, no pending

`LiveSenseActivity` now extracts a 4×4 luminance grid (Kotlin averages blocks only) and feeds
`FkwuSense.observe()`; fkwu runs the fused observation through the observe-table. The screen reads
presence / occupancy / where as `native fkwu · pf-present? / pf-occupancy / sf-objects → fo-fuse`, and
who/what as `un-witnessed (no organ this frame)` — the "pending flatten" labels are gone. The proof line
shows the raw native observation (`0 1 0 -1 0`) and names the recipe. Screenshot witnessed on metal.

## Honest floor / the real wall named

- **Witnessed:** the rich sense recipes (pf-present? · sf-objects · fo-fuse) run native on android metal via
  the flatten path; present + where are native readings on the phone, four-way 127.
- **The honest gap (a missing ORGAN, not a missing flatten):** who (identity) and what (sound) need a
  face-match organ and a microphone reading that a camera-only frame does not carry. They are un-witnessed,
  not pending — when those organs arrive their readings drop into the SAME `fo-fuse` with no new path.
- **c-bootstrap row:** clang-built (NDK). The clang-free `form-asm → form-elf` build is the separate pending
  row, exactly as the standard receipt names it.
