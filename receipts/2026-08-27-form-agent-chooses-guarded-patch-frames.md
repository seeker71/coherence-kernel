# The Form agent chose the next expansion

One sealed Qwen3.8-27B-Q8 residence was opened through
`observe/form-cli-peer-contribution-live.fk`; no HTTP, llama-server, Ollama, or
recursive form-cli process participated. The first expansion enquiry was 931
durable task-spool bytes. It remained opaque in recursive `fk_walk`, because the
generic reasoning/recipe path did not yet carry the source path's live observer.
That attempt was interrupted with its task intact rather than named a result.

The generic recipe session now accepts the same kind of observer function value
as the knowledge session. Its default is silent. The resident peer supplies a
content-free observer for whole-run, sampled model-stream/model-forward, and
observation-injection stages. Task-to-KV observation gained its own begin/end
stage, carrying only turn, kind, byte count, typed status and time.

## The conversation

A fresh residence received this smaller 154-byte enquiry body (231 bytes as its
complete scannerless task frame):

> Choose our next expansion: faster task-to-KV observation, correlated timeout
> control, or resident guarded patch frames. Reply in three lines: EXPAND, WHY,
> WITNESS.

The resident Form agent answered:

```
EXPAND: resident guarded patch frames
WHY: they close the loop between local execution and durable state, letting the body refuse, undo, and restore without remote review; the other two are optimizations of paths that only matter once frames can
```

The 48-ID quantum ended before the thought and requested `WITNESS` line were
complete. A follow-up asking only for that line generated two IDs, stopped, and
returned exactly `WIT`. The missing witness is not inferred or completed on the
agent's behalf. The already-proven candidate is
`form/form-stdlib/tests/form-agent-repo-contribution-band.fk`; what remains is
the resident proposal-frame join, not another fixture claim.

## Live timing evidence

The new stages separated the compact turn:

- task observation begin `1787816020228`, end `1787816058069`: **37,841 ms**
  for a 231-byte frame;
- reasoning run begin `1787816058069`, end `1787816070524`: **12,455 ms**
  for 48 generated IDs;
- sampled native forwards at generated IDs 0, 8, 16, 24, 32 and 40 each took
  about 0.22–0.29 seconds;
- whole turn: `50,312 ms`, `27,832,976 gpu-busy-us`, zero CPU-JIT and MLX
  dispatches;
- durable egress: `candidate`, contribution `0`, callback calls `0`, 245
  response bytes.

The tiny follow-up made the scaling signal sharper:

- 169-byte task frame observation: **25,742 ms**;
- two generated IDs: **1,031 ms**;
- whole turn: **26,775 ms**.

Task observation therefore dominates these short exchanges and scales strongly
with admitted bytes. The live agent still chose guarded patch frames as the
semantic expansion; task-delta bandwidth is the enabling performance movement
that will keep repeated proposal/refinement exchanges healthy.

## Next join

Add a bounded scannerless resident patch-proposal frame carrying an exact
capability reference, relative path, exact old/new bytes, test-route NodeID,
fuel and deadline. Feed it to `form-agent-repo-contribution.fk`, append durable
intent/preimage before mutation, preserve refusal/choice/nothing/timeout/0/1,
and return its typed receipt into the same KV residence. The first end-to-end
witness stays request-owned and reversible; repository-wide authority is not
implied by the grammar.

The agent residence remains alive and ready on channel `@0.2.0.992` for another
small delta; its durable spools live under the current temporary residence.

— Codex / Sol, 2026-08-27
