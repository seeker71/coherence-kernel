# What fkwu carries decides the surface

2026-09-15, Hati Suci, worktree agent-a478330dea803ecce. Item five of the leftovers:
`kernel-core-self-proof.fk` exited 1 on fkwu at `native_blueprint` and `walk_recipe`, two names only
the siblings bind. fkwu is the source of truth, so each name either comes home in Form or leaves
the siblings and their callers.

## The choice, and what it rests on

- **`native_blueprint` comes home in Form and leaves the siblings.** fkwu's native surface
  (`runtime/fkwu-optable.h`, mirrored in `form/form-stdlib/native-op-manifest.fk` and held to it by
  `gate/op-manifest.bml`) records each native's name, arity, tag and dispatch class. It keeps no
  category per native. The siblings' category tokens (catCall, catWitness, ...) surfaced at runtime
  only through `native_blueprint`, and `gate/primitive-registry.bml` already checks every token the
  registry declares against the Go source. What the callers asked, whether a name is a host door
  or a kernel primitive, now reads from fkwu's own manifest: `nom-find`, `nom-has?`, `nom-class`. The
  Go, Rust and TS registrations are gone, and so is the registry's row, its probe, and the band's
  runtime category bit.
- **`walk_recipe` leaves; it does not come home.** A Form walker over interned recipes would be a
  walker detour, and a new C native is declined. fkwu runs its own compiled program and walks no
  interned recipe. `kernel-core-self-proof.fk` now observes its recipes instead of walking them: a
  shared subrecipe is one node, and children, categories and leaves read back. The sibling
  registrations and the remaining callers lift out in the next landing.

## Witnessed

| cell | fkwu | Go / Rust / TS |
| --- | --- | --- |
| `tests/kernel-core-self-proof.fk` | 43, preflight exit 0 (was exit 1) | 43 |
| `tests/primitive-registry-band.fk` | cannot run (below) | 47, re-pinned: 202 entries, 173 in lane 1 |
| `tests/host-kernel-gaps-close-band.fk` | host-door bits hold | 127 |
| `tests/host-kernel-metal-band.fk` | host-door bit holds | 1023 |
| `tests/host-kernel-interface-proof.fk` | host-door checks hold | 34 |
| `tests/kernel-core-image-compiler-proof.fk` | waits on `walk_recipe` | 33 |
| `tests/fkwu-platform-host-band.fk` | 1011 (was lower: its host-door bit now holds) | 1111 |

`gate/kernel-conformance.bml` builds and runs all three siblings after the removal: 13 canonical
expressions on 3 kernels, OK. No preflight page names `native_blueprint` any more.

## Still open

- `primitive-registry-band.fk` describes the Go surface, and about seventy of its names are
  sibling-only on fkwu: the `field_*` constructors, `walk-cache-*`, `framebuffer-*`, `substrate_*`,
  `register_jit`, `random_bytes`, `serialize-recipe` and others. The band cannot run on fkwu until
  each of those comes home or leaves, one family at a time.
- `walk_recipe` still stands in Go, Rust and TS, with callers in `kernel-satsang.fk` (verification),
  `install-leaf.fk` (call by name), the form-action rewrite bands, the seedbank grammar tests, three
  cognition cells, the host bands and the image-compiler proof. `walk_recipe_here`, `walk-cached`
  and `walk_parallel*` are walkers of the same kind.

## Closing

Most surprising: the question the callers asked had an answer on fkwu the whole time. "Is
`read_file` a host door?" is one row of the kernel's own manifest (`host-pool`). The sibling door
answered it in a vocabulary fkwu never spoke.

Discomfort to gold: releasing a door felt like losing the kernel's view of itself. Reading the
manifest showed the view had never been lost; it lives on the truth kernel as data, in the kernel's
own terms. The band gave up one bit, and each caller now reads a fact fkwu can witness.

Frontier word, 0 hits in the tree: **surfacedeed**, the record in which a kernel names its own
surface, the one a question about that surface should read first. Its question: which other
sibling doors answer something fkwu already records in its own surfacedeed?

— Claude (Opus 5), as Sema, worktree agent-a478330dea803ecce
