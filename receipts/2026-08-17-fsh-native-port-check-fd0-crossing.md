# fsh native-port-check fd-0 crossing

Date: 2026-08-17  
Signed: Codex

## Observed crossing

A private Form-emitted carrier received exactly one terminal line:

```text
native-port-check 0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef
```

The carrier's Form table reached `read_line` (emitted tag 114), parsed the line
with `sh-parse`, unwrapped only the singleton `__seq__ -> __andor__ -> __pipe__`
shell shape, and then called exactly five private tag-256 nodes.  Its bounded
content-blind return stream was:

```text
fctp-opened-private
fctp-native-capability-pending-discovery
fctp-run-deterministic-complete
fctp-seal-closed
fctp-discarded-private
```

The matching raw mailbox exchange was absent afterwards.  The retained witness
`manifest.kv` existed and had mode `0600`.

An uppercase final hexadecimal byte was refused in Form before the port:

```text
schema=fsh-turn-process-spec-v2
decision=refused-invalid-exchange-id
```

## Gates

- Direct bootstrap: `42`, `55`, freshness `31`, and numeric list passed.
- The private profile and its structural band preflighted clean.
- The structural band returned `63`: one fd-0 ingress node, five fixed tag-256
  calls, and all fixed lifecycle literals were present in the serialized table.
- Form itself wrote the three temporary C translation units.
- One absolute, fixed `/usr/bin/cc` invocation compiled them with strict warnings.

## Boundary

This is a private emitted carrier, not the published `form-cli`.  The public
`turn` command remains an honest native-capability refusal.  The lifecycle's
`native-capability` record means pending discovery only; it did not select or
use Metal, a model, network target, or caller-provided executable.  The compiler
is still an explicitly named outside bootstrap organ.  The earlier zsh-driven
closed-table fixture and its test harness were retired rather than carried
forward.
