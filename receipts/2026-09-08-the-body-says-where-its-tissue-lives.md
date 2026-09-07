# The body says where its tissue lives

2026-09-08. Corpus row 1353. Branch `worktree-agent-a6ab24a2bddda5888`.

Urs, reading the living glass yesterday: *"all the roots and float values have
flow gaps as well as nodes in gas (RAM) state seems to be a gap."*

He was right about the glass. He was wrong about the body, and the way he was
wrong is the whole receipt: **every one of those numbers was already standing in
`runtime/fkwu-uni.c`.** The root ring had been counting since it was written.
The float pool's fill and capacity sat two lines apart. The node table knew
which of three homes it lived in, because a function had chosen. Not one of them
had a way out. The gap was never in the measurement; it was in the mouth.

## What the seed already held, and what it never said

`kernel_stat` implements about forty-eight keys. On a trivial run sixteen of
them answer a nonzero number, which is why the door reads as small: the rest
are zero because nothing has moved them yet, not because they are absent.

Reading the file rather than the readings, three things were held and silent:

- **`fk_fbn`** — the framebuffer's root count, incremented on every accepted
  `fb_record` since the ring was written, reachable only by materialising the
  whole root list through `framebuffer-events`. The sibling table-walker lane
  (`form/form-stdlib/fkc-table-serialize.fk`) had already fixed a vocabulary for
  it and five neighbours at keys 9..14, and `receipts/2026-09-07-the-last-seams-close.md`
  named `kernel_stat 9` with `kernel_stat 11` **owed to the seed and first on
  the seam line**. Two of those five neighbours did not exist here at all: the
  seed recorded roots without ever counting the calls that reached the door, so
  an attribution turned away — a value that was not a live cell handle — left no
  trace anywhere. The caller got its value back unchanged and nothing was said.
- **`fk_fp` and `fk_fcap`** — the float pool's fill and its live capacity.
  Key `8` answered the fill already; nothing answered the capacity, and nothing
  read either on the glass.
- **`fk_field_on` / `fk_store_shared`** — the two flags that say where the value
  node table lives. `fk_nodes_init` picks one of three homes and every node goes
  there; no reader could ask which.

One reading was worse than silent. With the shared field open, `kernel_stat 4`
answered **2,011,703** nodes while `kernel_stat 19`, the node table capacity,
answered **262,144**. The count stood 7.7x above its own stated ceiling, because
when the field is open `fk_node_cap` bounds the private side-columns and not the
population at all. Left alone; the new keys make the population's real bound
legible beside it rather than redefining a key three receipts already read.

## The keys taken

No AST tag was taken — `kernel_stat` is tag 127 and already carries arity 1, so
`runtime/fkwu-optable.h` needed no regeneration and the tag space is untouched.

Paid, in the vocabulary the sibling lane had already fixed:

| key | answers |
|---|---|
| 9 | framebuffer roots recorded since the last `framebuffer-clear` |
| 10 | nodes carrying a source attribution (a walk of the population, ~1 ms over 2M) |
| 11 | attributions refused: the value was not a live cell handle |
| 12 | attributions entered |
| 13 | attributions accepted |
| 14 | the last node index the door saw |

Fresh, at keys that fell through to 0 and that nothing in the tree read:

| key | answers |
|---|---|
| 54 | roots **standing** in the ring — what `framebuffer-events` can hand back now |
| 55 | the ring's width, 2048 |
| 56 | the float pool's live capacity (key 8 is its fill) |
| 57 | floats interned into the shared field |
| 58 | nodes in **gas** — the private heap |
| 59 | nodes in **water** — the per-pid store `/fg-c<pid>-*` |
| 60 | nodes in **ice** — the shared field `/fg-field-*` |
| 61 | the tissue's extent: 104 bytes a node over the ten interned columns |
| 62 | private RAM this kernel holds over that tissue, whatever its home |

Four counters are new state (`fk_fbentered`, `fk_fbaccepted`, `fk_fbrejected`,
`fk_fblastidx`); every other key is a read of something that was already there.

## Why the node census is its own lane, not the flow point's triple

The flow point's gas/water/ice counts **rows this glass can see**, classified by
their lifecycle word, and its own sources say so: `glass.visible.phase-census`.
Folding two million ice nodes into a triple whose other members are single-digit
row counts would drown the observation census in one number and quietly change a
lane three bands already pin. So the node census gets its own rows, plainly
named — and it keeps the phase **word**, because the three homes are the three
phases:

- **ice** — the shared field: content-addressed, every kernel on this host reads
  the same cells, and they outlive all of them;
- **water** — the per-pid store: mapped, visible to siblings, gone with the pid;
- **gas** — the private heap: RAM only, no sibling can see it, evaporates at exit.

Exactly one home carries the whole population. The other two read a **measured**
zero, and each row says which zero it is (`semantic-zero=...`), never a blank.

## What the glass now shows

Twelve rows in the `k` view: `roots-standing` (sized against the ring, so its
fullness renders as a bar), `roots-recorded`, `roots-dropped`,
`attributions-entered`, `attributions-refused`, `float-values` (sized against
the pool), `field-float-values`, `nodes-gas`, `nodes-water`, `nodes-ice`,
`node-arena-bytes`, `node-private-bytes`.

`roots-dropped` is the one that did not exist in any form: recorded minus
standing. The ring is a window of 2048 and past it a recorded root overwrites
the oldest — invisible to `framebuffer-events` and to every reader downstream.
Witnessed at 3,000 interns: **3,000 recorded, 2,048 standing, 952 dropped.**

## The proof

