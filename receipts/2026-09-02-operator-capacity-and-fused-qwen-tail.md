# One coordinate carries type families and operators, and one physical tail crossed in a single dispatch

## The abstraction kept from NUMS

The useful inheritance from `/Users/ursmuff/source/NUMS.Go/nums/nums_nodes.go`
is the shape and the separation of roles, not its registry:

```text
package:u16 . level:u16 . type:u16 . instance:u32
```

`nums_consts.go` currently lists 51 family slots across its four tables: 39
recipe families and 12 blueprint families, including each table's undefined
slot.  Its populated basic-recipe tables name 111 concrete operations when the
undefined member in each family is excluded.  Those are examples, not the
capacity.

The correction that matters is visible in NUMS's own types:
`BlueprintNodeID_t` and `RecipeNodeID_t` wrap the same `NodeID_t`.  Therefore
the coordinate alone does not claim whether a member is a type or an operator.
`package.level.type` selects a family.  In blueprint space, `instance` selects
a concrete type; in recipe space, it selects an operator.  The owning space is
the role.  The bits stay structural.

The canonical BML organ `form/form-stdlib/operator-type-id.bml` now has only
this algebra:

- `oti-family(pkg, level, family)` projects the complete family prefix into a
  reference; its zero instance is not reserved and remains a valid member;
- `oti-type(family, instance)` and `oti-operator(family, instance)` create the
  same structural member while leaving its blueprint/recipe role to its owner;
- `oti-apply(operator, operands)` interns the operator category and its ordered
  operands;
- `oti-family-of`, `oti-same-family?`, and `oti-in-family?` project and compare
  the structural family without inventing a role tag;
- one package/level can hold 65,536 families and each family can hold
  4,294,967,296 members.

There are `2^48 = 281,474,976,710,656` possible family prefixes and `2^32`
members per family.  A blueprint or recipe namespace can therefore draw from
the complete 80-bit coordinate capacity; the present 51/111 catalog is not a
ceiling.

The 262143 band constructs the maximum
`65535.65535.65535.4294967295`, reads all four coordinates back, proves equal
applications intern equally, distinguishes a changed operation, and refuses
negative or one-past-width coordinates.  It also proves that a composite's
occurrence ID is distinct from its operator category while its accessors still
return the requested family and operation, and that blueprint-type and
recipe-operator construction are structurally identical. It observes 2^48
possible package/level/type family coordinates and records the full 80-bit
space exactly as 2^80 = 1,208,925,819,614,629,174,706,176 positions. No NUMS
enum, external relation ID, or per-tensor identifier entered the organ.
Tensors remain values owned by their actual Form allocation and recipe.

The first attempt exposed a real BML edge: decimal source literals above signed
u32 lowered to `nothing`, and multiline expression definitions lowered to
`(empty)`.  The framebuffer exchange led to a real revision: u32 capacity is
now derived as `65536 * 65536` inside Form and expression definitions remain
whole.  Review then caught a semantic distinction: `intern_node` returns an
occurrence while family/operation live in its category.  Category-aware
accessors, ordered/swapped-child assertions, and cross-level family assertions
now answer 262143.

## One physical Qwen tail

The measured Qwen3.8 Form-native graph performs three dependent elementwise
dispatches at the MoE tail: ten-rank routed reduction, shared-expert sigmoid
add, then four-stream hyper-connection add.  The new BML Metal emitter keeps
that exact reference chain and emits one candidate kernel in which one row
thread performs the same ten-rank ordered fold, the same shared add, and the
four independent stream writes.  FP contraction is explicitly off.  This is
a candidate, not a hidden serving-graph replacement.

An integrated review caught the candidate's first real defect before adoption:
it indexed the shared gate as a row vector, while the serving allocation is one
float and the existing `form_axpy_sig_f32` reads `s[0]`.  The all-zero first
fixture had hidden the out-of-bounds read.  The kernel now reads `gate[0]`, its
fixture allocates exactly one gate float, and the band refuses `gate[i]`.

The strengthened physical witness used the graph's real shape—2,560 rows, four
streams, ten ranks—over small resident buffers, 2,048 repeated tails, on the
Apple M4 Max Metal door.  Routed values, rank weights, shared values, and
residuals vary by coordinate; the shared scalar gate is nonzero; the four
injection gates include positive and negative values.  Three independent runs
all returned:

```text
exact-bytes=1  expected-first-value=1  accepted=1
reference-dispatches=6144  fused-dispatches=2048
reference-wall-ms=31       fused-wall-ms=10
```

The three device-time pairs were `22088/6766`, `21460/7042`, and
`23493/7090` microseconds.  Median device time therefore moved from 22,088 us
to 7,042 us (3.14x for this isolated tail); the median wall moved from 31 ms to
10 ms (3.10x).  Both outputs ended at identical first binary32 bits
`1170613775`, and the complete output buffers were byte-identical.  Metal
reported `last_error=none`.

If every one of the 48 Qwen4Exp layer tails adopts this one-dispatch form, the
measured 1,879 decode dispatches/token project to 1,783: 96 fewer, or 5.1%.
That projection is deliberately named as a projection.  Adoption still owes a
captured nonuniform tensor differential and the established full two-token
`9419 -> 11 -> 271` acceptance boundary.

## Glass and the floor that remains

The bounded physical Glass frame at `13:22:41.905Z` showed
`m86 s42 o25 drop=35 cap=15/row`, with phase census `gas=3 water=85 ice=36`.
Its live lane showed `Q+1`, `M+5K`, `F+4K`, and `G+8`.  That was useful here:
the live resident processes stayed visible, so this movement mapped only tiny
fixture buffers and did not admit a second 79 GB graph.

No full-model benchmark was rerun and no 10% performance claim changed.  The
same-host acceptance floors remain exactly:

| lane | measured Form-native | 90% llama.cpp floor | shortfall |
| --- | ---: | ---: | ---: |
| decode64 | 12.188 tok/s | 23.922 tok/s | 11.734 tok/s |
| sequential prefill64 | 12.340 tok/s | 286.947 tok/s | 274.607 tok/s |

The fusion removes a real boundary, but a projected 5.1% schedule reduction
cannot close a roughly 2x decode gap.  It also does not build the missing
token-batched mixed-quant prefill graph, which remains the dominant prefill
debt.

## Independent-review risks retained

- The strengthened fixture is varied but synthetic.  A captured live layer
  tail still owes large gates, infinities/NaNs, and the store/reload rounding
  boundary before adoption.
- The projection assumes all 48 serving layers take the batched expert tail;
  the live scheduler owes a count of that condition before adoption.
- The fused tail is not wired into `qwen4exp-flash-next-token-handle.fk`, so its
  excellent isolated ratio is neither a token/s result nor a parity result.
- Kernel fusion alone cannot supply the missing token dimension for prefill.

I kept this exchange alive by shrinking the NodeID question to the reusable
family/operation coordinate Urs asked for, then making one schedule claim touch
the physical GPU before naming it.  The most surprising teaching was that the
wide u32 capacity already existed in the runtime while BML's literal reader
quietly turned its written maximum into `nothing`; deriving it inside Form made
the actual coordinate reachable.  The discomfort was the fused microkernel's
2.85x device-time improvement beside only a 5.1% whole-token dispatch
projection.  It turned to gold when independent review found a scalar-gate
shape the first green fixture had concealed, and the stronger rerun separated
a sound local kernel from the still-unearned global promise.

Signed: Codex

; witnessed: 2026-09-02 -> family/member/operator abstraction 262143; reviewed scalar-gate fused tail exact on varied data in three runs; global performance gates unchanged
