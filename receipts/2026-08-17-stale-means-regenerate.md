# Stale means regenerate

The first report said the standalone form-cli carrier “refused because stale.”
That was the wrong causal sentence. The Form source was healthy. Its generated
table/C carrier still embodied an older source identity, so the normal next
movement was regeneration and re-witnessing—not stopping.

## What actually stopped regeneration

The first retained regeneration reached the fkwu self-host flattener and then
answered:

```text
fourth_flatten_sources:9: PIPESTATUS[@]: parameter not set
```

`form/scripts/fourth-arm.sh` is sourced by both Bash and Zsh entry points. Its
pipeline read Bash's `PIPESTATUS` while `regen_form_cli_bootstrap.sh` ran under
its declared Zsh shebang with nounset enabled. A source change therefore made
the carrier stale, and the path meant to refresh it stopped on a shell-dialect
variable before it could publish anything.

The repair no longer reads either shell's pipeline-status array. It captures
the fkwu walk into a private raw file with a guarded scalar exit status, then
extracts marker-framed table bytes only after the walk succeeds. The same
function now parses under Bash and Zsh.

## Re-witnessed movement

Fresh bootstrap publication:

```text
regen: flatten fkwu self-host (form-cli table)
regen: voice canary — ping answers pong
regen: form-cli-emitted.c (1089820 bytes) stamp=b755e69afade1d99
functions=1993 nodes=60863 strings=2027 tokens=300342
```

Fresh standalone carrier:

```text
flatten: bootstrap table (no Go)
emit: bootstrap (no Go)
built form-cli (2903032 bytes, self-contained; carries 1761402B of its source)
```

The emitted C and binary are checkout carriers, not the home of the new
meaning. The TOKEN identities, surface evidence, bounded evaluator, provenance
axes, substrate binding, and route choice remain in
`form/form-stdlib/form-cli-ask.fk`. The generated carrier remains a shrink
target.

## Source and carrier now agree

Both paths answer an unregistered/negated query with the same honest decline:

```text
[ask: native and local lanes are quiet]
local-lane:quiet
synthesis-lane:quiet
```

Both paths execute the registered multilingual TOKEN recipes. Representative
results are English `42`, Chinese `15`, Japanese `-6`, German `1`, and Form/PL
`42`.

The complete evidence is:

```text
form-cli-token-route-band             2097151  exit 0
form-cli-band                         1048575  exit 0
homecoming-route-suite                32 cases
  before                              quiet 32
  after                               token 24 / quiet 8
  paired wins/losses                  24 / 0
  negative byte-identical             8
  wrong                               0
standalone carrier TOKEN suite        31       exit 0
```

The most surprising teaching is that the integrity guard was doing its job:
the defect lived in the refresh path behind it. Discomfort became gold when
“stale” stopped being treated as an explanation and the retained run exposed
the one dialect-specific variable that prevented regeneration.

Signed: Codex (OpenAI), 2026-08-17.
