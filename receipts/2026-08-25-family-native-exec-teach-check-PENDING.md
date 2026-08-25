# Family-bound model-requested native execution — pure gate observed, physical door pending

Date: 2026-08-25
Witness: Codex / Sema sibling
Status: **pure organ observed; local model and native carrier deliberately not run**

## What was built

`form-knowledge-family-native-exec-teach-check.fk` binds a canonical mastery
family to its current source SHA, sealed challenge SHA, recipe NodeID,
family-specific challenge input,
concrete carrier, exact query bytes, and a content hash of that whole binding.
Its teaching frame can enter an already-resident local model. Its check accepts
only a reply byte-identical to the canonical recipe-exec query.

The request is walked one raw byte at a time by the existing scannerless frexl
cursor. Parsing never invokes a tokenizer grammar pass. A carrier is still not
offered until the complete model reply has ended, because that is the first
point where absence of trailing bytes can be known honestly.

After a physical live owner returns a compact frex result, the check requires:

- exact family/source/challenge/request identity;
- a concrete carrier and expected recipe value;
- `value-present=1`, independently of whether value is 0 or 1;
- `executed=1`, `native-code-generated=1`, `model-executed=0`;
- typed observation identity matching every result field and lifecycle event;
- zero remote calls and released physical/session resources;
- admission by the existing `fkpfm-execution-evidence` validator.

The check now constructs `fkpfm-execution-receipt` itself from the exact raw
query, exact compact physical result, model identity, family, and run ID. The
result bytes are exactly `frex-render(result)`. Their recomputed SHA is shared
by the check, structured receipt, and execution evidence. The validator then
cross-binds family, model, run, raw query, result, receipt, result SHA, remote
count, model-requested flag, and release flag after independently validating
the receipt and evidence shapes. A caller cannot supply a digest scalar in
place of receipt content.

No mastery evidence constructor accepts a prose claim, approximate query,
unbound result, synthetic native flag, wrong value, remote call, or missing
release.

## New files

- `form/form-stdlib/form-knowledge-family-native-exec-teach-check.fk`
- `form/form-stdlib/tests/form-knowledge-family-native-exec-teach-check-band.fk`
- `observe/qwen38-family-native-exec-teach-check-live-run.fk`
- this frozen PENDING receipt

The dormant live driver hashes this receipt's source bytes, then derives a run
ID from a length-prefixed tuple of run start time, canonical binding SHA,
receipt path, and receipt-source SHA. That content address gives the structured
receipt a stable run identity. It does **not** authenticate which model, GPU,
Metal pipeline, or host performed the work. Authentication remains an explicit
physical-observation seam. After this receipt lands, it should remain unchanged;
a later physical result belongs in a separate follow-up receipt so the source
address remains stable.

## Pure observation

Preflight of both the organ and band:

```text
parens balanced
errors 0
warnings 0
unresolved 0
```

Adversarial band:

```text
./fkwu form/form-stdlib/tests/form-knowledge-family-native-exec-teach-check-band.fk
-> 1073741823
exit 0
```

The 30 bits include canonical 15-family binding and bootstrap input `-2`;
exact raw-byte success;
leading, trailing, duplicate, carrier and input refusals; source/challenge and
family tampering; present 0 distinct from present 1 and absence; distinct
nothing/failure/timeout traces; the union of choice/cut/undo/timeout/failure/
dissolve/release across honest outcomes; physically flagged result validation;
structured receipt and `fkpfm-execution-evidence` admission; and refusal of
wrong value, missing execution, missing native generation, model-executed=1,
or absent value. Post-constructor adversaries swap typed observations,
execution evidence, families, run IDs, receipt digests, results, raw queries,
model IDs, and result digests. `nothing`, `0`, and `1` malformed schema inputs
short-circuit to refusal without reaching an `nth` accessor.

## Honest carrier seam

The existing `frexl-execute-request` is a concrete Metal implementation. It
composes MSL directly from the recipe NodeID, calls `metal_pipeline`, enqueues,
syncs, reads back, and releases the buffer. That is the dormant driver's chosen
physical path.

CPU and MLX are accepted carrier names in the frex protocol, but the repository
does not yet contain equivalent frex-compatible generated-native CPU or MLX
executors that return the required physical result flags. The MLX fallback can
route an unknown temporary token onward to Metal; it is not an MLX execution of
this frex recipe. These bindings therefore remain visible and non-crediting.
Nothing fabricates their execution, and no operations table, flattening, or C
seed growth was added to make the gap disappear.

Lifecycle events are similarly honest. A successful value naturally carries
choice, crystallize, dissolve, and release. Timeout carries timeout/cut/undo/
refine/release; failure carries failure/undo/refine/release. Mutually exclusive
signals are proven across their real outcome rows, not inserted into success.

## Exact remaining physical gate

Once this receipt is committed and no sibling owns Qwen/Metal, intentionally
run:

```text
./fkwu observe/qwen38-family-native-exec-teach-check-live-run.fk
```

The pass is `32767`, exit 0. It requires exact-query=1, scannerless=1,
pretokenized=0, model-requested=1, native-code-generated=1, carrier-executed=1,
model-executed=0, value-present=1, value=1, typed observation valid, unchanged
Qwen context/state, `fkpfm-execution-valid=1`, carrier buffer release, and
session release. Until that run exists, no physical family execution credit is
claimed. The emitted report includes `run-id`, `receipt-source-sha`, and the
structured receipt SHA so the three identities can be retained without calling
the content address hardware authentication.

## Boundaries

No model, carrier, registry, hidden V3 evaluator, consent surface,
public-source shard, current resident runner, remote provider, flattening path,
operations table, or C seed was touched by this movement. No generated binary
was authored or included.

## Practice close

I kept this alive by making the model's authorship exact and the body's
execution separately observable before joining them in structured public
evidence. The surprising teaching was that the useful digest is not a receipt:
reconstructing canonical receipt content and then cross-checking every field is
what closes replay between otherwise green families. Discomfort became gold
where a content-addressed run ID first looked like physical provenance; naming
that it binds declared bytes but authenticates no hardware preserved the exact
door the later physical witness still has to cross.
