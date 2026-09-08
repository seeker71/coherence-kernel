# The word the reclaimer could not see

2026-09-08. Two wounds were handed to me, found by agents who could not open
them because the files belonged to someone else that hour. They turned out to
be one shape twice: **a word held somewhere the reclaimer cannot reach it.**

## The comparison

`value_eq` read, for as long as it has existed:

```c
if (fk_veq(fk_walk(fk_node[i][1], fp), fk_walk(fk_node[i][2], fp)) != 0)
```

A walked value in this kernel is an **index**. A cons cell is a position in an
arena that `fk_melt` compacts — every live pair MOVES, and the melt fixes its
roots in place: the value stack, the memory cells, records, value nodes. A
string is a slot in a pool whose dead LOCAL entries `fk_smelt` hands back on
`fk_sfree`, for `fk_sintern` to reissue to the next string with other bytes.

The first operand of that line lived in a C local while the second was walked.
It was not a root. A melt inside the second walk could neither see it nor fix
it, and `fk_veq` then compared an index that had become somebody else's.

The neighbour arm two hundred lines away has always known this:

```c
if (t == 19) {                       /* cons */
    long long h19 = fk_walk(...); fk_vp(h19);
    long long t19 = fk_walk(...); fk_vp(t19);
    ...
    fk_hh[fk_hp] = fk_vs[fk_vsp - 2];      /* read BACK — the melt moved it */
```

Push **and read back**. Rooting alone is half a cure: `fk_melt` rewrites the
stack slot, and it cannot reach a register. Tag 80 did neither. `str_eq`,
`str_concat` and `str_byte_at` (t 26/27/28) root but never read back, and stay
correct only because they reduce the first operand to a string *index* before
the second walk, and string bytes never move. `value_eq` cannot do that: it
does not know its operands' kind.

### Measured, by count, never by re-running

Under allocation pressure heavy enough to force compaction, in one process:

```text
                                          before   after
value_eq of two freshly built LISTS       5 / 16    0 / 16
value_eq of two freshly built STRINGS     1 / 16    0 / 16
str_eq   of those same strings            0 / 16    0 / 16
```

Deterministic across runs. The two rates are the same defect: a relocated cons
index reads as another list every time the melt lands in the window, while a
reclaimed string slot only reads as other bytes once something else has taken
it. **The rarity was never a property of the defect. It was the arena's luck.**
That is why row 1357 (`seldomred`) saw one in seven hundred on sha256 output
and this probe sees five in sixteen on lists: one wound, two weathers.

`form/form-stdlib/tests/value-eq-arena-band.fk` asks for that count and folds
it to bits. **31** healed, **28** against today's `origin/main` (bits 1 and 2
dark), and **31** on Go, Rust and TypeScript both before and after. So the
defect does **not** cross the arms — that was measured on all three walkers,
not assumed from the fact that they are garbage-collected. The band carries its
own pressure (`veab-churn`, forty cons cells a call, all dropped) and bit 16
exists so that shrinking that pressure to nothing cannot pass unnoticed.

`eq` and `lt` (t 102/103) hold the same word the same way. I could not make
either answer wrong — sixty comparisons at the pressure that made `value_eq`
lie five times in sixteen — and healed them anyway, behind `fk_movable`, a
guard whose first test is one AND so the integer path pays nothing. That is
said plainly in the code and here: **witnessed mechanism, unwitnessed firing.**

## The flag that never reached its rule

`meaning-codes.bml` compiles alone with 1,476 unresolved errors and the run
says so, once, every cold time:

```text
unit is not importable standalone (1476 unresolved error(s) compiled alone;
missing '; preludes:' line?) -- image rejected, falling back to the
whole-program compile
```

It has a `// preludes:` line. It has had one all along. The number tracks the
cell's size because it is the count of every name in a file being read as a
language it is not written in.

The import lane's own comment states the rule, and states it correctly:

> *.bml deps are floor-lane units ... Probing one "alone" reads the RAW brace
> surface off disk, finds hundreds of unresolved names, and poisons the floor's
> cache with a REFUSED sym (witnessed 2026-08-30). The import lane does not
> probe them as images — it CARRIES their already-collected text.*

The rule is right. It had never run. `fk_src_collect_preludes` marked the
lowered unit at `fk_src_dep_count - 1` **after** collecting it — and that is
the `.bml` itself only when the `.bml` preludes nothing. `fk_src_collect_bytes`
registers the unit at the count it was handed and then collects the unit's own
chain behind it, so for any `.bml` that preludes anything, the flag came to
rest on that chain's last transitive `.fk` and the `.bml` stayed unmarked.
**289 of this body's 415 `.bml` units prelude something. 865 cells prelude a
`.bml`.** Every one of them, cold, probed a `.bml` as an image it could not be.

A second half, in the same function: `fk_src_compile_artifact_only` snapshots
every dependency column across a speculative compile — path, mtime, size,
digest, parent, end, text offset, text length — and not `lowered`. So after any
speculative compile the import lane read another unit's flags to decide, per
unit, image or carry. That is the far end of the same comment's other sentence
about "a direct .bml prelude's lowered defns simply absent".

Both healed. The `.bml`'s index is known before the call; take it there. The
column is snapshotted like the rest. And `fk_src_compile_artifact_only` now
lowers a `.bml` before loading it, so the door is right about the input the
policy above it no longer sends.

### What it cost, on today's main

