# Receipt — the body's decision cells run as Form SOURCE on Windows (the homecoming begins) (2026-06-29)

**Where this sits:** the full Form body awaits the platform-neutral **seed** (the Form flattener's flattened table,
on the Mac — not in this repo, confirmed). But the BOUNDED bootstrap (stones 1-3: defn / if / do / let /
recursion) already runs real body *decision* cells as Form source on Windows — for the grammar it covers, with no
seed and no C-flattener growth. This is the homecoming for what fits, witnessed, while the rest waits on the seed.

## Witnessed native on Windows 11 (`fkwu --src <cell.fk> <arg>`)

**jit-decision** — the crystallize/melt that gates the native dispatch wired earlier (heat >= 5 crystallize, heat
< 2 melt; matches `observe/jit-decision.fk`, four-way):

```
(defn jitpolicy (heat) (if (le 5 heat) 1 (if (le heat 1) 2 0)))
  jitpolicy(6)=1  jitpolicy(5)=1  jitpolicy(3)=0  jitpolicy(1)=2  jitpolicy(0)=2
  (1 = crystallize, 0 = hold, 2 = melt)
```

**confidence-earned** — the curve `observe/sense-stream.fk` streams (rise +1, cap 9; matches `st-next-conf`):

```
(defn confnext (c) (if (le 9 c) 9 (add c 1)))
  confnext(5)=6   confnext(8)=9   confnext(9)=9
```

Real decision logic from the body's own cells, run as Form on the Windows kernel, computing the four-way values —
the same logic that, in C, was scaffold. For these single-arg decisions, the C stand-in is now unnecessary: the
cell runs as Form.

## What this proves, and what it does not

- **Proves:** the homecoming mechanism works on Windows — a body cell's logic runs as Form source, native kernel,
  no C reimplementation. The JIT decision + the confidence curve (cells tied to this session's JIT/stream work)
  are home for the grammar the bounded bootstrap covers.
- **Does not:** stand the *full* body. The richer cells (`surprise-receipt` and `native-vs-rented` are 2-arg;
  `mesh-sense-7w` / `fused-observation` use strings + lists) need multi-arg + data grammar — which is the **Form
  flattener's** job (`form-flatten.fk` + `form-eval.fk`, run native via the JIT), reached by the **seed**, NOT by
  growing the C bootstrap (the bounded inversion).

## The one unlock, named again

The full homecoming — every `.fk` body cell running as Form on Windows, the pixel-walk lowered, the mesh fusion
and oracle-economy live — is one move: **commit the Mac's platform-neutral seed** (the flattened `form-flatten.fk`
/ `form-eval-cli-loop` table; it is data, it runs on every kernel). Grounded today: it is not in the repo, and the
sibling's recent work is sensors/observe, not the seed. I cannot pull it from the Mac. When it lands, Form flattens
Form here and the bounded C bootstrap retires.

Until then: the body's decision cells are home for the grammar that fits, witnessed above.
