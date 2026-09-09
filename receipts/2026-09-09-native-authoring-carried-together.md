# Native authoring, carried together

Urs corrected the frequency: guidance is shared work. We detect the detour,
find the native reference, make the replacement, and carry its remaining work.
There is no new approval step, blocking language policy, or landing rule here.

The native guide is executable BML at
`form/form-stdlib/bml/native-authoring-guide.bml`. Run
`form-run ./fkwu observe/native-authoring-guide-run.fk`, or `heal guide` in
form-cli. `heal guide|<command>` returns guidance without executing the command.
AGENTS now includes temporary analysis helpers, measurements, and migrations in
our native authoring practice. The next agent inherits the work without asking
Urs to restate it.

Six Python implementations, 1,481 lines, leave the tree:

| Retired implementation under form/scripts | Native home |
| --- | --- |
| validate_fkwu_native_surface.py | gate/native-surface.bml |
| sync_native_op_manifest.py | gate/op-manifest.bml |
| validate_form_ontology.py | gate/ontology.bml |
| validate_primitive_registry.py | gate/primitive-registry.bml |
| verify_category_contract.py | gate/category-contract.bml |
| gen_flt_ops_from_manifest.py | gate/flt-ops-gen.bml and observe/native-op-emitter-run.fk |

Validation startup calls `gate/validation-start-run.fk`; Form selects and judges
its five existing checks. Shell carries the process and maps the actual status
and final value into validation's existing result. The native writer emitted the
197-row flt-ops slice, checked it, and a second run observed it already current.
No C seed code changed. No Python or replacement helper-language computation was
used to author this movement; the migration helpers were BML on fkwu.

## Observed checks

Preflight reported balanced source, zero errors, zero warnings, and zero
unresolved calls for the changed native bands. Every result below also exited 0.

| Witness | Observed verdict |
| --- | ---: |
| native-authoring-guide-band | 2047 |
| form-cli-native-guide-band | 7 |
| native-op-emitter-band | 31 |
| form-cli-heal-load-band | 15 |
| native-surface-band | 1013 |
| op-manifest-band | 1013 |
| flt-ops-gen-band | 53 |
| ontology-band | 245 |
| category-contract-band | 245 |
| primitive-registry-band | 489 |
| validation-start | five checks, 31/31; final 1 |
| drift-gates | 8191/8191; refused 0 |

The six validator bands retain their native live-input and planted-error cases.
Their masks now omit the historical Python comparison bits: 2 and 8 in the
first five, and 2, 4, and 16 in primitive-registry. Those comparisons and their
implementations remain in git; no replacement success bits were invented.

The disposable filesystem witness at `observe/native-op-emitter-witness.bml`
observed four outcomes, each checked as 1: restore the original after a compile
error, restore after an exit-0 result whose value is not 1, retain a verified
publication, and preserve the original when the staged file is missing. Its
local evidence directory was `.hearth/native-op-witness-64219`.

The real `form/validate.sh form-stdlib/tests/str-find-one-meaning-band.fk`
invocation observed one matching workload, 8191 on Go/Rust/TypeScript and zero
divergences. That selected workload had no fourth-arm registration; fkwu's 8191
was observed separately by drift-gates. This was a selected validation run, not
the full repository suite.

## Current source reading and work we carry

Execution candidates are static source matches, not observed process launches.
Ignored scratch, external installations, and computed commands are not covered
by this inventory. They remain part of our authoring responsibility. Compiler
specimens remain data for Form to read; historical receipts remain evidence.

The remaining implementations include proof harnesses, codec checks, device
generators, and model oracles. Their native replacement and equivalent behavior
coverage are work we carry. In particular, the kernel-conformance reference
explicitly leaves FORMBIN2 codec comparison work open; an available LoRA or
hearing reference also needs task-specific equivalence checks before retirement.
This receipt does not claim that every Python execution path has disappeared.

The self-panel observed orphans 0 and 11/12 performance lanes unobserved because
no hearth resident stood. The turn meter read session-output-tokens 518094 at
rollout byte 37782194; this is a session counter, not this movement's token cost.
The share reader was still discovering completed-turn coordinates, so it declared
the share unmeasured and withheld the percentage. None of these readings proves
model speed, LoRA improvement, or complete token attribution.

The surprising teaching was that native checks already existed while their own
tests kept requiring Python copies. The discomfort in Urs's correction became a
usable native guide and six observed retirements. We kept the exchange alive by
changing the work and carrying what remains ourselves.

Signed: Codex, 2026-09-09.
