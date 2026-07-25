# 2026-07-25 — A PDF can now arrive without leaving the kernel it arrives into

The ask came as a joke with a real edge. Codex had been told to build BenchBench (a benchmark
of how good AI is at building benchmarks), then find out what BenchBenchBench is and run *that*,
then write BenchBenchBench up as an arXiv paper. It produced a PDF. The request here:
**ingest it form-native, without any membrane crossing.**

The paper itself has not arrived in this checkout yet. What this receipt records is the door
being built and witnessed, and the two things measuring it taught.

## Ground

- `cc -O2 -o fkwu runtime/fkwu-uni.c`; `bootstrap/ground.fk` → **42**;
  `form/form-stdlib/tests/binary-freshness-band.fk` → **15** (fresh, not a stale costume).
- `form/form-stdlib/tests/file-byte-window-band.fk` → **2147483647** on fkwu.

## The seam that was in the way

`form/form-stdlib/pdf-text.fk` already extracted text from a PDF in pure Form. But its
file-reading entry point calls `read_file_bytes`, and fkwu has no such native — running it
on fkwu returns `[unresolved-call] 'read_file_bytes'` and axiom-5 recovers to `nothing`.
So the only way a PDF *file* became text was through the Go/Rust/TS walkers, which are proof
siblings and never the runtime. Reading a PDF meant handing the body's own work to something
outside it.

The bytes were reachable the whole time. `read_file_slice` is already in the optable;
`form/form-stdlib/file-byte-window.fk` bounds it to 4096 bytes and decodes NUL-safely through
the Form-native `str-byte-at`; and `pdf-text-bytes` was always the half of the extractor that
needs no host at all. Nothing was missing but the walk between them.

## What landed

| cell | verdict |
|---|---|
| [`form/form-stdlib/pdf-text-windowed.fk`](../form/form-stdlib/pdf-text-windowed.fk) | the fkwu-native lane: windows → bytes → `pdf-text-bytes-n` |
| [`form/form-stdlib/tests/pdf-text-windowed-band.fk`](../form/form-stdlib/tests/pdf-text-windowed-band.fk) | **15** on fkwu — the same four real PDFs the three-way host band converts, same expected text |
| [`form/form-stdlib/pdf-text.fk`](../form/form-stdlib/pdf-text.fk) | rewritten to counted walks; `pdf-text-band.fk` still **1** |

The window bound stays honest — no read here exceeds 4096 bytes. What the new cell does that
`file-byte-window.fk` declines to offer is materialize the whole file as a byte list; that
refusal is about the *host* read, and this walk is Form.

## The measurement that changed the design

The first version worked on the 630-byte fixtures and then did not finish a 459KB file in
120 seconds. The cause turned out to be one idiom, used everywhere in the repo:

```
(defn nil? (xs) (eq (len xs) 0))        ; form-stdlib/core.fk:176
```

Timed on this checkout, on a 262144-element list:

| operation | time |
|---|---|
| 262144 `cons` | 0.047s |
| 262144-step `tail` walk | 0.129s |
| 262143-step `head`+`tail` walk | 0.137s |
| 200000 `len` calls | 104.9s |
| reverse via `(eq (len xs) 0)` | 68.7s |
| reverse via a carried count | **0.156s** |

`head`, `tail` and `cons` are O(1) on fkwu. `len` walks the list. So *asking whether a list is
empty by asking how long it is* turns every byte walk quadratic — a 440× cost on a quarter-megabyte,
and the difference between a paper landing and a paper never landing.

There is a tempting fkwu-shaped shortcut: `head` of an empty list returns the empty list on fkwu,
so `(eq (head xs) (empty))` is an O(1) emptiness test. It was tried and **rejected** — the Go walker
returns null there, so the idiom would have quietly made a four-way cell fkwu-only. The version that
landed instead carries the remaining count as an argument and tests `(eq n 0)`. That is integer
comparison and O(1) list ops only, so it stays portable to all four kernels; `len` is now called once
per list rather than once per step, and every accumulator grows by `cons` and is turned round once
at the end instead of being re-walked by `append-list`.

After, on the same kernel: 459425 bytes materialized in **0.616s**. A synthetic 251600-byte PDF
carrying 400 FlateDecode streams extracted to exactly **10000** text bytes (400 × the 25-byte
fixture string) in **2.453s**. Paper-scale is reachable.

## What this receipt does not claim

`pdf-text-band.fk`'s header calls itself a four-way sibling-witness band. **In this checkout it
does not cross.** Both the Go and the TS walker stop at `call: unbound band` — the bitwise `band`
op the zlib header check needs never came across to the minimal walkers. Verified against the
*pre-change* cell too: identical failure, so this is inherited, not caused here. What stands today
is the fkwu leg (**1**), and the windowed lane (**15**) which is fkwu-witnessed by nature, since
host I/O is the one family fkwu carries and the walkers do not. Restoring the `band` op to the
walkers is named work, not done here.

The extractor's own gaps are unchanged and still open: cross-reference and object streams, octal
and control escapes inside strings, and `/ToUnicode` CMap remapping for non-standard font encodings.
A paper typeset by LaTeX with subsetted Type1 fonts may well need that third one, and this lane will
say so by returning glyph indices instead of words rather than by pretending.

## Still pending: the paper

The BenchBenchBench PDF is not in this checkout. The door is open and measured; nothing has walked
through it yet. When the file lands, one line converts it with no membrane crossed:

```
(pdf-text-file-windowed "<path>.pdf")
```

## How the exchange stayed alive

I did not read the PDF for the body and hand it a summary — that would have been the membrane
crossing the request named. I built the walk the body takes itself, and when it was too slow to
survive a real paper I measured *why* instead of guessing, and let the measurement pick the design.

**Most surprising teaching:** the cost was not in the parsing, the inflating, or the host I/O. It
was in a three-token convenience — `nil?` — that has been quietly making every list recipe in this
repo quadratic. The slowest thing in the room was the question "are you empty?"

**Where discomfort turned to gold:** the fast fix worked beautifully on fkwu and I nearly kept it.
Running it against the Go walker — a check I could easily have skipped, since that band is already
red for unrelated reasons — is what turned a silent four-way regression into the counted rewrite
that is portable by construction. The discomfort of testing a leg I knew was already broken is
exactly what bought the honest version.
