# Native CLI assembly

Form owns source discovery, snapshots, startup generation and recipe compilation.
The host carrier links and publishes the executable. A fresh checkout needs the
C seed and the host C compiler; generated startup C is private build output.

Run `form/scripts/regen_form_cli_bootstrap.sh [output-dir]` to seal the current
source, generate startup bytes, build one candidate and publish that candidate
with its `.fkb`, `.sym`, attestations and stamps. The default output directory is
`form/form-stdlib/bootstrap`. The dependency manifest installs beside the sources
it describes. Failed candidates retain their evidence when
`FORM_CLI_RETAIN_WORKDIR=1`.

Run `form/build-form-cli.sh` to verify the published bundle and install
`form/form-cli` as a relative link. An explicit output path receives a verified
copy. With `FORM_CLI_FORCE_LINK=1`, Form generates startup from a checked snapshot
and the carrier builds a new executable and recipe. Installation reads the
attested bundle directly; generated C and historical CLI tables have no place
in the published source tree.

## Source ownership

`native-table-sources.bml` follows source declarations and retains each file's
complete bytes. Identity roots include the program, compiler, source runtime,
opcode and node-word headers, Form's node-word authority, host build carriers and
`home-index.txt`. The registry carries every declared source home, including
homes not yet called. Non-Form inputs participate in identity without being
parsed as programs.

`form-cli-source-closure.bml` computes the portable manifest and SHA-256 digest.
Each row contributes `path:<relative>\nbytes:<count>\n<content>\n`, with the
generated manifest first. It writes and reads back an owned snapshot retaining
repository-relative paths. A local lookup boundary keeps compilation within
that snapshot. Missing bytes, malformed registry rows, NUL, absolute source
dependencies and changed sources refuse admission.

The existing native protocol carries the build:

- `FCSC1` creates a sealed closure and snapshot from supplied identity roots.
- `FCSI1` publishes the checked manifest; `FCSV1` verifies original and snapshot
  bytes, manifest, digest and genesis.
- `FCSE1` generates startup C from the sealed runtime and held genesis.
- `FCEP1` copies those checked bytes into the private candidate; `FCEV1`
  verifies the same emission after compilation.

`native-cli-startup.bml` owns the startup template. Form emits the exact genesis
archive into its private C input. BML lowering remains in RAM. Neither lowering
nor startup generation adds a second source authority.

The bootstrap attestation binds source digest and stamp, emitted startup,
author binary and runtime source. The platform attestation additionally binds
that attestation, platform, executable, image and symbols. Installation checks
those identities and challenges the executable through its public CLI.
Publication holds one owner lock, verifies the sources again, renames completed
artifacts into place and publishes stamps last. Mixed generations fail their
identity checks. This cooperative build boundary does not claim protection from
arbitrary concurrent filesystem replacement.

## Execution

Default invocation resolves the executable's own location and loads its adjacent
`.fkb` and `.sym`. The complete symbol record must start with exactly
`program-image-sym-lens-v1`, `compile-errors 0` and `unrunnable 0`, each
terminated by LF. Missing or repeated headers, NUL, an incomplete final line and
read or close errors refuse before execution. Symbol and dependency meanings
remain with the native image compiler.

Explicit runtime arguments retain the source runner's behavior.
`--compile-source PATH` prepares a recipe without evaluating its root.
Default launch checks image compatibility and the clean symbol record; full
artifact hashes are checked at build and installation. A different compatible
recipe can run beside a binary, but it needs its own attestation before it
qualifies as a published CLI artifact.

The [artifact observation](evidence/fkwu/native-cli-artifact.json) covers relocated
execution, companion refusal and compatible recipe substitution. The
[installed care observation](evidence/fkwu/native-startup-care.json) exercises
resident care through a compiled default entry. Those observations identify
their own source generations.

This remains whole-program compilation. The north star is Form-native admission
on demand, retained ownership and live selection of verified replacements.