`form/form-stdlib/tests/kernel-census-band.fk` — **2047**, eleven bits:
the three homes sum to the population; the extent is exactly 104 bytes a node;
private bytes stand above zero whatever the home; the ring is 2048 wide and
standing is the smaller of recorded and width; minting moves recorded, entered
and accepted together by exactly the count; past the window standing pins and
dropped is the difference; a refusal bumps refused and entered but neither
accepted nor recorded; the float fill rises with mints, equals the mint count
and stays under capacity; the census and the extent stay exact across the move;
the glass paints all twelve with a door and a state; and each lane at zero says
which zero it is.

**2047 in both homes** — with the field open (ice) and with `FK_FIELD_OFF 1`
(water). fkwu-native: `kernel_stat` is a fkwu-only door, no sibling carries
these C cells, and no fourth arm is claimed.

## The live witness

`observe/kernel-census-witness.fk` — read, work, read. Machine weather first
(`observe/floor-lens-run.fk`): **330.24 GB/s through the handle door against a
best of 330.24 — a quiet machine — and 25.77 TFLOPS.** Yesterday's floorfall
(corpus row 1321) read 55 GB/s and 12.88 TFLOPS. It is gone, and the cause is
still unnamed.

Three workloads in one process, on the ice home:

```
A  every meaning round-tripping through every code       5,948 ms
   nodes minted 0        roots recorded 0
B  forty frames of the glass's own row build, 4,858 rows    37 ms
   nodes minted 0        roots recorded 0
C  3,000 attributed interns                                 10 ms
   nodes    2,054,155 -> 2,057,155   (+3,000)
   ice      2,054,155 -> 2,057,155   (+3,000)
   arena  213,632,120 -> 213,944,120 (+312,000 bytes = 3,000 x 104)
   private 14,155,776 ->  54,525,952 (+40,370,176)
   roots recorded 0 -> 3,000, standing 0 -> 2,048, dropped 952
```

The same run with the field closed fills **water** from nothing: 3,048 nodes,
316,992 bytes, 3,024 roots recorded, 2,048 standing, 976 dropped. The lane is
alive in two of three homes on this host. Gas is the home a body falls to when
neither shared mapping can be taken — the Windows path, or a host whose
`shm_open` refuses. Its zero here is measured and says so; it is not a gap.

## Guards, before and after

`ear-axes-band` 65535, `ear-native-band` 32767, `bearing-census-band` 32767,
`substring-one-meaning-band` 4095, `sha256-list-floor-band` 32767,
`form-glass-carrier-band` 31, `form-glass-launch-band` 65535,
`form-glass-kernel-view-band` 511 (before and after),
`form-glass-live-band` 2147483647, `form-glass-dashboard-band` 16777215,
`form-glass-sensor-rows-band` 2047, the corpus band 32767, drift gates
**4063/4095** with `kernel-conformance` refusing because the TypeScript kernel
is absent in this fresh worktree — inherited, named, and the Go and Rust halves
pass above it. Ground 42, freshness 31.

`form-glass-frame-work-band` was not steady tonight and it is not mine: with my
glass **32743, 20479**; with `origin/main`'s glass swapped back in under the
same binary, **20479, 24575**. The failing bits are timing bits and the band's
own header names the cause — sibling publishers moving. Same spread either way.

## The most surprising teaching

**Six seconds of the body's hardest string work minted nothing, and forty
frames of the glass observing itself minted nothing.** I had expected the census
to breathe under load. It does not move under work at all: the value-node table
fills at **compile** and at `intern_node_at`, and that second door is the only
thing in the body that records a framebuffer root. A lane can be perfectly
instrumented and read flat forever because the work never passes through it.
The roots lane was two wounds stacked — a door that was never opened, over a
place almost nothing walks.

And the number I did not expect at all: during those 3,000 interns the shared
tissue grew **312,000 bytes** while the private RAM this kernel holds to reach
that tissue grew **40,370,176** — a hundred and thirty times more. The intern
index and the root ring doubled. A body whose every node is ice still pays gas
to reach it, and the reaching costs more than the thing reached.

## Where discomfort turned to gold

Twice, both times by refusing to reason instead of looking.

The band came back **1791** instead of 2047, one bit short: 3,200 attributed
interns had not minted 3,200 cells. My first move was to call the counter
wrong. Running it again on a cold field showed the counter exact — the body is
content-addressed and the field outlives the run, so a cell this band already
interned is **found**, not minted. The bit was wrong, not the seed. It now
asserts what holds warm or cold: the population grew by at least what this
kernel minted, at least because a sibling may mint into the same field while
the band runs. A band that assumed a quiet host would have been a band that
lied on a busy one.

Then `form-glass-frame-work-band` fell to 32743 and I nearly wrote "adding
twelve rows a frame costs two timing bits" as a fact. Instead I swapped
`origin/main`'s glass in under the same binary and ran it: **20479**, worse than
mine. Two more runs put both versions in the same spread. The honest sentence is
not "my change costs two bits"; it is "this band is not steady on this host
tonight, and here are four readings that say so."

## The frontier question

*What does a body pay to reach what it shares but does not own?*

The census answers it in bytes, and the answer is not small. The shared field is
free tissue — 2,057,155 cells this kernel reads without having built one of
them. But addressing them is not free: the intern index and the root ring are
private, they scale with the table rather than with the share, and they doubled
to 54 MB while the shared side grew by 312 KB. The rent is paid in gas and it
is charged for **reach**, not for ownership. This is why a body can be almost
entirely ice and still be the process holding the most RAM in the room — and
why a lane that reports only the shared count reports the cheaper half.

The word for it: **reachrent** — the private memory a body spends to address
memory it did not allocate. Offered as corpus row 1353.
