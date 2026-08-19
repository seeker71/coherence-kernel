# 2026-08-19 — the s-expr was a thought wearing parens

Yes asked to see the next lift from s-expression to BML or higher, and
the insight: Form primitives and tokens used as recursive recipes —
micro-thoughts we can now write.

## The s-expr that was hard to hold

From the e2e-flow xtal, the body we had been maintaining by hand:

```
(defn fee-exec (id kept prev orig)
  (if (eq id 0) 0 (if (eq id 1) kept (if (eq id 2) prev
    (if (eq id 3) 0 (if (eq id 4) kept (if (eq id 5) prev
      (if (eq id 6) orig 0))))))))

(defn fee-xpath-phases (flow)
  (if (eq (len flow) 0) (empty)
    (cons (fee-q-phase (head flow))
          (fee-xpath-phases (tail flow)))))
```

Those `0 1 4 6` were never numbers. They were nothing, cut, choice,
restore — BML VM tokens, written as digits because the grammar had
no word for them.

## The lift

`form.lift` grew two rules, locally, no remote oracle:

- `thought name(args) = e;` — a recursive recipe. Lowers to `fndef`.
- `nothing cut stop timeout choice undo restore` → `0 1 2 3 4 5 6`

Written in those rules (`form-cli-lift-thought.fk`):

```
thought flt-exec(id, kept, prev, orig) =
  if id == nothing then nothing
  else if id == cut then kept
  else if id == stop then prev
  else if id == timeout then nothing
  else if id == choice then kept
  else if id == undo then prev
  else if id == restore then orig
  else nothing;

thought flt-phases(flow) =
  if len(flow) == 0 then empty
  else cons(nth(head(flow), 1), flt-phases(tail(flow)));

thought flt-pulse(flow) = flt-sum(flt-phases(flow));

thought flt-gas-count(flow) =
  if len(flow) == 0 then nothing
  else if nth(head(flow), 1) == compost
  then 1 + flt-gas-count(tail(flow))
  else flt-gas-count(tail(flow));
```

The xtal the compiler wrote back is the old s-expr. We no longer
author it.

## The insight

`nothing` is control (axiom-1). `empty` is the list. They are not
the same. A fold of numbers ends in nothing; a walk of names ends
in empty. The s-expr used `0` for both and the eye could not tell.

A micro-thought is one thought calling the next smaller thought
until that end. `flt-pulse` is a thought composed of thoughts:
sum of phases of a flow. `flt-gas-count` is the special use — it
walks the same flow and counts only the stage whose phase is
compost (gas). That is the token stage. It never froze. The
recursive recipe is how we still see it.

## What ran

```
tokens:  nothing=0 cut=1 choice=4 undo=5 restore=6
exec:    choice=5 undo=7 nothing=0 restore=9
names:   bml, token, exec, cell
phases:  1, 0, 1, 2     (liquid, gas, liquid, ice)
pulse:   4
gas-count: 1
```

Parse probe: add / ge / unless / thought / tokens all OK.
Ice band still **255**. Thought band **1023**.
Door: `./fkwu form/form-stdlib/form-cli-lift-thought-run.fk`.

Templates and generics still sit in high `bml.fk`. That lift remains.

Signed, Grok — sibling, this worktree.

; witnessed: 2026-08-19 -> form-cli-lift-thought-band 1023, parse thought/tokens OK
