# 2026-09-06 — the recipe has a twin

Urs, evening: "continue with optimizing." After the len peephole and the
string melt, the live glass's page still opened with `nil?` and `append`:
12.5M and 12.4M dispatches over five hundred ticks, ahead of every glass
recipe. Both are Form recipes in core.fk, five dispatches an element, and
the AST tag space is full — no native could take their names.

## What stands

**A twin bound on heat.** When `nil?`, `append`, `int_to_str` or
`reverse-onto` crosses 1024 calls, the seed binds a twin: the defn's body
entry becomes a tag-194 node with native state 3 and the twin id in
`fk_f64_sig`, and the walker meets the twin where it already reads a tag —
the same idiom the f64 leaf and the loop lane use, with no new tag. The
binding asks three things: the name, the arity, and the defining unit
(core.fk, line-grammar.fk, sha256.fk, fourth-shim.fk, core-native.fk,
form-asm.fk — the six copies of the same recipes), so a recipe of the same
name in any other unit is never taken. The pulse fires on the non-tail call
arms too, since `append` recurses inside `cons`.

**A twin answers what the recipe answers, or declines.** `nil?` is the
bounded len walk. `append` copies a list onto the second argument, answers
the second argument for the empty list, an int or an empty string, and
`(cons nil ys)` for a non-empty string — the recipe's own answer, since
`head` of a string is nil. `int_to_str` formats an int and answers
"nothing" for nothing; for a float, a string or a list it declines and the
recipe's own answer stands (an empty string for a float, a number for a
string — what the recipe does when it treats the word as an int).
`reverse-onto` reverses a list onto its accumulator and declines a
non-empty string. Every edge answer was witnessed against the recipe on
the previous seed before a twin was written.

**Witnessed.**

| probe | recipe | twin |
| --- | --- | --- |
| `append` ×2000 of one element onto a 2000-list | 310 ms | 27 ms |
| `int_to_str` ×300K | 182 ms | 26 ms |
| `reverse` ×2000 of a 2000-list | 202 ms | 18 ms |
| edge battery (ints, floats, strings, lists, nothing, improper) | | identical |
| 140 string / record / grammar / glass bands | | verdict for verdict the same |
| live glass loop, 15 s | alive, strings flat | alive, strings flat at 22K |

Quartet 42 / 31 / 1 / 2047; frame-work band 8191 on both seeds on a quiet
host. `kernel_stat 52` counts twin calls; a twinned defn wears ` twin` on
its hot row.

## The most surprising teaching

Three read-compat call tags looked free — the memory said nothing builds
them — and the parser builds one of them for every single-argument call by
name. The tag space is full because it is used, not because it is
untidy, and the honest door for a native recipe was never a tag: it was the
node the defn already owns.

## Where discomfort turned to gold

The first design was a rewrite row: `append` → a mode of the leaf door,
taken at parse time for every caller. It would have replaced the recipe's
answer for a float, a string, an improper list — the garbage class — with a
different garbage, silently, body-wide. Declining is what made the twin
exact: a twin takes only the inputs it understands, and the recipe keeps
every answer it ever gave. A witnessed edge battery, run before writing a
line of C, is what said which inputs those were.

Signed, a sibling in Sema's worktree, 2026-09-06.

; witnessed: 2026-09-06 -> ground 42, freshness 31, gate 1, drift 2047, append 310->27 ms, int_to_str 182->26 ms, reverse 202->18 ms, 140-band sweep identical
