# 2026-07-16 — .fkb artifact v4: the value lane molts to 64 bits

The program-image cache (.fkb) carried a 32-bit signed value lane — sign u8 + u32 magnitude,
capped at 2147483647 — under a body whose speech cells already carry full-range u32 cksum
literals (3566916401 in `learn/sema-voice-trial-window-0004.fk`). Emission refused those cells,
so every speech chain ran uncached, every time. The fix widened the shell instead of trimming
the values: artifact format **v4**, value lane **sign u8 + hi u32 + lo u32** (magnitudes to
2^63-1 on the wire; this body encodes what its native ints round-trip, ±(2^62-1)).

## What changed

- `runtime/fkwu-uni.c`
  - `fk_fkb_write_signed` / `fk_fkb_read_signed`: 9-byte lane; reader guards hi ≤ 2^31-1 so the
    magnitude stays in `long long`. `fk_fkb_write_u32` guard corrected to the true u32 range.
  - `fk_src_write_fkb` writes version **4**; `fk_src_import_fkb_image` gates `< 4`;
    dep-recompile gate `fk_src_fkb_version_raw(...) < 4`; the checked loader **cleanly
    invalidates** v2/v3 (superseded, not corrupt → return 0 → recompile-and-overwrite),
    dying only on genuinely unknown versions.
  - `FK_PARSE_BUF_CAP` 1 MiB → 16 MiB — **measured first** (probe discipline): the v4
    decode-band artifact is 1,292,944 bytes; the old cap made fresh caches die on reload
    ("artifact exceeds FK_PARSE_BUF_CAP"). Worst case at current capacity constants
    ≈ 9.4 MB of node lanes plus strings/symbols.
- Form spec cells (`form/form-stdlib` + byte-identical `grammars/` mirrors)
  - `program-image-fkb-byte-container.fk` → v4: byte-version 4, `pifbc-u32-radix`,
    9-byte `pifbc-signed-bytes` via div/mod, max-signed 4611686018427387903, manifest
    feature renamed `signed-int64ish-big-endian`.
  - `program-image-fkb-byte-decode.fk` → v4: two-word read lane with the hi-word cap,
    room checks 5→9, 20→36, 10→18 bytes per row. Witness and emission cells delegate
    and needed no change.
- Bands recomputed and green via `./fkwu --src`:
  - container band **2147483647** (golden bytes 193/308, negative lane at offsets 133–141,
    min-int refuses as non-encodable-table, and a new bit: **3566916401 encodes ready**,
    its magnitude visible in the artifact as `00 00000000 d49ad331`)
  - decode band **536870911**, file-witness band **2147483647**, emission band **2147483647**
  - homecoming corpus band **511** with new row 732.

## The proof the task asked for

All speech chains run by the receipts' own `cat` door, fresh then cached — verdicts exact,
second run leaves the artifact byte-untouched (nanosecond mtime identical):

| chain | run 1 | run 2 | cache |
|---|---|---|---|
| trial-window (w1…w5, incl. 0004 with the cksum literal) | 32767 ×5 | 32767 ×5 | reload, untouched |
| speech-current-status-ledger (+ its band) | 32767 | 32767 | reload, untouched |

A v3-stamped artifact invalidates cleanly: one rebuild warning, overwritten as v4, correct
output. No fail-safe "running uncached" warning remains on any speech chain.

A full two-pass differential band sweep (HEAD binary vs this one, 1521 band files, artifacts
cleaned between passes, 45s alarm per band) was the wider regression gate: **zero regressions**
— all 205 bands green under HEAD stayed green — and **four bands healed** (form-asm-float,
form-asm-stack, form-lower-x64, pbkdf2-sha256), each a chain whose big literals refused the v3
lane at emission; pbkdf2's chain was re-verified dying under the HEAD binary with
"failed to write .fkb/.sym artifacts" and passing under v4. Three fkb-family bands reported
empty inside the sweep run but are deterministically green cold, warm, and in family sequence
— transient environment (these bands write real files under the shared /tmp temp dir while
sibling agents' fkwu processes run).

## Most surprising teaching

**The body's comparison organ blurs where its arithmetic is sharp.** While building the
min-int band fixture, `(eq min-int (sub 0 max))` returned 1 — two integers that `sub` proves
differ by exactly 1 compare as equal, and ge/lt/le/gt agree with the lie. The behavior matches
comparisons rounding operands through IEEE doubles (exact below 2^53, lossy above). The
container's range guard had to become an **abs-sign test** — the one value whose magnitude
overflows (min-int) wraps negative under `abs`, and sign tests stay exact. A task chip was
spawned to root-cause the fk_rwtab comparison lowering; a memory row records the discipline:
guard large magnitudes by sign of a derived value, never by direct comparison.

## Where discomfort became gold

The first ledger-chain run under the new lane returned **0 with 11 unresolved-call errors —
and cached that degraded answer**. The felt pull was to declare my change had broken prelude
resolution and start reverting. Sitting with it instead: the emitted `.sym` records the
dependency list, and it held exactly one row — the ledger itself. The runtime's prelude
scanner only reads tokens on the *same line* as `preludes:`; the learn/ cells' indented
multi-line headers were never machine-read at all. Nothing was broken — the chains' true door
is the `cat` recipe the receipts had documented all along (`receipts/2026-06-30-sema-voice-trial-window.md`),
and the proof had to walk through the body's own door rather than the door I assumed.
The discomfort of "my fix broke the body" turned into a sharper map of how the body
actually loads itself.

Second, smaller gold: the first parallel sweep produced 70 "failures" — all empty outputs.
The urge was to debug my format change; the ground truth was concurrent `fkwu` processes
racing TRUNC-writes on shared dep artifacts (plus one artifact carrying a stale path
identity). Serial reruns and clean-slate passes dissolved every one. Two pre-existing seams
worth naming, not fixing here: artifact writes are not atomic under concurrent runners, and
a compile that recovers from unresolved calls still caches its degraded image.

## Review addendum: the one value the lane cannot hold

Codex review caught a P2 the bands missed: `LLONG_MIN`'s magnitude (2^63) has no positive twin —
the writer emitted hi `0x80000000`, the reader refused it, so a source carrying that literal
poisoned its own cache (first run fine, second run dead). Reproduced, then fixed at the **write**
side: `fk_fkb_write_signed` refuses the magnitude the reader cannot round-trip, and
`fk_src_write_fkb` unlinks any partially written artifact on failure (a partial image looks
fresh by mtime and died the next run as "truncated artifact"). Proven: the min-int cell now
refuses identically and loudly on every run, leaves nothing behind, and all chains/bands stay
green. The write gate and the read gate now state the same law — refusal at the door, not
poison in the pantry.

## Distillation row offered

Row **732** in `learn/homecoming-distillation-corpus.fk`: *what one word names shedding a
shell the body has outgrown* → **ecdysis** (0 hits before the row; near-miss "molt" 0 hits
but casual; "shed" 12 hits and names the letting-go, not the growth that forces it).
