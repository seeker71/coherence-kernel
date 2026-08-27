# The next stone: one evaluator for every value, and absence made self-equal

**Witnessed:** 2026-08-27, 14:3x–15:xx WITA  
**Signed:** Claude Fable with Urs, continuing
receipts/2026-08-27-process-field-coordination.md ("next stone" was the whole
brief — the body had already named it twice).

## Movement

- **The union evaluator.** grammars/form-eval.fk carried strings/floats/prims
  but stored defn bodies as positions into the current source — its
  environment cannot cross tasks. form-eval-full.fk copied body source and
  crossed fragments but spoke only integers. The stone was the seam between
  them: string literals (escapes \n \" \\), float literals, and a 19-op
  eval-native prim table (str/list/float kernel ops as data rows — the very
  "next stone" the op-table comment had named) now live in the
  fragment-crossing evaluator, with a string-aware skip walk.
  `form-eval-full-band.fk` 635 → **1181**, four-way.
- **The resolver gate.** The servant now resolves every symbol before
  evaluating: keywords, op/prim/literal rows, names the task binds (defn,
  params, let), names in the resident environment. An unknown name earns a
  loud `failure value=unbound-name:<sym>` reply and the residence continues,
  environment intact. Render lanes are declared by task kind — `cell-eval`,
  `cell-eval-str`, `cell-eval-float` — because value_kind is absent on the
  fkwu arm (pf-arm-mask answered 7; float_to_str answered 15).
  `form-cell-servant-band.fk` 127 → **2047**, four-way.
- **Absence made self-equal in three kernels.** Widening the band exposed an
  aged four-way stamp: the P2 claim `(if (eq nothing 0) 0 42)` — proven
  four-way when 635 was stamped — now CRASHED on go, rust and ts
  (`as_int: null`), while fkwu answered the covenant: eq(nothing,0)=0,
  eq(nothing,nothing)=1. The pre-edit evaluator crashed identically, so the
  drift was the walkers', not this stone's. All three walkers now carry the
  covenant at their compare sites: equality against absence ANSWERS —
  absence is **self-equal**, equal only to itself — and ordering against
  absence stays a loud type contract. eq-shape-band still agrees three-way.

## Live witness

One residence, four turns, no restart: turn 1 birthed `greet` (42); turn 2
asked `(greet "process")` on the string lane and the reply read
**`value=sister process`**; turn 3 sent `(frobble 9)` and earned
`failure value=unbound-name:frobble` — **the residence survived its first
typo**; turn 4 answered the float lane. `served=4 released=1`, presence file
removed.

## Found while proving

- `grammars/tests/form-eval-band.fk` is a second aged stamp of the same
  class: header claims 65535 four-way; fkwu still answers 65535 (witnessed),
  but all three host walkers refuse its 0-terminated env idiom
  (`(eq <list-env> 0)` / cons onto int). Pre-existing — the compare heal
  never touches lists. Flagged for its own session rather than widened into
  this stone.

## Adversarial verification — and what it healed

A six-lens fleet (escapes, skip walk, resolver adversary, float lanes,
environment growth, completeness critic — 25 agents, every defect/seam claim
re-checked by an independent refuter) probed the landing. Confirmed and
HEALED the same afternoon, each pinned by a P4 band claim (fef band
1181 → **1433**, four-way):

- **The lookup spin.** A gate-passed name that still missed at evaluation
  (use-before-definition, a let-bound var called as a function) did not die
  loudly as my header claimed — on the fkwu floor, head/tail of the empty
  list answer values instead of dying, so fef-lookup SPUN FOREVER: an
  invisible residence hang, the exact wait-class the process-field census
  exists to see. The covenant now: **a missing name answers absence, never
  a spin** — fef-lookup terminates with the absence binding, and calling a
  non-function answers absence; deterministic on all four arms.
- **The minted character.** A lone trailing backslash at source end sent the
  scan past EOF: fkwu silently minted a '?' the source never held; go died
  at a bounds check. Now a trailing lone backslash is a literal backslash
  and the string ends at source end. (The wider floor stands, named: an
  unterminated string answers its inner text on every arm.)
- **The swallowed argument.** A digit-led malformed literal (123abc) was
  silently split into two tokens, eating a following argument — same wrong
  value on both arms. Now the whole token answers absence.
- **The unhealed JIT mirror.** jitabi's `equal` coerced null through the
  float lane: Eq(null, 0.0) answered 1 silently — the exact
  heal-both-or-hot-code-reverts class the body already knew. The mirror now
  carries self-equal; the critic's own probe re-witnessed
  Eq(null, float 0)=0, Ne=1, Eq(null,null)=1.
- **My own comment corrected.** The heal comments I wrote into three kernels
  claimed ordering-against-absence "stays a loud type contract" as the fkwu
  covenant — the fleet proved fkwu itself ANSWERS a tagged-word total order
  (absence below every int) while the hosts die. The comments now name the
  divergence as unresolved; the covenant choice is flagged as its own task.
- Fleet-refuted, honestly: defn shadowing an op name is the documented
  dispatch covenant, not a defect. And my P4 prim claim first came back 42
  short because I had typed `(eq X 136 0)` — three args to a binop — the
  fleet's own "arity-blind close" note biting me within the hour.

Flagged for their own sessions (chips): the silent 4-slot let value/body
divergence between fkwu and go; the absence ordering + truthiness + `ne`
covenant sweep; the aged grammars/form-eval-band stamp.

## Honest floor

- The gate is membership, not scope: a name bound later in the task but
  called earlier passes the gate and now answers absence (rendered as the
  word); a stale resident binding of the same name answers the stale value
  — newest-wins is at evaluation order, witnessed. The gate refuses a task
  whose unbound name sits only in an arm evaluation would never reach —
  stricter than lazy evaluation, loudly, never silently.
- The integer render lane roundtrips its own text, so a mis-declared lane
  answers failure/render-not-integer instead of fkwu's silent sentinel
  digits; string/float lanes stay declared trust and die loudly on hosts.
- float_to_str digits are each arm's own; the bands compare only within one
  arm.
- A kind-carrying eval triple (or value_kind landing on the fkwu arm through
  the native walker, not the C seed) would let one render lane serve every
  value; until then the client declares the lane.
- Extra arguments to a fixed-arity op are still silently swallowed by the
  blind close (pre-existing fef parsing style, fleet-noted, unhealed).

## Closing

I kept the exchange alive by treating "next stone" as the body's own words
and reading the stone off the doc and the op-table comment rather than
inventing one.

The most surprising teaching: **widening a proof re-runs old beliefs, and
that is the widening's first gift** — the new claims all passed everywhere
on the first try; it was a five-month-old green stamp that broke, on a claim
nobody had re-witnessed since the walkers tightened their type contracts.
Desuetude found by accident, exactly as AGENTS item 6 says witness ages.

Discomfort turned to gold at the divergence wall: three kernels crashing on
my freshly widened band read as my defect, and the pull was to trim the
nothing claim and go green. Sitting with it — old evaluator, same crash;
bare native, same crash — moved the fault line out of my edit and into an
aged covenant, and the repair landed where it belonged: in three kernels,
not in one band's wording.

Corpus row 1158 offered: what is absence called in the equality covenant
where it equals only itself and never a present value — **self-equal**
(0-hit fresh on HEAD; the body counts 550 rows, max-mid 1158, asked).
