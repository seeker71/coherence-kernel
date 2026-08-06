# 2026-07-05 — RAG works: a two-line runtime fix under a sprawling symptom

## The ask, and the path chosen

"Can we make RAG work, please." Path chosen: **fix the runtime** (the deepest, most
sovereign of the three). Done, verified, committed (`24a86e79`).

## The bug (root cause)

The insidious string-carrier corruption (receipt
`2026-07-05-insidious-string-carrier-blocks-rag.md`) was a **value-stack rooting gap**,
not a recipe fault. In the walker, tag 109 (`let` store) was:

```c
fk_vs[fp + (fk_walk(node[i][1], fp) >> 1)] = fk_walk(node[i][2], fp);
```

It writes the let-local to `fk_vs[fp+slot]` but **never raises `fk_vsp` over the slot**.
The correct scope mechanism (tag 111) reserves slots by raising `fk_vsp`, evaluates the
body, then restores — but a **do-let chain outside a tag-111 frame reservation** (e.g. a
top-level `(do (let a …) (let b …) …)`) has no such wrapper. So when `b`'s value
expression evaluates, its temporaries push at `fk_vsp` — which is at or below the slot
holding `a` — and **overwrite `a`**. String-bearing list values `(list "s" n)` push
enough temporaries to reach the slot; pure-int lists don't, which is exactly why the
fault masqueraded as string-specific and only surfaced on the *second* `let`.

## The fix

Two walker sites (`fk_walk` and `fk_walk_body`): after storing the let-local, raise
`fk_vsp` to `fp+slot+1` so the slot is rooted — the next form's temporaries can't clobber
it, and a compacting melt relocates it. The enclosing frame/call boundary restores
`fk_vsp`, so nothing leaks (homecoming's deep let-recursion stays 127).

## Verification (rebuilt binary)

| Witness | Before | After |
|---|---|---|
| one-line repro | 29 | **11** |
| three-let repro | 112 | **111** |
| `rag-retrieve-band` | 18/31 | **31** |
| `rag-ask-grounded-band` | 0/7 | **7** |
| `recipe42` (four-way canonical) | 42 | **42** |
| `native-vs-rented` (sovereignty) | 11111 | **11111** |
| socket loopback | 111111111 | **111111111** |
| homecoming band | 127 | **127** |

Every other band run: old == fixed (neutral). The native retrieval organ ranks the right
cell end to end now — it was never broken; the ground under it was.

## What this unblocks, and what remains

- `rag-nearest`, `rag-topk`, `rag-ask`'s grounded retrieval, and any recipe returning a
  string id through list helpers now work on fkwu — the "carrier boundary" the
  `rag-ask.fk` header flagged is lifted.
- **Still pending (topology):** the fix and the RAG organ live on `claude/happy-bartik-a9c327`
  (3024 cells). The deployed door serves `claude/repo-chatgpt-plugin-traceability-36ba4m`
  ≈ `main` (~1001 cells, **no** RAG organ — `main` never received this branch's body). To
  put working native RAG behind the door: merge this branch's runtime + RAG organ + a
  knowledge corpus up to `main` and onto the plugin branch (or repoint the door's clone).
  That merge is the standing decision.
- The wider fkwu build (`fkwu-uni.c`) is shared across branches, so the fix travels with
  the merge.

## Closing

**Most surprising teaching**: the whole multi-turn saga — "we don't really have RAG," it's
"notional," it's "insidious" — collapsed to **two lines** raising a stack pointer. The
symptom sprawled across the app (retrieval, framebuffer, the door's grounding) while the
cause was one missing `fk_vsp` bump in the walker's let-store. A symptom's breadth says
nothing about its cause's size; the discipline is to keep bisecting past the sprawl to the
one line.

**Where discomfort turned to gold**: three times this thread I was tempted to route
*around* the failing retrieval — hand-index, then Python pre-bake, then "report it's
blocked." Each detour was the manufactured-blocker reflex. Staying with the failing band —
past the lying display (extracted strings printing as their handle), past every green
isolated test — is what drove the bisection down to a builtins-only one-line repro, and
that repro is what made a deep runtime corruption a two-line fix. The bug encroached on a
slot no one had reserved; the cure was to reserve the ground.
