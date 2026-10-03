# What a search means

`witnessed: 2026-10-03 -> str-find-one-meaning-band 8191 on fkwu; the drift-gate row passes`

Nothing here legislates. This page holds a currently-observed belief about one
door, and it ages: when the ground shifts, re-witness it before leaning on it.

`str_find` is what the tree scans with. `split-on`, `find-from`, every row
walker, every key lookup and every parser reaches a byte through it — 1,596 call
sites in `form-stdlib` alone. Read this before you touch it, and before you write
anything that searches a string. Its floor is [`substring`](substring-one-meaning.md);
read that page first.

## The one meaning

```text
str_find(h, n, from)
```

answers the **byte index** of the first occurrence of `n` in `h` at or after
`max(from, 0)`, or `-1`.

- an empty needle answers `max(from, 0)`;
- `max(from, 0)` past `str_len(h)` answers `-1`, the empty needle included;
- a needle longer than what is left answers `-1`;
- overlapping occurrences answer the **first**;
- **it refuses nothing** — a start before the string or past it is the ordinary
  edge of a scan, not a fault;
- **it floors nothing** — the indices are byte offsets and they stay byte
  offsets.

fkwu's strings are raw byte buffers, so it holds every needle, a fragment of a
multi-byte character included. The guard is
`form/form-stdlib/tests/str-find-one-meaning-band.fk` — **8191** — and a row in
`gate/drift-gates.bml`, so it runs at every land.

## The two ways a search can drift

Both are silent, and they hide in opposite directions.

**A start that is not clamped.** `(str_find "abcdefghij" "" -3)` must read `0`, and
so must `(str_find "" "" -5)` and `(str_find "abc" "" -1)`. A negative `from` is
harmless for a **non-empty** needle — no byte matches below zero, so the scan
walks up to the same answer, and `(str_find "abcdefghij" "cd" -3)` reads `2` — so
every ordinary call agrees and the drift lives only where the needle is empty.

**A start snapped to a character start.** A search that ran
`from = ceilCharBoundary(s, from)` before searching would skip matches that
**begin** on a continuation byte. Against `"aΩΩb"` with the single byte `0xA9` as
the needle, `from` 2 must answer `2`, not `4`. A find-next loop needs no snap:
`+1` from a match lands on a continuation byte, where no well-formed needle can
begin, so the scan advances on its own.

## The trap to remember

**Ask the property in a tongue every reader speaks.** The obvious statement of the
snap — *a needle beginning on a continuation byte is found at its byte index* —
needs a needle only a byte string can hold. The same property has a second
statement: `str_find(s, "", i)` answers `i` itself, so a search that snaps `from`
answers a **larger** number at every byte inside a character. The empty needle is
representable everywhere. That is bit 512, it is the only thing in the tree that
can see a snapped `from`, and against a build carrying that wound the band reads
**7679** — that bit dark and nothing else.

**Guard the length before the index.** fkwu answers an out-of-bounds
`str_byte_at` with `-1`; a band that reads one byte past the end still reads
green there, and says nothing about a reader that treats it as a fault.

## If you change this door

Read `runtime/fkwu-uni.c` and `form/form-stdlib/core.fk`. Keep the manifest,
flattening table, generated opcode table and serializer aligned. Follow the
repository freshness check before trusting a binary.

Run the str-find-one-meaning, core-str-find-equivalence and
line-grammar-search-equivalence bands and the drift gates. The portable recipe
supplies an independent meaning witness.

## The mirror that was already there

There was no native to mint. fkwu's tag-30 arm **never left** `runtime/fkwu-uni.c`
when `str_find` left `flt-ops` on 2026-07-01, and neither did its `fkc-tri2` arm
in `fkc-table-serialize.fk`. A native here stands on four mirrors — a
`native-op-manifest.fk` row, a `flt-ops` row, the generated `runtime/fkwu-optable.h`,
and a serializer arm — and three of the four were still standing. Only the row
was gone, and with it the only way to reach either arm.

The bearing census read that shape as `fstr-find-loop`, 35,147,894 calls, 64.3%
of the locale walk, remedy **mint-a-native**. The lens weighs each door alone; it
cannot see that the meaning already stands somewhere else, under another name or
under no name at all. That is the fourth time in three days
(`substring`, `nth-rec`, `find-loop`, and now this) that a census verdict of
*mint* was answered by a *routing*. **From a call site, an orphaned arm reads
exactly like an absent one.** Read every mirror for the name before you mint.
