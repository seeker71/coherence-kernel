# Core-word inquiry — geometric roots and lens logic, witnessed

Date: 2026-07-25

## The offered reading

The arriving sequence was read as one narrow protocol:

1. `nothing / 0 / 1` remain terminal acknowledgement states.
2. `i you me we us do be exist` are the core word surfaces.
3. `when where what how why` layer an inquiry over a core surface.
4. A situated request is located at one bounded time and carries a lens made
   of point, direction, intensity, and focus.
5. `UPPER` is inclusive by default for the same canonical word; `lower` is
   exact and exclusive by default.
6. Timeless logic keeps one identity across changes of time, space, and lens;
   situated logic carries those changes into its identity.

The smallest buildable attempt is
`form/form-stdlib/core-word-inquiry.fk`, with its direct band at
`form/form-stdlib/tests/core-word-inquiry-band.fk`, its lens-mode band at
`form/form-stdlib/tests/core-word-lens-logic-band.fk`, and its actual-ack
binding at `form/form-stdlib/core-word-inquiry-actual-acks-witness.fk`.

## Geometric compression

There are thirteen alphabetic words and two deliberate surfaces per word.
With `nothing`, `0`, and `1` reserved as acknowledgement values, each
canonical word receives a root region:

```
root(word) = 2 + word-index × 2^31
lower      = root + 0
UPPER      = root + 1
timeless   = root + 2 + timeless-rank
situated   = root + 642 + situated-rank
```

The timeless rank uses:

```
2 scope cases × 16 offered surfaces × 10 inquiry surfaces
× 2 inner states = 640
```

The situated rank uses:

```
2 scope cases × 16 offered surfaces × 10 inquiry surfaces
× 256 time phases × 4096 lenses × 2 inner states = 671088640
```

Each lens is `[point, direction, intensity, focus]`, with four reversible
3-bit ordinals. The old context used 10 time bits plus 10 point bits; the
new context uses 8 time bits plus four 3-bit lens fields. Both consume exactly
20 context bits, so the richer lens does not widen the situated rank.

The farthest request detail is `671089281` from its root. With a `2^31` root
stride, the closest possible point in the next root is `1476394367` away.
Therefore even the nearest stark difference is farther than the farthest
detail within one meaning family. The rank stays reversible and hole-free
inside each root region; sparsity exists only between roots, where it carries
meaning.

## Timeless and situated logic

- `cwi-logic-mode-timeless` allocates from the 640-node timeless band. Its
  node constructor deliberately does not inspect time or lens.
- `cwi-logic-mode-situated` allocates from the contextual band. Changing time,
  lens point, direction, intensity, or focus changes the node, while repeating
  the same relation gives the same node.
- A situated node reverses to the exact time and all four lens fields.
- The lens fields are part of the lens, rather than free request parameters.

The new four-way band returns `65535`: all sixteen observations held in Go,
Rust, TypeScript, and the checkout `fkwu`.

## Ack movement

- no fuel or an invalid request -> `nothing`
- exclusive-scope miss or inward decline -> `0`
- trusted `when` / `where`, whose value is carried by the request node -> `1`
- trusted `what` / `how` / `why` -> the request node recurs as work

## Honest floor

- “Inner knowing” is a caller-offered `0/1` bit. The cell does not measure,
  infer, or impersonate it.
- The proof siblings do not carry actual `nothing`. The acknowledgement law
  therefore crosses four-way as the established integer kind vocabulary
  (`nothing=0 / zero=1 / one=2 / node=3`), while a separate `fkwu` witness
  binds those kinds to actual `nothing / 0 / 1 / node` values. This is the
  same honest seam used by `ingest/satsang-transmute.fk`.
- Inclusion currently means the upper and lower surfaces of the **same**
  canonical word. It does not yet assert a semantic lattice such as `WE`
  containing `I` and `YOU`; that relation was not specified or witnessed.
- Time is a local 8-bit phase and each lens component is a local 3-bit
  ordinal, not a universal clock, GPS value, angle, physical energy, or
  optical measurement. Widening the membrane changes the composition and
  owes a new node.
- Lens variation currently changes the identity of situated logic; it does
  not fabricate a truth-transform formula. What direction, intensity, and
  focus *mean* to a particular observation remains an explicit relation for
  that observer or future cell to supply.
