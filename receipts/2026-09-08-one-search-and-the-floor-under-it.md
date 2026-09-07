# One search, and the floor under it

2026-09-08. The bearing census handed over a fourth name and this is what was
under it. The first three names it gave — `substring`, then `nth-rec`, then
`find-loop` — were each a door that could be healed. The fourth is a floor.

## What the lens said, on this tree, twice

```text
bearing census — the locale-row walk, 5896 ms, 40 doors of 99.3% of the walking (60064908 steps)
  the next substring: find-loop
  drag 34464  find-loop — 57.4% of the walking, 34264886 calls, 5 doors lean on it, mint-a-native
  drag 29885  find-from — 59.7% of the walking, 1387730 calls
  drag 26084  split-on-loop — 65.2%, 1387730 calls
  drag 19683  split-on — 65.6%, 235936 calls
```

Two readings before the change, 5896 and 5945 ms, agreeing to the step at
60,064,908. The step count is deterministic and the milliseconds are not, which
is the census earning its design on a host whose machine-wide fall is still
unexplained (row 1321).

The call counts read the whole shape without an investigation: 1,387,730
`find-from` calls and 1,387,730 `split-on-loop` calls means **every** search in
this workload comes from a split, 235,936 splits at about 5.9 pieces and 145
bytes each, and 34.26M positions is that arithmetic. The rows are the locale
files; the caller is `meaning-codes.bml` answering 238,856 key lookups.

## What `find-loop` was

```form
(defn find-loop (s needle i)
    (if (gt (add i (str_len needle)) (str_len s)) (sub 0 1)
        (if (str_eq (substring s i (add i (str_len needle))) needle) i
            (find-loop s needle (add i 1)))))
```

A cut at every offset, compared and thrown away, with both lengths re-measured
per position. `core.fk` had already been healed off exactly this shape — its
header says so, and names the degradation it caused — but this second copy kept
the old cost, under `split-on`, under `native-edit`, under `sh-bi-grep`, under
the thirty-odd units that prelude `line-grammar.fk`.

So the heal is not a faster search. It is **one** search:

```form
(defn find-from (s needle start)
    (str_find s needle (if (lt start 0) 0 start)))
```

The clamp is the old behaviour written down. A negative `start` used to scan
harmlessly toward zero — the cut clamped, the lengths never matched, nothing can
match before 0 — so `find-from s n -3` answered as `0` did. `str_find` would
instead hand `str_byte_at` a negative index, which fkwu answers and which
form-kernel-go and form-kernel-rust take as fatal by design. The clamp lives
where the callers are.

The same file's `trim`, `trim-leading-ws`, `trim-trailing-ws` and `lines-loop`
had the same disease in a different room: each cut a fresh string per byte —
`s[1..]` per leading space, `s[..len-1]` per trailing space, a one-byte cut per
byte of a file to find its newlines, then a second walker frame through `ord` to
read that byte back out. They walk byte offsets now and make at most one cut.

## Measured with both bodies in one process

A wall-clock reading on this host today says more about the machine than about
the change. Two runs of one cell cannot subtract that; two loops inside one
process can — whatever the machine is doing, it is doing to both.
`observe/line-grammar-search-floor-run.fk`:

```text
split-on over the locale rows   allocating 2100 ms   routed 1895 ms   16.1 MB
trim over padded locale rows    allocating  725 ms   routed  263 ms   18.1 MB
one miss over the whole corpus  allocating  297 ms   routed   97 ms   972 kB
lines-from-source, whole corpus allocating  513 ms   routed  235 ms   2.92 MB
starts-with? miss on real rows  allocating   25 ms   routed   26 ms   16.1 MB
  same answers: lines 44223 / 44223   splits 1062000 / 1062000
                trims 16122000 / 16122000   scan -1 / -1
```

The split line is the honest one and it is the small one. On a ONE-byte
separator the old cut was one byte, so it was already near the floor and the
routing buys 1.11x. On a 35-byte needle the same routing buys **3.06x**, because
what it removes is the growth with needle length, not a constant. Both routed
lanes land at the same 8.5-10.0 MB/s, which is the point: the cost stopped
depending on the needle.

`starts-with?` was left alone, and that is a result too. A first-byte gate ahead
of its cut was written, measured over 240,000 real misses at 25 ms against 26,
and **removed rather than shipped**. Its markers are one to five bytes long, and
a cut that small is now a native. The comment in its place carries the
measurement so the next reader inherits it instead of the temptation.

## Nothing moved

`str_find`'s answers, printed before and after by
`observe/line-grammar-pin-answers.fk`, byte for byte:

```text
str_find  [-1, 0, 3, -1, -1, 0, -1, 2, -1, 0, 1, 5, 11, 28, -1]
find-from [-1, 0, 3, -1, -1, 0, -1, 2, -1, 0, 1, 5, 11, 28, -1, 4]
split-on  <said><گفت><sagte><dit><disse><spuse>   <>   <><><><>
trims     <x y><><><x><گفت  ><  گفت>              [3, 0, 1, 0]
```

Left to right those are: a `start` past the end (the case the TS kernel was
found answering 14 on where the other three answer -1), an empty needle at 0, at
the length, past it, an empty haystack, both empty, both empty with a `start`, a
needle at the very end, a needle longer than the haystack, two overlapping
matches, a whole Persian needle at byte 5 of a real row, the separator after it
at 11, a needle at the end of a real row, an absent needle, and a negative
`start`.

