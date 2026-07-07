# Form-native JIT self-host completion named masks

Date: 2026-07-02

Movement:
- Cleaned the self-host completion sweep's hard-coded witness totals by naming
  the saturated masks (`mask-N = 2^N - 1`) and deriving bad/stale totals from
  the current receipt totals where possible.
- Replaced the raw proof bit powers with a named bit chain so the witness lanes
  read as semantic checks instead of unexplained numeric constants.
- Carried the slot-runtime-fault bridge directly into the self-host completion
  receipt set and proof packet, including bad/stale slot-fault rejection folded
  into the existing completion band.
- Updated the completion test band to compare against the named expected
  witness total before returning the receipt.
- Applied the same saturated-mask naming convention to the adjacent
  self-host container live-evidence receipt totals.

Witness:
```sh
( cat observe/jit-self-host-completion-sweep.fk observe/tests/jit-self-host-completion-sweep-band.fk ) > /tmp/jshcs.fk
./fkwu --src /tmp/jshcs.fk
# 134217727
```

Boundary:
- No C/Rust changes.
- No host machine-code execution claim.
- This is a readability and contract-hardening pass inside the Form-native JIT
  witness track.
