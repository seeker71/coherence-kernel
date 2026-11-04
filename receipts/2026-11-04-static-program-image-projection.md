# An admitted cell projects to a static graph

2026-11-04. The catalog stored on a cell admission is the graph. `pifbc-project` walks it through callbacks and does not read a hold. `fkwu-native-ops` names the same rows the census still reads as `flt-ops`.

Observed on this tree, after `cc -O2 -o fkwu runtime/fkwu-uni.c`:

- `form/form-stdlib/tests/cell-run-band.fk` → 16777215. A library change moves the admission identity. A fresh load counts 2 source units and 3 source functions. The resident call parses nothing.
- `form/form-stdlib/tests/program-image-static-projection-band.bml` → 127. An admitted `(defn pisp-answer () 7)` projects (5 nodes, 1 symbol). A second projection is the same graph. `nothing()` refuses. The rows are a sealed program image, and the demand wire is their flat. The function body in that image is a native page and answers 7.
- `form/form-stdlib/tests/runtime-program-image-fkb-current-answer-capability-band.fk` → 16777215, including the synthetic projection cases (changed observation, unknown tag, unowned root, wrapper cycle, optional child) and a live admission: a retained closure still answers 445 and 446, the rewritten library answers 336, the resident call parses nothing, and the projection writes no effect.

The projected rows are the program image. `pifbc-projection-table` fills the table with those rows, the function bodies, and the string bytes. `pifbc-projection-container` seals that table. `bdj-projected-program-wire` is the flat a later demand compares.

Observed on the same cell: the band answers 127. The table is valid. The container is ready. A second projection flats to the same image. A process id was rewritten and is absent from that flat. The demand wire is the flat.

The function body in that image is the page. Demand, rooted there, installs it and the page answers 7. The kernel tags stay in the image; they are not instructions of that page. `bml-demand-jit-guard-band.fk` still answers 511.

The catalog reader stays a temporary seed carrier. Form owns the projection.
