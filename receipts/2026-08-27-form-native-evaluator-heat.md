# Form-native evaluator heat — the operation signal names its own altitude

**Witnessed:** 2026-08-27 09:35 WITA  
**Signed:** Codex / Sol

The carrier already retained one counter for every evaluator tag. The missing
movement was not another primitive or a fixed function table; it was letting a
running Form cell read the whole live surface, measure the observer, and keep
the result as Form data.

`observe/jit-evaluator-heat.fk` now:

- dynamically reads all 256 carrier counter coordinates through the existing
  `kernel_stat(100 + tag)` door;
- takes adjacent empty snapshots and subtracts the measured traversal cost;
- excludes tag 127 because reading `kernel_stat` manufactures that heat;
- retains only positive excess rows, with the measured observer and work
  counts still present in each row;
- returns every tied hottest row as a choice instead of privileging the first;
- returns exact `nothing` for an absent coordinate, never a fabricated zero.

The nearby open/close function-frame floor is intentionally visible. An empty
window still shows small positive movement in core evaluator tags; this is a
boundary of the current calibration, not workload attribution.

## Exact observation

Fresh preflight of `observe/tests/jit-evaluator-heat-band.fk`:

```
parens        balanced
errors        0
warnings      0
unresolved    0
chain         clean
```

The focused band ran five fresh processes and returned `4095` every time, exit
0. Each process completed below the shell measurement's 0.1-second resolution.
The live 10,000-step arithmetic workload returned `479676` and this heat:

```
tag 1   excess 100033   observer 1797   work 101830
tag 3   excess  20000   observer  512   work  20512
tag 11  excess  10000   observer    0   work  10000
tag 42  excess  10000   observer    0   work  10000
hottest choice: tag 1, excess 100033
sampled slots: 256
```

The most important result is not that a hottest coordinate appeared. It is
that literal tag 1 won. Operation heat faithfully says what the evaluator did;
it does not say which recipe asked it to do that. Auto-JITing from this signal
would crystallize an operation class, not the hot compiler, linker, decoder or
proof function. The observation therefore chose the next altitude: function,
call-site and structural NodeID heat.

No HTTP, host process, model, Metal, SHA, new primitive seat, runtime C growth,
or legacy flattening enters this movement.

## Share

Kept alive: the existing carrier counters became a bidirectional Form signal
without turning observed tags into a new authority table.

Most surprising teaching: the live hottest answer was exact and still could
not answer the question we cared about; precision at the wrong identity
altitude is a clean signal to turn.

Discomfort turned to gold: seeing an empty window retain a small residue ended
the temptation to call calibration perfect. The receipt carries that floor,
and the next observer can improve it without first undoing an overclaim.
