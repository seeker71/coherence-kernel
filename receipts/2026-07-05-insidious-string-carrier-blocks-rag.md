# 2026-07-05 — "can we make RAG work?" — the blocker is an insidious fkwu runtime bug

## The question

"Can we make RAG work, please." Yes — but the blocker is not the recipe. It is the
runtime, and I pinned it to one line.

## Root cause (confirmed, minimal, builtins only)

```
(do (let a (if (str_eq (nth (list "alpha" 1) 0) "alpha") 1 0))
    (let b (if (str_eq (nth (list "alpha" 2) 0) "alpha") 10 0))
    (add a b))
```

Expected 11. Actual **29**. A single string-from-list extraction + `str_eq` is correct
(→1); **two of them corrupt the values** (→29 — nonsense arithmetic, a value-table /
interning aliasing corruption in fkwu). No `core.fk` needed; only `list`, `nth`,
`str_eq`, `if`, `let`, `add`. It reproduces with and without core.

The native RAG returns a string **id** through exactly this path: `rag-nearest-id` =
`(rag-id (rag-nearest …))` = `(nth entry 0)`, then the band `str_eq`s it. So the second
retrieval in any program corrupts — which is exactly why `rag-retrieve-band` scores
18/31 (distance math on ints passes; every string-id claim fails) and
`rag-ask-grounded-band` scores 0/7. **`rag-ask.fk`'s own header already named this:**
"the live host-read JSONL path avoids passing host strings through helpers until that
carrier boundary is lifted." The organ knew; the boundary was never lifted.

## Two masks that hid it (and nearly made me report the wrong thing)

1. **The display lies.** fkwu prints a string *extracted from a list* as its internal
   numeric handle (e.g. `(head (list "alpha" 99))` prints `0`/`70`), not the text. My
   first probes read that as "ranking broken" — it wasn't; `str_eq` on the same value
   returns 1. The value is fine in isolation; only the print and the *repetition* mislead.
2. **Every single-case test passed.** `bit1`, `bit4`, `bit8` each pass alone. The fault
   only appears when two string-carrier comparisons chain. A green isolated test proved
   nothing about the second call.

## Why the door still works

The door (`chatgpt-plugin.fk`) retrieves correctly over 899 cells (witnessed live:
framebuffer→ll-buffer, trust→judged-trust) because its selection compares **integer
scores**, and it emits ids by **`str_concat`/`str_byte_at`** (building output), never by
summing the results of chained `str_eq`-of-extracted-strings. It threads around the
insidious trigger by construction. So working retrieval exists today — it just isn't the
`rag-ask` organ.

## The three honest paths to working native RAG (no ersatz)

1. **Fix the runtime** (`fkwu-uni.c`): lift the string-carrier boundary so `nth`/`head`
   of a string list element returns a value that survives repeated `str_eq` without
   corruption. The deepest, most sovereign fix — makes `rag-ask` and everything native
   work — but it is careful C work and must preserve the four-way proof. Not a
   tail-of-session hack.
2. **Redesign RAG's id carrier** (recipe-level): give cells **integer** ids in the index
   and keep a separate id→path table resolved only at output time via `str_concat`
   (the path the door proves safe). Sovereign, tractable, sidesteps the runtime bug;
   `rag-retrieve` ranks on int vectors (already correct) and never str_eq's an extracted
   string for control flow.
3. **Native-index the working door path**: keep the door's proven retrieval, replace the
   Python index generator with a native fkwu indexer (walks the deployed body, emits the
   index — no Python, no hand-curation). Fastest to "working + native", but it is the
   door's retrieval, not the `rag-ask` organ.

Recommendation: **2 or 3 now** (both give working, sovereign, no-Python, no-hand-curation
retrieval this session), and **1 as the true north** (file the runtime bug with this
receipt's one-line repro; lifting the carrier boundary unblocks the whole native RAG
organ four-way). All three still need the RAG organ + a knowledge corpus **deployed** on
the door's branch (the standing topology decision).

## Closing

**Most surprising teaching**: I nearly shipped a wrong diagnosis twice — "ranking is
broken" — because the instrument lied (extracted strings print as numbers) and because
every isolated test was green. The truth only appeared under *repetition*, which no
single probe could show. An insidious fault is defined by exactly this: it passes every
trial you run one at a time. The discipline it demands is to test the *interaction*, not
the act — and to distrust a green isolated test as proof of a chained one.

**Where discomfort turned to gold**: the pull, again, was to "just make RAG work" by
bolting something on. Sitting with the failing band instead of routing around it drove
the bisection down to a one-line builtins-only repro of a runtime value corruption —
worth far more than any patch: it names the real boundary the whole native-RAG dream is
waiting on, and hands whoever lifts it an exact, minimal witness to fix against.
