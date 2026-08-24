# 2026-08-24 — who withholds share, and the real data

Yes asked who is withholding `kind=declared` and how to get counted
receipts instead of a wishful offering.

## Who withholds what

The withholder is a cell, not a person.

`form/form-stdlib/form-cli-share-run.fk` prints

```
kind=(fcte-kind-name evidence)
```

then, if `(fcte-valid? evidence)` is 0:

```
measurement-health=unreconciled share=withheld
```

So the percentages stay off the page. Walk lanes (`fcr-walk`) still
print. That is the withhold: native/local/remote parts of 100, not the
kind fold itself.

Kind is already a fold in `form-cli-turn-evidence.fk`:

```
observed  if the row reconciles
embodied  if a carrier row is present but incomplete
declared  if there is no usable carrier
```

This checkout had no Codex `.form-cli-turn-rollout` and no
`.form-cli-turn-evidence-row`, so `fctel-row` was the string
`unmeasured`. Carrier absent. Kind declared. Share withheld.

The wishful numbers live elsewhere and were never what share-run
prints when unreconciled:

```
form-cli-local-law.fk
(defn fcl-this-kind () (fcl-share-kind-declared))
(defn fcl-this-turn () (fcl-share 50 20 30))
```

50/20/30 sums to 100 by construction. It is an offering, not a tally.

## Where the real data already was

This Grok session keeps public measurement rows in

`~/.grok/sessions/.../019ffe4b-9018-7d11-82eb-80f3b02f8f3b/`

- `terminal/*.log` — each form-run ends `@form KIND EXIT STDOUT STDERR SHOWN`
- `events.jsonl` — `"type":"turn_started"` and `"type":"tool_completed"`

Native is a completed `fkwu` receipt. Local is any other `@form` kind
(git, python3, otool, sh, …). Remote is a `turn_started` event. Prompt
and answer text are not stored.

## How the row became observed

1. Rebuild `fkwu` (binary was 22 Aug; `fkwu-uni.c` was 23 Aug). Freshness
   `</dev/null` → 31. Metal and MLX both linked.
2. Accept Grok as a source: `fcte-source-kind-grok` =
   `grok-form-run-logs+explicit-form-binding`. `fcte-valid?` uses
   `fcte-source-ok?` instead of Codex-only.
3. `fctel-row` keeps a valid Grok row without a Codex bind.
4. `form-cli-turn-evidence-grok.fk` counts `@form` receipts and event
   marks, then builds the 35-field pipe.
5. Bind the session dir (gitignored `.form-cli-turn-grok-session`).
6. `form-cli-turn-evidence-grok-refresh.fk` scans `terminal/` and
   `events.jsonl`, writes gitignored `.form-cli-turn-evidence-row`.

Fixture band: `./fkwu form/form-stdlib/tests/form-cli-turn-evidence-grok-band.fk`
→ **255**, exit 0. Codex evidence-band still **4095**. Live-band still
**1023**.

Live refresh (473 logs, 1.9 MB + 7.5 MB events) took 487 s on this
host. Floor, not a miss: Form walked every file. Then:

```
./fkwu form/form-stdlib/form-cli-share-run.fk
form-cli share kind=observed scope=grok-session-form-receipts
events native=405 local=461 remote=104 total=970
share native=42 local=47 remote=11 sum=100
form-commands=866 form-failures=64
fkwu-failures=37 form-shown-bytes=942066
tool-calls=2538 tool-outputs=2538
duration-ms=859199083
measurement-health=observed semantic-contribution=not-measured
```

Largest-remainder on 405/461/104 is 42/47/11. Token volume is still
not used for share. Semantic contribution is still not measured.

The private row is gitignored. Re-run the refresh to re-witness; do
not type 50/20/30 in its place.

Signed, Grok — sibling, this worktree.

; witnessed: 2026-08-24 -> grok-band 255, share kind=observed 42/47/11 from 405/461/104
