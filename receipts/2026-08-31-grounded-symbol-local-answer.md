# Grounded symbol becomes one local answer

Signed: Codex, 2026-08-31

## Crossing

The high-grammar BML resident authority now names `ground ` as a distinct
task form:

```text
ground <symbol> :: <question>
```

The Form action first resolves `<symbol>` through the source snapshot that was
born at peer admission. A one-valued route alone may read its caller-owned
current source. The source identity and size are checked again after that read;
a changed source becomes `stale`. A scannerless cursor then follows exactly one
balanced `(defn <symbol> ...)`, including strings, escapes, and line comments.
It is not a fixed byte table and it does not search other paths.

Only a `hit` observation enters the direct resident local-model user turn. A
malformed task, absent symbol, plurality, absent context, stale source, or
missing definition returns typed Form evidence with no model turn. The model
receives the attributed source definition and question, not a path or ambient
repository authority.

## Local live answer

One resident `fkwu` peer was given the sealed local
`Qwen3.8-27B-Q8_0.gguf` and one task:

```text
ground psci-schema :: Reply only with the exact string returned by this source definition.
```

Its framebuffer and terminal evidence were:

```text
admit-prefill-ms=281995
bootstrap=READY
route=grounded-answer
callback-calls=0
tool-status=symbol=value;current-source=hit;model=value;lookups=2
generated=11
response=public-source-concept-index-v1
release-ok=1
```

The source definition itself returned that exact string. There was one local
model residence and no callback/tool forwarding from the peer; the 11-token
decode is local-model work, not a provider request. This is a concrete zero
remote-model-call route for the task, rather than an estimated token saving.
Cold admission remains material and observable; follow-on tasks can reuse its
resident weights and KV rather than paying this prefill again.

## Verification

- `form-cli-peer-grounded-symbol-answer-band.fk` -> `127`
- `form-cli-peer-grounded-symbol-source-live-run.fk` ->
  `route=grounded-answer`, current source `hit`, two lookups, zero callbacks
- `form-cli-peer-live-grammar-band.fk` -> `66060287`
- BML authority cache -> `bml-cache state=ready bounded=1`
- Preflight clean: `form-cli-peer-grounded-symbol-answer.fk` and
  `observe/form-cli-peer-agent-live.fk`
- `git diff --check` clean.

## Next stone

Keep the resident alive across a compact series of grounded questions and
measure hot per-turn source observation plus decode timings. Then let the
framebuffer correlate each source admission, decode checkpoint, and terminal
answer without exposing source or answer bytes. The next question is not
whether a remote loop can answer this one—it already stayed local—but how much
of the recurring Form question surface can stay inside the warm resident.
