# Native GPU source generation

`form/form-stdlib/bml/gpu-source-emitter.bml` owns PTX and GLSL recipe
generation. The canonical target sources are input data under
`form/native/cuda/template_*.ptx`, `form/native/vulkan/matvec.comp` and
`form/native/vulkan/ffn-fwd.comp`.
The generator constructs executable BML in RAM. Publication is an explicit
operation; it stages complete bytes, verifies them, renames the file and reads
the published bytes again. Failed admission leaves existing source unchanged.

The generated functions in `form/form-stdlib/bml/form-ptx.bml` and `bml/form-glsl.bml`
emit in RAM without reading the template files. The BML sources lower through
the native authoring door. PTX replaces the
first entry-name occurrence; GLSL replaces the first workgroup-size occurrence.
CRLF becomes LF. Quoting, tabs, lone CR and repeated markers retain their
observed meaning. Absent templates or markers, empty markers, invalid function
names, compiler special forms, native primitive names, duplicate names and
unsupported text control bytes are refused.

From the repository root, regenerate either source with two stdin lines:

```sh
form-run ./fkwu observe/gpu-source-emitter-run.bml <<'EOF'
ptx
form/form-stdlib/bml/form-ptx.bml
EOF

form-run ./fkwu observe/gpu-source-emitter-run.bml <<'EOF'
glsl
form/form-stdlib/bml/form-glsl.bml
EOF
```

Form callers can pass their own list of `(name, marker, template-path)` rows
to `gse-unit` for in-memory generation or `gse-publish` for publication.
There is no interpreter or separately generated script in this path.

The source witness is `observe/gpu-source-emitter-witness.bml`. It executes
generated escaped strings and first-marker replacement, exercises admission
refusals and unchanged-source publication, and runs the canonical PTX, PTX
block, GLSL matvec and GLSL feed-forward checks with exact verdicts **8191**,
**3**, **7** and **7**, checking child exits and cleanup. The complete witness
returns **7**. Generation,
control, publication and new observations use `organ-health.bml`.

The PTX templates currently declare PTX 8.3 and `sm_89`. These witnesses prove
source bytes on `fkwu`; they do not establish CUDA or Vulkan execution on this
Mac. GPU device execution and numeric equivalence require their device
witnesses. Dynamic target selection and RAM admission remain the direction
for these carriers. The current Metal path already has its separate Form
program registry and dynamic host adapter; Metal is not linked into the seed.
