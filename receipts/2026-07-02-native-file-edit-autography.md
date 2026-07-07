# 2026-07-02 — autography: the body edits its own files, in its own hand

## Ground

```sh
./fkwu --src bootstrap/ground.fk                               # 42
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk   # 15
# native-edit string layer, four-way                           # 127
```

Urs: *"I hope we can do the file edit native as well, that will be very helpful."* The body already
has native grep (`sh-bi-grep`); this rounds out the self-tending suite with a native **editor** —
find/replace over strings, and a safe file edit that reads, replaces, and writes through the body's
own primitives, so edits no longer require the rented hand.

## What was built

`form/form-stdlib/native-edit.fk`, two layers:

- **String layer (four-way):** `ne-index-of` (first match at/after an offset, −1 if none),
  `ne-count` (non-overlapping), `ne-replace-first` / `ne-replace-all` (left-to-right, never rescans
  the inserted text, so `new`-contains-`old` cannot loop). Pure Form data + arithmetic over the seed's
  `substring`/`str_len`/`str_eq`/`str_concat` — the walkers run it identically.
- **File layer (fkwu-carrier):** `ne-edit-unique` and `ne-edit-all`, over `read_file` +
  `write_file_text` (the real write primitive — `write_file` is not an op; that mis-name declined to
  `nothing` on the first probe and wrote an empty file, caught immediately). `ne-edit-unique` carries
  the rented tool's own safety: it replaces **only when `old` occurs exactly once** — `1` replaced,
  `−1` not found, `−2` ambiguous (>1, no write), `−3` write failed.

## Witnessed

- **String layer four-way = 127** (fkwu = Go = Rust = TS) — `form/form-stdlib/tests/native-edit-band.fk`
  proves index (found / found-after-offset / not-found), count, replace-first vs replace-all, and the
  no-loop case (`ne-replace-all "xx" "x" "yy"` → `"yyyy"`, not an infinite loop).
- **The body edited a real file, natively:** on a two-line target, `ne-edit-unique "quick brown" →
  "swift red"` returned `1` and the file became "the swift red fox"; `"unicorn"` returned `−1`; `"the"`
  (twice) returned `−2` and left the file untouched. `1,−1,−2` — the exact-unique-match contract, live.

## Honest floor

The file layer is host I/O (fkwu-carrier), so it is proven live, not four-way — the same honest seam
as `read_file`. `ne-edit-unique` matches by exact substring, not by line or regex (that is the next
layer if wanted). And it rewrites the whole file (read-all → write-all), fine for source cells, not for
gigabyte files. The path goes through `fk_cstr`, which now dies loudly if a path exceeds 4095 bytes
(the earlier fix) rather than truncating to the wrong file.

## The most surprising teaching this work left behind

The safety was the feature, not the mechanics. Find/replace is a few lines; what makes this an
*editor* the body can trust itself with is the uniqueness gate — the refusal to touch an ambiguous
match. Without it, `ne-edit` would be a footgun that silently changes the wrong "the"; with it, an
ambiguous edit is a declined `−2`, not a quiet corruption. The rented edit tool taught this contract by
example, and porting the *contract* mattered more than porting the string-slicing.

## Where discomfort turned to gold

The discomfort was small and immediate: `write_file` returned a huge-negative `nothing` and wrote an
empty file on the very first probe. The pull was to assume the primitive was broken. Witnessing
instead — grepping the optable — showed the op is `write_file_text` (tag 104); `write_file` simply
does not exist and (thanks to this session's earlier fix) declined loudly to `nothing` rather than
pretending. The compile witness I built two turns ago caught my own typo the same way it would catch
anyone's — the tools we harden to keep us honest keep *us* honest too.

## Corpus

Row 653 **autography** — writing, and re-writing, in one's own hand (fresh; the body editing its own
files through its own primitives — `ne-edit-unique` reading, replacing, and writing a real file with no
rented hand, the native counterpart to `sh-bi-grep`).
