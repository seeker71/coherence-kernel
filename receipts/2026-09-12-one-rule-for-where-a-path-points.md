# One rule for where a path points

2026-09-12, past midnight, M4 Max, Hati Suci. Receipts 9 and 10 measured host paths resolving by three
rules across the four kernels, and a walk that climbed out of a worktree into the main checkout. This
piece gives every kernel one rule, bounded at the checkout.

## Carried

- **One host-path rule for every read door, bounded at the checkout** (0416d9d2). A path that
  stands where the kernel runs names itself; otherwise dir/p, dir/form/p and dir/form/form/p are tried
  from the working directory upward, stopping at the first directory with a .git entry. Go, Rust, TS and
  fkwu each carry it once: resolveKernelHostPath, resolve_kernel_host_path, resolveHostReadPath and
  fk_host_resolve. Every read-side door asks it: read_file, the byte and slice reads, file_size,
  file_mtime, exists, is_dir, fs_list, source_inventory, source_scan_file, read_form_binary and fkwu's
  file_open. Doors that create or change a file never walk. Go's write_form_binary stops resolving, and
  Rust's second write_form_binary registration, which replaced the first, goes. In TS the doors resolve
  through Host.resolveReadPath and the node host's primitives name paths as given, as Go's and Rust's
  do, so the rmdir and remove guards never walk. Go's resolver drops its api/config/api.json branch,
  which the bounded walk covers.
- Probes from form/, before and after: a body-root-relative path read fkwu 8, Go 15, Rust 9, TS 9, and
  now 15 on all four; a file only the main checkout holds read Go 3, Rust 1, TS 1, fkwu 0, and now 0 on
  all four; the directory doors read fkwu 0, Go 24, Rust 0, TS 24, and now 23 on fkwu and 31 on the
  siblings (fkwu has no file_byte_at).
- **Of the twelve root-path splits receipt 9 listed, four close on all four kernels**:
  form-cli-bml-cache 32767 (Rust and TS read 26546), form-frontier-surface 8191 (7422), reason-coverage
  2147483647 (1073741823) and source-compiler-native-text-boundary 1023 (1020). Five that receipt 9
  saw stop under validate agree on all four when run directly, before and after:
  program-image-fkb-byte-decode, program-image-fkb-byte-file-witness,
  runtime-program-image-fkb-attempt, runtime-program-image-fkb-capability-bound and
  source-compiler-fkb-file-emission. form-cli-author-altitude, outside that list, now reads its
  declared 783 on all four (the siblings read 776). On fkwu, json-meaning-ingestion and whisper-block0,
  which stopped at str_len of a read that found nothing, now read their rows: 1000 and 1023.
- **Over the 137 bands that read a body-root-relative path**, run directly on the four kernels before
  and after: 129 read the same, seven moved to four-way agreement, and four-way agreement rose from
  101 to 107. The seven: form-cli-author-altitude, form-cli-bml-cache, form-frontier-surface,
  form-knowledge-integration-census, msl-lane-coverage (the siblings read 18 of 31), reason-coverage
  and source-compiler-native-text-boundary. The one band that moved the other way, dsv4-control-emit,
  ran past the 90-second watchdog on Go under load; alone it reads 31 on the old and the new Go, in
  about 91 seconds each.
- **All seven take fourth-arm rows** (fe760009): each at the verdict its head declares, and
  form-knowledge-integration-census, whose head declares none, at its full 1048575.
- **climbstop is row 1455** (17bdd24f).

Witnessed at fe760009 through validate.sh: form-cli-bml-cache 32767, form-frontier-surface 8191,
source-compiler-native-text-boundary 1023, form-cli-author-altitude 783, reason-coverage 2147483647,
msl-lane-coverage 31 and form-knowledge-integration-census 1048575 on all four kernels, and
whisper-block0 1023 on all four, its row red on fkwu before this piece. In the same run freshness 31,
the corpus band 32767 and the drift run 8191 of 8191, porcelain 0 before and after. Rust's 59 tests,
Go's fkwu tests, the TS node-host proof (7 of 7) and both TS type-checks pass.

## Still open, measured

- **fkwu has no file_byte_at**; Go, Rust and TS do. The directory probe reads 23 on fkwu for that bit
  alone, before and after this piece.
- **Go resolves a probe's `; preludes:` from the body root differently**: a scratch probe outside the
  tree stops at `import "form-stdlib/core.fk"` on Go from the root, before and after this piece. The
  prelude loaders do not use the door resolver on any kernel.
- **qwen4exp-flash-next-generate stops on Rust and TS where bytes pass through a text door.** Its
  tokenizer reads a GGUF model's metadata through equireach's string slices, and Rust and TS decode a
  slice as UTF-8 and answer binary bytes as null: str_byte_at meets null in eqr-at
  (equireach.fk:65) under egg-kv-count and gmt-find. fkwu and Go read 255. It is bytesmear's family, and
  equireach holds the window as a host string on purpose: its header measured a list carrier at 166
  seconds, and the native str_byte_at reaches any byte at once. Carrying bytes on Rust and TS needs a
  byte window that keeps that reach.
- **form-cli-resident-turnwheel-bml stops on Rust and TS at jit_leaf_inram**, a door only fkwu and Go
  carry, and Go reads 33279 against fkwu's 65535.
- **json-meaning-ingestion now reads its row's 1000 on fkwu, and Go, Rust and TS still stop under
  validate at one of its preludes**, form-stdlib/compiler.fk, which carries a raw `section [form.bml]`
  block that prepare_sources does not lower. The band stood red on every lane before this piece.
- **A band that scans the live tree reads the moment it runs in.** form-knowledge-integration-census
  read 983039 on Go and Rust once, while my own passes were writing caches into the tree it scans;
  alone, twice, it reads 1048575 on all four, and each condition of its bit 65536 passes on all four.
  A witness of such a band wants a quiet machine.

## Surprise, and where the discomfort went

Two fkwu rows that stood red were never missing anything. json-meaning-ingestion and whisper-block0
had been read as bands waiting on host fixtures; their files stood in the tree all along, and fkwu,
standing at the body root, looked for them where they are named from form/. Given the same rule as the
siblings, fkwu found them, and both read their rows.

The discomfort was a number of my own making. With two passes running beside it, the census band
read 983039 on Go and Rust against 1048575 on fkwu and TS, and it would have been easy to name that a
kernel seam. Taking its bit 65536 apart one condition at a time showed all four kernels agreeing on
every condition, and the band, run alone, reads 1048575 everywhere. It carries a row now, and the
teaching that a band reading the live tree is witnessed on a quiet machine.

Frontier word, row 1455: **climbstop**, the place where a search climbing toward the root
stops: the checkout that holds the one who searches.

— Claude (Opus 5), as Sema, worktree lucid-lehmann-db15b8
