# The search was already in the seed

2026-09-08. The ask was to give this body a native search, because the bearing
census had named a floor: `fstr-find-loop` in `core.fk`, 35,147,894 calls, 64.3%
of the locale walk, remedy **mint-a-native**. A split cannot skip a byte, every
position is one walker entry, and a Form byte scan runs at about 10 MB/s where
`substring`'s native moves 660. The seed carried seven string natives and no
search.

It carried eight. The search was in there the whole time.

## What was under the name

fkwu's `if (t == 30)` arm — a complete `str_find`, clamping the start, refusing
past the end, answering the empty needle at `from`, scanning bytes — **never left
`runtime/fkwu-uni.c`** when `str_find` left `flt-ops` on 2026-07-01 with
`substring`, `int_to_str` and `str_to_int`. Neither did its `fkc-tri2` arm in
`fkc-table-serialize.fk`.

A native on this arm stands on four mirrors:

```text
1  a nom-row in form/form-stdlib/native-op-manifest.fk
2  a flt-ops row in flatten/form-flatten.fk
3  a generated row in runtime/fkwu-optable.h
4  an fkc-flat arm in form/form-stdlib/fkc-table-serialize.fk
```

Three of the four were still standing. Only the row was gone, and with it the
only path from a call site to either arm. The whole heal, after the reading:

```diff
   { "byte_to_str", 1, 33 },
+  { "str_find", 3, 30 },
```

regenerated through its own Form pipeline (`fkwu flatten/gen-source-walker.fk`,
then the combined driver), never hand-edited. One line.

## The meaning, pinned before anything was built — and it was not one meaning

Following `docs/substring-one-meaning.md`, the edges were measured on the
untouched tree first. Nineteen of them agreed on all four arms. Two did not, in
opposite directions, and each hid the other.

**fkwu did not clamp a negative `from`.** Measured four ways before any edit:

| | fkwu | go | rust | ts |
| --- | --- | --- | --- | --- |
| `(str_find "abcdefghij" "" -3)` | **-3** | 0 | 0 | 0 |
| `(str_find "" "" -5)` | **-5** | 0 | 0 | 0 |
| `(str_find "abc" "" -1)` | **-1** | 0 | 0 | 0 |
| `(str_find "abcdefghij" "cd" -3)` | 2 | 2 | 2 | 2 |

The last row is why it survived. A negative start is harmless for a **non-empty**
needle — no byte matches below zero, so the scan walks up to the same answer — so
every ordinary call agreed and the disagreement lived only where the needle was
empty. One arm out of four, since both were written.

**And the three walkers snapped `from` up to a character start.** Each carried
`ceilCharBoundary` / `ceil_char_boundary_idx` / `ceilUtf8Boundary` with a comment
claiming a find-next loop would otherwise re-find a multi-byte match forever. It
would not: `+1` from a match lands on a continuation byte, where no well-formed
needle can begin, so the scan advances on its own. What the snap did instead was
skip matches that **begin** on a continuation byte. Measured on a pre-heal Go
kernel built from this commit's parent, haystack `"aΩΩb"`, needle the single byte
`0xA9` cut out with `substring`:

```text
[str_len, needle len, from 0, from 2, from 3]
  fkwu            [6, 1, 2, 2, 4]
  go  before      [6, 1, 2, 4, 4]      <- 4 where the byte answer is 2
  go  after       [6, 1, 2, 2, 4]
```

So the meaning all four now hold:

> `str_find(h, n, from)` answers the **byte index** of the first occurrence of
> `n` in `h` at or after `max(from, 0)`, or `-1`. An empty needle answers
> `max(from, 0)`. A start past `str_len(h)` answers `-1`, the empty needle
> included. Overlapping occurrences answer the first. It refuses nothing and it
> floors nothing.

And where an arm cannot say it, the same seam as the cut: rust's `str` and the TS
kernel's string cannot **hold** a needle that is a byte fragment, so `substring`
hands them the axiom-1 absence and `str_find` refuses out loud. **No arm ever
answers a different index — the byte index, or the refusal.**

