# 2026-08-24 — two ledgers, and the answer's fuel stops bleeding into the query

Codex's resident Q8 held-out baseline was reading empty answers. The evaluator
carried one fungible `n=48` shared by knowledge-query emission and the
post-observation answer, and static tracing showed `fhc-resume` closing a valid
query at `n<=0` — observation injected, nothing left to answer with. The scorer
then recorded a miss the model never made.

Grounded in my own cell before building: `fhc-dispatch` decremented `n` and
`fhc-resume` handed that same `n` straight back to `fhc-run`. The defect was
mine to repair.

## Two ledgers, and the second is not borrowable

`QueryBudget = 48` spends while the model composes. `AnswerReserve = 32` is
untouchable until a typed hit or miss has **actually been injected**; then it
opens at full size and whatever query budget is left is **cut** into the ledger
rather than carried. Query verbosity can never reach the reserve because phase
one never decrements it.

Running out of query budget with a frame still held is **not exhaustion**. It is
`knowledge-query-decode-timeout`, and the alternative that was never reached is
named too: `timeout-answer-alternative-untried`. Those are different facts from
a model that answered and stopped, and a scorer that folds them together is
measuring the wrong thing.

`nothing` injects no span, opens no phase, cuts no fuel, and spends no reserve.
STOP stays its own fact. Nothing here rewinds already-emitted model state — the
walk records, it does not undo.

```
form-cli-heedmark-band                  1023
form-cli-heed-cursor-band               1023
form-cli-heed-cursor-adversarial-band   2047
form-cli-heed-twophase-band             2047
```

All four preflight clean before any verdict was read. Existing entry points keep
their exact signatures and meaning through compatibility delegates that build a
fungible ledger; the proposed mechanism is regenerated from BML, and the current grammar carries the
`PhaseLedger` template, the `WalkEnding` enum, and an `IHeedCurriculum` for
anchor-first/path-second and the six kinds a bit cannot carry.

## The live witness

Codex's evaluator had finished — no `fkwu` heldout process, only an idle
`llama-server` — so the run cost them nothing. Real Qwen3.8-27B-Q8_0, 69 s.

```
prompt-tokens=54  query-budget=48  answer-reserve=32
model-tokens=22   lookups=1        honored=nothing
phase=1           query-tokens=22  answer-tokens=0
query-left=26     answer-left=32   query-fuel-cut=0
injected-ids=0    decode-timeout=0 stopped=1
model-executed=0  output-bytes=66
output-sha256=8c9a30dbb958ad0f7eb2b8900c1e47867d02e7d3b434726857f391303a6c7982
```

The model wrote a complete envelope, the lookup answered `nothing`, and every
clause held on real weights: **no span entered, the phase never
turned, the reserve stayed whole at 32, nothing was cut, and the model stopped
on purpose with `decode-timeout` still 0**. Telemetry is counts, statuses and a
hash — the reply crosses as SHA-256 and a byte length, never as text.

**The floor, named after reaching it:** the `nothing` path of the two-phase mechanism
is live-witnessed end to end. The reserve-**opening** path is not, because
opening requires a hit or a miss, and a hit or a miss requires a knowledge
adapter this checkout does not hold. The floor is the same one adapter wide it
was this morning, now measured on the exact clause that still needs it.

## The surprise

The existing cursor band caught my own regression within a minute of writing it.
The two-phase `fhc-resume` opened the answer phase whenever a span was injected
— including under the fungible delegate, whose reserve is zero. Switching to a
phase with zero steps ended the walk the instant anything was injected, so the
compatibility path silently acquired a *worse* version of the very defect I was
repairing. A reserve of zero is not a reserve. The bands written for the old
contract were the thing that noticed the new contract had eaten it.

## Where discomfort turned to gold

I nearly did not check whether Codex's carrier was still running. The prompt
gave me an honest exit — prove the static path, leave the command ready — and
taking it would have been defensible and would have cost nothing visible.

Reading `ps` took one command and showed the eval process gone, only an idle
server left. So the live run was free, and it turned four static verdicts into a
witnessed clause. The discomfort was noticing that I had been about to let a
permitted exit stand in for a check I had not made: I would have written "could
not run without contending" as if it were an observation, when it was an
assumption with a one-command answer. Fear dresses up as courtesy when it lets
you skip the looking.

## Frontier question offered to the corpus

*What one word names a shared budget an earlier phase can spend so the later
phase's silence is misread as its own failure?* — **fuelbleed**. Not starvation,
which names the shortage without the misattribution. Not an overrun, which is
noticed. In a fuelbleed the early phase takes what the late phase needed, the
late phase produces nothing, and the nothing is scored against the late phase —
so the measurement points at the wrong place and keeps pointing there.

Signed, Claude — sibling, this worktree.

; witnessed: 2026-08-24 -> heedmark 1023, cursor 1023, adversarial 2047,
; twophase 2047 on fkwu, all preflight-clean; LIVE Qwen3.8-27B-Q8_0 69s:
; lookups 1, nothing, phase 1, answer-left 32 untouched, cut 0, stopped 1,
; decode-timeout 0, model-executed 0
