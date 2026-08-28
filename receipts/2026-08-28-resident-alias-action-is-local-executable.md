# Resident alias action is local executable grammar

Signed 2026-08-28 by Codex.

The previous alias attempt established a useful refusal: a short task alias
does not teach a model to regenerate a long hidden patch frame.  This movement
changed the response contract rather than relabelling that refusal.  The model
now emits one explicit, scannerless BML action while Form resolves the
caller-born alias to the already-held capability-bound frame.

## Grammar and boundary

The response grammar is exactly:

```bml
(agent-act emit-exact-frame p)
```

The live decoded-byte cursor retains only an expected-action-sized suffix.  It
does not wait for a token boundary, line, or complete generation quantum.  A
complete byte match resolves `p` through the local alias registry, checks that
it is a `repo-patch` task reference, and sends its held payload to the existing
Form patch guard.  `nothing`, `choice`, malformed aliases, a non-patch target,
`cut`, `undo`, and `timeout` remain distinct: they never manufacture a patch.

The pure grammar witness is `127`, after clean preflight.  It covers dynamic
alias resolution, the explicit BML action, split-byte recognition, bounded
cursor retention, absent/ambiguous/wrong-kind refusal, typed partial-action
timeout, and zero model/carrier/filesystem opening.

## Physical local witness

The already-running resident released its Metal session before the run.  With
the local `Qwen3.8-27B-Q8_0.gguf`, one bounded 48-token attempt reported:

```
task surface       (agent-task emit-exact-frame p)    31 bytes
action surface     (agent-act emit-exact-frame p)     30 bytes
route              repo-patch-alias-action
model response     (agent-act emit-exact-frame p)<STOP>
status             value
contribution       1
callback calls     1
source v2          1
intent durable     1
terminal durable   1
release ok         1
```

The task entered the same residence at `1787881733773` and was live at
`1787881746415`.  The action cursor recognized the byte stream at
`1787881748657`; guarded observation injection began eight milliseconds later,
was complete at `1787881758744`, and the run closed `value` at
`1787881759325`.  The local result carried 91 injected bytes / 35 IDs; the
former tool-role return for that same result would have used 47 IDs.

No HTTP service, llama-server, Ollama process, remote model, flattening route,
or model-supplied patch authority participated.  The fixture only changed from
`(do 1)` to `(do 2)` after durable intent and terminal records were observed.

## Next owed attempt

Generalize this successful one-action grammar into a held-out task family:
admit several caller-born aliases with separate capability contexts, rotate
them through one retained residence, and measure exact-action success,
ambiguity, timeout, undo, and refusal before using it for arbitrary repository
proposals.  In parallel, bind tokenizer diagnostics to the resident source so
opening the 27 GB GGUF is not paid per map.

The surprising teaching is that the model did not need to hold the full patch
in its emitted text; it needed an executable grammar that lets its intentional
act meet Form's held meaning.  The earlier `held` became gold because it named
the missing layer precisely enough to build it.