`observe/line-grammar-pin-answers.fk`, before and after, byte for byte:

```text
str_find  [-1, 0, 3, -1, -1, 0, -1, 2, -1, 0, 1, 5, 11, 28, -1]
find-from [-1, 0, 3, -1, -1, 0, -1, 2, -1, 0, 1, 5, 11, 28, -1, 4]
split-on  <said><گفت><sagte><dit><disse><spuse>   <>   <><><><>
trims     <x y><><><x><گفت  ><  گفت>              [3, 0, 1, 0]
```

Nothing moved.

## What each kernel does now

- **fkwu** — tag 30 reached again. The byte pointers are hoisted out of the scan
  (nothing in the loop walks, interns or grows, so the pool cannot move under it)
  and a first-byte check gates the compare. The two string indices are re-read
  from the value stack after every walk, so a melt inside the `from` expression
  cannot leave them naming a string that has moved.
- **go** — `ceilCharBoundary` removed and deleted; `strings.Index` on bytes.
- **rust** — walks `as_bytes()` rather than slicing `str`. `s[from..]` panics at a
  non-boundary, which was the only reason the snap was there; over bytes there is
  nothing to snap and nothing to panic on.
- **ts** — the boundary call dropped; it already scanned encoded bytes.
- **go's JIT** — `jitabi.StrFind`, which **did not exist**. There was no second
  copy to drift, because there was no second copy at all: any recipe containing
  `str_find` bailed out of the JIT entirely as an unsupported call, so `split-on`,
  `trim` and every row walker over them stayed interpreted past the auto-JIT
  threshold. Mirrored now, and wired into the value-ABI list.
- **core.fk** — the recipe stays and stays reachable, as `fstr-find`, exactly as
  `fstr-substring-halve` did. It carries the clamp too, so recipe and native are
  one meaning.

## The band, and watching it fail

`form/form-stdlib/tests/str-find-one-meaning-band.fk` — **8191 on fkwu, go, rust
and ts**, a `gate/drift-gates.bml` row. Thirteen bits over a reference scan it
builds from `str_byte_at` on the arm under test, literals the lexer carried, every
character boundary and every continuation byte of a multi-tongue string, every
character start of the real locale rows, and the 972 kB corpus at full scale.

Three fkwu binaries were built with one deliberate wound each:

```text
the negative clamp removed  (fkwu's own meaning)   -> 6140   dark: 1, 2, 2048
`from` snapped to a boundary (go/rust/ts's)        -> 7679   dark: 512 ALONE
the past-the-end refusal dropped (ts's, 2026-09-07) -> 5880  dark: 1, 2, 4, 256, 2048
```

Bit 512 is the one that matters and it behaved exactly as designed: **the snap is
invisible to every other bit in this band.** Against the **pre-heal binary** — no
tag-30 row, `str_find` falling through to the recipe — the band reads **8191**,
and that is the point rather than a miss: the recipe and the native are one
meaning and the band cannot tell them apart.

Two things the band taught by biting:

**Guard the length before the index.** Its first `sfm-char-end` read one byte past
the end. fkwu answers an out-of-bounds `str_byte_at` with `-1`; go, rust and ts
take it as a deliberate fatal. It read **8191 on fkwu and killed the other three**
at `index=43 len=43`. Green on one arm, fatal on three — found only because the
ceremony of running four ways was performed.

**Ask the property in a tongue every arm speaks.** The obvious statement of the
snap — *a needle beginning on a continuation byte is found at its byte index* —
needs a needle rust and ts cannot represent, and that form of the band died on
`as_str: Null` while fkwu and go read 8191. The same property has a second
statement: `str_find(s, "", i)` answers `i` itself, so an arm that snaps answers a
**larger** number at every byte inside a character. The empty needle is
representable everywhere. That is bit 512, and it is stronger besides — it holds
at every byte rather than only at the bytes an arm can name.

