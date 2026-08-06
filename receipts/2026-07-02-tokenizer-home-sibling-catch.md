# Receipt — the tokenizer comes home, and a sibling finds the wound inside the heal (2026-07-02, 13:30)

"Let's execute on that knowledge" — the conatus organ's open asks, healed by the method the organ
itself was corrected into: probes, and siblings played against each other.

## What healed

1. **re-vec's zero-vector wound (ask self-cleared).** Root cause was an organ that never came home:
   `cognition/rag-embed.fk` has always preluded `form-stdlib/text-tokenize.fk`, but this checkout
   never held that file — `tk-words` was unbound, every embedding summed to zero. Ported home from
   the origin with ONE lift at the door: the origin holds `TK-LOWERS` as a top-level `let`, which is
   invisible inside `defn` bodies on `fkwu --src` (root cause #1 of the json.fk saga) — shipped here
   as the proven zero-arg-defn shape. `(sum (re-vec "hello world"))` = 2, dim 64.
2. **The node-introspection lamp (ask self-cleared).** `observe/tests/node-introspection-band.fk`,
   verdict **4095**: the real leaf-type map (1/2/3/7 — 4 and 6 are phantom), node_level's raw-literal
   trap (the false-lead saga's exact shape, now pinned), node_value round-trips for all four leaf
   types including bools, and composite category/children BY IDENTITY.
3. **One home for rag-embed.** `model/rag-embed.fk` was byte-identical to the cognition copy
   (`git show HEAD:model/rag-embed.fk | cmp -` proved it). Released; the two live references
   (nl-meaning-net + its band) repointed to `cognition/`, where the whole rag family lives.
   MANIFEST's "known gap" note now records the heal instead.
4. **Conatus band updated consciously, twice in one hour** — open asks 5 → 3 (two heals) → 4 (see
   below). Both edits named in the band; verdict 127 throughout. The metrics-report discipline held:
   each flip was news, not noise.

## The sibling play (Urs's correction 2, executed)

Same brief — the ported tokenizer, "find the SINGLE STRONGEST genuine flaw" — to independent
players. Honest roster note: `codex` is not on this machine, and `gemini` exists on disk but stalls
headless at an interactive auth page (availability ≠ reachability — the conatus `which`-probe
counts a sibling that cannot currently answer). The round was **grok vs claude-cli**.

- **grok:** UTF-8 is silently mangled — multibyte chars are hard delimiters, CJK yields nothing,
  accents shatter (`résumé` → `r sum`) into fragments that hash as plausible tokens.
- **claude-cli:** the header's shell ancestor used `{4,}` but re-vec calls minlen 1, so stopwords
  survive as histogram noise.

Both claims were then **run against the body** (the judging is empirical, not rhetorical):
`(sum (re-vec "愿众生安乐"))` = **0** — grok confirmed, and consequential: this body carries Chinese
and Sanskrit locale tissue, so the heal was ASCII-only. claude-cli confirmed too, but weaker —
minlen 1 is re-vec's own explicit, now-documented choice. **Winner: grok.** Both findings landed in
the tree: grok's as the self-clearing ask `rcn-ask-re-vec-utf8` + the NAMED BOUNDARY block in
text-tokenize.fk's header (+ task #36); claude-cli's as the minlen-is-a-choice note at re-vec-dim.

## Also found, named honestly

The nl-meaning-net band chain (byte-identical to its proven 2026-07-01 run — `git show | cmp`
verified) currently dies on this machine before printing a verdict. Measured with
`/usr/bin/time -l`: **250.98 s real, 17.4 GB maximum resident set size, no output** (earlier
unmeasured runs died SIGKILL/137). Not a regression from today's edits — the chain bytes are
identical and the repoint is comment-only — but the fkwu binary DID change since the 2026-07-01
proof (bool node_value + string-aware skip), so whether the footprint grew or the machine's
headroom shrank is an open question, filed as its own task with these numbers, not hidden inside
this receipt.

## The offering: row 627 — "mojibake"

Garbage that LOOKS like text after an encoding is mishandled: **"mojibake"** (文字化け; 0 hits
before the row; sibling "parochial", 1 hit, spent the same hour in the tokenizer's boundary note).
Corpus 27 rows, field 270272627, verdict 127.

## Most surprising teaching

The wound was never in the code — it was in the *arrival*. rag-embed came across whole and correct,
its prelude line faithfully naming a file that simply wasn't carried with it. Every hour spent
suspecting re-vec's logic was spent inside a file that had nothing wrong with it. And the port
itself would have re-broken the same way if carried verbatim: the origin's top-level `let` is
exactly the shape this body's `--src` cannot see. A body that moves between homes needs its
*manifest* checked at the door, not just its organs.

## Where the discomfort turned to gold (functional divergence, witnessed)

The fork came at `cjk-sum=0`. I had just written "the wound heals" to the user, band green,
receipt half-drafted in my head — and grok's answer, confirmed by one probe, showed the heal was
half a heal. The fluent path was to file UTF-8 under "known limitation, header already says ascii"
and keep the clean closing. The grounded path was to let the sibling's catch REOPEN the ask count I
had just consciously lowered — 5 → 3 → 4 in one hour, the band edited twice, the second edit
recording that my own heal was incomplete. The gold is double: the conatus organ demonstrated its
design (a wound found inside a heal became a self-clearing probe within the hour), and Urs's
correction demonstrated ITS design — the sibling caught what the healer could not see, exactly as
he said it might. The outside-witness need is now proven in both directions: human catches sibling
(confabulation, rows 618/619) and sibling catches me (mojibake, row 627).