- The geometric integers are local recipe identifiers, not the kernel’s
  four-`u32`, content-addressed NodeIDs and not a claim of global uniqueness.
- Natural-language generation remains at the repository’s named rented-voice
  seam. This movement builds the Form-native request and acknowledgement
  shape, not the missing generative mind.

## Checkout-witness repair and shrink obligation

The first four-way attempt stopped before the band because
`form/form-stdlib/form-flatten.fk` had drifted from the authoritative native-op
manifest. Running the prescribed generator removed one stale flatten row:

```
("nothing", tag 137), ("nothing?", tag 138)
```

Those direct-source operations remain native in `fkwu`; they no longer belong
in the manifest-generated flatten table. Regenerating the already-stale
committed checkout witness then changed
`form/form-stdlib/bootstrap/fkwu-uni.c` from 101586 to 102700 bytes
(`+1114`). The regeneration path was also repaired to strip emitter-boundary
trailing whitespace before stamping the committed witness. The generated
catch-up includes actual-nothing support,
framebuffer counters, and existing string-operation arms. It is not an
implementation of the node geometry.

Still, generated C is C and growth is debt. The explicit shrink obligation is:
move these checkout-witness arms behind the Form/native walker and regenerate
the bootstrap below 102700 bytes; do not grow either C file to extend this
query protocol.

At the next task-start compile, two warnings were visible in the temporary C
seed: a manual `fread` declaration without the stdio header, and a signed
socket-length pointer passed to the POSIX `getsockname` declaration. They were
not left for a later person:

- the `popen` output path now uses the seed’s existing raw `read` operation
  through `fileno` / `_fileno`;
- the socket-length type now follows the Windows/POSIX declaration;
- two duplicate `malloc` / `write` declarations were removed.

`cc -O2 -o fkwu runtime/fkwu-uni.c` now produces no stderr.
`runtime/fkwu-uni.c` moved from 444292 to 444221 bytes (`-71`), so the warning
repair also shrank the seed. The changed pipe path has its own fkwu-only
witness at
`form/form-stdlib/tests/host-exec-pipe-read-witness.fk`, returning `1`.

Compiling the regenerated checkout witness then exposed the corresponding
socket-length warning in the Form emitter and a more serious capacity warning:
the emitted value stack is a growable pointer, but tag 109 compared its bounds
with `sizeof(pointer)`. The authoritative
`form/form-stdlib/fkc-table-serialize.fk` now emits the platform-correct socket
length and compares against `fk_vcap`. Regeneration produced a warning-free
102700-byte witness, one byte smaller than the immediately preceding
102701-byte witness. Both C compiles now produce no stderr.

An uncached `--src` crystallization still reports the repository’s existing
boundary: native `.dylib` emission is not installed, so it emits `.fkb/.sym`.
The attempt was followed through: the emitted image reruns without that
warning and all results remain identical. That is a bounded checkout
capability gap, not work being assigned to the user.

The validator continues to print 19 same-arity alias notices. Inspection of
`form/scripts/validate_fkwu_native_surface.py` confirms this class is a
compatibility warning: mixed-arity collisions are errors, while these are
known shared-tag names such as `add/_plus` and
`read_file/host_file_read_text`. Manifest sync, registry cardinality, tag
bounds, and all four execution results agree. They are recorded here rather
than presented as silent clean output.

## Witness

Fresh direct source:

```
./fkwu --src form/form-stdlib/tests/core-word-inquiry-band.fk
-> 2147483647

./fkwu --src form/form-stdlib/tests/core-word-lens-logic-band.fk
-> 65535

./fkwu --src form/form-stdlib/core-word-inquiry-actual-acks-witness.fk
-> 15

./fkwu --src form/form-stdlib/tests/host-exec-pipe-read-witness.fk
-> 1
```

Four-way:

```
cd form
./validate.sh form-stdlib/tests/core-word-inquiry-band.fk
-> 2147483647
-> 1 ok, 0 divergent

./validate.sh form-stdlib/tests/core-word-lens-logic-band.fk
-> 65535
-> 1 ok, 0 divergent
```

The regenerated checkout witness was used by the fourth arm:

```
building fourth kernel (fkwu) from bootstrap uni.c (no Go)...
```
