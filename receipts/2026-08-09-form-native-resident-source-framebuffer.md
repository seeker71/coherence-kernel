# Form-native resident source framebuffer

Date: 2026-08-09

## Claim boundary

This receipt observes a small resident Form source door.  Once its body is
loaded, an exact consented source offer is evaluated by the Form cursor walker,
each top-level expression becomes an attributed derivation cell, and enter/leave
progress streams through the same framebuffer while the child is active.

It does not make the checkout bootstrap parse visible.  `fkwu` must still parse
enough source before the first target Form cell can run.  It is not a general
compiler, artifact loader, native voice, model-learning result, or CLI handoff.
No C seed grew.

## Boundary and carrier

`SRFB-OFFER` contains the exact source cell, one four-arm acknowledgement,
provenance, and an optional lens.  Only `ONE` opens evaluation.  `NOTHING-now`,
`ZERO`, and a `NODE` alternative return unchanged and create no expression
step.  A sender can condense its own optional timeout contract before offering,
so pending and expired-to-default histories remain outside this four-arm door
without being collapsed.

`SRFB-STEP` contains the exact offer, expression ordinal, start/end offsets,
and observed result.  It is source-attributed with `intern_node_at`, so the
derivation cell itself is retained in the framebuffer; it is not merely a log
line.  `SRFB-RESULT` contains the exact offer, ordered steps, and final result.
Repeated equal offers have the same content identity.

The text projection exposes only stage, expression ordinal, offsets, total
source size, modifier, and source attribution.  Source text, bindings, lenses,
provenance payloads, and evaluated values remain unspoken.

## Native observation

The successful offered program had three top-level expressions.  The resident
door observed:

```text
accepted expressions                 3
ordered derivation cells             3
projected call-phase cells           9
total retained framebuffer cells    12
final bounded result                42
declined expression calls            0
declined framebuffer cells           2  (window, not-accepted)
```

The final freshness-stamped run completed in 0.27 seconds wall time.  Its first
resident frame was at elapsed 0 ms; all three expression boundaries were
visible immediately.  The direct Form verdict was:

```text
1023                               (10/10 claims)
```

There was no five-minute wait.  The five-minute source-size/direct-stack check
was not reached.

## Failed path removed

The first attempt wrapped the old section compiler and reached scan, lower, and
persist frames, then stopped exactly at:

```text
fkwu: form_error: source-compile: resident compiler failed to persist section image
@form fkwu 1 1528 11928 13456
```

The compiler unit exposed 66 unresolved sibling-kernel dependencies, including
generated BMF context definitions, `walk_recipe_here`, and
`write_form_binary`.  Adding more imports would not make those primitives
native.  The attempted `source-compiler-framebuffer` files were removed rather
than retained as a false resident compiler claim.

The replacement uses `form-eval-full.fk`, whose cursor walker already executes
in current `fkwu`, carries no file writer, and requires only the current Form
core.  It evaluates already offered source in memory and does not normalize a
whole-file read as its input boundary.

## Regression surface

All executions used `./fkwu <file.fk>` through the observation wrapper.  No
`--src`, Python answer path, remote model, shell answer path, or C edit was used.

```text
source-runner-framebuffer-band.fk       1023
form-eval-full-band.fk                   635
offer-ack-core-band.fk                  1023
live-call-framebuffer-band.fk           2047
living-world-prospective-suite-band.fk  1073741823
```

## Exact artifacts

```text
form/form-stdlib/source-runner-framebuffer.fk
  sha256 95f759f29b2789f91a159d2f37551bc8dae94afd17f8a8372ba19e684e11eba3
form/form-stdlib/tests/source-runner-framebuffer-band.fk
  sha256 bde3af53bfe264f26b03c9ab83ae989ec48da277c893d8a5b58abfb400735196
form/form-stdlib/form-eval-full.fk
  sha256 9f558d5e23dc304ff0e94abb737624b1e3f2fc57f7db599c1b627724d2d24af9
form/form-stdlib/offer-ack-core.fk
  sha256 9808f79778da9ef3e6c2497b90fb55e5c6bb4f40193191dbf1ff567d9a57c15a
observe/live-call-framebuffer.fk
  sha256 ac1b5b68b54d8765ca834775cc4f318655c06e17455c1537424e1140411c3ce3
```

Exact receipt path:

```text
receipts/2026-08-09-form-native-resident-source-framebuffer.md
```

## Plain-language placement

- Execution: success for already resident, consented Form source.
- Evidence: new per-expression call and derivation evidence arrived.
- Comparison: no model-quality comparison occurred.
- Quality: no model or native-language improvement is claimed.
- Authority: only an explicit `ONE` acknowledgement opened evaluation; no
  promotion or broader authority moved.
- Remaining gap: bootstrap source preparation is still invisible until the
  resident door opens, and the checkout CLI does not yet route new source
  through this door.
