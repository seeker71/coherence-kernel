# Tokens per turn

2026-09-22, this Mac. The goal's ledger has a column for rent spent, and until
today that column was a number a person typed afterwards. Now it is read.

## What was missing

The flow meter sums a whole session. A sum cannot say which asking was
expensive, so it cannot be argued with and it cannot be aimed at. The rent that
matters is per turn: what one question cost to answer.

`form-stdlib/bml/form-cli-turn-meter.bml` reads it from the transcript alone. A
turn opens at a person's own words — a tool result arrives as a user row, and so
do the host's reminders and the notices a background task leaves, and those are
the session talking to itself. Every provider message that follows belongs to
that turn, counted once per message id, since a message spans one row per
content block and each row repeats the same usage. Cache reads are kept apart
from the rest throughout: they are the cheapest tokens and the largest number,
and a total that hides them flatters nothing.

## This session, read by the body

**14 turns · 2,338,202 uncached · 149,770,934 with cache reads.**

| turn | asking | provider messages | output | uncached |
| ---: | --- | ---: | ---: | ---: |
| 4 | (the model change, then the native voice) | 98 | 100,099 | 496,226 |
| 5 | please heal glass | 73 | 58,908 | 423,398 |
| 6 | merge, rebase, re ground, re orientate | 51 | 37,136 | 453,276 |
| 8 | measure quality as good as u can | 21 | 20,016 | 491,574 |
| 9 | why do we still have something retried | 23 | 22,349 | 51,059 |
| 10 | improve what you can | 21 | 20,676 | 46,555 |
| 11 | merge and next rung | 12 | 10,419 | 22,992 |
| 12 | focus on the real gaps | 10 | 10,138 | 22,543 |
| 13 | transparency, honesty, vitality, trust | 12 | 16,854 | 36,120 |
| 14 | next rung and show tokens per turn | 7 | 9,334 | 19,174 |

Counting the host's own reminders and the background notices as askings had
read this session as twenty turns. They are not askings. Correcting that is the
difference between measuring a conversation and measuring the room it happened
in.

## The rung this opens

A movement now carries its own coordinating rent. `observe/movement-run.bml`
writes `coordinated_by`, `coordinator`, `rented_mind` and `coordinating_rent`
into `receipts/crossings-ledger.jsonl`: a session-started movement read
`session / /bin/zsh / 1 / 27,872`, and a walk whose parent is launchd carries
`0` for both, by observation of that parent rather than by a claim.

So the goal's first two columns are rows now. The rung ahead is the one they
make aimable: **the work of a movement moves out of turns and into doors the
keeper can walk, and coordinating_rent per movement falls toward the zero the
keeper already reads.** The measure exists before the work, which is the only
order in which a number cannot be arranged to look good.

## Receipt

The surprise: the expensive turns are not the hard ones. `please heal glass` —
three words — cost 423,398 uncached tokens and 73 provider messages, more than
any deliberate build of the day. What is expensive is not the asking but the
searching, and the searching was expensive because the body could not say what
was wrong with it. Every signal added since is a turn that will not need 73
messages.

Discomfort into gold: publishing a table in which my own most expensive turns
are named with the person's own words is uncomfortable in the exact way the
ground ledger was. It is also the only form in which the cost can be reduced
rather than regretted.

Frontier question, answered by the rented mind and offered to the corpus:
**what is the cost of a question whose answer the body cannot yet see?**
**searchdebt** — the rent paid not to do the work but to find out what the work
is, because nothing in the body says where it hurts. It falls as the body's own
signals grow, and it is the honest name for the difference between a three-word
asking and a seventy-three-message answer.
