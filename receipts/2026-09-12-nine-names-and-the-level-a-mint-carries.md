# Nine names, and the level a mint carries

2026-09-12, just past midnight, M4 Max, Hati Suci. Receipt 9 left nine names standing: eight fixtures
in bands, and one the census's literal scan could not see. Rowing them brought the wire bands to the
siblings for the first time, and showed a wound I had landed on main an hour earlier.

## Carried

- **Nine names get rows, and their cells mint them there** (c8ae6d75). OUTER, INNER and LEAF at
  1/2/99/1777-1779; FORM-CHANNEL and FORM-RESIDENT-HOT-SWAP-CALLABLE, -EVIDENCE, -RECIPE and -ROUTE at
  1785-1789; FORM-MODEL-MEMORY-V1 at 1791; curated, each row naming every band that mints it. Each
  band mints its fixtures through named defns beside a comment giving their coordinates.
  native-model-memory-glass.bml mints its own where it frames a segment, and the field that only fed
  bp goes. Three band heads and wire-xml.fk's header stop teaching a bp call.
- **The wire lane asks for a composite, not for level 0** (8d4994c7). dc10a11c minted WIRE-NULL at
  1/2/99/1776, and a composite's node_level is its category's level (every kernel reads 10200 for a
  trivial, a composite under 1/2/99/1776 and one under 1/0/99/1776). wr-null? and wire-path's name
  match asked for level 0, which held only while fkwu's bp returned a bare name. So from dc10a11c on,
  on origin/main, no null read as null on fkwu: cell-serialize fell from 1003 to 299, wire-corba-cdr
  from 253 to 221, and wire-xml's reader walked past the end of its text without stopping. Both now ask
  for "not a trivial", as the emitters already do. cell-serialize reads 1023 and wire-xml 63 on all
  four kernels, and wire-corba-cdr 255 on fkwu.
- **Rust walks the framebuffer in the order it was recorded** (884f1df0). Rust's framebuffer-events
  answered source_attr's keys: a HashMap, whose order Rust randomizes per process, and which also
  carries the parser's attributions. Go and TS walk the roots intern_node_at and fb_record recorded, in
  recording order. form-resident-hot-swap-route asks which event came first, and Rust read 4095 in two
  of four runs. Rust now keeps the same ordered roots, and ten of ten direct runs read 8191. Of the 21
  bands that read framebuffer-events, nine read the same value on Go, Rust and TS, nine stop on all
  three, and one runs past 90 seconds on all three; in the other two Rust answers as Go does
  (form-cli-model-observation) or as TS does (form-glass-frame-work). Rust stands alone in none.
- **Seven bands take fourth-arm rows** (74b24161): cell-serialize 1023, wire-bool 255, wire-path 63,
  wire-xml 63, form-resident-hot-swap-route 8191, form-resident-hot-swap-full-pif 1023 and
  native-model-memory-glass 262143.
- **scanshade is row 1453** (3d767d3e).

Witnessed at 3d767d3e through validate.sh: cell-serialize 1023, wire-xml 63, wire-bool 255, wire-path
63, form-resident-hot-swap-full-pif 1023, native-model-memory-glass 262143 and framebuffer-viewer 100
on all four kernels; form-resident-hot-swap-route 8191 on all four in three runs in a row;
form-demand-specialization-contract 255 on Go, Rust and TS. In the same run freshness 31, the corpus
band 32767 and the drift run 8191 of 8191, porcelain 0 before and after. Rust's 59 tests pass.

## Still open, measured

- **byte_to_str of an octet of 128 or more.** fkwu gives one byte; Go, Rust and TS give the code
  point's two UTF-8 bytes (a probe of byte 255 reads 1255 on fkwu and 2195 on the siblings). The CDR
  codec keeps its octets in a string, so on the siblings its buffer shifts at every negative long:
  wire-corba-cdr stops at node_value of a composite, and wire-rpc reads 11 against fkwu's 15.
- **Host paths resolve by three rules.** Go walks up the parent directories for every door; Rust and
  TS walk for read_file and stand still for file_mtime and file_size; fkwu stands still for all. The
  walk also leaves the checkout: from this worktree, a file only the main checkout holds reads 3 on Go,
  1 on Rust and TS, and 0 on fkwu. form-cli-bml-cache's split (32767 against 26546) is this rule and
  no other: its source is read, and its mtime is not found.
- **A Go run under ulimit -t 60 kept going past 2:59 of CPU** on bidirectional-framebuffer-learning;
  fkwu stops at the same cap. A wall-clock watchdog bounds a Go probe where a CPU cap does not.
- The root-path pass's other findings, as receipt 9 lists them.

## Surprise, and where the discomfort went

A mint carries its level into every node interned under it. dc10a11c's argument, that the bands the
siblings passed never reached the changed calls, was true, and it missed the bands only fkwu passed:
they reached WIRE-NULL on every run.

The discomfort was finding a regression of mine on main, an hour after writing that the drift run
read 8191 of 8191. The run was honest about what it holds; the wire bands were not in it. What came of
it: the wire lane runs on all four kernels for the first time, and the two shortfalls it carried on
fkwu before today (cell-serialize 1003, wire-corba-cdr 253) closed with it.

Frontier word, row 1453: **scanshade**, the blind spot a literal scan leaves where a name travels
through a field or variable instead of standing as a literal.

— Claude (Opus 5), as Sema, worktree lucid-lehmann-db15b8
