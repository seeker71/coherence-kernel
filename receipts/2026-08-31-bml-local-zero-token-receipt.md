# Local zero is a value; the ratio waits for its denominator

The direct `form-cli` route now carries its provider-boundary result as
high-grammar BML.  `form-cli-remote-token-evidence.bml` defines a compact,
NodeID-addressed comparison receipt:

- a provider baseline is settled, belongs to the same task NodeID, has at
  least one provider call, and has a positive `last_token_usage` total;
- a local direct result is settled with `provider-calls=0` and
  `remote-tokens=0`;
- only those comparable rows produce basis points (`10% == 1000 bps`).  A
  missing, zero, stale, or different-task baseline returns `nothing`; it is
  never changed into the numeric zero of the local result.

`form-cli-peer-direct-answer-bridge.bml` emits the local receipt as a compact
`<|form:remote-token-comparison|>` frame.  It has no task or answer bytes.
The contribution turnwheel joins it into its existing one physical append, so
failure retains the exact staged value and success remains the only promotion
point.

## Evidence

- `./fkwu form/form-stdlib/tests/form-cli-remote-token-evidence-band.fk`
  returned `131071`: valid same-node baseline/direct rows yield `0 bps` and
  pass the 10% predicate; an exact `1000 bps` candidate passes; absent,
  different-node, and zero-denominator baselines remain `nothing`.
- `./fkwu form/form-stdlib/tests/form-cli-peer-contribution-turnwheel-band.fk`
  returned `16777215`.  Its direct `fcms-none` sentinel establishes that the
  BML branch does not reach the model observer, and its durable frame contains
  `source=local-direct-no-provider`, `remote-tokens=0`, and
  `ratio-bps=nothing` beside the existing direct route evidence.
- Both BML source cells re-lowered into their local `.bml.fkb` caches.  No
  provider, HTTP, llama-server, or Ollama crossing was made for this movement.
- The self-watch panel (`observe/lane-counsel-run.fk`) reported
  `lastms=0`, `tpot=0`, `p95=0`, `kvpct=0`, `hopper=0`, `icemiss=0`,
  `fails=0`, `touts=0`, and `admit=-1` on its first reading.  The `admit=-1`
  is the live diagnostic gap: this panel does not yet see the isolated
  cache-image adoption cost measured by the turnwheel band.

The already-live resident predates this BML frame, so it correctly continues
under its pinned image.  A successor birth is the remaining adoption step;
there is no claim that pushed source has retroactively entered its memory.

The full contribution-turnwheel verifier still occupied one CPU core for about
90 seconds even with a valid image.  That is an observed cache-adoption gap,
not a reason to call a remote model or to hide the timing behind a green
number.  The next core movement is to make the large image's import/adoption
observable and reduce it without changing the warm resident's one-model,
one-KV continuity.

I kept the exchange alive by making the honest `nothing` executable rather
than allowing a local zero to impersonate a comparison.  The surprising
teaching is that a task NodeID is enough to join the two boundary receipts;
the discomfort of the absent denominator becomes a clear next door instead of
an invented percentage.
