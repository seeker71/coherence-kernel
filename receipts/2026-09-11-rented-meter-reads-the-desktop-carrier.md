# The rented turn meter reads the Claude Code carrier

Authored by Claude (Opus 5, nifty-maxwell worktree) on 2026-09-11.

`./fkwu observe/rented-turn-meter-run.fk` printed `session-output-tokens=0` for
a Claude Code desktop transcript, in baseline and in delta mode. Reproduced
here on a frozen copy of that transcript (2,565,980 bytes): 0, then 0 from
byte 2,565,948.

## Why it read 0

The scanner found rows only by `"last_token_usage":`, the Codex rollout's
call-local key. The desktop transcript carries none (0 of 246 usage rows). Its
spend sits in the assistant row's `"usage":{…"output_tokens":N…}`, followed on
the same row by `"apiBlockIndex":k,"requestId":"req_…"`. The carrier writes
`"output_tokens":367` with no space, as the mark expects; the anchor key was
simply absent.

## One spend or several

The carrier's own records decided it. On the first transcript, 246 usage rows
collapse to 56 message ids, 56 request ids and 56 distinct (message, request,
output) triples, in 56 consecutive runs, and `apiBlockIndex` counts 0, 1, 2…
inside each run. Each row is one content block of one response, repeating that
response's usage: one request, one spend.

A 4,064-row transcript agreed (1,361 = 1,361 = 1,361) and showed a second
shape: 21 requests appear again about 400 lines later with the same uuid and
the same timestamp, re-appended verbatim. A last-request check counts those
twice, so the meter keys a seen-set on `requestId`. Every usage row also
repeats `output_tokens` inside `iterations`; only the first mark after
`usage` counts. `<synthetic>` rows carry usage and no request, and count
nothing.

On the first file, summing every row would read 1,022,947 and every mark
2,045,894. One per request reads 175,122.

## The heal

In `form/form-stdlib/form-cli-movement.bml`:

- a row counts on either carrier's mark; a Claude Code row needs its
  `requestId`, and a request counts once;
- a row decides once its newline stands in the slice; an open marked row stops
  the slice, and `resume` names its first mark;
- the walk steps slice to slice by that resume, and state v3 keeps the decided
  extent, the total and the counted requests.

Reading the old walk showed a second seam: the 32-byte overlap re-read 32
bytes and then began scanning after them, so a mark cut across a chunk edge
fell outside both slices. Resuming at the decided boundary closes it.

## Witnesses

- `form-cli-rented-turn-meter-band.fk -> 8191` (was 255). The Codex bits
  stand; bits 2 and 8 now prove the resume contract, bits 256–4096 the
  desktop carrier: the iterations repeat, content-block rows, a verbatim
  re-append, a tool result echoing usage, a synthetic row, a request counted
  by an earlier reading, an open row resumed whole.
- Preflight through `observe/preflight-stdin-run.fk`: parens balanced, 0
  errors, 0 warnings, 0 unresolved, exit 0.
- `form/validate.sh`: go=0 rust=0 typescript=0, fourth arm 8191, 0 divergent.
- `form-cli-movement-band -> 15` again; its structural-gate row follows that
  band to 16383. Freshness band 31 after the rebase onto the reunion.

Real carriers, each equal to an independent grep/sort count:

| carrier | meter | one per request |
|---|---|---|
| desk-a, 2,565,980 bytes | 203,910 | 203,910 |
| desk-b, 29,407,948 bytes, 28 chunk edges, 21 re-appends | 3,062,086 in 0.21 s | 3,062,086 |
| desk-b cut mid-row at byte 7,416,377 | 832,470, decided through 7,416,327, the open row's `usage` mark | 832,470 |
| the same file grown whole, delta from 7,416,327 | 3,062,086 | 3,062,086 |
| desk-b walked in 4 KiB and in 64 KiB chunks | 3,062,086 and 3,062,086 | 3,062,086 |
| this session's own transcript, 1,121,508 bytes | 97,559 | 97,559 |

Panel: spendglass read 97,559 output tokens for this session at that reading.
The drift gates door first refused kernel-conformance: Go and Rust passed all
13 canonical vectors, FORMBIN2 interop and 12 malformed artifacts, and the
TypeScript kernel was absent from this fresh worktree. With its dependencies
copied from the main checkout under an identical lock, the door read
8191/8191, refused 0.

## Where the floor stands

The meter reads the transcript path it is given. A session's subagent spend
lives beside it in `<session>/subagents/*.jsonl` (238 such files on this Mac);
a session total that includes them walks those files with the same request
seen-set.

## Frontier row

Question: what is a tally that counts each echo of one spend as another spend?
Answer: **echotally** — 0 hits before this row, landed as corpus row 1436. It
names the family both meter wounds belong to: Codex's cumulative
`total_token_usage` summed into 25 billion, and one response's usage summed
once per content block.

## Closing

Most surprising teaching: a JSONL row is not an event. The carrier writes one
row per content block and sometimes re-appends old rows whole; the event is
the request, and only the carrier's own ids (`requestId`, `apiBlockIndex`,
`uuid`) say so. Counting lines was an echotally too: the 246 lines holding
`"output_tokens"` today are 56 requests.

Discomfort to gold: after the first file the rule looked settled, since 56
runs made 56 requests and every repeat sat next to its twin. Widening the
witness to a second transcript showed 1,382 runs for 1,361 requests, and my
ready design would have double counted. Staying with that mismatch until the
uuids showed verbatim re-appends turned it into the seen-set. Later my own band
read 8189; the failing bit was my fixture placing the resume at the row's `{`,
one byte before the mark, and bit 8 had already resumed correctly from exactly
that value.

I kept the exchange alive by letting the carrier's records choose the rule
rather than the first three hits, and by widening the witness until it
disagreed with me.

— Claude (Opus 5)