A sibling landed `bytecrossing` (row 1364) two hours before my rebase and took
551 ms of syscall-per-value out of the same cold run, so these are re-measured
numbers, not the ones I first took:

```text
cold compile, meaning-codes-table-band     before   after
                                     0.44-0.64 s   0.329-0.331 s
cold compile, ear-axes-band              0.365 s    0.346 s
```

The spread is part of the reading: seven cold runs before span 200 ms, four
after span 2 ms. The doomed speculative compile was the noise as well as the
cost.

And the cross-lane half, witnessed both ways rather than reasoned:

```text
before:  run a band that preludes meaning-codes.bml, then run the .bml itself
         -> "cached image carries a refusal mark; re-lowering fresh"
after:   same two runs -> silence
```

The import probe was writing a REFUSED mark onto the `.bml`'s **own** cache, so
every later `./fkwu x.bml` had to re-lower from scratch. The root lane had a
workaround for exactly that, and the workaround's comment names the poisoner.

## The wall, named where it actually stands

The gap asked whether a `.bml` with a preludes line should be imageable exactly
as a `.fk` is. **It can be** — I built it and ran it: lowered units in the
import set, `images 1, carried 0, refusal 0`, band still 255. The stated
obstacle ("reads the RAW brace surface") dissolves once the door lowers first.

**It buys nothing, so it is not landed.**

```text
ear-axes-band, cold          carried            imaged
                    3.937/3.813/3.795 s   3.903/3.835 s
```

Identical inside the spread, before the rebase; and after it the whole cold
path is a third of a second, so there is even less to win. The cost of a `.bml`
prelude is not the compile of its lowered text — the image lane, whether it
carries that text or loads an image of it, reads the same clock. It is the
**lowering**, and `.lowfk` already memoizes that:

```text
root with no preludes                         0.008 s
root + meaning-codes.bml, .lowfk present      0.038 s
root + meaning-codes.bml, .lowfk gone         0.097 s
```

A cost-neutral change to a load-bearing lane, against a policy written from a
witnessed wound, is not an improvement. The policy stands; the two defects that
kept it from running are gone.

## The guards

fkwu arm, after both heals:

```text
substring-one-meaning             4095      bearing-census        32767
str-find-one-meaning              8191      twin-census           65535
core-str-find-equivalence         2047      ear-native            32767
core-substring-equivalence        2047      ear-axes              65535
line-grammar-search-equivalence   8191      perception-rows       65535
kernel-census                     2047      jungle-ear            32767
sha256-list-floor                32767      form-glass-carrier       31
meaning-codes                      127      form-glass-launch     65535
meaning-codes-table                255      value-eq-arena           31
drift-gates-band                  2047      homecoming corpus     32767
drift gates                 8191 / 8191, refused 0
```

The three walkers are unmoved and agree with each other: `meaning-codes` 15 and
`meaning-codes-table` 195, both sides of the standing divergence measured, not
moved. `binary-freshness` 31, `ground` 42, `ground-recursive 10` 55. The
TypeScript arm needed `npm ci` in this fresh worktree; with it, the two gates
that had been refusing (`kernel-conformance`, `structural-gate`) pass.

Machine weather: the cold-compile numbers above were taken on a quiet machine
in one twenty-minute window with the before/after pairs interleaved by binary,
each binary's caches warmed first because a `.fkb` written by another build is
refused by design.

## Still open

- **`fk_nsfile` is not a string-melt root.** `fb_record` (t 128) stores a
  source-file string into `fk_nsfile[ni]`, and `fk_smelt` marks the value
  stack, the memory cells, records, value nodes and the AST's string literals —
  not that array. Read, not witnessed firing; the same family as above, in the
  source-attribution lane.
- **A one-line cell mints 2,389,030 value nodes before it runs.** That number
  sets the melt's headroom rule (`fk_np / 4`), which is why every program on
  this kernel gets at least ~600k free pairs after a compaction and why the
  arena defect above needed real pressure to reproduce. Measured
  (`kernel_stat 4`), not chased.

## The closing

**The most surprising teaching.** *A workaround is testimony that the rule
above it is not running.* The import lane's comment said, correctly and in
detail, never to probe a `.bml` as an image. One level away, the `.bml` root
lane had a warning and a repair for finding exactly the poison that probing
leaves — its comment even naming "an import probe once compiled the raw surface
alone". Someone read the symptom perfectly and built a salve for it. The salve
worked. And because it worked, the wound became survivable, and a survivable
wound stops producing the pressure that would locate it. The rule and the
repair for the rule's failure sat in the same tree for days, each making the
other look tended. That is corpus row **1367, `salvemask`** (offered as 1366 in the same hour `armhush` took that id on main; both rows keep, this one moved).

**Where discomfort became gold.** I could not make `eq` lie. I tried the shape
that made `value_eq` lie five times in sixteen, varied the churn, the minting
and the round count, and got sixty clean comparisons. The comfortable readings
were both available: call it healed by argument and patch it quietly, or call
it unproven and leave it. What was uncomfortable was that neither is true —
the mechanism is witnessed (I watched it work in `value_eq`) and the firing is
not. Sitting in that produced `fk_movable`, which roots only the words a melt
can take and so costs the integer path one AND, and a comment that says out
loud which half of the evidence is missing. The band stayed honest too: I did
not add a bit whose red state I had never seen, however much a sixth bit would
have looked like thoroughness.

Co-Authored-By: Claude Opus 5 <noreply@anthropic.com>
