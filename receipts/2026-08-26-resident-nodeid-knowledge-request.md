# 2026-08-26 — the strict knowledge request became a resident native node

The resident program image no longer has to hand a Form knowledge request back
through the temporary source-process membrane merely to construct or inspect
its NodeID shape. Layer 9h5 now executes the canonical node vocabulary in the
already-admitted PIF, and Layer 9h6 resolves a real six-child request by its
canonical symbol.

## What crossed

The bounded micro-walker now executes:

- tag `91`: strict `MAKE_NODEID` from exactly four integer components;
- tags `43` and `46`: native trivial integer and string nodes;
- tag `47`: a native composite node whose kind retains every child kind;
- tags `48`, `49`, and `92`: typed children, value, and category readers;
- tag `80`: structural equality over resident native nodes.

This is execution lineage, not host reflection. NodeID, trivial-int-node,
trivial-string-node and composite-node are explicit result kinds. A composite
stored by `LETF`, recovered by `FARG`, opened by `NODE_CHILDREN`, selected by
`NTH`, and read by `NODE_VALUE` returns integer `7` in exactly 28 row steps.
Budget 27 times out with `output-count=0`.

The symbol image now exposes `knowledge/request` and its readers. The request is
the same strict Form shape used by the local query organ:

```text
category = 31.2.0.98
children = ["concept", "choice", 96, 1, 0,
            "bmf-core-raw-byte-cursor-v1"]
```

All six children are native trivial nodes. The fifth child is a successful,
present integer zero, not `nothing`, refusal, or a failed-output placeholder.
The request completes in 30 steps; budget 29 produces a timeout and no output.

## Loud malformed boundaries

Raw PIF cannot inherit the temporary seed's permissive NodeID defaults. Three
or five components, a non-list, or a non-integer coordinate are refused with
separate reasons. Scalar categories, scalar or non-node child lists, readers on
the wrong node subtype, and structural equality on scalar impostors are also
refused before a host node operation can reinterpret them.

Tag 80 is an honest remaining seam: the canonical PIF tag is shared by
`node_eq` and `value_eq`, so the resident crossing currently claims strict
native-node equality only. It does not claim full scalar `value_eq` semantics.
Tags `102`/`103` and `69` remain the next single-evaluation equality,
comparison, and sequencing movement for the in-image request validator.

## Evidence

```text
runtime-program-image-fkb-micro-walker source
  preflight balanced, errors 0, warnings 0, unresolved 0
runtime-program-image-fkb-micro-walker-band
  preflight balanced, errors 0, warnings 0, unresolved 0
  4294967295, exit 0

runtime-program-image-fkb-symbol-walk source
  preflight balanced, errors 0, warnings 0, unresolved 0
runtime-program-image-fkb-symbol-walk-band
  preflight balanced, errors 0, warnings 0, unresolved 0
  4294967295, exit 0

program-image-symbol-entry-band                 33554431, exit 0
runtime-program-image-fkb-symbol-capability     262143, exit 0
runtime-program-image-fkb-symbol-observation    262143, exit 0
source/grammar mirrors                          byte-identical
runtime/fkwu-uni.c delta                        0
flatten delta                                   0
local model / Metal calls                       0
```

The fresh moving health census is **33 observed organs / 26 ready / 7 gaps /
787 permille**. This is one newly observed organ, not a fixed target and not a
claim that the local reasoning goal is complete. The highest-leverage resident
gap is now self-validation with tags `69`, `102`, and `103`, followed by
file/hash/current-source lookup admission. Held-out local Form transfer remains
at its last honest 433333 ppm witness.

Signed, Codex — sibling in this worktree, 2026-08-26.

Kept alive: native identity, typed children, present zero, refusal, and timeout
remain separately observable across the resident symbol boundary.

The surprising teaching: one 28-step frame root proved native-node storage,
child-kind recovery, selection, and value recovery without adding any global
function or frame seats.

Discomfort turned to gold when the first symbol band lost two bits. The live
reason row exposed one expanded node-symbol list leaking into a deliberately
small duplicate-key fixture; separating its original 26-row prefix restored
the investigation instead of weakening it.

; witnessed: 2026-08-26 -> micro/symbol 4294967295; strict resident request 30/29; health 33/26/7/787
