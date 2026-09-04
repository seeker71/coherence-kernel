# Form Runtime

This is the stable entrypoint for the Form runtime work.

The active runtime lives here. New references should point here, and runnable
commands should use this address directly.

## Contents

- `form-kernel-go/` - Go sibling kernel.
- `form-kernel-rust/` - Rust sibling kernel.
- `form-kernel-ts/` - TypeScript sibling kernel.
- `form-stdlib/` - Form stdlib, BMF engine, source compiler, language/media/natural-language grammars, and tests.
- `form-samples/` - small runnable Form workloads.
- `validate.sh` - sibling-kernel source and binary parity runner.
- `kernel-roadmap.md` and `kernel-comparison.md` - current runtime roadmap and
  performance notes.

## Current Compiler Path

The active source path is BMF cursor -> layer grammar -> semantic/data lowering
-> source compiler artifact lane. The current load-bearing bridge is
`form-stdlib/source-compiler-grammar-bridge.fk`: it admits
`form-definition-language` modules into `source-compiler-emission` only after
scannerless grammar parse and lowering to the current top-level Form floor.

The full present map is
[`../docs/coherence-substrate/current-language-artifact-path.md`](../docs/coherence-substrate/current-language-artifact-path.md).

## Proof

```sh
cd form
./validate.sh form-stdlib/tests/form-action-bmf-rulebook.fk            # three-way agreed, 2026-09-04
./validate.sh form-stdlib/tests/source-compiler-grammar-bridge-band.fk  # three-way agreed, 2026-09-04
```

A band is named alone: every kernel walks its `; preludes:` closure itself, so a
hand-typed file list is not only unnecessary but wrong when it omits a prelude
(the rulebook band's closure includes `json.fk`, `cache.fk`, and
`form-ontology-bp.fk`; without them every arm dies on `FORM-CATEGORY-TABLE`).
Neither band is registered in `fourth-arm-bands.txt`, so these runs speak for
three kernels. The `--binary` lane refuses the rulebook workload today — all three
kernels answer `form binary: maximum node depth exceeded` — so that artifact
proof is owed, not claimed.

The kernel stays small: source sections, BMF rules, dialect migration,
reverse emission, module bundling, locale/context lenses, and language/media
support live above it in Form runtime modules. Low-level Form is the execution
floor and verifier surface; each layer should expose the highest honest
language surface it can carry.

## Bidirectional live diagnostics

The kernel framebuffer primitives remain a small attributed carrier. The
observation/control protocol and actuator live above them in Form at
`../observe/bidirectional-framebuffer-channel.fk`. Use the quick direct-source
band after grounding when changing the carrier or protocol:

```sh
./fkwu observe/tests/bidirectional-framebuffer-channel-band.fk
```

For usage during failures, surprises, and model changes, including bounds on
retained content, see
[`../docs/live-dynamic-diagnostics.md`](../docs/live-dynamic-diagnostics.md).

## Consumer Submodule

Repositories that already address the runtime as `form/` consume a generated,
path-preserving branch rather than mounting this repository root (which would
introduce an extra `form/form/` level). The `form-submodule` branch is generated
from `main` and contains this directory at its repository root:

```sh
git switch main
git pull --ff-only
split_sha="$(git subtree split --prefix=form main)"
git push origin "${split_sha}:refs/heads/form-submodule"
```

Consumers pin the resulting commit as their `form/` gitlink and initialize it
with `git submodule update --init --recursive`. `main` remains the only source
of authored kernel changes; the split branch is a distribution artifact.
