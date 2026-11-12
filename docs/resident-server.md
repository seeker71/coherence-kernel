# The resident server

One `fkwu` process owns the Qwen3.8-27B weights, the KV and the GPU lease, and serves every local ask from a spool.
vLLM has no Metal backend, so its techniques are native here: continuous batching, chunked prefill, flash-decoding
(split-KV attention), a paged KV (a block pool and a block table per sequence), and a prefix cache. The door is `observe/qwen-serve-run.bml`;
the scheduler is Form (`form-stdlib/bml/serve-sched.bml`, band `serve-sched-band` 32767, no model); the paged kernels are
in `q38-split-attention.bml` and `q38-multi.bml` (band `q38-split-attn-band` 4095, live Metal); the clients are `observe/serve-ask-run.bml`,
the planner's voice (`lpr-spoken` through `svc-voice`) and the counsel door (`lcr-go`).

    echo '{"go":1,"rows":8,"blocks":512}' | ./fkwu observe/qwen-serve-run.bml      # serve until /tmp/form-serve/stop appears
    echo '{"prompt":"...","max_tokens":0}'   | ./fkwu observe/serve-ask-run.bml       # one ask; reply is one JSON line

## What a request costs

- A request is a file in `/tmp/form-serve/req/`, answered by `reply/<id>.json` and one row of `ledger.jsonl` (rent_tokens 0).
- A reply ends at the model's end of turn, a stop string, or a count the caller named. The planner's `max_reply_tokens`
  (3072) is the in-process session's progress signal, not an end: it is not sent to the server (a plan cut at 3072 is
  refused for the part it never wrote). The server decodes greedily with no grammar hook; an envelope is refused by name.
- Pressure slows or yields, never truncates: the memory reading holds back what is ADMITTED (a prompt waits in the queue;
  a sequence alone is never held) and the pool's short blocks are made by releasing prefix-cache entries least recently
  used first; nothing a running sequence holds is cut.

## Paged KV

