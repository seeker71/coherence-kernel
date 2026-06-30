# Receipt — satsang stdlib import, rebased and direct-source grounded

**Source:** `~/source/Coherence-Network` at `e6787e648`.
**Target base after rebase:** `0309ed7`.

Imported the missing satsang stdlib surface into this checkout:

- `form/form-stdlib/satsang*.fk`
- `form/form-stdlib/kernel-satsang.fk`
- `form/form-stdlib/recognition-router.fk`
- focused `form/form-stdlib/tests/*satsang*` and `recognition-router*` bands
- the small stdlib dependencies needed by those bands: `channel-interface`, `form-cli-router`, `form-cli-judge`,
  `form-cli-sufficiency`, and `core`
- named substrate docs for satsang, recognition-router, channel-interface consent, form-cli north star, and
  kernel self-composition

The import is not just file grounding. The local `fkwu --src` runner was extended to carry the imported cells:

- multi-argument user calls now pack operands into a Form list and unpack them into frame slots;
- `true` and `false` source literals now lower to numeric truth values;
- unknown calls now skip balanced source forms, so unsupported op families become an honest `0` floor without
  corrupting the rest of the parsed file.

Direct-source witnesses after the import:

| witness | output |
|---|---:|
| repo ground: `observe/native-vs-rented.fk` + `(native-vs-rented-check)` | `11111` |
| channel-interface direct check | `127` |
| recognition-router band | `127` |
| recognition-router-compute band | `63` |
| recognition-router-vision band | `31` |
| satsang band | `127` |
| satsang-field band | `255` |
| satsang-share band | `255` |
| satsang-flip-witness band | `4095` |
| satsang-guidance-event band | `255` |
| satsang-health-memory band | `1023` |
| satsang-host-boundary band | `2097151` |
| satsang-listen-route band | `255` |
| satsang-room-memory band | `255` |

Honest remaining floor: `kernel-satsang` returns `193` on this fkwu arm, matching its own comment that the
interned recipe execution, node image projection, and `.fkb` binary image families are not yet carried by the
fourth arm. That is a named coverage gap, not a divergence and not a missing flattened table seed.

Follow-up correction: the imported bands now wrap their sequential `let`s in explicit function frames for this
bounded direct-source runner, and the raw-source witness commands omit the high-grammar `core.fk` prelude where it
is not needed. See `receipts/2026-06-30-direct-source-band-wrappers.md`.
