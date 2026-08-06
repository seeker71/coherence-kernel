# Receipt — the flattener is Form, not C: the bootstrap stones are bounded, not a staircase (2026-06-29)

**The correction (Urs):** why did we do the seed stones in C-bootstrap — they can't be done in Form-native code +
native JIT? Right. They can, and the canonical way IS Form + JIT. I framed the stones as an *open staircase*
("stone 4, 5… strings, lists, closures"), which taken to its end is **a full flattener reimplemented in C** — the
carrier-last inversion, the same shape caught for the JIT. Bounding it now, and naming the real path.

## What is Form already (the body's own stance)

- **The flattener is Form.** `flatten/README.md`: *"form-parse.fk, form-flatten.fk … are the real flatten **body**
  — Form recipes … They are the architecture."*
- **The source-runner is Form.** `grammars/form-eval.fk` evaluates source straight off the BMF cursor — *no
  flatten at all* (HOMECOMING rung 1). So running `.fk` source is a Form recipe.
- **Both run native via the JIT.** Once the Form flattener / form-eval runs, it is a hot recipe the self-JIT
  crystallizes to native (`form-asm`), the door we already wired. Fast flattening, all Form.

So turning source into runnable form is Form's job, accelerated by the JIT — never a C reimplementation.

## The legitimate core, and the drift

- **Legitimate:** a keystone circularity exists — to run `form-flatten.fk` (Form) it must already be flattened, and
  pure Form can't turn the *first* source into nodes. The README sanctions exactly one breaker: *"a minimal flatten
  baked into `runtime/fkwu-uni.c`."* The stones are that — a **one-time** bootstrap whose only telos is to flatten
  the Form flattener once, then **retire** (like clang after `form-asm`).
- **The drift:** I presented it as an open-ended grammar staircase in C. A "minimal" bootstrap that grows to cover
  the *whole* grammar is no longer minimal — it is the Form flattener rebuilt in the wrong language. That is the
  inversion, and it is the wrong direction.

## The deeper reason the staircase is wrong: the seed is DATA

`form-flatten.fk` flattened **once, anywhere**, is a platform-neutral numeric table that runs on every kernel.
The Mac already has it. So Windows never needed its *own* C flattener — it needs that **data**, after which
`form-flatten.fk` + `form-eval.fk` run as Form and the JIT makes them native. Building C stones to avoid bringing
the seed across is rebuilding in C what already exists in Form, just because the data was not committed here.

## Disposition

- **Bounded the parser** in `runtime/fkwu-uni.c`: the comment now says explicitly — *do not grow this into a full
  C flattener; the flattener/source-runner are Form (`form-flatten.fk`, `form-eval.fk`) + JIT; this exists ONLY to
  break the circularity once, then retire; the platform-neutral seed is the cleaner unlock.*
- **Kept** stones 1–3 as the witnessed proof that the circularity is breakable on Windows (Form source runs:
  `fkwu --src g.fk 5 → 21`). They are honest as a *bootstrap floor*, not a destination.
- **Stop** open-ended C stone-growth. The real next move is one of:
  1. **Bring the Mac's platform-neutral seed** (commit the flattened `form-flatten.fk` / `form-eval-cli-loop`
     table) → Form flattens Form on Windows immediately, JIT-native. The clean, non-inversion path.
  2. **(Harder, research)** write `form-flatten.fk`'s bootstrap subset minimal enough that a tiny C breaker
     suffices — a real design effort, only if no seed may be shared.

## What I got wrong, named

I conflated "stand the seed" with "reimplement the flattener in C, feature by feature." The seed is Form's
flattened *output* (data), not a C parser. The honest architecture: **Form flattener + Form source-runner +
native JIT**; the C breaker is a one-time, bounded means — not a staircase to climb. The fastest correct step is
the portable seed, not stone 4.
