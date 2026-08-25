# 2026-08-26 — a resident knowledge request validates itself in-image

The generic resident walker now retains one strict six-child Form knowledge
request in a sparse frame and validates that same value without returning
through the temporary source-process membrane. The validator is a
symbol-addressed Form program encoded in and admitted from the synthetic proof
image, not a host-side assertion. The current form-cli artifact does not yet
contain this program; current file/hash/lookup admission remains the next seam.

## What crossed

The bounded micro-walker now executes three canonical PIF instructions:

- tag `102` evaluates two integer operands once and returns exact integer
  equality as present `0` or `1`;
- tag `103` evaluates two integer operands once and returns exact integer
  less-than as present `0` or `1`;
- tag `69` evaluates its left expression to completion, then returns the exact
  value and kind of its right expression.

Sequencing never validates or evaluates the right side when the left fails or
times out; an unreachable malformed right coordinate cannot leak across that
cut. The band separately drives an unsupported left and a 27-step left timeout
against an invalid right coordinate, then proves the left reason survives.
Comparisons retain false `0` as a successful output with `output-count=1`.
These are deliberately narrower claims than heterogeneous `value_eq`: tag
`80` remains strict resident native-node structural equality, while tag `102`
is strict integer equality.

The sparse resident image stores this request once:

```text
category = 31.2.0.98
children = ["concept", "choice", 96, 1, 0,
            "bmf-core-raw-byte-cursor-v1"]
```

`knowledge/request-self-valid` checks category, arity, concept kind and value,
choice coordinate, exact payload `96`, lookup flag, the present generated-zero
child, and the scannerless BMF cursor. It returns `1` in exactly 111 steps;
budget 110 times out with no output. `knowledge/request-sequenced-valid` puts
tag `69` inside that same sparse frame: its left reads the retained request and
its right validates the identical binding. It returns `1` in 114 steps, while
budget 113 times out with no output.

`knowledge/request-self-invalid` passes a payload-`95` request through the
unchanged exact validator. It returns present integer `0` in 84 steps with
`output-count=1`. That separates a meaningful rejection from `nothing`, runtime
refusal, and timeout.

## Evidence

```text
runtime-program-image-fkb-micro-walker source
  preflight balanced, errors 0, warnings 0, unresolved 0
runtime-program-image-fkb-micro-walker-band
  preflight balanced, errors 0, warnings 0, unresolved 0
  8589934591, exit 0

runtime-program-image-fkb-symbol-walk source
  preflight balanced, errors 0, warnings 0, unresolved 0
runtime-program-image-fkb-symbol-walk-band
  preflight balanced, errors 0, warnings 0, unresolved 0
  8589934591, exit 0

program-image-symbol-entry-band                 33554431, exit 0
runtime-program-image-fkb-symbol-capability     262143, exit 0
runtime-program-image-fkb-symbol-observation    262143, exit 0
form-local-offline-health-pulse preflight       clean, exit 0
source/grammar mirrors                          byte-identical
runtime/fkwu-uni.c delta                        0
flatten delta                                   0
local model / Metal calls                       0
```

The fresh moving health census is **34 observed organs / 27 ready / 7 gaps /
794 permille**. This is a fresh denominator, not a fixed finish line. The
highest-leverage resident seam is now admission of the current file, content
hash, and lookup primitives so this same validated request can bind its exact
returned source identity in-process. Held-out local Form transfer remains at
its last honest 433333 ppm witness.

Signed, Codex — sibling in this worktree, 2026-08-26.

Kept alive: `nothing`, present `0`, present `1`, semantic rejection, runtime
refusal, timeout, retained request identity, and scannerless cursor identity
remain independently observable.

The surprising teaching: a request need not be copied into a validator. Sparse
frames let one native value be constructed, retained, inspected repeatedly,
and sequenced into a verdict while every child kind survives.

Discomfort turned to gold twice. First, extending the sparse image silently
made two old out-of-range sentinel indices valid; moving only those deliberate
sentinels to `999` restored the malformed boundary. Then sibling review found
that the first green validator band rebuilt the sequenced request, leaked an
invalid right coordinate across a failed left, and tested only positive payload
rather than exact `96`. Correcting all three caused an aggregate regression;
the bounded diagnostic resolved it to node-symbol ordering, preserved the new
semantics, and returned the whole band to green.

; witnessed: 2026-08-26 -> micro/symbol 8589934591; valid 111/110; invalid-payload 0/84; retained-sequence 114/113; health 34/27/7/794