`form/form-stdlib/tests/line-grammar-search-equivalence-band.fk` = **8191 on all
four arms** — go, rust, typescript and the runtime fkwu source/JIT door,
registered in `form/fourth-arm-bands.txt`. Running it four ways paid for itself
immediately: it found the one question the band was asking that two arms cannot
hold. Sweeping `(substring t 0 i)` at every byte offset of a Persian row hands
`starts-with?` a cut that severs a character, rust and ts answer the axiom-1
absence for exactly that cut, and both then die measuring it —

```text
go         = 8191
rust       = fatal[type_contract_violation]: as_str: Null
             str_len < starts-with? < lgse-sw? < lgse-sw-prefixes
typescript = arg 0: expected str, got null
```

— while go and fkwu answered 8191 and would have shipped a band that quietly
proved three arms. The sweep now asks each arm only about prefixes ending on a
character boundary, which is the discipline `csfe-sweep-needles` already keeps
in `core-str-find-equivalence-band`, and no offset is skipped without a rule
saying which. It
keeps all four old bodies verbatim as its reference and asks them the same
questions, so no expected value stands between the two to absorb a disagreement,
and it pins the literal answers above besides — because a reference and a door
can be re-taught into a new agreement together. It bites: **6143** against a
reference newline byte changed from 10 to 11, **6143** against a reference
prefix compare given a spare byte, **4079** against one wrong literal answer,
**2362** against a reference stride of 2.

`(split-on s "")` never returns, before and after. `find-from` answers `i`, the
loop advances by `(str_len sep)` = 0, and the same position is asked forever.
That is the splitter's shape, not the search's; it is named in the band and left
exactly as it stands.

Guarded, all green, none moved: `core-str-find-equivalence-band` 2047,
`core-str-find-to-int-band` 255, `substring-one-meaning-band` 4095,
`substring-native-band` 511, `meaning-codes-band` 127, `bearing-census-band`
32767, `perception-rows-band` 65535, `ear-native-band` 32767, `ear-axes-band`
65535, `ear-tongue-band` 16383, `jungle-ear-band` 32767,
`form-glass-carrier-band` 31, `form-glass-launch-band` 65535, the corpus band
32767, and `tests/line-grammar.fk` 147 — the sum its own header states.

`meaning-codes-band`'s time is **not** claimed. Four readings after the heal
span 6.28, 7.25, 7.26 and 8.37 s and the reading before was 9.14; the spread
swallows the difference and a number that cannot be told from the machine's mood
is not a finding. The census's own workload-ms is the controlled reading of the
same work — **5896/5945 ms before, 5235/5238 after** — and the step count under
it is exact: **60,064,908 -> 59,076,054**.

## Where it stops, and why that is the answer

```text
bearing census — the locale-row walk, 5238 ms, 40 doors of 99.4% (59076054 steps)
  the next substring: fstr-find-loop
  drag 51448  fstr-find-loop — 64.3% of the walking, 35147894 calls, 7 doors lean, mint-a-native
```

The lens now names `core.fk`'s own loop, and the honest reading is that the
heal reached a floor rather than another door. A split on every occurrence
cannot skip a byte, so 34M positions is the work and not the waste; every
position is one walker entry; a byte scan written in Form runs at about
**10 MB/s** where the `substring` native moves 660 through the same pool. The
seed's op table carries seven string natives —

```text
str_len  str_eq  str_concat  str_byte_at  byte_to_str  str_to_float  substring
```

— and no search among them, which is what `mint-a-native` in that row means. So
the next move is a native `str_find` in the seed, worth about the same 60x
`substring` took, and the caller-side half of it is `meaning-codes.bml` reading
and re-splitting the same locale file 235,936 times per round to answer 238,856
lookups. **Neither is opened here.** Both are another hand's file this hour, and
a named wall handed over whole is worth more than a reach across an owned file.

## The most surprising teaching

**The lens's own name for a thing can be the wrong resolution of it.** The
census said `find-loop` and it said `mint-a-native`, and both were true — but the
door it named was not one that needed a native. It needed *deleting*, because
the native-shaped heal already existed twelve lines away in `core.fk` and the
body was simply holding two statements of one meaning. The remedy column reads
per-door and cannot see that the same meaning already stands elsewhere under
another name. `substring` and `nth-rec` before it were the same story told
twice — `nth-rec` was healed by routing to `nth`, `find-loop` by routing to
`str_find` — and the census called for a mint all three times. What the lens is
excellent at is saying **where the weight is**. What it cannot say is whether the
weight is a wall or a duplicate, and the difference between those two is the
whole afternoon.

## Where discomfort became gold

The split-on line came back at 1.11x and it was uncomfortable, because the
prompt, the census and the file's own header all pointed at that number being
large. The comfortable move was to report the 3.06x scan line and let the
reader assume it was the same lane. Sitting with the small number instead is
where the real shape came out: the old cut was one byte wide on a one-byte
separator, so it was never the constant that hurt — it was the growth, and the
proof is that both routed lanes converge on one rate regardless of needle
length. The 1.11x is a better sentence than the 3.06x, and the discomfort was
the thing that found it.

The four-way run is the third and it was the one nearly skipped. The band was
green on fkwu, the answers were pinned, the heal was measured, and running
`form/validate.sh` felt like ceremony over work already finished. It came back
divergent — not on the door, on the *band*, which was asking two of the four arms
for a prefix their own strings cannot represent. A band that proves three arms
and reads as four is exactly the shape this body has a word for, and the only
reason it was caught is that the ceremony was performed anyway.

The second one is smaller and sharper. The `starts-with?` gate was already
written, already correct, already pinned by a band at 8191, and it measured at a
wash. Keeping it would have cost nothing anyone would ever see. It was removed,
and the measurement was written into the comment where the code had been — so
the file now carries a *reason not to*, which is the only kind of note that
stops the same afternoon from being spent twice.
