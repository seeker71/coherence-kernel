# Native CLI assembly

The native CLI assembly uses the current native source runtime and a Form REPL image.
`native-cli-startup.bml` emits the startup from that runtime's exact source,
binds the generation digest, and preserves explicit source/image invocation.
Default invocation loads `<executable>.fkb` and `<executable>.sym` beside the
executable. The builder checks those three files as one platform artifact. Native
`--compile-source` prepares the recipe without evaluating its root expression.
Default startup reads the complete symbol record and requires the exact first
three lines: `program-image-sym-lens-v1`, `compile-errors 0`, and `unrunnable 0`.
Each line ends with LF. Missing or malformed fields, repeated diagnostic
headers, NUL bytes, an incomplete final line, and read or close errors refuse
before the image executes. Symbol and dependency row meaning remains with the
native image compiler. Explicit source/image invocation keeps the source
runtime's admission behavior.
Build and installation attestations verify the exact executable, image and
symbol bytes. Default launch checks the complete clean symbol record and the
runtime's image compatibility; it does not read the platform attestation or
hash the companions. A different compatible image and clean symbol pair can
therefore execute beside the same binary. The embedded source digest and genesis
identify the binary's build generation; they do not bind the bytes of replacement
companions. Such replacement requires a new attested build before it qualifies
as the published CLI artifact.
The compiler's BML lowering remains in RAM; no lowered source twin is published.
The [artifact evidence](evidence/fkwu/native-cli-artifact.json) records the
14 artifact cases this contract answers: relocated default execution and
restored companions pass; missing or malformed companions refuse; a different
compatible recipe runs with the same binary and refuses the original
installation attestation. The
[installed resident care execution](evidence/fkwu/native-startup-care.json)
shows the resident care organ running from such an installed set. The published
generation holds 396 source files (the manifest and the 395 it names) under
source stamp `da5ebd6434fecdbb`; its Darwin ARM64 executable is 8,190,296 bytes,
the `.fkb` 42,307,765 and the `.sym` 3,553,616.

The identity roots include the program, its compiler, the source runtime,
opcode and generated node-word headers, the Form node-word authority and its
verification door, and the host build carriers (`build-form-cli.sh`,
`regen_form_cli_bootstrap.sh`, `form_cli_source_list.sh`,
`form_cli_bootstrap_proof.sh`). The platform name and the source stamp are
computed in `form_cli_source_list.sh`; kernel validation's fourth-arm carrier
and its flattener table are not part of this build. Both generated and ordinary
copy installation run the repository runtime's node-word verification first;
that door must remain reachable. The sealed copy includes the checked header.
`native-table-sources.bml` follows
the actual source declarations and holds the complete bytes of every row.
`form-cli-source-closure.bml` computes the portable dependency manifest and its
digest from those held bytes. Each digest row has the existing byte framing:
`path:<relative>\nbytes:<count>\n<content>\n`; the generated manifest is first.
Non-Form inputs participate in identity without being parsed as programs.
The name registry `home-index.txt` is an identity root. Form carries every
declared source home and its dependencies, including homes not yet called;
the runtime resolves names from that exact copied registry. Comment and blank
lines carry no mapping. Malformed rows, NUL bytes and non-source homes refuse
admission. Absolute home paths refuse snapshot publication. The
[registry observation](evidence/fkwu/native-home-registry.json) executes a
function by name from a copied snapshot and retains the actual refusal cases.

The same held rows are written and read back in a private `body` snapshot with
their repository-relative paths. Compilation uses this snapshot, including the
BML compiler door and its dependencies. Absolute source dependency declarations
refuse snapshot admission. A local read-lookup boundary prevents fallback into
an enclosing checkout. The snapshot contains authored bytes, never lowered twins.
The held, file-marked genesis archive is emitted as exact C byte data by Form.

`FCSC1` supplies the Form base, a new private output directory, an optional
carrier owner and identity roots as bounded lines. Empty carrier input uses the
original native sources. The explicit substitution option remains available to
the table compiler's separate proof path. Complete checked files carry the seal;
stdout is not completion evidence. `FCSI1` installs the held manifest through a
checked same-directory temporary file and rename. `FCSV1` checks the seal hash,
all original and snapshot rows, the complete manifest, digest and genesis.

`FCSE1` emits startup C from the sealed snapshot runtime and held genesis. Its
output record binds the exact generated bytes. `FCEP1` copies those checked bytes
into the candidate; `FCEV1` checks the candidate against that same record after
the build. The publisher holds the common lock. Failed reads, writes, truncated
metadata or changed sources refuse acceptance. Prior complete output stays in
place until the corresponding checked publication step succeeds. This is a
cooperative local build boundary, not protection against arbitrary concurrent
filesystem replacement.
If the initial lock owner-file write fails after creating a partial file, the
publisher removes that owned file and directory before returning refusal;
failure to release either is reported.

The native bootstrap attestation binds source digest/stamp, startup C, author
binary and runtime source. A platform attestation additionally binds the exact
bootstrap attestation, platform, executable, image and symbols. Readers reject
missing, duplicate, unknown, truncated and mismatched fields. The public stamp
moves last, after the candidate's source/image checks and identity challenge.

One door regenerates everything: `form/scripts/regen_form_cli_bootstrap.sh
[output-dir]` seals the closure, emits the startup, builds one candidate and
publishes both the bootstrap set and that same witnessed candidate as
`form-cli-<platform>` with its `.fkb`, `.sym`, platform attestation and stamp;
nothing is rebuilt for publication. The output directory defaults to
`form/form-stdlib/bootstrap`; another directory receives the whole set for
inspection, while the dependency manifest always installs beside its sources.

`form/build-form-cli.sh` with no argument validates this host's published
bundle without running a source compiler and makes `form/form-cli` a relative
link to it. The executable resolves its own path, so the `.fkb` and `.sym`
beside the bundle are the ones it loads, and the identity challenge runs through
the link before the build reports success. Given a path, the builder copies the
validated trio there; with `FORM_CLI_FORCE_LINK=1` it prepares a new image with
the exact startup binary that will load it. The dependency manifest is derived
evidence; source and compiler changes invalidate the generation. Historical
table/C artifacts in the bootstrap directory and the `NTC2` compiler route
remain independent proof surfaces, outside the CLI build and runtime.

On this host the whole regeneration (closure, emission, one candidate, both
publications) took 1,186 s wall, the default link install 4.5 s and the
behavior proof through the link 2.0 s. These are single observations on a
shared, loaded machine; they establish neither peak memory nor general
throughput.
A regeneration stops without publishing when any held source changes while it runs, and a
published generation reads stale as soon as any of its sources changes; with
several authors editing the closure, regenerate after their work lands.
This is a whole-program bootstrap path. The runtime north star remains native
module admission on demand, retained context ownership and live selection of
verified replacements. See [native care](native-core-care.md) for the running
interface's source and observation contracts.
