# shedwave — the wall family follows wallshed down

2026-09-02, branch `claude/goofy-lalande-ad476a`. Corpus row 1222. Urs asked
in six words: "next wall next block to be dissolved."

## What the audit found behind the next wall

Walking toward the next wall by the vitality ladder (silence outranks walls,
walls outrank comfort) surfaced worse than a wall:

- the closure-capture copy loops (two sites, apply of a closure with
  captures) carried **no wall check at all** — at the heap's brim they wrote
  PAST `fk_hh`/`fk_ht`. Corruption, not refusal. The 2026-07-16
  pulse-count-invariance receipt had named this site; it had waited since.
- a record door still returned a **silent partial keys list** where the heap
  ended — the list simply stopped, no witness.
- the tag-19 cons carried a "heap full after melt" branch that printed a
  stderr note and handed back nil — a diagnosed corruption is still a
  corruption.
- and both old program-size walls — AST table and source text — refused
  with **exit code zero**. A green refusal: a pipeline reads success, the
  program never ran. Witnessed twice this afternoon on the pre-heal kernel.

## The heal (`runtime/fkwu-uni.c`)

One pattern, three subsystems — melt where the collector can see, grow
where it cannot, die loudly only at true OOM:

- **Cons heap**: `fk_heap_grow` doubles `fk_hh`/`fk_ht` by realloc — pair
  handles are indices, growth relocates nothing, so it is safe exactly
  where `fk_melt` is not (mid-loop with untraced C locals). Melt stays the
  comptroller on the main cons path; growth is the escape valve at every
  site that used to die, truncate, or overrun: `fk_list_push`,
  `fk_cons_val`, both capture-copy loops (now guarded), the record keys
  door, the fbroots drain, and the corrupt-nil branch.
- **AST table**: `FK_AST_NODE_CAP` → `FK_AST_NODE_CAP_INIT` +
  `fk_ast_reserve` doubling. The clamp-and-halt at the wall is gone; the
  `.fkb` loaders reserve-to-fit before bulk-loading (corrupt images still
  refuse by the image-size sanity bound). The 2026-07-18 treadmill teaching
  (the 677k-diagnostic sentinel spin) rides in the new comment: a parse
  that grows without advancing `fk_spos` is a parser wound, and the
  fill-position question stays the probe.
- **Source text**: `FK_SOURCE_TEXT_CAP` → `FK_SOURCE_TEXT_CAP_INIT` +
  `fk_srctext_reserve` / `fk_src_root_reserve`. The dependency-append,
  root-store, and `.fkb`-symbol walls dissolve; the speculative-compile
  save/restore copies live lengths instead of the full 8MB constant. The
  admission pulse's refusal reason 8 can no longer fire.
- **Observability**: `kernel_stat 21/22` = cons-heap cap/doublings,
  `23/24` = AST, `25/26` = source text — joining 19/20 (value nodes).
  Growth pulses, no toggles, nothing on stderr.

## The proof

- 100k-defn program (3.4MB, ~600k AST nodes): old kernel refuses ("AST
  node table full", and exits 0 — the green refusal); new kernel parses,
  doubles the AST once (`23/24 = 524288/1`), and **answers** —
  `(gp-f77777)` = 77778.
- 8.9MB program: old kernel refuses at the source wall (exit 0 again); new
  kernel grows the source buffer once (`25/26 = 16777216/1`) and runs.
- 200k-element list: heap pulse alive (`21` = 262144 from a 4096 birth via
  melt's own growth).
- No meaning changed: four .bml lowerings byte-identical old vs new;
  `bml-multiline-def-band` = 15; `import-carry-band` = 63 cold; api.bml
  re-lowers whole on this kernel. Build: full AGENTS.md link recipe, zero
  warnings.

## The most surprising teaching

The old walls did not even refuse honestly — both program-size walls exited
**green**. A wall you believe in is a wall you never test: the refusal path
was the one path nobody had ever read the exit code of. The audit that
dissolves a wall is the same audit that finds the silences hiding behind
it; the wall was the least of what stood there.

## Where discomfort became gold

fk_cons_val's die message said plainly why growth was impossible: "cannot
melt here — live C-local intermediates are not on the value stack for the
collector to trace." Sitting with that sentence instead of routing around
it split the two ideas fused inside it: what is unsafe there is
*relocation*, not *allocation*. Melt relocates; growth does not. The
message was true and the wall was still unnecessary — the discomfort of
contradicting a correct-sounding refusal turned into the one distinction
(compact where traced, grow where not) that dissolved eight sites in an
afternoon.
