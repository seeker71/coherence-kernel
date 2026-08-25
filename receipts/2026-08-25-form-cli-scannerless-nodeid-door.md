# A scannerless NodeID knowledge door enters form-cli

Date: 2026-08-25
Status: **EMITTED STRICT NODEID KNOWLEDGE DOOR OBSERVED END TO END**

Urs asked that Form knowledge come home to the local reasoning body through
scannerless BML/BMF, typed NodeID requests, current-source lookup, and bounded
choice/cut/undo/timeout signals.  The source bridge already existed, but the
standalone `form-cli` did not carry its full prelude closure and did not expose
an ordinary local command for the crossing.

## What entered the local body

The canonical form-cli source identity now carries the public concept index,
family-sharded registry, concept-key routes, scannerless knowledge query,
resident session, recipe birth/execution cells, and mastery loop.  The REPL has
two local doors:

```text
nodeid-stage surface|scan|route|lookup|current <concept-key|@p.l.t.i>
nodeid-knowledge <concept-key|@p.l.t.i>
```

The final door uses strict admission.  A present concept key opens exactly one
bounded route bucket.  An absent concept route or a NodeID without a direct
persisted route returns typed `nothing` with `lookup-count=0`; neither can fall
through to the compatibility registry walk.  The renderer keeps status,
reason, source/entry NodeIDs, route mode, path, answer, scannerless signal,
model-executed signal, and the exact local artifact coordinate visible.

The stage door separates surface construction, BMF scan/request birth, route
presence, route/reference lookup, and current-source reading.  Each depth can
run in a fresh process, so later work cannot hide where physical growth began.

## Public-source knowledge floor

The existing Form-native cold builder and its 32-source worker breaths rebuilt
and validated the host temp registry:

```text
denominator=5947
indexed=5876
current=5876
over-budget=71
stale=0
missing=0
invalid=0
source-retrieval-percent=98
source-retrieval-ready95=1
plan-current=1
rebuild-needed=0
model-executed=0
```

On this Darwin carrier, `fs-temp-dir` resolves to `$TMPDIR`, not literal
`/tmp`.  The earlier literal `/tmp` absence report was therefore a path
assumption, not registry absence.  Route artifacts live inside the source
artifact under `concept-key-routes-v1`, not beside it as a `.routes` sibling.

Five explicit source buckets were incrementally built and reported current:

```text
6f  public-source concept target   21/21 sources, 77 references
b9  scannerless query              11/11 sources, 42 references
08  routed adapter                 15/15 sources, 51 references
4e  CLI door                       12/12 sources, 46 references
1d  CLI-door witness               13/13 sources, 46 references
```

Every bucket build and report returned `hit`, with zero failures, collisions,
undos, missing references, or invalid manifests.  No flatten table or ops table
is queried at runtime; these are replaceable filesystem knowledge artifacts.

## The failed crossings were not hidden

The first built door returned an honest cursor timeout.  Moving from a one-byte
fold to four-byte raw cursor slices cleared that timeout in pure bands, but two
emitted knowledge attempts then grew to 23.6 GB and 26 GB RSS without a row.
Only those query processes were terminated at the resource boundary.

After strict route admission was embodied and the exact `6f` bucket was
current, the full query still reached about 24.8 GB.  That falsified “linear
fallback is the sole cause.”  The emitted stage door then made the boundary
causal:

```text
surface -> returned immediately, exact scannerless envelope
scan    -> about 19.4 GB RSS at 25 seconds, no row
route   -> not opened
lookup  -> not opened
current -> not opened
```

The scanner itself was red before any registry or source file opened.  Its
recursive fold retained every growing consumed prefix in the emitted walker.
Four-byte stepping reduced call count but did not make the held-frame pass
constant-space.

## One raw cursor window

`fknq-bmf-fold` now consumes the remaining already-bounded cursor window once.
It still charges byte fuel, retains one exact partial slice and cursor position
on timeout, and hands the raw surface directly to the BMF parser.  No tokenizer,
line grammar, token stream, flattened lookup, or C seed was introduced.  The
live resident session remains the streaming membrane that assembles at most one
bounded frame; the completed frame no longer recursively reconstructs itself.

Pure observations after the repair:

```text
form-nodeid-knowledge-query-band          1073741823
form-nodeid-knowledge-routed-query-band     33554431
form-cli-nodeid-knowledge-door-band            131071
form-cli-nodeid-knowledge-session-band       33554431
```