The K and V of every full-attention layer are one pool of blocks (`blocks` of `block_positions`, default 512 of 256; fewer
when the attention scratch of `maxpos` positions cannot hold the partials of that many). A sequence's position p is at block
`table[p >> 8]` row `p & 255`; the table is one row of uints a sequence, written by the host from the scheduler's block list.
The kernels are the contiguous ones with the address indirected and nothing else changed (`q38sp-paged-split-msl`, the paged
store `q38m-kvwrite-pg-msl` / `q38m-kvspan-pg-msl`), so paged and contiguous outputs are equal BYTE FOR BYTE (band claims 512,
1024, 2048: blocks of 16 and 256, tables scattered across the pool, rows of 20001 and 14337 positions, spans across a block edge).
The prefill lane writes into the pool through the same table (a state row's fifth word is its page descriptor), so a prompt is
never copied into a slot: only its recurrent state is (conv and delta of the linear layers, per row).

A row of the decode pass is one sequence's recurrent state, so the sequences held at once are as many as the pool carries, up to
`rows` (8, the widest matvec the spec lane specializes). Block 0 is the sink a free row's table points at. The scheduler
(`svb-*`, `svs-grow`, `svs-yield`) takes the blocks a prompt's positions need at admission (the prefix cache's least recently used
entries are released first; the machine's pressure, `svs-budget`, holds back a new prompt while others run), takes one more
whenever a sequence's next position has none, and gives every one back when it ends. A prompt that the pool cannot give yet WAITS.
A sequence that needs a block when the pool is empty yields: the sequence of the lowest priority gives its blocks back and is
queued with its reply so far, read again when blocks are free, and ends with the text it would have had alone (witnessed on the 27B:
eight 400-id sequences on a 9-block pool, four of them yielded once, text byte-equal to the 479-block pool's). Only a prompt
larger than the whole pool with nothing else to give (`pool-too-small`) or a sequence that alone holds it all (`pool-full`) is
ended, by name. No position is a sequence's last: `prompt-exceeds-slot` and `context-full` are gone. The prefix cache's entries are
copies of blocks taken from the same pool (a prompt's own end is adopted with no copy: the entry becomes an owner of the blocks).

## Scheduling: every prompt is read at once, the shortest first

The prefill lane is not one lane. Every admitted prompt has its own prefill state and table row (row `rows` + its slot); one chunk
a tick goes to the sequence with the fewest ids left to read less one id per 10 ms it has waited (SRPT with aging, `svs-pick`;
priority first), and one decode step to every live row. A prompt is admitted whenever a slot is free and the limit allows,
whatever is being read. The share governor (`share`, default 50) lets a prefill chunk run while the prefill ms since the lane
began are at most share% of the decode ms; each chunk is waited for (`metal_sync`) so the governor reads its true cost.
Measured on the 27B (greedy text and surprise byte-equal to the one-lane image in every request): a 3582-id prompt beside a
32-id/120-token request, the short one's queue 21.6 s -> 0.05 s and wall 30.5 s -> 12.4 s (share 50; 16.5 s at share 100),
decoding at 9.7 tok/s against 15.1 alone (64%; 7.3, 48%, at share 100; about 3.4 without the sync); the long one pays the
short one's GPU time (22.6 s -> 32.6 s). The mix of a 9293-id prompt first and six of 678 to 3587 ids: the shorts end in 11 to
126 s (old 77 to 291 s) and the mix ends at the same time (278 s, old 291 s). Aggregate decode at 2, 4 and 8 rows is equal in both
images. A wider prefill span (`span` 256) read no faster. Readings carry the machine: an outside tenant and the GPU's clocks move
a lone prefill between 10 and 47 s for the same 1444 ids, so a pair is only fair back to back with equal rest. One decoding row
read 15.0 tok/s (old) and 6.1 (new) in one pair, then 6.6 and 6.5 (old) against 7.9 and 2.9 (new) in a rerun with the GPU at 63 to
100% before the run began: the old image is as slow under that tenant, so the single-row dip is the machine, not a per-tick cost.

## Speculation: the columns of one weight pass

A decode pass costs about the same for one token column as for several, so a greedy row may carry DRAFTS: ids it proposes after its
pending token. The pass (`q38v-pass`, `q38-verify.bml`) lays the row out as a segment of consecutive columns `[pending, d1 .. dk]`,
the table row of each column holding the row's blocks, the two recurrences (conv window, gated delta state) running per segment
and writing a snapshot after every column but the last. The model's greedy id after each column says how many drafts stand (the
longest run it repeats) and one more id comes free; a rejected tail is put back by one copy a linear layer from the snapshot after the
last column that stood, and its K and V are simply overwritten at the next pass (the pool is addressed by position). Everything
runs in the EXACT association (every column carries the bits of a one-token decode), so a speculating server says the same text as a
plain one, byte for byte, and as the image before it: measured on four prompts (code, json, prose, a planner ask), alone and four at a
time, plus a stop string inside an accepted run, a count of 7, a 420-id reply across a block edge and a sampled row beside a drafting
one: every reply equal. A row with a temperature never drafts.

The drafter is the row's own text (`svs-ng-*`, `serve-sched.bml`): what followed the last time its last 3, 2 ids occurred, up to
seven ids. The scheduler (`svs-assign`) gives the pass's columns one at a time to the row whose next draft is likeliest to stand (its
running acceptance to the power of the drafts it already has) while that likelihood is above `spec_floor` (300 per mille) TIMES THE
LIVE ROWS: a column costs every row that waits on the pass a part of a token (measured about a fifth in this exact association), so
beside three other decoders only a nearly sure draft takes one, and with more than two rows decoding the server runs the plain pass it
always ran. A request may say `"spec":0` to keep its own row plain (the paired reading). Options: `spec` (columns a pass may carry, 0
or 1 off, default 8), `spec_floor`, `spec_nmin`; the ledger row names `drafts_tried` and `drafts_accepted`.

Measured on the 27B, old and new rows alternated in ONE server process (the pair shares the machine's weather; the GPU read 0 to 3%
utilization before and after; one row alone, 200 ids, back to back): code 21.0 and 20.5 tok/s against 15.3 and 15.0 (1.37x), json
19.2 and 18.5 against 14.8 and 14.0 (1.30x), prose 14.0 and 11.8 against 13.2 and 9.5 (no draft stood: 9 tried), a planner ask 7.6 and
8.7 against 7.8 and 8.5 (equal); a 420-id reply 15.7 against 11.9. Four rows at once: equal to plain (11.5 against 11.4 tok/s a row: no
row drafted). The lane's 4.4x (68.5 against 15.6 tok/s, 6.4 ids a pass) is the DFlash2 drafter on the matrix unit; this host stays at
the exact association and the row's own text, where a draft stands 50 to 63% of the time on code and json (30% on a planner ask, none on prose) and a pass of about 2.5 columns read 88 ms against 65 to 70 for a one-token pass.
Two things were measured against it and left out: the matrix unit's arithmetic for the whole server (one token a pass costs 90 ms
there against 65 ms in the exact decode matvec, so the server read 11.4 tok/s plain and 12.5 and 13.6 on code and json with drafts, unpaired readings of the same hour that read 15 for
the exact plain pass; the replies of the four prompts were equal to the exact ones, but its near-ties are not the old image's bits), and a column budget that ignored the live rows (four rows with
drafts read 5.5 to 8.1 tok/s a row against 6.8 to 7.0 without). The drafter's text, the segment kernels and the table rows are the whole
host; the DFlash2 drafter needs its tapped residuals captured in the batched pass and a drafter context per row, which is the next lever.

