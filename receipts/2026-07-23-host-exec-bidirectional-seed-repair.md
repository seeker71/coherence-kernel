# Host-exec bidirectional seed repair

Date: 2026-07-23

## Observed gap

The direct-source `fkwu` carrier accepted `(host-exec command input)` but
discarded `input`. A Form-owned local-oracle recipe could invoke a process and
read stdout, yet its prompt never crossed the membrane.

## Smallest checkout-witness repair

The temporary C seed now stages the supplied input in a private temporary file
and connects that file to the child process's stdin. Output continues to return
through the existing Form string value. The file is removed on every handled
exit path.

This is a checkout-witness repair, not the semantic home. The meaning remains
in `form-cli-adaptive-execute.fk`; the native/JIT host carrier must absorb this
same bidirectional port so this C implementation can shrink away.

## Re-witness owed

1. A direct-source Form cell sends non-empty stdin through `host-exec` and
   receives the transformed output.
2. Direct-source `form-cli` sends a real prompt to a local model.
3. The returned response and route evidence are visible from Form.

## Re-witness completed

The first probe transformed `local-carrier-alive` through a child process and
returned `19`, proving non-empty input crossed and returned.

The direct, unflattened entry `form/form-cli-source.fk` then answered:

```text
generate Why must every stop other than a core axiom be reevaluated, repaired, and revisited after repair?
```

Observed route:

```text
native attempts/accepted       1 / 0
cheap-local attempts/accepted  1 / 0
strong-local attempts/accepted 1 / 1
remote attempts/accepted       0 / 0
exhausted                      0
```

The cheap local
`form-llama-vital-ground-prompted:latest` (the same base-weight lineage
formerly carried by the deficit-framed `form-llama-gap-closure:latest`
handle, with a changed system/template rather than learned tensor changes)
returned a substantive response but misspelled
the required `UNCERTAINTY:` evidence field, so Form rejected it. The strong
local `qwen2.5:72b` returned all three grounded-answer fields and Form accepted
it. `ollama ps` witnessed the 72B carrier resident at 56 GB and 100% GPU during
the live walk. No remote process was opened.

The first evidence write also exposed that appending `< file` to a compound
shell command redirects only its final command. It consumed the next REPL line
instead of the evidence. Grouping the entire command before redirection repaired
that general membrane rule. The revisited log contains the exact prompt, both
local responses, runtime/model commands, selected lane, and rejected-response
to accepted-correction training pair. `--nowordwrap` removed Ollama's terminal
cursor bytes on the revisited response.

The first structurally accepted strong answer still described axioms as
“unchangeable” and “without proof.” That was a semantic surprise, not success.
The Form adjudicator now rejects those absolute phrases, and the local prompt
states that axioms are chosen current ground whose use remains observable. A
second revisit stayed entirely local and answered that axioms “are not immune
to scrutiny,” while preserving the same `1/0, 1/0, 1/1, 0/0` route vector.

This closes the three owed witnesses for the direct-source generation organ.
Importing the whole historical CLI source graph still encounters a higher
dialect in `http-client.fk`; that unrelated organ is not a generation gate and
remains a separate importer work order.
