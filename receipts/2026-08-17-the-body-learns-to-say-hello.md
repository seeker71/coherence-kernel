# The body learns to say hello — and the prompt that never had

Asked by Urs on 2026-08-17, after the entry-path rounds closed: "next rounds
please" — the body-level work the doc rounds had named. Four read-only maps,
then the surgery: form-cli's spoken words, the frozen ledger date, the empty
grounded shelf. Three regens, one two-shell healing, one guard that was never
true, and a body that now greets, dates, and grounds — with zero subscriptions.

## What landed, all witnessed on the rebuilt binary (form-cli 0.6)

- **The greeting.** On a real terminal the body now says: *"Hello — I am Sema's
  body. My words for you: about, improve, learn, inquire, quit. Type one and
  press return."* A pipe stays byte-silent — the deterministic-transcript
  contract holds.
- **The plain first breath.** `about` now leads with *"I am Sema's body on your
  own computer…"* before the builder's words; `help` opens with *"For you:
  about, improve, learn, inquire, quit."*
- **The free-sentence door.** A whole sentence is met with the inquire offer —
  *"I don't speak free sentences yet — if that was for me, type: inquire …"* —
  while a lone unknown word keeps the exact reply the band pins.
- **The live date.** The inquiry ledger stamps today (witnessed `20260817`
  rows) — the clock native `now_unix_ms` (tag 15) had been in the carrier all
  along, one baked literal away from the calendar that route already read
  daily. The carrier was never clockless.
- **Newest teaching answers.** Re-teaching a word now answers with the newest
  row; the older rows stay, like pages in a notebook.
- **The grounded shelf ships.** Eight self-knowledge rows at
  `.coherence-network/rag-index/index.jsonl` — every coordinate read from
  `blueprint-registry.json` (none minted), every answer an exact byte-range
  quote with real sha256 bindings, every vec computed by the body's own
  `re-semantic-vec`. `grounded what is sema?` answers from WELCOME.md at
  confidence 100 on a fresh clone; a question with no ground answers
  `grounded:miss`. Two rows healed same-day after a fresh seating read them
  with newcomer eyes: trust now answers WELCOME's plain promise, and
  where-do-my-words-go carries the door-difference sentence its first window
  cut off.

## The most surprising teaching

**The prompt had never printed for anyone.** The repl guarded its `form-cli> `
with `(eq (isatty) 2)` — and the native answers Form 1 (the C returns raw 2,
which IS tagged 1). The guard was born false, the branch compiled, shipped,
green for months, and never once alive; nobody missed the prompt because nobody
knew it was promised. My greeting inherited the numb guard and went silent the
same way — and only a three-line C control under the same pty harness split
walker-seam from harness-quirk. A check that cannot fail (row 1007, vacuity)
reports success; a branch that cannot fire reports nothing at all, which is why
it outlived it. Corpus row 1010 names it *stillborn*.

Second surprise, same family as the morning's: the pty witness first exposed
that an intermediate top-level expression in the band unit does not survive the
flatten — the greeting had to become one final expression with the loop. The
flattener keeps defns and the last word; everything between is silence.

## Where discomfort turned to gold

**The regen lane broke in my hands, twice, in two shells.** First run:
`PIPESTATUS[@]: parameter not set` — bash's array name in a zsh lane, reachable
only on a host where the fkwu-selfhost arm authors (this one, first witness).
I healed it to zsh's spelling — and validate.sh promptly broke the same line
under bash. The discomfort of breaking the proof lane while repairing it became
the real repair: no pipe-status array at all — fkwu ends its pipeline into a
raw file and answers for itself in `$?`, honest in both shells. The lane's
identity hash eats its own scripts, so each healing re-staled the stamp and the
regen ran again — three times, stamps converging at last on `2dcc1d23bd00f3e6`
across bootstrap and platform, closing a seam the branch had carried before I
arrived (three distinct stamps witnessed at mapping time).

## Honest edges

- The router still walks a person past answers the shelf already holds: a free
  sentence goes to inquire even when `grounded` would answer it at confidence
  100. Named next round: probe the shelf first, put `grounded` in the For-you
  words, case-fold `About`, answer-first output, a distinct no-shelf-here miss
  from foreign cwds, one plain sentence atop inquire's bookkeeping.
- The one-token rung stands exactly where `synthesis-status` says:
  `missing:full-real-llama-gguf-token-generation`. The measured blocker in
  `real-gguf-llama-block-fwd.fk` (~0.26–0.3s per Q4_K row on the Go walker)
  meets an unmeasured Metal door (`metal-matvec`, `metal-decode-step`,
  observed). That is its own session, scoped, not attempted here.
- The `learn` shelf appends duplicate rows on identical re-teach (harmless
  under newest-wins, unpruned). The greeting is witnessed under a pty harness;
  Urs's first real terminal is its first human witness. Intel and the door-1
  re-witness stand as before.

## Proof

| check | verdict | exit |
|---|---|---|
| preflight: form-cli.fk / -inquiry.fk / -repl.fk | unresolved 0, chain clean | 0 |
| `tests/form-cli-band.fk` on fkwu | 1048575 | 0 |
| `validate.sh form-stdlib/tests/form-cli-band.fk` | four-way, 0 divergent | — |
| `form_cli_bootstrap_proof.sh` | behavioral proof OK | — |
| regen voice canary | ping → pong | — |
| `grounded what is sema?` | confidence 100, WELCOME.md | — |
| `homecoming-distillation-corpus-band.fk` (rows 1009+1010) | 32767 | 0 |

## The frontier question

**What names a branch born that never drew breath?**

The word is **stillborn** — written, compiled, shipped, present in every build,
and never once executed, because its guard compared the world to a value the
world never gives. 0-hit fresh. Corpus row 1010, landed under the counterweight:
404 rows, 404 admissible, max id 1010, dup rows 0 — probed in one cell, exit 0,
before the numbers were written down.
