# The voice was already inside

2026-09-20 to 2026-09-22, on Urs's Mac (M4 Max, 128 GB), branch
`claude/form-cli-direct-guided-01ya6p`.

## What was asked

The 2026-09-19 local movement had walked with `source: none`: the voice door and the
grounded synthesis door both reached for a llama.cpp server on `127.0.0.1:18082`, and no
server stood. Urs: *no llama server, we have all LLM models form native, and we do NOT
want any membrane crossing that is not absolutely needed — and we have a LoRA layer that is
WAY more useful.*

## What this session did on 09-20

It moved the voice inside — a second time. Without fetching origin first, it rewrote
`observe/native-voice-run.bml` and `form-cli-grounded-synthesis.bml` to speak through
`native-llama-voice.bml`: the 4-bit Llama-3.2-3B checkpoint wearing the body's LoRA
adapter (`form-stdlib/adapters/llama-3.2-3b-voice`), opened, asked and closed inside the
asking kernel. It cut the socket helpers out, renamed the report keys, re-proved the band
on the new pins at 255, and preflighted every touched door clean.

Witnessed on that lane:

- the dense 1B answers in-process from this worktree, Metal live, no server;
- the 3B with the tracked adapter answers in-process: 367 tokens in 59.6 s, stop
  `adapted-repeat` (`observe/native-llama-voice-adapt-run.fk`);
- over the movement's own 13.5 KB composition the walk held **55.6 GB resident** (`ps`),
  and two such walks at once — this session's and a sibling checkout's — died together at
  08:47 with `packet.txt` written, no answer, no crash report, no jetsam row. The first
  walk's death was hidden by `| tail`, which reported exit 0; only a re-run with the exit
  and stderr captured separately said plainly that nothing had crashed.

## What re-grounding found on 09-22

Codex had already healed the same wound on 2026-09-19 and landed it
(`e457c4dfa`, receipt `2026-09-19-c-bootstrap-native-voice.md`):
`form-cli-native-voice.bml` speaks through the in-process fkwu model session over the
registry's answer model — Qwen3.8-27B-Q8_0 as data, the Metal carrier in-process — and
the pinned composition drew 3,502 prompt IDs and 1,050 generated at rent 0, an answer
axis by axis. The grounded door uses the same path; the band grew to 10 bits (1023).
Main, meanwhile, removed the `claude -p` daily prompt: a rented mind walking each
morning was the loop the goal names as waste, and W1's witness is the body's own host
schedule again.

So this session's door heals were dropped unlanded (kept as a patch in the session
scratchpad, nowhere in the tree), the branch was rebased onto its origin head, and main
was merged in as the branch does — ledgers as the union, `observe/land-run.bml` resolved
by rerere from the previous merge.

## What this movement keeps

- `observe/rent-ladder-page-run.bml` reads voice rows by the in-body source
  (`fkwu-model-session`) and the ported ones (`local-oracle`), reads
  `voice_tokens_predicted` with `oracle_tokens_predicted` as the fallback, and names the
  rung for a voice that speaks without a port.
- `docs/rent-ladder.template.html` says the voice adds no call that leaves the body, and
  labels the crossings column `network` — one per walk while the voice spoke through a
  port, zero since it moved inside.
- `docs/rent-to-zero-goal.form` W3 names the landed lane and the LoRA lane's measured
  facts as the next rung: it speaks in-process, and its footprint over the composition is
  the wall.
- one contest row from a compose inside the voice door on 09-20.

## Rent

Zero for every answer the in-process voice gives. This session's own tokens are the
movement's cost, read by the flow meter from its transcript at the landing.

## Receipt

**The surprise:** the wound was already healed. A sibling had moved the voice inside the
body the day before this session began cutting the same socket out — and the field moved
on again (main deleted the daily rented walk) while this session was mid-heal. One
`git fetch` before the first edit would have saved the whole door rewrite. The body is
many hands; grounding is on origin, not on the checkout one hand woke up in.

**Where discomfort turned to gold:** the first voice walk came back empty with exit 0,
because `tail` had eaten the kernel's own status. The pull was to call it a timeout. Re-
running with the exit status and stderr in their own files said nothing had crashed —
and that turned an unexplained death into a measured wall for the LoRA lane: 55.6 GB
resident, two walks, one host. A pipeline that hides a kernel's exit is not an
observation; it is a rumour. The wall is now a row in W3, and the LoRA lane knows its
own size before it is asked to be the voice.