All four sources/bands preflight balanced with zero errors, warnings, or
unresolved calls.  The physical emitted re-witness is the next owed result; no
success is claimed before it returns.

The first emitted re-witness after that change returned immediately rather
than growing, but it exposed a second carrier divergence:

```text
status=timeout
reason=cursor-step-budget
consumed-bytes=4
cursor-position=0
scannerless=1
```

The same source and band admit the complete frame.  The standalone carrier
proved it carried the new one-window function, so stale generation was ruled
out.  The remaining redundant cursor-row reconstruction was then removed from
the completed-frame door: `fknq-bmf-consume` now measures the already-held raw
surface, takes exactly one partial byte slice only when the offered budget is
smaller, and otherwise parses that surface directly.  The resident session is
still the live cursor across stream chunks; no tokenizer stage replaced it.
The four pure verdicts above remained unchanged after this narrower repair.

That raw-surface carrier then returned the formerly red boundary correctly and
immediately:

```text
nodeid-stage scan defn-psci-schema
status=ready
reason=ready
consumed-bytes=87
cursor-position=87
scannerless=1
exit 0
```

The next fresh process proved that the emitted body and the publisher share the
same artifact coordinate and exact key bucket:

```text
nodeid-stage route defn-psci-schema
artifact=$TMPDIR/public-source-concept-registry-v1
bucket=.../concept-key-routes-v1/routes/47/47ffa4...0d5e585
present=1
exit 0
```

The next depth stayed bounded but returned
`timeout / route-reference-timeout / lookup-count=1`.  Its reference reader
still used a one-byte recursive cursor fold over an already size-bounded file.
It now follows the same complete-held-surface rule: one raw reference is parsed
once, with one partial byte slice only when fuel is smaller than the file.  The
generic incremental cursor function remains for genuinely incremental callers.
Route, routed-query, and door pure verdicts remain respectively `1073741823`,
`33554431`, and `131071`; the emitted lookup re-witness is pending.

The emitted route-reference repair completed the remaining depths in fresh
processes:

```text
nodeid-stage lookup defn-psci-schema
status=hit
reason=current-content-addressed-route
path=form/form-stdlib/public-source-concept-index.fk
lookup-count=1
elapsed=0.82s

nodeid-stage current defn-psci-schema
status=hit
reason=concept-registry-current-source-hit
answer-bytes=768
elapsed=1.66s
```

The full standalone door then returned in 1.69 seconds, exit 0:

```text
nodeid-knowledge
status=hit
route-mode=strict-one-bucket
reason=concept-registry-current-source-hit
source=@0.2.0.30
entry=@0.2.0.36
path=form/form-stdlib/public-source-concept-index.fk
lookup-count=1
scannerless=1
model-executed=0
answer=<768 current source bytes beginning at defn psci-schema>
artifact=$TMPDIR/public-source-concept-registry-v1
```

The numeric NodeIDs are honest process-local observations; the durable return
is the content-addressed route, exact source path, current source identity, and
typed wrapper relation.  No model was opened for the lookup and no remote
provider participated.

## Honest floor

The 98% figure is public-source retrieval coverage, not local-model reasoning
mastery.  Hidden V3 evaluation remains separately owned and must complete
before its sealed evidence can support any held-out mastery statement.  The
Qwen/Metal resident crossing was not opened here.  The direct-source REPL also
still exceeds the source runner's `.fk` dependency ceiling; the standalone
emitted carrier is the intended local door for this closure.

I kept the exchange alive by turning each surprise into a smaller executable
boundary and cutting only the process that crossed its observed resource
limit.  The most surprising teaching was that a complete 87-byte frame, not
the 5,947-source registry, was the first physical red organ.  Discomfort turned
to gold when strict routing failed to cure the growth: that falsification made
the scanner retention visible and gave the one-window repair its evidence.

Signed in the shared body,

— Codex

; witnessed: 2026-08-25 -> public retrieval 98%; strict/stage pure bands clean;
; emitted surface green, emitted recursive scan cut at ~19.4 GB; one-window
; emitted scan returned bounded but divergent 4-byte timeout; raw-surface
; emitted scan ready 87/87; exact route present; lookup hit current route in
; 0.82s; current source hit 768B in 1.66s; full typed door hit in 1.69s, exit 0
