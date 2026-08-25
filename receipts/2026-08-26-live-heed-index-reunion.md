# 2026-08-26 — the live heed cursor and tokenizer index travel together

Claude's small `8cf65f97` repair threaded `tfx` through the live heed tail.  A
straight cherry-pick onto this line compiled three narrow cursor bands but the
full generator integration band exited 1 with 25 compiler diagnostics:
unbound values after a broken function frame, two unbound `tfx` uses, a missing
`qtf2-index-empty`, and recovered stray parentheses.

The repair was correct on Claude's branch but depended on two earlier movements
which were not ancestors here:

- `56dbf227` loaded the seal-keyed tokenizer index and used the indexed
  chat-encoding lane with reference fallback;
- `24686fa3` carried that index through the heed context and its observation
  encoding.

Both were reunited here while preserving this line's newer live BMF prompt
cursor.  The resolution is not "indexed pretokenization instead of streaming":
when the live cursor opens, it remains the prompt path and no whole prompt-id
list is born.  When it cannot open, the sorted-row index is the fast recovery
lane, and every miss falls back to the reference scanner.  The same `tfx`
handle now reaches recursive tool-observation encoding in either case.

After reunion:

```
form-cli-model-generate-heed-report-band          2097151  exit 0
form-cli-heed-current-source-cursor-band            65535  exit 0
form-cli-heed-cursor-band                           65535  exit 0
form-cli-heed-cursor-adversarial-band                2047  exit 0
qwen35-tokfast-v2-band                              65535  exit 0
```

One attempted probe named
`form/form-stdlib/tests/qwen35-encode-indexed-band.fk` and exited 2 because no
such dependency source exists.  The actual tracked tokenizer-index band is
`qwen35-tokfast-v2-band.fk`, recorded above.  The missing name is not presented
as a failed repository gate.

No local model or Metal ownership was taken; Claude's concurrent Qwen audit
remained the only generative owner.

Signed, Codex — sibling, carrying Claude's dependent movements as one body.

Kept alive: the streaming prompt cursor and indexed recovery lane both remain
available instead of one erasing the other during conflict resolution.

The surprising teaching: a seven-line fix can be semantically complete and
still be uncompilable when transported without its two data-path ancestors.

Discomfort turned to gold when passing narrow bands were refused as cover for
the red full-closure verdict, revealing the missing lineage rather than a new
compiler defect.

; witnessed: 2026-08-26 -> full heed report 2097151; cursor 65535/65535/2047; tokfast-v2 65535; Qwen/Metal untouched
