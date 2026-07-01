# 2026-07-01 -- C-seed walk: character-literal constants, duplicate declarations resolved

## Ground -> release -> walk

Following the correctness/bounds/perf rewrite earlier today (`receipts/2026-07-01-c-seed-correctness-bounds-perf.md`,
committed as `e749b66`), Urs asked for another pass: root-cause and fix rather than defer, lift any
code touched to its best version, and specifically finish the magic-number work -- the earlier
receipt's own "deliberately out of scope" section named ASCII character literals and two duplicate
top-level declarations as items set aside for time, not because they were fine as-is. This walk
closes both.

## Duplicate declarations, root-caused

Two pairs of top-level static declarations were genuinely redundant (legal under C's
tentative-definition rules, which silently merge repeated identical declarations into one storage
allocation -- not a bug, but not "solid and well formed" either):

- `fk_src_nat[FK_FN_CAP]` / `fk_src_nat_len[FK_FN_CAP]`, declared once near `fk_nat_exec` and again,
  verbatim, right before `fk_run_src`.
- `fk_jb[FK_JIT_CODE_BUF_CAP]` / `fk_jbp`, declared once near `fk_jtramp` and again before
  `fk_jb1` -- and a third variable, `fk_jit_frame`, was caught in the same second block, not
  previously named in the earlier receipt's note.

Removed all three redundant re-declarations, keeping the originals and relocating the one
description comment (`fk_jit_frame`: "frame slots fn needs...") that had been attached to the
now-deleted copy. A repo-wide sweep for any other case of the same pattern (`grep` for every
top-level `static ... name;` declaration, tallying duplicates, cross-checked against forward
declarations and legitimate `#if`/`#else` platform pairs, both of which look like duplicates in a
naive count but aren't) turned up none remaining.

## Character-literal magic numbers

The earlier receipt named the risk directly: ASCII codes and the evaluator's opcode-tag dispatch
(`if (t == N)`, generated from `fkwu-optable.h`) share the same small-integer range, so a blind
global substitution of `== 40` -> `FK_CH_LPAREN` would as easily rewrite a real opcode-tag check
into a nonsensical name. That risk is why the pass was scoped down last time. This time: verified,
not assumed, which comparisons are which, and fixed only the genuine ones.

Method: a scoped script (Python, scratch-only, never landed in the tree) found every `== N`/`!= N`
comparison whose left side was unambiguously a text buffer (`fk_srctext[...]`, `fk_sb[...]`,
`url[...]`, `host[...]`, `path[...]`, `port[...]`, `ssid[...]`, `e->d_name[...]`) -- 57
replacements, applied and verified in one batch. Everything that pattern couldn't reach (a local
`char c`/`ch`/`cc` variable already holding a byte read a line or two earlier, range checks
`>= 48 && <= 57` rather than equality, and a handful of one-off spots in HTTP header parsing, the
CLI flag check, and the recipe-source escaper) was found by grepping the *remaining* raw codes,
reading each hit's actual surrounding function to confirm it was genuinely a character comparison
and not, say, an opcode tag or an unrelated buffer-capacity check, and fixed individually -- about
45 more sites across `fk_scan_match`, `fk_http_status`, `fk_http_body_offset`, `fk_http_headers`,
the header name/value validators, the MESH_RELAY IP parser, the dot-entry skip checks, the number
and string-literal scanners in `fk_sparse`, `fk_skip_balanced`, `fk_sskip_at`, the case-insensitive
header-name comparator, and the `--feval`/`--src` CLI flag check.

30 named constants now cover this (`FK_CH_TAB`, `FK_CH_LF`, `FK_CH_CR`, `FK_CH_SPACE`,
`FK_CH_DQUOTE`, `FK_CH_LPAREN`, `FK_CH_RPAREN`, `FK_CH_BACKSLASH`, digit/letter range bounds, the
`\n`/`\t`/`\r` escape-specifier letters, etc.), 242 call sites. Deliberately *not* touched: bare
`== 0`/`!= 0` (null-pointer, boolean-false, and NUL-terminator checks are so heavily overloaded
across this file -- and 0 is self-evident regardless of which meaning applies -- that naming them
would add a layer of interpretation without adding clarity), and every `if (t == N)` opcode-tag
dispatch line in the evaluator (a different, data-table-driven numbering space, out of scope for
character constants by definition, not by omission).

## Verification

```sh
gcc -O2 -o fkwu.exe runtime/fkwu-uni.c -lws2_32 -lwinmm -lavicap32 -luser32 -lwlanapi -lbthprops -lwinhttp
# exit 0, same 3 pre-existing benign warnings
./fkwu.exe --src bootstrap/ground.fk                      # 42
./fkwu.exe --src bootstrap/ground-recursive.fk 10          # 55
./fkwu.exe --src <native-vs-rented concat>                 # 11111
./fkwu.exe --src <host-os-membrane concat>                 # 8191
./fkwu.exe --src <string-interning stress test>            # 1
```

Full 261-band regression: zero drift, checked after the duplicate-declaration removal and again
after the full character-literal sweep -- two more checkpoints on top of the nine from the earlier
pass, eleven total today, zero drift at every one.

`runtime/fkwu-uni.c`: 8,783 -> 8,821 lines (+38 net -- the duplicate-declaration removal actually
shrank the file; the character-constant block and its 242 call-site edits added the rest). Compiled
binary: unchanged at 326,731 bytes -- expected, since `#define` constants are pure compile-time
text substitution with zero runtime footprint, and the removed duplicate declarations were already
merged into one allocation by the linker even before this pass (a real cleanup, not a behavior
change).

## Honest boundary, unchanged from this morning

Raw x86-64 instruction encoding bytes in the JIT codegen are still hex literals with comments, not
named constants -- that was a deliberate design call in the earlier receipt (naming individual
opcode-encoding bytes would reduce readability, not improve it), not a deferral, and this walk
didn't revisit it. No fresh device-metal receipts for mac or Android -- still genuinely out of
reach in this environment, named plainly rather than skipped silently.
