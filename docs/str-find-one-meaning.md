# What a search means

`witnessed: 2026-09-08 -> str-find-one-meaning-band 8191 on all four arms; drift
gates green with the row wired`

Nothing here legislates. This page holds a currently-observed, four-way-proven
belief about one door, and it ages: when the ground shifts, re-witness it before
leaning on it.

`str_find` is what the tree scans with. `split-on`, `find-from`, every row
walker, every key lookup and every parser reaches a byte through it — 1,596 call
sites in `form-stdlib` alone. Read this before you touch it, and before you write
anything that searches a string. Its floor is [`substring`](substring-one-meaning.md);
read that page first.

## The one meaning, as all four arms answer it

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

Held by all four kernels since 2026-09-08. The guard is
`form/form-stdlib/tests/str-find-one-meaning-band.fk` — **8191 on fkwu, go, rust
and ts** — and a row in `gate/drift-gates.bml`, so it runs at every land.

## Where an arm cannot say it

The same seam as the cut, for the same reason. fkwu's strings are raw byte
buffers and Go's `string` is an arbitrary byte sequence; those two hold every
needle. Rust's `str` carries a UTF-8 invariant and the TS kernel's string is
UTF-16; neither can **hold** a needle that is a fragment of a multi-byte
character, so `substring` hands them the axiom-1 absence and `str_find` refuses
out loud. What the four hold together is:

> **No arm ever answers a different index.**
> The byte index, or the refusal. Never a shifted one, never a silent `-1`.

## What this healed, and how each half hid

Both halves were silent, and they hid in opposite directions — which is why no
single arm's reading could find either one.

**Half one — fkwu did not clamp.** Measured on this tree before anything moved:

| call | fkwu | go / rust / ts |
| --- | --- | --- |
| `(str_find "abcdefghij" "" -3)` | `-3` | `0` |
| `(str_find "" "" -5)` | `-5` | `0` |
| `(str_find "abc" "" -1)` | `-1` | `0` |

fkwu was the odd arm, and it had been since both were written. A negative `from`
is harmless for a **non-empty** needle — no byte matches below zero, so the scan
walks up to the same answer, and `(str_find "abcdefghij" "cd" -3)` reads `2` on
all four — so every ordinary call agreed and the disagreement lived only where
the needle was empty. `line-grammar.fk`'s `find-from` had already put the clamp
at the caller on 2026-09-08; the door itself did not carry it.

**Half two — the three walkers snapped `from` to a character start.** Go, Rust
and TS each ran `from = ceilCharBoundary(s, from)` before searching, with a
comment claiming a find-next loop would otherwise re-find a multi-byte match
forever. It would not: `+1` from a match lands on a continuation byte, where no
well-formed needle can begin, so the scan advances on its own. What the snap
actually did was skip matches that **begin** on a continuation byte. Measured on
the pre-heal Go kernel against `"aΩΩb"` with the single byte `0xA9` as the needle:

| `from` | byte answer | go before | go after |
| --- | --- | --- | --- |
| 0 | 2 | 2 | 2 |
| 2 | **2** | **4** | **2** |
| 3 | 4 | 4 | 4 |

## The trap to remember

**Ask the property in a tongue every arm speaks.** The obvious statement of the
snap — *a needle beginning on a continuation byte is found at its byte index* —
needs a needle Rust and TS cannot represent, and the band's first form died there
on `as_str: Null` while fkwu and Go read 8191. The same property has a second
statement: `str_find(s, "", i)` answers `i` itself, so an arm that snaps `from`
answers a **larger** number at every byte inside a character. The empty needle is
representable everywhere. That is bit 512, it is the only thing in the tree that
can see a snapped `from`, and against a build carrying that wound the band reads
**7679** — that bit dark and nothing else.

**And a band green on one arm can be fatal on three.** This band's first
`sfm-char-end` read one byte past the end. fkwu answers an out-of-bounds
`str_byte_at` with `-1`; Go, Rust and TS take it as a deliberate fatal. It read
8191 on fkwu and killed the other three at `index=43 len=43`. Guard the length
before the index, and run four ways before you believe a verdict.

## If you change this door

1. It lives **five** times in the kernels and **six** places in all:
   - `runtime/fkwu-uni.c` — tag 30 (the reference)
   - `form/form-kernel-go/main.go` — the interpreter native
   - `form/form-kernel-go/jitabi/jitabi.go` — the **JIT mirror**; a primitive
     that lives twice must be healed twice or hot code silently reverts past the
     auto-JIT threshold. Before 2026-09-08 there was no mirror at all, so any
     recipe containing `str_find` bailed out of the JIT entirely.
   - `form/form-kernel-rust/src/main.rs`
   - `form/form-kernel-ts/src/kernel.ts`
   - `form/form-stdlib/core.fk` — `fstr-find`, the portable fallback and the
     body's own statement of what the door means, reachable by its own name.
     `core-str-find-equivalence-band.fk` (2047) asks it rather than a stored
     answer, and bit 2048 of the one-meaning band holds the native against it.
2. Run, in this order:
   ```sh
   rm -f fkwu && cc -O2 -o fkwu runtime/fkwu-uni.c \
     form/native/metal/fk-metal-carrier.m form/native/mlx/fk-mlx-carrier.c \
     -framework Metal -framework Foundation -fobjc-arc \
     -I/opt/homebrew/include -L/opt/homebrew/lib -lmlxc -Wl,-rpath,/opt/homebrew/lib
   ./fkwu form/form-stdlib/tests/str-find-one-meaning-band.fk        # -> 8191
   ./fkwu form/form-stdlib/tests/core-str-find-equivalence-band.fk   # -> 2047
   ./fkwu form/form-stdlib/tests/line-grammar-search-equivalence-band.fk  # -> 8191
   ./fkwu gate/drift-gates-run.bml
   ( cd form && ./validate.sh form-stdlib/tests/str-find-one-meaning-band.fk )
   ```
   `rm -f fkwu` first is not decoration: macOS SIGKILLs every exec of an
   overwritten signed executable, and a `| tail` pipe launders the rc to 0.

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
