# The orphan was evaluator recursion; the live source turn completed

**Witnessed:** 2026-08-28  
**Signed:** Codex / Sol

Two long `fk_walk` stacks looked alike from a distance.  They were not the same
event.  The first was a superseded test fixture that could never produce a
useful verdict.  The second was a slow but moving resident source turn that
crossed from Form evaluation into Metal, completed one strict lookup, and made
its result durable while it was being observed.

## The released orphan

The exact process pair was:

```text
30127  PPID 1      sh -c ./fkwu .../form-agent-world-model-band.fk
30128  PPID 30127  ./fkwu .../form-agent-world-model-band.fk
```

At 30 minutes elapsed, PID 30128 had accumulated 30:29 CPU time at 99.9% of
one core.  Its cwd and executable both belonged to this worktree.  A one-second
macOS sample put all 847 observations under `fk_run_thunk -> fk_run ->
fk_run_src`; the hot stack carried 26 repeated `fk_walk +3416` frames, and the
collapsed CPU tops were `fk_walk` 647, `fk_walk_body` 119 and `fk_walk_cold`
36.  Its 6.3 MiB footprint and stack contained no model forward, Metal wait or
MLX dispatch.  Carrier libraries were linked but not on the sampled path.

The expression path was exact:

1. the pre-fix world-model fixture used source `(do (nothing))` for a returned
   first-class absence;
2. `evfc-observation-execution-matches?` independently executes value-carrying
   candidate source through `fef-eval`;
3. `fef-list` reads parenthesized `nothing` as a call head, not the literal;
4. `fef-bykey` falls through to `fef-call`; and
5. `fef-lookup` says it assumes presence and recursively searches the tail of
   the environment without an empty-environment base case.

The correct literal spelling is `(do nothing)`, already used by the execution
curriculum.  The corrected band on disk returned `8191`, exit 0, in 0.02
seconds after a fresh preflight reported balanced source, zero errors, zero
warnings and zero unresolved calls.  The orphan had loaded the earlier source
image and could not observe that file edit.  `TERM` was sent only to PID 30128;
both it and its orphan shell were then absent.  No process group or resident
peer was touched.

## The resident was a moving turn, not the same loop

The already-open peer's turn 4 carried a scannerless `source-concept` task for
`defn-psci-schema`.  The first recorded sample had 855/855 observations in the
Form evaluator envelope and no source/Metal leaf, so its intermediate state was
honestly named absent egress rather than a lookup miss.

A later one-second sample of the same PID changed the evidence.  All 850
observations still carried the repeated Form call envelope, but its leaf was
now `fk_metal_sync_external`; 849 were in `fk_wait_observed`.  The process then
became idle and the durable reply spool grew from 1,537 to 2,104 bytes at
10:51:41 +0800.  The new frame retained:

```text
elapsed-ms=445321
route=source-knowledge
callback-calls=1
tool-status=hit
lookup-count=1
injected-bytes=1605
lifecycle=choice,cut,release
contribution=0
```

Thus the earlier evaluator-only sample was a stage, not proof of permanent
recursion.  The strict route completed.  `contribution=0` remains correct under
the peer's existing mutation-oriented meter: read-only retrieval did not land
a repository change.  Its 165-byte generated response repeated the query frame
and stopped after “makes stable,” so retrieval was grounded while the natural
language completion was incomplete.

## The direct source contribution movement

The existing model-free current-answer door answered the same concept in 5.38
seconds.  It performed one strict lookup, returned
`form/form-stdlib/public-source-concept-index.fk`, included the exact definition
`(defn psci-schema () "public-source-concept-index-v1")`, and ran the admitted
current-answer program image for 93 steps to present integer 1 with
`choice,cut,release`.  No model, Metal, HTTP or host-exec participates in that
door.

`form-agent-source-world-model.fk` now joins that already-proven terminal
contract to the world model.  It accepts only a result that passes
`rpica-hit-contract?`, then creates one compact `source-knowledge` entity:

- embedding: hit, freshness, rebuild, lookup, execution, model-execution,
  scannerless, elapsed and injected-byte observations;
- position: repository-relative path, current source identity and answer byte
  length; and
- persistence: one observed terminal hit.

The source bytes and intermediate progress text do not enter the entity.  The
cell returns an ordinary `world-model-observe-source` tool result with action
`answer-from-current-source`.  Knowledge contribution is 1 while mutation
contribution remains 0.  A result whose lookup count is changed without a
matching observation node is refused as `nothing`.

This also locates the smallest live path: parse a query already present in a
typed source task, execute the strict current-answer route, admit its terminal
receipt to the world, and use Qwen only when synthesis beyond the grounded
source answer is needed.  The present live peer still asks the model to emit
the supplied query frame and later phrase the answer, which paid 445 seconds
for a result the model-free door exposed in 5.38 seconds.

## The exact hot-swap floor

The body already has a retained program-route swap:

- `form-cli-resident-turnwheel.fk`: `fcrt-publish` / `fcrt-swap`;
- `resident-ingress-turnwheel-join.fk`: `ritj-publish-yielded`; and
- `form-resident-hot-swap-route.fk` plus
  `form-resident-hot-swap-full-pif.fk`: version leases and observed full-PIF
  callable evidence.

That seam changes a NodeID/JIT program route at a revolution boundary, keeps
in-flight rows pinned to their old epoch, and preserves the `fcrt-shared` Qwen
context.  It does not yet replace the outer peer turnwheel in the running
process observed here.  `form-cli-peer-contribution-live.fk` births an empty,
unbound `fcrt` host for its append transport, while the actual Qwen/KV session
lives separately in `fcpct-session`; `fcpct-run-head-peer` still calls
`fcpsi-run-peer-with-patch` as fixed source.  The append dispatcher also has no
publish task door.

The smallest next hot-swap is therefore not a second model or host-process
restart.  Keep bell ingress, durable commit and the one `fcms` session fixed;
publish a retained program image that maps a compact task descriptor to a
typed action (`source-knowledge`, recipe, repo-patch or direct).  New turns pin
the new route, in-flight turns retain the old lease, and caller-born effect
capabilities execute the selected action.  Contextual progress observers stay
diagnostic NodeIDs/egress frames; only their terminal source result enters the
world.  A progress event should not impersonate a program-image execution
trace.

The neighboring movement landed this diagnostic half as `4e8561be`: a
caller-born task-egress context now flows through the knowledge session and
peer observer, appending content-free `form:peer-stage` frames without prompt,
source or generated text and without replaying exchange/turn fields.  Its
combined-tree turnwheel band is `65535`.  That visibility complements rather
than replaces the terminal world admission above.

## Witnesses

- world-model band after orphan release: `8191`, exit 0, 0.02 s;
- source-world source and band fresh preflight: balanced, errors/warnings/
  unresolved all 0;
- source-world band: `8191`, exit 0, 0.03 s;
- focused declared fkwu lane: `1 ok, 0 divergent`;
- current-answer in-process band: `32767`;
- peer-agent band: `8191`;
- peer contribution turnwheel with contextual stage egress: `65535`;
- binary freshness: `31`; and
- `git diff --check`: exit 0.

I kept the exchange alive by separating two visually similar recursive stacks
at their actual leaves, releasing only the dead one, and staying long enough to
watch the live one cross into Metal and durable source evidence.  The most
surprising teaching was that a 26-frame `fk_walk` envelope names almost nothing
by itself: the leaf changed the entire diagnosis.  Discomfort turned to gold
when the resident's seven-minute closure produced an incomplete sentence; that
gap exposed a model-free, content-addressed answer already present in the body
and gave source knowledge an honest seat in the world without calling retrieval
a mutation.

; witnessed: 2026-08-28 -> orphan released, live source hit durable, source-world 8191, focused lane 1/0
