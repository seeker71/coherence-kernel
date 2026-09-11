# A typo spelled right

2026-09-12, around half past seven in the morning, M4 Max, Hati Suci. Receipt 27 left preflight
calling a name no kernel resolves a typo, where a unit in the tree defines it and the chain does not
load that unit.

## Carried

- **preflight names the unit that defines a name no kernel resolves** (9afc7862). When all four
  kernels miss a name, preflight searches the tree for the unit that defines it — `(defn name ` in
  direct source, `def name(` in high grammar — and answers `UNPRELUDED — defined in <path>, which
  this chain does not load`. The path is repo-relative, a spelling `; preludes:` reads. Only a name
  no unit defines still reads TYPO. The walk is BML in preflight-source.bml, over fs_list,
  fs_is_dir, source_inventory, read_file and str_find, which all four kernels carry. Its roots are
  the tree's top-level directories, read off the disk; dot-directories are passed by, and so are
  tests, seedbank, node_modules and target inside the roots.
- **Readings.** On four scratch cells: tn-softmax reads UNPRELUDED in
  form/form-stdlib/transformer-numerics.fk, pfs-ends? in form/form-stdlib/bml/preflight-source.bml,
  pf-cat in observe/preflight.fk, and a name defined nowhere reads TYPO. Each page takes a second
  or less, the typo's whole walk included.
- **The doors** — AGENTS.md item 9, BOOTSTRAP.md, CLAUDE.md and preflight's header — name three
  repairs where they named two.
- **falsetypo is row 1471** (25243bca).

Witnessed at 9afc7862: preflight-band 131071 through validate.sh on its fkwu lane and on Go run from
the root, its new bit 65536 reading tn-softmax's unit; preflight-source-band 65535 four-way; drift
31 of 31 in each run; freshness 31, the drift run 8191 of 8191, porcelain 0 before and after.

## Still open, measured

- **An fkwu process that runs hot leaves its boards in its working directory.** fk_heat_write
  writes `.fkwu-heat.<pid>` and `.fkwu-boxing.<pid>` and removes only its `.tmp` staging files.
  At 07:33 this worktree's root held 7403 entries, 7349 of them dot-entries: 5886 heat boards, and
  1443 boxing files, the oldest from 2 September. The main checkout holds 3546 and 67.
  lane-motion.bml reads a board by its hearth's pid and jit-speed-witness.fk reads the day's boards
  after the runs, so the heal has to know when a board has been read. The comment above the writer
  still says one file per working directory.
- **Six rowed bands reach a door fkwu does not carry**: pg_exec, pg_query and pg_connect (Go and
  Rust carry them). They read their registered verdicts on fkwu with that call unresolved.
- **The five vk live lanes** read on fkwu apart from their registration, staged on the Vulkan door
  through host-exec, which this host is not carrying now.
- **Go reads form-cli in 135 s where Rust and TS take 1 s**, lowering each BML prelude through the
  whole compiler chain on every run.
- The open items of receipts 12 to 27 stand where they are not named here.

## Surprise, and where the discomfort went

The surprise was what the tree's root is made of. Of its 7403 entries, 54 are the tree. The other
7349 start with a dot, and 7329 of those are boards fkwu processes wrote and never took back. They
came to light only because the walk had to ask the root what it holds.

The discomfort was the first green. The first search walked form/form-stdlib, where tn-softmax's
unit stands, and it read right on the one case written for it. It was the same mistake one
directory out: a name defined in observe/, learn/ or model/ would still have read TYPO, preflight's
own pf-cat among them. What turned it was asking the walk about a unit outside the directory it
searched. The answer is the whole tree now.

Frontier word, row 1471: **falsetypo**, a name spelled right and read as a typo, because the reader
searched the chain that called it and not the tree that holds it.

— Claude (Opus 5), as Sema, worktree lucid-lehmann-db15b8
