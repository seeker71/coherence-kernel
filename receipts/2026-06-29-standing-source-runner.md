# Receipt — the standing source runner; fkwu runs Form source from a file, natively (2026-06-29 ~05:00 MDT)

**What happened:** `form-eval-cli` (PR 3872, `agent/form-eval-cli.fk`) — `fec-read` reads Form source from a
FILE (`argv[3]`, via `input_byte`, byte by byte) and `form-eval` evaluates it off the BMF cursor. fkwu runs
arbitrary Form source from the command line, with NO flatten of the source.

**Witnessed by native run** (the one flattened thing is the cli recipe itself, the cursor seed; all *source*
runs through it un-flattened):
    (add 40 2)                                 -> 42
    (sub 50 8)                                 -> 42
    (if (le 1 2) (add 40 2) 99)                -> 42
    42                                         -> 42
    (add (sub 50 8) (if (le 9 1) 100 0))       -> 42

**Honest classification:** fkwu-NATIVE, not four-way — `input_byte` (the staged-input op) is fkwu-only, so the
witness is the native run, not a four-way band. The EVAL it rides (`form-eval` / `form-eval-full`) IS four-way.

**What this closes:** rung 1's (b), the standing. Combined with `form-eval-full` (a, four-way 131 — real recipes),
the knot named all night is gone: running a recipe never requires flattening it. Flatten is optional speed.

**Honest floor:** the cli rides `form-eval` (core grammar) — a one-line swap to `form-eval-full` gives the full
grammar (defn/let/calls). It's a single-file runner, not yet an interactive loop. Polish, not the gate.

**How it crossed:** by *doing* it, not declaring it gated — three small bugs found and fixed in sequence
(band only read instead of eval'd; stdin vs file; literal vs file-path). Each a five-minute fix. The "gate" was
three bugs deep, not a mountain.
