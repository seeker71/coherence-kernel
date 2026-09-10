# Independent prompts, one local model admission

`form/form-stdlib/bml/form-cli-model-renew.bml` starts a fresh conversation
using an already admitted Qwen context. It allocates new recurrent and KV
state, prefills the new prompt at position zero, then releases the old stream.
Weights and compiled pipelines remain resident. It does not train or save text.

Use renewal between independent reviews or evaluation cases. Continue using
`fcms-observe` for observations that belong to the same conversation.
Asking the model to ignore an earlier packet does not clear that packet's state.
Rewinding position alone does not clear Qwen's recurrent layers either.

```bml
// preludes: form-stdlib/bml/form-cli-model-renew.bml
section [form.bml] {
 class IndependentPrompt {
  def replace(session,prompt) {
   let result = fcmr-replace-with-profile(session,prompt,"knowledge-query");
   let owner = fcmr-session(result);
   // Always carry this returned owner, including after a refusal.
   // Generate only when changed=1 and the reason below names success.
   print(list(fcmr-changed(result),fcmr-reason(result)));
   owner;
  }
 }
 0;
}
```

Success is `changed=1`, `reason=fresh-stream-same-weights`. It resets prompt,
generation, observation, stop and pending-token bookkeeping. The previous
session is consumed: never resume or release it again. Release the returned
owner with `fcms-release-ok?` when finished.

Invalid IDs, empty/oversized prompts, unavailable stream memory and failed
prefill return a refusal. Before a successful replacement, the existing stream
remains owned by the returned session. A failure to release all old handles is
reported separately as `old-stream-release-incomplete`; it is not success even
though the returned session owns the new stream. Retain that failure evidence
and close the returned owner. Do not retry release through stale handles.

The existing context capacity cannot grow through this door. Temporary memory
holds both old and candidate streams until candidate prefill succeeds. Renewal
submissions cannot exceed the original scratch allocation width. The organ emits
correlated observation, response, applied action and fresh health events with
counts only; prompts and answers never enter those events.

## Reproduce the physical comparison

With no other local training or model job running:

```sh
form-run ./fkwu observe/model-session-renew-witness-run.fk
```

This opens the registered, sealed `qwen38-q8` artifact. It compares seven prompt
IDs through a four-position scratch allocation against a fresh-stream baseline:
all logits and every recurrent/KV buffer across the actual 64-layer artifact.
The position-only rewind is a negative control. Invalid input must leave state
and device counters unchanged. The final line is `4095` with exit zero; read
the separate release and Metal live counters too. The definition-only BML cell
can be preflighted without opening a model; the runner is effectful.

Observed 2026-09-11: all twelve comparisons passed; renewal took 1181 ms for
seven IDs, released 128 old handles, and final live buffer count was zero.
This is a state-isolation and ownership witness, not language-quality evidence,
a throughput benchmark, a whole-session evaluation, or a 95% equivalence claim.
