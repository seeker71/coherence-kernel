# The daily walk, on the Mac

The walk runs directly through the local kernel. It uses registry model weights
and the host's optional Metal carrier in-process.

## Run

Follow the [C-bootstrap and freshness instructions](../AGENTS.md#ground-the-kernel-first-temporary-c-seed-shrinking-to-zero), then run from the repository root:

```sh
./form-run ./fkwu observe/movement-run.bml <<'FORM_MOVEMENT'
{"movement":"local walk","transcript":"","subject":"Local native movement"}
FORM_MOVEMENT
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