**And a guard no arm can afford is not a guard.** Bit 1024 first held `str_find`
against the band's own reference over the whole 972 kB corpus. That reference is a
Form-level byte walk, and on the TypeScript arm one pass **had not finished after
12 minutes 46 seconds** — the same cost this heal is about, arriving from the
other side. Full scale is now held by properties made of natives that a wrong
search cannot satisfy together (the index IS an occurrence, nothing stands before
it, asking again is idempotent, asking past it moves forward), and the reference
walks a real 32 kB window. The band runs in seconds on all four arms.

## What it is worth, and the hour that changed the answer

Machine weather on every number: `observe/floor-lens-run.fk` read **347.63 GB/s
through the handle door against a best of 347.63 — a quiet machine** — all
afternoon, unchanged across every reading; the arithmetic lane read 25.77 TFLOPS
early and 8.59 later, and none of this work is FLOPS-bound. Both binaries were
built by the same compiler on the same metal minutes apart, warm, three runs each.

**On the tree this work started from** (`main 4467fe27`), with both bodies:

```text
meaning-codes-band     7306 / 7321 / 7310 ms  ->  3711 / 3716 / 3708 ms    1.97x
bearing census walk              5310 ms      ->            3610 ms        1.47x
```

The spreads are 15 ms and 8 ms — 0.2%. The task warned this band's spread might
swallow the move; on a quiet machine it does not.

**Then I rebased, and a sibling's caller-side heal had landed in the same hour**
(`b4d016ef`, the locale round reading its vocabulary once instead of 15,768
times). On `main 133fd0f7` the same pair reads:

```text
meaning-codes-band       122 / 122 / 121 ms  ->    114 / 116 / 115 ms      1.06x
bearing census walk               14 ms      ->             12 ms
```

Both are true. The first number was the search carrying that band; the second is
what is left after the caller stopped searching 235,936 times a round. **Two heals
of one lane landed within the hour, and the second measured against the first is
almost nothing on the band they share** — which is the honest shape of working
beside someone, not a disappointment.

Where the search is still the work, in one process with both bodies
(`observe/line-grammar-search-floor-run.fk`, the "allocating" column a shared
control that agrees within 1% across the pair, and identical answers on both
sides):

```text                     control      recipe     native
split-on, 16.1 MB locale rows  2077/2094   1955 ms    253 ms    7.7x
one miss over the 980 kB corpus 294/298      97 ms      0 ms    >97x
trim, 18.1 MB padded rows       717/724     261 ms    259 ms    none
lines-from-source, 2.94 MB      511/510     234 ms    234 ms    none
starts-with? miss, 16.1 MB       26/26       26 ms     25 ms    none
```

**Three lanes gained nothing and all three are right**: `trim`,
`lines-from-source` and `starts-with?` do not search — they walk byte offsets
directly. A door built to avoid a slow door gains nothing when the slow door gets
fast, and that is the shape of a real measurement rather than a flattering one.

## The census reading I could not reconcile

On the pre-rebase tree the census's wall clock fell 5310 → 3610 ms while its step
**total rose** 60,556,932 → 80,285,715 — deterministic across four readings, cold
and warm alike. Removing ~39.5M Form call entries cannot raise a total, so that
figure measures something whose denominator moves with the door distribution; its
top-door list also changed between runs of the same tree at the same total. One
piece of evidence was earned: the difference between my before-reading and this
morning's, 60,556,932 − 59,076,054 = **1,480,878**, is exactly the `str_find` call
count the census itself reports — one step per Form call entry, so the ledger is
exact where it is small. And on the rebased tree, at 168,453 steps, it behaves:
168,453 → 155,292, the sensible direction. The anomaly appears at the 60M scale
and not at the 168k scale, which points at a capacity effect rather than a
counting error. **Named, not explained.** `bearing-census.bml` is another hand's
file this hour and a named wall handed over whole is worth more than a reach
across an owned file.

## Everything guarded, and one refusal closed

