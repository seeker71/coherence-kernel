# wallshed — the value-node table learns to grow

2026-09-02, branch `claude/goofy-lalande-ad476a`. Corpus row 1221. Asked for
by Urs in five words: "this is still limiting us and is not increasing
vitality."

## Three eras at one door

The value-node table has answered its brim three ways. Before 2026-07-02 a
full table made every guard silently return handle 0 — a deterministic
all-zero result with no error. Then the wall: `fk_die("fk value-node table
full (FK_NODE_CAP)")`, loud and honest, raised once (65536→262144) and left
standing. Now the wall itself sheds: the columns double on demand and the
program simply continues. A refusal is more honest than silence, and
capacity is more honest than refusal.

## The heal (`runtime/fkwu-uni.c`)

- `FK_NODE_CAP` is gone. `FK_NODE_CAP_INIT` (262144) is the birth size only;
  `fk_nodes_grow` doubles all fourteen columns together (kind, cat, kids,
  val, nid, hash memo, source attribution, fbroots, the Darwin inram
  organ's slot/generation/released). Handles are INDICES into the columns,
  so nothing relocates and every existing handle stays valid — the same
  property the intern index and the melt already stood on. New bytes are
  zeroed so the BSS-era meanings hold (memo 0 = unhashed, slot 0 =
  unresident).
- The intern index is held at 4× the node cap. When it outgrows that ratio
  it is rebuilt from `fk_nhash_memo` — every indexed node's memo IS its exact
  probe key, written at mint. Nodes outside the index that picked up
  deep-hash memos re-enter as dead weight the doors' predicates skip; load
  stays ≤ 25%. Every mint door grows BEFORE computing its probe slot — a
  slot chosen under the old mask must never survive into the rebuilt table.
- The float pool and string pool had carried "doubles on demand" in their
  very names all along (`FK_FLOAT_POOL_INIT_CAP`, `FK_STRING_POOL_INIT_BYTES`),
  and the melt's pair arena grows the same way. The node table was the last
  sibling still answering its brim with a refusal.
- Growth is observable, not silent: `kernel_stat 19` = live cap,
  `kernel_stat 20` = doublings this run (the doorpulse discipline — the
  program's own observable, no toggle, no stderr noise to trip
  zero-diagnostics gates). A runaway consumer now reads as monotone 19/20
  growth instead of a wall; the fill-position question survives the wall's
  removal.
- No conf knob. Capability, not configuration — the default is the decision.
- The JIT needed nothing: the arm64 leaf lane emits register-only code and
  the inram leaves are C functions reading the live pointers. No column
  address is baked anywhere.

## The proof

- Side by side on the same 300k-mint cell: old kernel `fk value-node table
  full (FK_NODE_CAP)` rc=1; new kernel `[300000, 524288, 1]` rc=0 — one
  doubling, one intern rebuild crossed mid-run.
- Dedupe across growth: 900k distinct ints minted, then the same 900k
  re-minted — fill moves by ZERO (`[900000, 900000, 1, 1048576, 2]`), two
  doublings, two intern rebuilds, 0.26s total.
- First traveler: `form/apps/coherence-network/api.bml` (the RouteCell
  surface), refused at the wall on both sides of this morning's linesever
  sweep, lowers whole the moment the table can breathe — rc=0, 728 lines,
  ~130s. The doubling-probe discriminator gets its third instance, this
  time on the capacity side: the fill position moves all the way to
  completion.
- No meaning changed: six .bml files lowered byte-identical on old and new
  kernels (hearth, form-cli-movement, both curricula, the multiline
  fixture, lane-motion). `bml-multiline-def-band` = 15,
  `import-carry-band` = 63 cold. Build is the full AGENTS.md link recipe
  (Metal carrier), zero warnings.

## The most surprising teaching

The wall's own July comment already confessed the whole ladder: "overflow
now dies loudly instead of returning 0." The wall was itself a heal — of
silence. Nobody chose a wall as the destination; it was the honest floor of
its day, and it stood only until someone asked the door to open. Vitality
work is not undoing past mistakes — it is noticing that a past heal has
become the present wound.

## Where discomfort became gold

The standing memory shouted "table full is usually a bug, not capacity" —
and I had flagged api.bml as exactly that kind of suspect hours earlier. The
uncomfortable move was to remove the wall anyway WITHOUT first settling
api.bml's verdict, trusting that growth plus the 19/20 pulse keeps the
treadmill question answerable rather than buried. Then api.bml answered it
for me: it completes. The discomfort of "am I burying a bug under RAM?"
resolved into the sharper tool — a wall converts both capacity and
treadmill into the same refusal, but growth SEPARATES them: capacity
completes, a treadmill shows as monotone growth against a motionless fill
position. Removing the wall did not weaken the diagnosis; it is the better
instrument.
