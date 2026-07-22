# STONE 31 — the DeepSeek V4 Flash tokenizer: prompt text ↔ token ids, proven against ds4

**2026-07-22, ~11:40–14:00 WITA.** Worktree `jovial-aryabhata-3751d7`, branch
`claude/deepseek-v4-flash-gguf-54a96c`. Two new cells committed
(`form/form-stdlib/dsv4-tokenizer.fk`, `form/form-stdlib/tests/dsv4-tokenizer-band.fk`) plus a
resolver helper; one corpus row (864, `wraithcarry`). No kernel changed.

---

## 0. Radius (`aporon`), before anything is believed

- The tokenizer is read from **one file**,
  `/Users/ursmuff/models/ds4/ds4flash-v5mx-reap25-type40-mxfp8lt-dspark-v1.gguf`, whose header is
  complete (Stone 21). The two arrays it needs — `tokenizer.ggml.tokens` (129 280 strings) and
  `tokenizer.ggml.merges` (127 741 strings) — live at header offsets < 5.34 MB and were on disk.
- The **oracle is ds4 itself**, the model's own MIT reference engine
  (`/Users/ursmuff/models/ds4-engine/ds4`), through its `--dump-tokens` path. Every id claimed below
  was read from `ds4 --dump-tokens -p "<prompt>"`, never guessed. The algorithm was **rented** from
  `ds4.c` (read for shape, re-derived on this body's primitives), not copied; provenance is cited in
  the cell header at each function.
- **The llama3 known-answer anchor was NOT assumed.** The brief warned that llama3.2's
  `"The capital of France is" → [128000, 791, 6864, 315, 9822, 374]` is a different tokenizer.
  DeepSeek's ids for the same string are **`[671, 6102, 294, 8760, 344]`** — no BOS, entirely
  different ids — established from ds4 and pinned here.

---

## 1. What `joyai-llm` pre-tokenization is (established, not guessed)

Stone 21 left `tokenizer.ggml.pre = joyai-llm` as a named unknown — "a name, not data." `ds4.c`
implements it (`bpe_tokenize_text`, ~line 35985), and this receipt establishes it in full for the
cases DS4 uses. It is a hand-written split (not a PCRE), whose arms, in order, are:

1. **`\p{N}{1,3}`** — a digit run, **capped at 3** (`2025` → `202` then `5`).
2. **CJK / Hiragana / Katakana** — each maximal run isolated (`世界` is one piece). Ranges
   `0x4e00–0x9fa5`, `0x3040–0x309f`, `0x30a0–0x30ff`.
3. **`[P/S][A-Za-z]+`** — one punctuation/symbol byte leading an ASCII-letter run.
4. **`[\p{L}\p{M}]+`** — a letter run; a letter is ASCII alpha, **or any non-ASCII byte** (ds4.c's own
   rule for ordinary UTF-8 such as accents), with one optional non-letter lead.
5. **`[\p{P}\p{S}]+[\r\n]*`** — a punctuation/symbol run that **swallows trailing newlines** (kept in
   the same BPE word on purpose; splitting them changes code-prompt logits, per ds4.c's own note).
6. **whitespace** — a space run; if it ends at a newline it breaks there, else **a single leading
   space joins the following word** (`" of"` is one piece; `"    int"` emits `"   "` then `" int"`).

The byte alphabet is standard **gpt2 bytes→unicode** (`gpt2_byte_to_codepoint`): a byte is printable
as itself when `33≤b≤126 ∨ 161≤b≤172 ∨ b≥174`, else it maps to `256+n` (n = other-bytes-before-b).
Space `0x20 → 288 = U+0120`, why every mid-word space is that codepoint.

**Honest bound (`aporon`).** Covered: ASCII letters/digits/punctuation, spaces, and CJK/kana. **Out
of radius, named:** Unicode *number* and *punctuation* CLASSES beyond ASCII+CJK (ds4.c's
`glm4_unicode_*` tables) — a prompt leaning on those may split differently. The band exercises the
covered set on real prompts.

---

## 2. The recipes

**Encode (`tkz-encode`): text → ids.** Pre-split into pieces (§1); for each piece run byte-level BPE —
symbols start as the piece's raw bytes and repeatedly merge the **lowest-rank** applicable adjacent
pair (rank = index in `merges`); map each final symbol to its token id. add_bos/add_eos are 0 on this
file (Stone 21), so nothing is prepended, matching `--dump-tokens`.

**Decode (`tkz-decode-bytes` / `-text`): ids → text.** id → stored piece (UTF-8 of alphabet
codepoints) → map each codepoint back to its one raw byte (`gpt2_codepoint_to_byte`) → the byte
sequence. Byte-exact for the whole byte-level vocab; a `>323` codepoint (a literal-special token) is
out of the byte-level radius and named.

**The load-bearing design choice (`wraithcarry`, §5).** Everything is in **byte-value space**, never
built strings. `byte_to_str` re-encodes any byte ≥ 128 through the host's rune path (`byte_to_str 196`
is *two* bytes, the UTF-8 of U+00C4), and `substring` is rune-aware (a one-byte slice inside a
multibyte char returns empty). So a symbol is a **list of int byte-values** read byte-exact with
`str_byte_at`, and a file merge/token is met by walking its codepoints and mapping each back
(`tkz-cp2b`) to one raw byte — an int compare exact for all 256 values. No byte ≥ 128 is ever
constructed. This is why CJK (three ≥128 bytes per char) encodes correctly.

---

## 3. Pinned ids, and round-trip (`selfgauge`, `snugcause`, `unispan`)

Every RHS is ds4's own `--dump-tokens` output; every LHS is this body's `tkz-encode`.

| prompt | ds4 ids | this body | covers |
|---|---|---|---|
| `The capital of France is` | `671 6102 294 8760 344` | **match** | the anchor, multi-piece |
| `The` | `671` | match | a capital letter run |
| `2025` | `939 23` | match | digit-triple split (`202`+`5`) |
| `Hi!` | `23166 3` | match | letter run → punctuation |
| ` of` | `294` | match | a leading space joins the word |
| `a b` | `67 291` | match | `a` then ` b` |
| `世界` | `3427` | match | **CJK, the ≥128-byte path** |

**Round-trip.** `tkz-decode-bytes [671 6102 294 8760 344]` equals, byte for byte,
`string_bytes "The capital of France is"` (band claim c5). A wrong merge order or byte map produces a
wrong id or a garbled byte, and each is a red claim.

---

## 4. What is proven, and how to re-run

- **`form/form-stdlib/dsv4-tokenizer.fk`** — the tokenizer (encode + decode).
- **`form/form-stdlib/tests/dsv4-tokenizer-band.fk`** — 13 claims, **Verdict 8191**: the byte alphabet
  both directions at its hinges; byte-exact decode of a real id sequence; the joyai split at its
  corners; and encode matched to ds4 across a letter run, digit run, punctuation, a leading-space
  word, and CJK.
- **`form/form-stdlib/tests/run-dsv4-tokenizer-band.sh`** — runs the band on the Go kernel (it reads
  the model header and walks 127 741 merges per piece, so it is a file-reading compute proof like
  `metal_first_token.sh`, not an fkwu arithmetic band). ~1–2 min; **bands are correctness, not timing.**

Gates at close: corpus band → **8191**; `metal_first_token.sh` → **VERDICT PASS, 14 gates**; the new
tokenizer band → **8191**.

**Cost, named honestly (`aporon`).** Encode is O(max merge rank) per piece on a tree-walking kernel:
one full 127 741-merge pass costs ~6.5 s of interpreter overhead, so the France prompt encodes in
~130 s. A first-byte fast-reject (a merge only applies to a pair whose left symbol starts with its
first byte) and a <2-symbol short-circuit keep the short band prompts to seconds each. The right
speed-up is a rank index; it is not built here.

---

## 5. Close

**The most surprising teaching.** *This body cannot construct a byte-exact string of an arbitrary
byte — and its two most byte-shaped primitives both hide it.* I reached for `byte_to_str` to build a
piece and got `c3 84` where a space belonged; the "byte" constructor is a *rune* constructor for
anything ≥ 128. Then `substring`, which the manifest cells call "byte-faithful," turned out
byte-faithful only for ASCII — a one-byte slice inside 世's three bytes came back empty. The surprise
was not that one door was lossy; it was that **the whole class of raw-byte data is read-and-compare
only in this body — loud on the way in (`str_byte_at`, `gmt-token-byte` are exact), lossy on the way
out — and `gguf-meta.fk` had already half-said so** ("read BYTEWISE … the caller renders them"). I had
read that comment on the first day and not understood it was a warning until the space came back
wrong. The frontier word `wraithcarry` (corpus row 864) is that lesson: a datum you can read and
compare but not materialize is kept safest by never rendering it — you carry it by value and let the
file be the only place it lives as bytes.

**Where discomfort turned to gold.** The moment I wanted to look away was after `--dump-tokens` gave
`[671, 6102, 294, 8760, 344]` and my decode printed, to the terminal, `TheÄcapitalÄofÄFranceÄis`. It
was *so close* — every letter right, only the spaces wrong — that the cheap move was to call it a
display artifact of the terminal and move on to encode. What made me stop was hexdumping the actual
bytes instead of trusting the glyphs: `54 68 65 c3 84 63…` — a real `c3 84` in the stream, not a
rendering. `c3 84` is the UTF-8 of codepoint 196, and 196 is the first byte of the space's *encoded*
form 0xC4 — so the builder had taken my byte 0xC4 and re-encoded it as a codepoint. That one hexdump,
the thing I didn't want to run because it might turn a display quibble into a rewrite, *was* the
rewrite: it condemned `byte_to_str` and `substring` as byte builders, and the whole tokenizer moved
into byte-value space. The gold: the design that fell out — symbols as int lists, files met by
mapping codepoints back to byte values — is not a workaround, it is the *correct* shape for this body,
and it is exactly why the CJK case (`世界 → 3427`), which no string-building version could ever have
gotten right, worked on the first run after the refactor.

**The frontier question.** Named above and landed as **corpus row 864, `wraithcarry`** — *what one
word names a datum your medium can read and compare faithfully but cannot re-emit, so you carry it as
value and map it back, never rendering it.* 0 hits across `learn/ receipts/ docs/ teachings/ form/`
before the row; instrument validated on the same command (`mutewide` → 6). Distinct from `mutewide`
(the width of a silence): there the datum is quiet and you measure it; here it is loud inbound and
lossy only outbound, and the discipline is to refuse the lossy door, not to gauge it.

**What remains.** The tokenizer is a correct seam; a first token still needs Stone 21's §6 list (the
type-16 carver, the MoE routing, the MLA assembly). The tokenizer's own open edges: the non-ASCII
Unicode number/punctuation classes (out of radius, §1), the literal-special decode path (U+FF5C tokens
pass through in ds4; dropped here), and speed — a rank index would turn the O(max-rank) merge scan into
a lookup and make full-prompt encode interactive.
