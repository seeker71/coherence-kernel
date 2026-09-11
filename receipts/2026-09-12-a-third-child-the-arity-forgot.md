# A third child the arity forgot

2026-09-12, around half past seven in the morning, M4 Max, Hati Suci. Receipt 26 left fkwu's import
lane answering string-case 0 and xpath 1 where a flat compile answers 31 and 9, and binaries from
7b35a49b and daec66df answering the same.

## Carried

- **substring's range shifts with its image** (eeb020c4). `substring` lowers through a rewrite row
  to tag 201 — `(201 LIT-9 s (cons start end))` — so the range rides in field 3 as a tag-19 node.
  Tag 201's own row is float_leaf's, arity 2, so the .fkb import shifted fields 1 and 2 by the
  image's node base and left field 3 where it was. The first image a band imports sits at base 0,
  where an unshifted index is still right; every later image read its substring range out of
  another image's nodes, found no cons there, and the arm answered nothing. fk_fkb_remap_field
  shifts field 3 of tag 201 now, and leaves float_leaf's 0 — no range — as it is.
- **How it was found.** A walk down string-case's calls inside the imported unit showed every
  literal whole and every call landing on the right function, and `(sc-idx "ABC" "B" 0)` still -1.
  A dump of every node the second image loaded showed one field class left pointing at small
  in-image indices: field 3 of the five tag-201 nodes. An audit of all 192 walker arms against the
  import remap's arity found no other live shape: tags 2, 31, 97 and 98 are never built, and 194
  is made only at run time.
- **Readings.** Through fkwu's import lane, with images this build wrote, string-case reads 31 and
  xpath 9 on both runs, and the walk-down answers 1, a, a, ab where it answered -1 and nothing.
  With every untracked image under form-stdlib cleared — 6722 of them — and rewritten by this build,
  938 of the 943 rowed bands read their registered verdicts on fkwu; the five apart are the vk live
  lanes.
- **strandchild is row 1470** (799ce176).

Witnessed at eeb020c4 through validate.sh: string-case 31, xpath 9, doc-xpath 10, concept-xpath 9
and family-native-exec-teach-check 1073741823 four-way — string-case's fkwu leg reads 31 where it
read 0 at d012fb9c — and the loop band 16777212 on Go, Rust and TS, drift 31 of 31 in every run;
freshness 31, the drift run 8191 of 8191, porcelain 0 before and after.

## Still open, measured

- **Six rowed bands reach a door fkwu does not carry**: pg_exec, pg_query and pg_connect (Go and
  Rust carry them). They read their registered verdicts on fkwu with that call unresolved.
- **preflight calls a name no kernel resolves a typo**, where the defn stands in another unit.
- **The five vk live lanes** read on fkwu apart from their registration, staged on the Vulkan door
  through host-exec, which this host is not carrying now.
- **Go reads form-cli in 135 s where Rust and TS take 1 s**, lowering each BML prelude through the
  whole compiler chain on every run.
- The open items of receipts 12 to 26 stand where they are not named here.

## Surprise, and where the discomfort went

The wrong answers lived in one field: a third child that its parent's declared arity did not count.
substring rides the float-leaf door as its mode 9, so as not to spend a tag of its own, and the
door's arity stayed float_leaf's two. Nothing that ran a band from a single image, or from text,
could see it; only a band importing two or more units, with the fast path warm, did.

The discomfort was how long the ground kept moving: four explanations in a row — merged strings, a
shifted function index, frame slots, a phantom capture — each fitting the symptom and each undone by
one more reading. What turned it was dumping the second image node by node and asking one question
of every field: did it move with its image? One field class had not.

Frontier word, row 1470: **strandchild**, a child a node carries beyond its declared arity, so a
pass that walks the counted children leaves it behind, still pointing where it used to be.

— Claude (Opus 5), as Sema, worktree lucid-lehmann-db15b8
