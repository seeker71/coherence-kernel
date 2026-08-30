# 2026-08-30 — airlower: the lowering went airborne, and the ground holds no footprint

Urs's refinement, minutes after the file-lane floor landed: "this shall
be possible without creating a file, all in memory." Built and
witnessed within the hour.

## What changed

The lowered text no longer touches disk. The compile door
(`bml-floor-compile.fk`) gained a fileless mode: asked with `-` as its
output, it prints the lowered source (its `// preludes:` line carried)
closed by a sentinel, and the runner captures it from the pipe. The C
side split the source collector so a unit root — or any `; preludes:`
dependency — can enter from an in-memory buffer registered under the
`.bml`'s OWN identity (path, mtime, digest of the lowered bytes). The
`.bml` lane then rides the ordinary compile tail:

```
cold  ./fkwu form-cli-rest-force.bml   5.8 s   lowered in the pipe,
                                               compiled, frozen as ice
      derived source file:             NONE
      artifact:                        form-cli-rest-force.bml.fkb only
warm  ./fkwu form-cli-rest-force.bml   6 ms    ice loads directly
```

Witnessed after removing every derived artifact first: rest-force-bml
band 31, offer-ask-bml 65535, author-high 1023 — all three lowering
their `.bml` preludes in memory mid-collection — organ band 134217727
and native-vs-rented 11111 for radius, ground 42 and freshness 31 on
the rebuilt seed.

## Seams, named not hidden

- The warm gate reads the `.bml`'s own mtime: an edited PRELUDE beneath
  an untouched `.bml` wants its `.bml.fkb` removed; the hash-checked
  lane would re-lower on every run to know better. One honest widening
  remains here (a dep table peek in the ice).
- A `.fk` root that PRELUDES a `.bml` recomputes its unit hash each run,
  which lowers the dep in memory each time (~1s): correctness paid in
  child-spawn time. The same dep-peek widening heals both.
- Windows arm still answers a named refusal.

## The most surprising teaching

The fileless lane came out SIMPLER than the file lane it replaced — no
mtime dance between .bml and a derived source, no gitignore rows doing
law's work, one identity instead of two. The intermediate file had not
been carrying convenience; it had been carrying an extra truth that
then needed guarding.

## Where discomfort turned to gold

Splitting fk_src_collect_file felt like surgery too deep for the hour —
the collector is the trunk every source run climbs. The discomfort
sharpened the cut to exactly one seam (after-read vs read), and the
split immediately paid twice: the same collect-bytes door serves both
the root lane and the prelude lane, and the ground witnesses (42, 31,
11111) held on first rebuild.

; witnessed: 2026-08-30 -> fileless .bml lane: cold 5.8s / warm 6ms, no
; derived source, ice only; bands 31 / 65535 / 1023 through in-memory
; prelude lowering; radius 134217727 + 11111; corpus row 1171 airlower
