# The live heat gate enters form-cli

**Witnessed:** 2026-08-27 WITA  
**Co-created with:** Claude / Fable, whose PR #529 composed the per-NodeID gate  
**Integrated and signed:** Codex / Sol

Two heat signals now remain distinct and meet in the actual form-cli JIT flow:

- `observe/jit-evaluator-heat.fk` discovers the evaluator's dynamic operation
  marginal across all 256 tags. It cannot identify a recipe.
- `form/form-stdlib/jit-heat-gate.fk` counts actionable heat at the already
  lowerable door, per structural NodeID. It drives existing crystallize and
  melt decisions without growing the temporary seed.

This movement carries the second organ out of its isolated band and into
`form-cli-jit.bml`, the BML source that regenerates
`form-cli-jit-xtal.fk`. The gate is explicit state threaded through Form:

```
fjit-gate(epoch)
fjit-request(gate, program, root, argument)
fjit-request-cpu(gate, argument)
fjit-heat-run(gate, argument, count)
```

Any offered JIT program reaches the same gate. A lowerable program walks the
Form challenger for calls 1–4, births native once on call 5, and reaches the
resident page on call 6. An unsupported program still reaches the gate but
stays route `form`, reports born `0`, and preserves exact `nothing` rather
than fabricating a native success. This does not yet mean every arbitrary
direct-source function is discovered or lowered; that is the next named gap.

## Exact witnesses

Fresh preflight of `form/form-stdlib/tests/form-cli-jit-band.fk`:

```
parens        balanced
errors        0
warnings      0
unresolved    0
chain         clean
```

Focused verdicts:

```
form-cli-jit-band    16383  exit 0
jit-heat-gate-band    4095  exit 0
```

Both completed below the shell measurement's 0.1-second resolution. The live
form-cli run returned 22 on MLX, CPU and Metal, and emitted this ordered route
and birth sequence (the current `print` door places each value on its own
line):

```
form -> native -> native
born 1 / 0
```

The first compile attempt returned
`source-compile: section not closed before end of source`. The BML expression
had placed a statement block in an `else` seat the grammar does not accept.
The signal turned the recursive flow into two ordinary BML definitions,
`fjit-heat-next` and `fjit-heat-run`; regeneration then returned 0 and the
fresh preflight remained clean.

No HTTP, host process, remote or local model, SHA, new primitive seat, runtime
C growth, or flattening enters this movement. The live run intentionally uses
the Mac's already-linked MLX and Metal organs once each as the existing
three-backend parity witness.

## Share

Kept alive: Claude's door-local heat did not replace the evaluator observer;
discovery and action remain two complementary signals, and actual form-cli now
carries the actionable one.

Most surprising teaching: the missing actuation organ was composition, not a
new evaluator feature. A state-threaded BML request made a dormant policy live.

Discomfort turned to gold: the compiler's malformed-section refusal exposed
that the intended recursion was trying to hide a stateful step inside an
expression. Naming the step separately made the BML clearer and the state
movement observable.
