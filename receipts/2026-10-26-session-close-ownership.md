# Session close follows owned work

An ordinary `form-cli ask "native bundle source identity"` returned its native
lookup, then exited 1 with `fkwu: form_error: session learning event write failed`.
The unconfigured session had no learning home. Its close path nevertheless tried
to append a settlement event.

`nsm-session-settle` now writes and waits only when the supervisor PID is positive
and belongs to the closing process. A lookup with no owned worker closes without
creating a learning home or touching another session's events. An owned worker
still settles after scope withdrawal; its completion and failure evidence remain
the responsibility of the process that admitted it.

Observed with the C-bootstrapped `fkwu` JIT:

- `./fkwu form/form-stdlib/tests/session-scope-band.bml`: 22 passed, model_calls 0.
  New cases cover missing-home close, an unowned live PID, successful owned-child
  reaping after withdrawal, and exact failure evidence with prior examples intact.
- `printf '%s\n' 'ask native bundle source identity' | ./fkwu form/form-stdlib/form-cli-repl.fk`:
  same native lookup text, exit 0. This establishes clean source-runtime closure;
  it makes no new claim about retrieval sufficiency or semantic quality.
- Read-only review confirmed the owned wait and failure-retention path remains
  reachable after withdrawal. The added failed-child observation passed.

No model generation or new retention permission was used for these checks.
Implementation and review used OpenAI gpt-6-astra. The useful distinction is
between permission to retain new experience and responsibility for work already
admitted: a quiet session creates no learning event, and owned work finishes whole.
