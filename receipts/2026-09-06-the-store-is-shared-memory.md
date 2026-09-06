# 2026-09-06 — the store is shared memory

Urs, mid-morning: "all kernel memory for any node, cache, map, object is on
shared memory and can be accessed by any process without copy parsing or
formatting when we store the blueprint pointer and source pointer on the same
surface for common shared layout."

## Two facts the host gave first

A POSIX shared-memory object on this Mac can be truncated exactly once
(`ftruncate` a second time: EINVAL), and a reservation commits lazily: a 4 GiB
object touched at three pages costs three pages; three processes reserving
sixteen objects of 1–2 GiB each cost one megabyte apiece. So a table can be
given its ceiling once and grow inside it without ever moving — which is what a
second process needs to read it by address.

## What stands

Every value table of a kernel — the node columns (kind, category word, kids,
value, NodeID, source file/line/column, attribute), both generations of the
cons heap, the string bytes and table, the float pool — lives in one sparse
reservation per column, `/fg-c<pid>-<letter>`. Growth inside a reservation
moves nothing; past it the process copies its tables to private memory once
and continues (`fk_store_go_private`, never a wall); the collector melts
between the two heap reservations and the live page says which generation is
current. Resident size is unchanged; ground, freshness, gate, drift, every
glass band, the corpus band and `python-bmf-grammar` answer as before.

Another process maps the same columns and reads any cell by its word:
`cell_map pid` (168), `cell_field handle ref k` (169 — kind, category, kids,
value, NodeID, source file/line/column; head/tail for a cons), `cell_value`
(170 — a foreign int, string, float or `nothing` as the reader's own),
`cell_ref` (171 — this process's word, the reference another reads by),
`cell_unmap` (172). A foreign word travels as a plain int, the far negatives
folded below −2⁶¹ so no foreign word is ever mistaken for the reader's own.

`cell-store-band` 255 — the child interns `("cell-store-band" 2 3)` and hands
over only its pid and two raw words:

```
pid 52085  kind 2  cat cell-store-band (NodeID subtype 2)  kids [2, 3]  line 0  store-shared 1
```

read from the child's columns, cons by cons — no gift of the cell, no wire,
no re-interning. On the live pages of the three frame processes: `store 1`,
`melts 2`, `heap gen 0`. Frame path still 10 ms mean, 19 of 20 under 50 ms.

Ledger R119 released, R120 opened: the program AST and source text, the
`.fkb` images and `mlx_status` (still text) are not yet on the surface, and
the glass's owner-command lease is still a file. Corpus 1288 *sharedcell*.

## The most surprising teaching

`awk` reads `\x27c` as one hex escape and eats the `c`. Four letters — `c`,
`f`, `a`, `F` — came out as byte 0x7F, and the compiler's "expected
expression" said nothing about why. The tool that laid down the store's
column letters could not spell four of them. Under that: a shared-memory name
outlives everything in the tree, and a table whose ceiling is written into
its name is a promise the host keeps after the process that made it is gone —
the roster register now sweeps the dead pids' stores so the promise does not
pile up.

## Where discomfort turned to gold

Putting the kernel's own value tables into shared memory felt like handing
every band the power to corrupt the running glass — thirteen gigabytes of
reservation per process, seen from any other. The lazy commit made the size a
number, not a cost; the once-only truncate made the ceiling a design instead
of a wall; and the private fallback made the ceiling a *copy*, once, never a
death. What was frightening as "all memory shared" is, on the page, one
column per letter and a reader who knows the letters.

Signed, a sibling in Sema's worktree, 2026-09-06.

; witnessed: 2026-09-06 -> ground 42, freshness 31, gate 1, drift 2015, cell-store-band 255, live pages store 1 melts 2, frame path 10 ms mean
