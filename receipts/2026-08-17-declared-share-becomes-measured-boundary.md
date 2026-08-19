# Declared share becomes a measured boundary

The correction was exact: **50 / 20 / 30 was not measured**. It was a typed
allocation presented on the live surface. A declared number cannot become an
observation because its sum is 100.

The live door now reads an explicitly bound Codex rollout in Form. Python,
SQLite, and the Python test carrier were removed. The 51 MiB rollout exceeds
one kernel response window, so the Form witness persists a private accumulator
after each complete JSONL boundary. An interruption resumes the same turn.
Only a fully completed and reconciled accumulator is promoted to `observed`;
progress and candidate rows remain private and ignored by git.

The meter names its scope narrowly:

- native: completed `@form fkwu` receipt rows found in the final 512 bytes of
  outputs whose call requested `fkwu`;
- local: other completed tool-output events, plus non-fkwu receipt rows seen
  in those native-request tails;
- remote: provider-usage events;
- semantic contribution: **not measured**.

Token volume and tool wall time do not affect the share. Largest-remainder
normalization turns the observed counts into percentages deterministically.

## Working observation

For completed turn `01a00e3e-1c57-76e0-a310-1b9574f91f7a` in thread
`019ff50c-fcb0-7d73-8a2a-a141473df4ea`, Form reconciled:

```text
events native=106 local=233 remote=320 total=659
share  native=16  local=35  remote=49  sum=100
tool calls=317 outputs=317
form receipts=121 failures=15
native receipts=106 failures=13
form bytes stdout=78052 stderr=20626 shown=98678
measurement-health=observed semantic-contribution=not-measured
```

The failed receipts remain visible. They are completed native boundary events,
not successful semantic outcomes.

## Witness

```text
form-cli-turn-evidence-live preflight       clean
form-cli-turn-evidence-refresh preflight    clean
form-cli-turn-evidence-live-band            1023, fkwu, exit 0
form-cli-turn-evidence-band                 4095, fkwu, exit 0
form-cli-share-run                          observed 16/35/49, sum 100
Python live carrier                         removed
```

The optional proof-sibling binaries were absent in this checkout, so this
receipt does not turn the live band's fkwu witness into an unrun four-way
claim.

The surprising teaching is that resumability is part of measurement honesty:
an observation may need many interruptions, while its identity remains one
completed turn. Discomfort turned to gold when the first reconciled candidate
showed an empty model identity; the candidate stayed withheld, the top-level
`turn_context` shape was witnessed, and the next complete walk became valid
without changing its counts.

Signed: Codex

; witnessed: 2026-08-17 -> Form-native completed-turn boundary share observed
;   as 106/233/320 events and 16/35/49 percent; semantic contribution unmeasured
