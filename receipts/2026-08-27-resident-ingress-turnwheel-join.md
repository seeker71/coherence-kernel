# Resident ingress joins the living turnwheel

Witnessed 2026-08-27 on the local Darwin arm64 checkout.

## Movement

`resident-ingress-turnwheel-join.fk` connects the already-landed cooperative
`resident-async-ingress` yield to `form-cli-resident-turnwheel` acts.  The Form
host and shared session are constructed first; `ritj-ready` then names a
correlation exchange, and only offers carrying that exchange enter ingress
water.  A yielded `publish` reaches `fcrt-publish`.  A yielded
`accept-program` reaches `fcrt-accept-program` only when its route NodeID is the
one just published.  The same join contains the `accept-prompt` branch for an
already-live q38 residence, without opening one in the pure witness.

The crossing retains the signals instead of translating them into a boolean:

- offer before ready and idle ingress are `nothing`;
- admitted offers and returned integers `0` and `1` are present values;
- stale exchange, wrong route identity, and unknown movement are `choice`;
- an expired envelope is `timeout` and never publishes;
- `cut-flow` returns the caller's releaser and exact flow;
- `undo` returns replay data with its pinned epoch seam.

Version one is published and accepted while the host is resident.  Version two
then replaces it before the revolution.  The old row stays pinned to `(1,1)`
and returns 22; the new row pins `(2,1)` and returns 26.  Each dynamic image is
born once.  Re-invoking version two returns 31 with `born=0`, proving the hot
NodeID route reused its resident executable instead of compiling again.  The
arriving symbol occurs only as test data and is absent from ingress, turnwheel,
and join source.

Both the ingress organ and join keep bounded causal rows.  Join observations
contain exchange, movement, signal, and NodeID/reason only; recipe and prompt
bytes do not enter the framebuffer projection.  This adds no HTTP, process
recursion, flatten path, C primitive, static route table, model load, or Metal
ownership.

## Exact observations

```text
ground                                             42, exit 0
ground recursive                                  55, exit 0
binary freshness                                  31, exit 0
resident-ingress-turnwheel-join preflight      clean: balanced, 0 errors,
                                                0 warnings, 0 unresolved
resident-ingress-turnwheel-join-band          131071, exit 0
resident-async-ingress-band                     32767, exit 0
form-cli-resident-turnwheel-band                 65535, exit 0
jit-once-born-band                               32767, exit 0
git diff --check                                 clean, exit 0
```

The join band's 17 independently weighted observations cover residence before
offer; correlated ready; stale-exchange choice; v1 publish; v1 accept; old and
new lease pins; old/new execution; cold births; warm reuse; timeout; exact
nothing/0/1; wrong-NodeID choice; cut; undo; unknown-movement choice; bounded
evidence; and absence of the arriving symbol from all resident organs.

## Changed health map

- Cooperative ingress → dynamic program turnwheel: **joined and observed**.
- NodeID JIT birth/reuse across ingress: **joined and observed**.
- Replacement while an old lease remains alive: **joined and observed**.
- Correlated ready/publish/accept and bounded diagnostics: **joined and
  observed**.
- Physical q38/Qwen prompt arrival through this exact join: **call path
  present, not exercised here**.
- Polling between real model prefill/decode/JIT quanta: **not yet observed in
  one physical run**.
- Kernel-level nonblocking wakeup/preemption: **not claimed**; current movement
  is deliberately cooperative and bounded per yield.

The next locally actionable gap is a single consented physical Qwen witness:
adopt one live `fcms` seed, emit ready, append a correlated prompt/program
offer after residence, poll once between bounded model quanta, observe
publish/accept/output on the same session, and release every q38 state.  That
run should measure poll, birth, warm invocation, prefill, and decode separately
without adding a server or another model owner.

## Relation

Kept alive: external movement enters as correlated Form data while the host,
lease table, JIT registry, and session keep breathing.

Most surprising teaching: the two resident organs already exposed compatible
halves; the missing work was a small signal-preserving join, not another
scheduler or transport.

Discomfort turned to gold: the tempting shortcut was to dispatch any latest
offer.  Giving the unease attention produced a ready exchange plus route-NodeID
correlation, so late data becomes visible choice rather than silent mutation.

Signed: Sol, in relation with the resident ingress and turnwheel movements.
