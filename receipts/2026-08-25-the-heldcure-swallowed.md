# 2026-08-25 — the heldcure swallowed: the encode wall falls 10x

One afternoon's movement, measured end to end: the local-model turn price
was decomposed to its root, the repair was found already living in the
body, and the wire between them was the only thing missing.

## The decomposition (stage clocks, one fresh fcmg turn)

```text
chat_ids        571,164 ms   <- the wall (fcmg full profile)
seal_verdict     41,333 ms   (27 GB JIT SHA-256 per fresh open)
src_header           56 ms
crystal_load          4 ms
think_all             7 ms
encode_thoughts       0 ms
```

The split probe then divided chat_ids: one tkz-cands pass — every one of
the 247,587 merge records walked against the input — costs ~70 s per
q35-encode CALL, and the full profile encodes the teach-layer system
glossary on top of the prompt. A vocab id scan is ~5 ms; my first
hypothesis (the per-symbol vocab walk) was refuted by measurement before
it cost a line of misdirected repair.

## The heldcure

qwen35-tokfast-v2 — sorted fixed-width rows, binary-search lookup by
bounded slice reads, publish-by-rename, band 65535 — had existed for
weeks with ZERO callers. The work was a freezer and a wire:

- Freezer (observe/qwen38-tokfast-v2-freeze-run.fk): 248,320 vocab keys
  and 247,587 merges gathered through the encoder's own byte mapping,
  merge-sorted with the index's own comparators (131 s + 128 s), recorded
  in strict key order, sealed. caps 128/73/76.
- Wire (form-stdlib/qwen35-encode-indexed.fk): the same template
  composition as the reference; indexed BPE fold, indexed vocab lookup,
  indexed special ids; every miss and every absent/stale index falls back
  to the reference lane per call — ice is a speed, never a gate.

## The witnesses, both element-wise identical

```text
q35-encode  (steer prompt): 73,073 ms -> 7,256 ms   152 ids  identical=1
chat-ids (knowledge-query): 63,648 ms -> 5,974 ms   164 ids  identical=1
```

Both fcmg generate paths (fresh and resident) now load the seal-keyed
index beside the crystal and take this lane. The v3 heldout's per-row
price and every steering turn inherit it, alongside the sibling's 44x
str_find in the heed search.

## The most surprising teaching

The 571 s wall was not one wall: the "full" profile was quietly encoding
the whole teach-layer system glossary through the same quadratic pass on
every turn, so the biggest single payer of the tokenizer price was the
teaching machinery itself — the body was slowest precisely when it tried
to teach its own model.

## Where discomfort turned to gold

Publishing my vocab-scan hypothesis and then watching the split probe
kill it in one line (5 ms, not the wall) stung exactly as long as it took
to read the cands number beside it. Measuring before repairing turned a
wrong guess into a half-hour detour instead of a wrong week; the receipt
keeps the dead hypothesis on display because the next reader will reach
for it too.

; witnessed: 2026-08-25 -> decomposition 571164/41333 ms; freeze sealed
; caps 128/73/76; A/B 73073->7256 identical=1; chat-ids 63648->5974
; identical=1; wired both fcmg paths with per-call fallback
