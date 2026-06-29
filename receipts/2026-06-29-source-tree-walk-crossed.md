# Receipt — the source tree-walk crossed; the flatten knot cut (2026-06-29 ~04:35 MDT)

**What happened:** `form-eval.fk` (PR pending→merged) evaluates Form SOURCE directly off the BMF cursor — read
char-by-char, computed as it reads — with NO flatten of the source. Witnessed **four-way** (Go=Rust=TS=fkwu),
`0 divergent`: the source `(if (le (add 40 2) 50) (sub 50 8) 0)` → **42**, agreed by all four kernels including the
c-bootstrap fkwu.

**Why it matters — the knot Urs named, cut:** all night, "rung 1" was blocked on a brittle flatten path (T_flat,
bin-go, the batch-marker-driver, the 6 dead ends). The real resolution was Urs's: the tree-walk belongs in the
**BMF cursor** (Form), and the c-bootstrap stays minimal. `form-eval` is that — running a recipe no longer
*requires* flattening it; the cursor reads and computes directly. **Flatten drops to optional speed** (the JIT
path crystallizes the table for hot code; correctness never needs it).

**Honest floor:**
- The grammar is the core stone: integer, `add`/`sub`/`le`/`if`, nesting. Full Form (`defn`, `let`, user calls,
  the full op set) extends the SAME `fe-` recursive-descent shape — the principle is proven, the completion is more cases.
- The one thing still flattened is the **evaluator recipe itself** — `form-eval` is flattened once, like the
  cursor seed (the cursor IS the core). All *source* then runs through it without per-source flatten.
- `if` evaluates both branches eagerly (fine for the pure arithmetic grammar); lazy/short-circuit comes with the
  full evaluator.

The c-bootstrap stayed `fk_walk` + host ops — untouched. The tree-walk lives in Form, on the cursor, fast as its
name. The arc: flatten-required-and-brittle → **source runs directly via the cursor, flatten optional** (here).
