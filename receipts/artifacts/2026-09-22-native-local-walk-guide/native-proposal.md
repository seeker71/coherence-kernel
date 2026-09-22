# The daily walk, on the Mac

The walk runs directly through the local kernel; the model runs in-process. No
external assistant, no loopback server, no transcript discovery, no schedule.

## Run

Build the kernel once per checkout (see AGENTS.md for the full C-bootstrap and
freshness recipe):

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c
```

Then make one call, replacing `<date>` with today (YYYY-MM-DD):

```sh
printf '%s\n' '{"movement":"local <date>","subject":"Local movement <date>: rows and the redrawn ladder","body":"Walked by the body in one call (observe/movement-run.bml) on the Mac: the in-process model, the native single call, the flow meter, the page redrawn from the ledgers."}' | ./form-run ./fkwu observe/movement-run.bml
```

## Inputs

Stdin is one JSON object; every field is optional.

- `movement`: name for the run (default `movement`).
- `voice` (1/0): run the in-process model voice (default 1).
- `compare` (1/0): run the native compare (default 1).
- `transcript`: path to a supported transcript, or empty. Leave it empty unless
  the operator deliberately supplies the relevant supported transcript. Empty is
  unmeasured, not zero cost.
- `lane`, `adapter`, `model`, `max_reply_tokens`: voice/model options.
- `land` (1/0): land the movement (default 1). `land=0` skips landing.
- `restart`: optional restart command.
- `paths`: extra paths to include in the landing.
- `subject`, `body`: commit message fields.

## Outputs

The call prints one JSON line with:

- `movement`: the run name.
- `voice`: source, model, predicted tokens, axes, receipts cited, ms, answer.
- `compare`: the native compare result.
- `flow`: provider_calls, native_fkwu_calls, non_form_calls.
- `page_bytes`: size of the redrawn page.
- `landing`: `landed` only when the push reached origin; otherwise a held
  reason. Read the actual landing and push outcomes; a completed process does
  not establish answer quality.
- `push`: the push's own exit and last line.
- `health`: readings, unease, spool.
- `census`, `dispatches`, `planes`: census window totals.

## Landing

`land` defaults on. It pushes the current branch. Use `land=0` to skip.
Actual landing and push outcomes must be read from the output; do not assume
success.

## Failure

A failure asks for inspection and repair from retained evidence before
continuation. An observation timeout is not terminal and must not trigger
another model admission.

## Bootstrap

See AGENTS.md for the C-bootstrap and freshness requirements. Do not duplicate
that recipe here.