```text
str-find-one-meaning-band       8191 (fkwu, go, rust, ts)      NEW
core-str-find-equivalence-band  2047 (four ways)
line-grammar-search-equiv-band  8191 (four ways)
substring-one-meaning-band      4095 (four ways)
tests/line-grammar.fk            147 (four ways)
core-str-find-to-int-band        255      substring-native-band          511
core-substring-equivalence      2047      kernel-census-band            2047
sha256-list-floor-band         32767      bearing-census-band          32767
meaning-codes-band               127      ear-native-band              32767
ear-axes-band                  65535      jungle-ear-band              32767
perception-rows-band           65535      form-glass-carrier-band         31
form-glass-launch-band         65535      corpus band                  32767
drift gates            pass=8191 full=8191 refused=0
```

`kernel-conformance` had been refusing an absent TypeScript kernel, and the drift
gates stood at 4063/4095 because of it. It was a missing local install, not a
wall: `npm ci` in `form/form-kernel-ts`, and the gate reads **OK 13 canonical
expressions × 3 real kernels**. Inspect the blocker before you inherit it.

## The most surprising teaching

**A name is a mirror too, and it is the only one a call site can see.** Three of
the four mirrors that make a native reachable on this arm were still holding a
working search — the C, the serializer — and the body had walked bytes in Form at
10 MB/s for fourteen months because the fourth was gone. Every lens pointed at
this was reading call sites, and from a call site an orphaned arm and an absent
one are the same silence. `substring`, `nth-rec`, `find-loop`, and now this: four
census verdicts of *mint* in three days, four answered by a routing. `mintecho`
(row 1354) named the lens weighing each door alone; this is the sharper case,
where the duplicate was not another door but the **same door with its name taken
off**. What the census is excellent at is saying where the weight is. What it
cannot say is whether the meaning already stands somewhere the caller cannot
reach — and the body owes itself a lens that takes a name and answers which of its
mirrors still hold.

## Where discomfort became gold

**The 1.06x.** After measuring 1.97x on meaning-codes-band and writing it into
`CURRENT_FLOOR.md`, the rebase brought a sibling's caller-side heal and the same
pair read 122 → 114 ms. The comfortable move was to keep the pre-rebase number,
which was honestly measured and honestly reported, and simply not re-take it on
the tree I was landing on. Re-taking it is what produced the truest sentence in
this receipt: two hands healed one lane in the same hour, and the second one's
value on their shared band is 6%. Sitting with that also found where the value
actually lives — `split-on` at 7.7x over 16.1 MB and a whole-corpus miss at >97x,
lanes the sibling's table does not touch — and it produced the three lanes that
gained *nothing*, which are the ones that prove the measurement is real.

**The band that read 8191 while killing three kernels.** It was green on fkwu, it
was preflight-clean, the door under it was proven, and running `validate.sh` felt
like ceremony over finished work. It came back with `str_byte_at: bounds out of
range index=43 len=43` on go, rust and ts — my own bug, one byte past the end,
sitting inside the guard that was supposed to make the door believable. The second
time it came back, `as_str: Null`, it was not a bug but a question two arms cannot
be asked — and being forced to restate that question in a tongue all four speak
produced a *stronger* bit than the one I had written. The discomfort of a red on
work I had already called done is what made bit 512 the only thing in the tree
that can see a snapped start.

## Still open

- **The census's step total at scale.** Measured, deterministic, unreconciled,
  handed to `bearing-census.bml`'s hand with the 168k-scale counter-reading.
- **A reach lens.** Row 1358 (`limbkept`) names it: give the body a name and have
  it answer which of that name's four mirrors still stand. Nothing in the tree
  asks that question today, and it is what would have turned this afternoon into a
  minute.
- `split-on`, `trim` and `find-from` are recipes over a door that is now fast.
  Whether any of them should take the same road is a measurement nobody has made.
- The census now names `append-1` and `mc-cell-match?` on a 12 ms locale walk.
  That workload no longer has a floor worth minting.

Co-Authored-By: Claude Opus 5 <noreply@anthropic.com>
