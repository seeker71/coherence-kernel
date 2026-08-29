# The held-out question reached the resident; the old response route answered `<FAIL>`

## Crossing

The local Qwen resident now has a Form-native, one-row held-out client and
scorer:

- `observe/form-cli-peer-heldout-v3-one-send.fk` admits one sealed v3 row only
  when its immutable manifest and that row's current source hash agree.  It
  sends the question alone through the existing spool/bell transport.  The
  expected answer never enters the task frame.
- `observe/form-cli-peer-heldout-v3-one-score.fk` locates exactly one durable
  contribution frame by `response-bytes`, not by a delimiter in model speech.
  Its public result contains hashes and scalar metrics only.  No frame is
  `nothing`; multiple or malformed frames are `choice`; the model's `<FAIL>`
  payload is an error, not an incorrect answer.

The batch corpus remains the full freshness/leakage proof.  The one-row path
checks the sealed manifest and the selected source only; it does not re-hash
every held-out source before each request.  Repeating that O(all-sources)
filesystem scan took minutes of one CPU core during this observation and did
not serve a single already-selected question.

## Resident receipt

The only live local model process was the existing resident:

```
PID 22895  ./fkwu observe/form-cli-peer-contribution-live.fk
model     /Users/ursmuff/models/qwen38-27b/Qwen3.8-27B-Q8_0.gguf
```

It was retained; no llama-server, Ollama, HTTP server, or second model
residence was started.

The selected row was `v303` in the `bootstrap` family.  Its dataset seal was
`31af3c7901d024d4772ab5466ac059e6db43e683262512cda67ac76a996ccf48` and
its prompt digest was
`2312e1880d79c0bade4742080b1f0c672289e8e027be52a0045297de03b95fd1`.
Neither its expected answer nor the reply text is copied here.

Before sending, a nonexistent turn produced `reply-state=nothing` and
`reply-frame-count=0`; no model was claimed.  Two durable attempts then
arrived through the old resident's recipe route:

| turn | ingress kind | route | response digest | model result |
| --- | --- | --- | --- | --- |
| 5 | `heldout` | `recipe` | `ec001fe0f6360da2774c68418705beca9a404953a1d3d9b282051e0a847a53d3` | error=1, exact=0, elapsed=3500 ms |
| 6 | `research` | `recipe` | `ec001fe0f6360da2774c68418705beca9a404953a1d3d9b282051e0a847a53d3` | error=1, exact=0, elapsed=3866 ms |

Both payloads were the model's typed `<FAIL>` response.  The first live
framebuffer movement reported 3 generated tokens, 68,702 ms wall time, and
38,095,478 µs GPU busy before durable egress; the committed frame retains the
shorter model-result elapsed value separately.  The second completed and the
process returned to its FIFO bell wait.  A one-second stack sample at the
idle boundary showed the Form walker at a local `open` wait, not an active
Metal forward; memory was 898.9 MB resident and 4.4 GB peak.

## Verification

```
./fkwu observe/preflight-run.fk   # sender: clean
./fkwu observe/preflight-run.fk   # scorer: clean
```

The direct scorer was also exercised against the actual durable spool for an
absent turn, then both committed model frames.  It retained the error signal
instead of scoring it as a zero answer.  `git diff --check` was clean.

## Floor and next movement

This is not evidence that the local model has learned this row; it is evidence
that the current permanent resident cannot produce a direct plain-answer
completion through its loaded recipe-only effect route.  The old image cannot
gain a new caller-owned effect hook by a policy swap.

The next healthy crossing is a successor resident born with a stable
direct-answer effect offer beside the existing recipe/source offers.  Its
hot-swappable policy may choose that hook, while the caller still owns model
session, task identity, source authority, and durable egress.  Then the same
sealed one-row client can obtain a real unassisted baseline before curriculum,
RAG, or LoRA changes are credited.

I kept the exchange alive by sending one question through the warm resident,
letting its durable `<FAIL>` become the result, and making the next birth
boundary precise rather than retrying a prompt.  The surprising teaching is
that a model may be fully live while its already-born effect surface cannot
express the question.  The discomfort was the failure payload; it became a
clean route-level measurement instead of a hidden zero.
