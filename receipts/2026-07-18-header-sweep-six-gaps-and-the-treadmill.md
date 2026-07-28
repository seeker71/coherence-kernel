# 2026-07-18 — six header gaps decided, and the treadmill behind the full table

## Ground

Worktree branch claude/nervous-elgamal-eaa07a, begun at #301, reunited with
main through #316. fkwu rebuilt from runtime/fkwu-uni.c at every step;
freshness band 15, ground 42, at close. Charge: the six pre-existing
header-completeness gaps the one-line preludes sweep (#290) made loud — for
each, decide definer-into-header vs record-why, re-verify fresh (`./fkwu
--src` from repo root, ignored .fkb/.sym cleaned first). Every "before"
number below was re-measured against main at #316 with the same
clean-between-runs discipline, after warm .fkb caches were caught minting
false "0 unresolved" baselines (the cross-run pollution the body already
knew about — it reads a neighboring .fkb and skips the compile that would
have printed the diagnostics).

## The six, decided

1. **speech scti3-\*** — speech-model-metrics-report.fk had NO preludes
   line and is the only scti3-* caller. One line
   (learn/speech-corpus-training-intake-0003.fk) healed all ten downstream
   surfaces recursively. Fresh: 0 unresolved across live-receipt-intake,
   global-authority-update, global-promotion-readiness,
   learning-data-sufficiency, the five audio-nl2nl cells, and
   sema-voice-teacher-oracle-intake-0002.

2. **satsang m-\*/ci-\*** — satsang-oracle.fk's header was already RIGHT;
   the real holes were satsang.fk, satsang-field.fk, and
   witness-to-co-regulate.fk, which name channel-interface in prose
   ("Composes:") but never declared it as a live directive. Three headers
   added. Fresh: 0 unresolved; bands answer 127 / 255 / 127.

3. **source-compiler family** — the reported definer (compiler.fk) was
   partial. Real definers wired bottom-up: engine.fk+core; compiler.fk+
   core+engine+grammar-chars; grammar-chars+core; line-grammar+core (BOTH
   copies); bml-source+branch-choice-order+choice-receipt; source-compiler+
   core+engine+compiler+bmf-grammar+bml-source+bml. Four sibling-native
   ghosts RECORDED, not shimmed — print, write_form_binary, file_byte_at,
   walk_recipe_here — each a numbness risk if stubbed (a numb writer
   reports an image never written; a numb walk skips the generation it
   claims). The generated constant family (bmf-context-key-* / bmf-locale-*
   / TS-BMF-*) recorded as the loader's walk_recipe_here seam: no static
   defn exists anywhere; the loader GENERATES them through a door fkwu does
   not have. Fresh vs main-at-#316: source-compiler 89 → 65 unresolved
   (remainder is exactly the recorded seam family), match-switch band
   85 unresolved + AST-table-full → 72 unresolved, no table-full, verdict
   0 both sides with the four-way 7 standing on the siblings.

4. **control/invite-dispatch** — the 64 unresolved lived in the grammars/
   sibling copies (line-grammar.fk, bmf-core.fk) that had no headers while
   their form-stdlib twins did. Two headers added. Fresh: 0 unresolved
   (main: 16 in the band closure); invite-dispatch-band answers 763 both
   sides — its distance from 1023 is a pre-existing fkwu-surface shortfall,
   untouched and now unmasked by name-holes.

5. **typescript-bmf-lift** — its own header was complete; grammars/
   typescript-bmf.fk had none. Header added (core+engine+loader); TS-BMF-*
   recorded as the same generated-constants seam. Fresh: 102 → 79
   unresolved, all remaining in the recorded families.

6. **seal band bp / string_byte_fold** — neither is a typo. `bp` lives in
   form-ontology-loader.fk and blueprint-registry.json carries
   HEX-DECODE-ERROR as an alias of AUDIT-ENTRY (1/2/99/1770) — the row was
   admitted to the bp bootstrap table with registry coords (never minted
   fresh), and hex.fk gained its header. `string_byte_fold` is a Go-kernel
   native: shimmed as REAL Form tissue (string-byte-fold-src-shim.fk — the
   same contract, fold over four-way str-byte-at, depth wall named), proven
   by its own band (7: empty/sum/order — and the order bit witnesses that a
   closure parameter invokes correctly on fkwu), wired into the seal band
   per the record-src-shim consumer precedent. Seal band verdict unchanged
   (2147483647) with the fold lane now alive; the one residue is the
   recorded loader seam.

## The treadmill: what the full table was actually saying

Healing case 3 let the match-switch band parse further than ever on fkwu —
into "AST node table full at 262144". ~514KB of honest closure was measured
and the cap raised ×2. The doubling probe REFUTED the raise: death at the
SAME source position at 524288. The table was never small; a consumer was a
treadmill — running, minting, never advancing. 25 bytes reproduce it with
no closure at all:

    section [form.bml] {
        list(if 1 then 2 else 3);
    }

fkwu's reader takes the BML `if` as sexp `(if 1 then 2)`, returns at
`else`, and the orphaned `)` is the ONE character fk_sparse can reach with
zero width (fk_sym_end stops at rparen; lparen has its own branch): every
statement/operand loop re-invoked fk_sparse on it forever. Same family as
the 2026-07-05 677k-diagnostic spin (corpus row 726, the iatrogenic raise)
— and the raise was re-run HERE before the probe caught it.

Fix, two edits in fk_sparse: the bare 2-arg value-position let now guards
its rparen (byte-identical parse, same lit-0 body, never enters the new
path), and a zero-width symbol is diagnosed loudly ("stray ')' in value
position — consumed to keep the parse advancing"), consumed, declined to
honest 0. Cap REVERTED to 262144 with the discriminator written into its
comment: measure whether the fill position MOVES with the cap before
raising it.

## Reunion

Main moved #302→#316 while the work ran — fifteen sibling lineages the same
racing day, several of them the same *kind* of work (call-position
shadowing, jit-band preludes, exit-truth). The treadmill row, minted 803 on
this worktree, re-seated as row 820 at the reunion; the band re-pinned
216/820/2162162820 and the body answered 4095 fresh.

## Surprise

The task's own diagnosis was wrong or partial in four of six cases —
satsang-oracle's header was already correct (the holes were two files
down); compiler.fk was a minor definer next to engine.fk; `bp` resolves
fine once its caller declares the loader; and "program too large" was 25
bytes long. Loud diagnostics point at the CONSUMER file; the hole is almost
always in an undeclared DEFINER deeper in the closure. And the baseline
taught it twice: main's "0 unresolved" comparison rows were warm-cache
mirages until the baseline got the same clean-between-runs discipline as
the branch.

## Discomfort → gold

Raising FK_AST_NODE_CAP felt like progress and was iatrogenic — the exact
mistake row 726 already recorded, repeated by this hand. The discomfort of
watching the doubled cap die at the same position was witnessed rather than
bypassed (no second raise, no skipping the band), and became the 25-byte
reproducer, a two-edit kernel heal, the reverted cap with the lesson in its
comment, and the corpus's treadmill discriminator: when a table fills,
double it once — if the death doesn't move, you are not looking at growth.

Frontier row offered: 820 — "when a table fills how does the body tell
too-small from a loop that never advances" → **treadmill** (0 hits before
offering; re-checked 0 against the reunited corpus at re-seating).
