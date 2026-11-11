# The resident server

One `fkwu` process owns the Qwen3.8-27B weights, the KV and the GPU lease, and serves every local ask from a spool.
vLLM has no Metal backend, so its techniques are native here: continuous batching, chunked prefill, flash-decoding
(split-KV attention), and a prefix cache. The door is `observe/qwen-serve-run.bml`; the scheduler is pure Form
(`form-stdlib/bml/serve-sched.bml`, band `serve-sched-band` 4095, no model); the clients are `observe/serve-ask-run.bml`,
the planner's voice (`lpr-spoken` through `svc-voice`) and the counsel door (`lcr-go`).

    echo '{"go":1,"slots":4,"maxpos":14336}' | ./fkwu observe/qwen-serve-run.bml      # serve until /tmp/form-serve/stop appears
    echo '{"prompt":"...","max_tokens":0}'   | ./fkwu observe/serve-ask-run.bml       # one ask; reply is one JSON line

## What a request costs

- A request is a file in `/tmp/form-serve/req/`, answered by `reply/<id>.json` and one row of `ledger.jsonl` (rent_tokens 0).
- A reply ends at the model's end of turn, a stop string, or a count the caller named. The planner's `max_reply_tokens`
  (3072) is the in-process session's progress signal, not an end: it is not sent to the server (a plan cut at 3072 is
  refused for the part it never wrote). The server decodes greedily with no grammar hook; an envelope is refused by name.
- Pressure slows or yields, never truncates: the memory reading lowers how many sequences are admitted (never below one)
  and releases prefix-cache entries least recently used first; nothing a running sequence holds is cut.

## The prefix cache

The prefill lane keeps a snapshot of its state (the full layers' K and V to a position, the linear layers' state) at: a chat
request's question end, 1024, 2048, 4096 ... ids, and every prompt's end. A later request whose ids begin with a snapshot's
ids (compared by their packed bytes) starts from a copy and reads only the rest (`cached_ids` in its ledger row). Witnessed on
the 27B: a 2614-id prompt again, prefill 19.5 s -> 0.18 s, same text and surprise; counsel's three lenses on 5620 ids, 43 s
+ 0.6 s + 0.6 s (they were 3 x 43 s); a boundary hit and a cold run gave the same text and 555073 micro-nats; four growing
session turns, 12.6 s then about 1 s each; a planner's second option reading from the 2048-id head the first option shared.

## Sampling

A request may name `temperature_milli` (thousandths; 0 is greedy) and `seed`. The row then draws its token by the Gumbel-max
rule among the tokens within min-p 0.05 of the likeliest (`q38m-sample-msl`): one seed and one prompt always draw the same
text, another seed another, and greedy and drawn rows share one pass (a drawn row's neighbours keep their greedy text).
The planner's first option is greedy and its later options draw (seed = the option number), because a greedy retry of a
refused plan answered the same plan byte for byte in three options. The surprise a row reads stays that of the greedy token.

## The ledgers

The spool's own ledger has one row a request (who, status, ids, `cached_ids`, ms, rent_tokens 0). Each answer also
lands one row in `receipts/local-run-ledger.jsonl` (the ids the lane read this time and the ms they took), which the planner's
read-time estimate reads; the planner's own plan rows land in `receipts/query-coverage-ledger.jsonl` with rent_tokens 0.

## The ear

The live ear skips its whisper pass while `.hearth/ear.yield` stands and another process holds the lease; the server writes
that flag while it serves. `form-stdlib/bml/ear-yield.bml` holds the one predicate (`el-yielding?`) the ear and its stand-in
`observe/ear-yield-load-run.fk` share. The stand-in is the cadence with no microphone: a native whisper-turbo pass per beat
while not yielding. Beside the server, single-stream decode read 13.3 tok/s with the stand-in yielding and 13.3 with it
absent (equal rest before each), against 5.0 tok/s with it passing (0.8 passes/s). The microphone itself waits in
`AudioQueueStart` for a grant only Urs can give; when it is granted the same flag path runs unchanged.

## Where it is going

- Sampling prefill rows in the same pass as decode rows (a true mixed batch); the prefill lane is still one lane.
- Envelope and grammar on the server (per-row top-k and a grammar object in the loop), so the planner's envelope applies there.
- A paged KV (block table, on-demand pages) instead of a slot's whole `maxpos`.
- A request's `fresh` start that retires its own stale witness band (`band-exists` stops every later option of a request whose
  earlier plan left `.hearth/local-plans/witnesses/<id>-band.bml`).
- Any A/B on this Mac gives both arms equal rest first: the GPU throttles under minutes of load, and other tenants
  (a browser's renderer, another agent's planner) move a reading more than a kernel does.
