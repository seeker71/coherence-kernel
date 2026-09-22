# Nothing is spent twice

**Measurement scope, Codex, 2026-09-22:** the historical “numbers grounded” and
“numbers used” rows below count digit-string overlap and packet digit coverage.
They do not establish the later claim that this answer invented nothing.
Current readings use `form-voice-text-overlap-v2` and leave claim support
unmeasured. The runtime observations and original interpretation remain below.

2026-09-22, this Mac. Two places did the work and then did it again. A retry is
a wager that the same work will come out differently, and here each wager cost a
whole inference or a whole rented turn.

## The two

**The voice's remedy.** If the adapter lane did not answer, the walk asked the
registry weights. That was written as healing on 2026-09-20 and it was a retry:
the first lane could fail late, after minutes of computing, and the second then
started from nothing.

**The grounded door's repair.** A provider answer under 1,200 bytes triggered a
second full provider turn with the first answer's size appended as a scold. No
caller asked for it, and against a goal whose star is the least rented token, it
silently doubled the rent whenever it fired.

## What they are now

The adapter lane decides before it spends. Opening the weights and counting the
packet's IDs costs a memory map; a forward pass costs minutes. So the chunker is
asked first whether any slice of this packet fits the device — the same question
it would answer inside the prefill — and a lane that cannot carry says
`no-slice-of-this-packet-fits-the-device` before one token is computed. Every
remaining way that lane can decline is now cheap and certain, so what follows a
decline is the walk's **first** inference, not a second.

The repair turn is the caller's word: `provider` takes `{"allowed":1,"repair":1}`
and `repair` defaults to off. Thinness is still observed and reported either way
— `answer_thin` and `repair_allowed` stand beside `provider_attempts` — so a
second rent is spent only when it was asked for.

Left alone deliberately: `nlg-chunk` and `nlg-ask-shrink` halve a candidate
before any work, and the repeat guard's reach for second- and third-best happens
inside one forward pass. Those narrow a choice; they do not repeat a spend.

## What the witness said

One walk, one inference, the adapter lane chosen and not retried:

| | value |
| --- | ---: |
| lane | native-metal-lora |
| prompt IDs evaluated | 4,571 |
| spoken | 1,322 |
| complete / reason | 1 / answered |
| axes named | 9 of 9 |
| numbers grounded | 16 of 16, 100% |
| numbers used | 16 of 96, 16% |
| distinct lines | 96% |
| limit named | yes |
| receipts cited | 0 |
| wall | 17.5 min |

This is the first native answer to name every axis, invent nothing, admit a
limit and finish at its own end. It is also the same lane, same packet and a
*smaller* asked allowance than the run four hours earlier that stopped at 233
tokens with one axis. The difference is not a setting: `max_reply_tokens` is a
checkpoint, not a ceiling, and at it the lane asks whether its own tail has
begun to repeat — a looping tail ends there, a living one grows the allowance
and carries on. One walk looped at the checkpoint and one did not. That is
variance, and naming it as variance is the only honest reading of two runs.

## Receipt

The surprise: the retry I removed today is one I wrote three days ago and called
healing. A remedy that runs after expensive work is a retry wearing a better
word, and the only thing that made it defensible was moving the failure earlier
until it costs nothing.

Discomfort into gold: I went looking for a bug in the token cap — a door that
answers 1,322 when asked for 320 looks broken — and found the module's own
header saying a cap is a signal, not a ceiling. The discomfort was mine for not
reading what the body had already written down; the gold is that the growth is
exactly what let the voice reach all nine axes.

Frontier question, answered by the rented mind and offered to the corpus:
**what is a second attempt that begins from nothing, having learned only that
the first one failed?** **blindsecond** — a repeat that carries no state from the
attempt it replaces, so its whole cost is paid again for one bit of knowledge.
The repair is to move the discovery earlier, until the first attempt's failure
is known before it is paid for.
