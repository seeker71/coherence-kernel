# Adaptive artifact traversal

Form owns FORMBIN2 traversal, continuations and attention in
`form/form-stdlib/bml/formbin-depth.bml`, and the wire codec itself in
`form/form-stdlib/bml/formbin-codec.bml`: numeric payloads stay eight
little-endian bytes (every int64, signed zero, infinities and NaN payloads), and
composite continuations and decoded ownership live in Form data, independent of
host stack depth. The body runs on fkwu throughout.

Depth is an observation. A linked continuation holds open composites, including
their remaining children. Both descent and completion can yield between slices
(`fbd-slice`). A deeper valid artifact advances the attention watermark; it is
never rejected because it crossed a depth number. Byte, node and child-count
format checks still reject malformed or oversized input.

The initial policy (`fbd-initial-attention`) offers 256 operations, a depth
watermark of 32 and a 50 ms attention target. These are starting observations,
not depth limits or a total-work budget. `fbd-attend` reads a slice's peak depth,
elapsed time and collections: a slow slice halves the next quantum and yields; a
slice below one quarter of the target doubles it; otherwise it continues. Actual
GC activity requests attention, but only measured delay requests smaller slices.
The continuation survives each choice. There is no aggregate duration timeout.

The seed's own FORMBIN reader (`runtime/fkwu-uni.c`) still refuses an artifact
nested deeper than 256 with `form binary: maximum node depth exceeded`; the Form
codec has no such ceiling. A repair of that refusal follows the real artifact:
read the Form codec's depth and node count for it (`fbc-decode`), preserve the
continuation, then witness the chosen path.

The traversal cell is `form/form-stdlib/bml/formbin-depth.bml`;
`gate/tests/canonical-conformance-band.fk` (1023) reads the pinned kernel artifact
through the Form codec.

Where it is going: every reader of FORMBIN2 meets the same Form traversal, and the
seed's depth refusal lowers into it.