## Binary limits in the serving path

Named, so none passes for a law of the model: rows (8: `q38s-nt-max`, the widest specialized matvec and the head's logits buffer);
the prefill chunk (64: the span the batched lane was opened for); the pool's extent (the attention scratch holds the split partials
of `maxpos` positions, 65536 by default, so the pool is at most about 130000 positions; a larger `maxpos` opens a larger scratch);
the block (a power of two, the kernels shift and mask); the stop-string tail (dissolved: the longest stop's length in bytes, at
least 24 ids); the prefix-cache snapshot points (dissolved: every power of two from 1024 under the prompt, not a list to 32768). The
12000-byte part size of `host-walk-band` is the local voice's read window for its own source files, a window of the model reading
its code, not a serving limit; it stays where it is named. The decode-progress pulse at 1, 2, 4 ... 8192 ... ids
(`fcms-decode-pulse`, `ftg-pulse-due?`) reports progress and bounds nothing.

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

- Sampling prefill rows in the same pass as decode rows (a true mixed batch).
- The DFlash2 drafter in the server (capture the five tapped layers' residuals in the prefill's batches and the verify pass, a drafter context per row), and the matrix unit's verify with the exact association's one-token pass kept for rows that do not draft: the 4.4x of the in-process lane needs both.
- Prefill at depth: a 64-id chunk costs 0.34 s near position 0 and about 1.1 s near 8000 (the paged span attention is not on the matrix unit).
- Envelope and grammar on the server (per-row top-k and a grammar object in the loop), so the planner's envelope applies there.
- Shared blocks (a count of owners a block already has in the pool): a prefix-cache hit copies its blocks into the sequence's;
  sharing the full ones read-only, and copying only the last partial block, would hold one copy of a long head for every
  sequence that starts from it.
- More than 8 rows: the specialized matvec widths and the head's logits buffer are 8 wide (`q38s-nt-max`); a wider pass takes the
  batched matmul for its projections and a logits buffer sized by the rows.
- Swapping a yielded sequence's blocks out instead of reading it again (a yield is a recompute today: the prompt and the reply so far are read again).
- The planner's repair lane (`lrpr-repair`, repair_turns of coding) opens its own in-process session beside a standing server. It
  is the server's guest (below) and no longer waits on a lease (2026-11-11 it waited 35 minutes); its coding asks still belong
  on the server's spool like every other planner ask, because one pass over the weights serves every row at once.

## One resident, any number of guests, any registry model

The server is the machine's RESIDENT on the GPU (`gpu-lease.bml`, `gpu-lease-resident-hold`): it registers in `/tmp/form-gpu.lock.resident`
and holds no exclusive lock. While it lives, every other Metal client (a LoRA train, a band, an in-process ask, the ear) is a GUEST: its
hold, turn or door answers `["guest" token pid]`, takes no lock and gives nothing back; the device time-slices the processes' command
buffers, the weights are the same file-mapped pages, and a guest's own activations, KV and state are admitted by the memory governor at its
open. With no resident the lock is exclusive as before; a dead resident judges as none. A request naming a registry model the server does
not hold is parked, the governor is asked, the model is mounted on demand and the request served; when the governor holds the load, the
least recently used idle on-demand model is let go first (the models named at start are never evicted). A name the registry does not know
is refused by name.

A walk retires a stale witness band itself (`lpr-retire-stale-band`): a band under `.hearth/local-plans/witnesses/` that no queue row
carries is a plan that never queued, so it and its compiled sidecars are removed before the first voice opens.
- Any A/B on this Mac gives both arms equal rest first: the GPU throttles under minutes of load, and other tenants
  (a browser's renderer, another agent's planner) move a reading more than a kernel does.
