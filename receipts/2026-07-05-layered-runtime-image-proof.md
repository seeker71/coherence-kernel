# 2026-07-05 -- Layered runtime image proof

## Question

Why were we building a giant fourth-arm/source-driver block instead of building
functions and recipes layer-by-layer from the c-bootstrapped kernel toward native
runtime parts?

## Work Done

Added a concrete Form stdlib layer:

- `form/form-stdlib/layered-runtime-image.fk`
- `form/form-stdlib/tests/layered-runtime-image-band.fk`

The layer path is:

```text
recipe object
  -> write_form_binary .fkb
  -> write .sym sidecar
  -> check source/.fkb/.sym freshness
  -> read_form_binary
  -> explicit walk_recipe_here at the consuming load site
  -> live function binding
```

The proof builds two runtime function layers directly as Recipe objects with
`intern_node`:

- layer 0 defines `lri-base(x) = x + 2`
- layer 1 defines `lri-top(y) = lri-base(y) * 2`

Then it persists both layers as `.fkb + .sym`, validates freshness and sidecar
shape, loads both layers, and calls `lri-top(20)`. The expected live result is
`44`; the full band score is `127`.

This is not a source text compiler path, not module concatenation, not a `.tbl`
driver, not a C seed expansion, and not the retired Go-only compile/run surface.

## Abstraction Lesson

The first version tried to hide loading behind:

```text
(lri-load-layer path) -> (walk_recipe_here (read_form_binary path))
```

That failed. `walk_recipe_here` is env-aware: it binds into its caller frame.
When wrapped in an ordinary Form helper, the layer binds into the helper's frame,
not the consumer's frame, so the loaded `lri-top` function was not visible.

The corrected abstraction is:

```text
(walk_recipe_here (lri-read-layer path))
```

Until Form has a real macro/env-aware function abstraction, the scope-changing
splice must remain visible at the load site. Hiding it would be a false
abstraction.

## Validation

```text
cd form && ./validate.sh form-stdlib/tests/layered-runtime-image-band.fk

  ✓  core.fk+layered-runtime-image.fk+layered-runtime-image-band.fk  -> 127

  1 ok, 0 divergent -- kernels agree on every sample.
```

## Boundary

The C checkout witness was not grown. The proof runs on the Go/Rust/TypeScript
proof siblings that already carry `write_form_binary`, `read_form_binary`, and
`walk_recipe_here`. Moving those capabilities into the final native body should
happen by shrinking the C seed and lifting the native walker/runtime image path,
not by adding more permanent C.
