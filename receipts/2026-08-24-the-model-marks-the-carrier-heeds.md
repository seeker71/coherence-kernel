# 2026-08-24 — the model marks; the carrier heeds

Yes asked for Form-native execution tokens that do knowledge lookup as
query tokens in the LLM stream, and for the local share to grow so a
rented cutoff cannot stop the work. Three agents hold three lanes; this
sitting is the production-bridge lane's first stone.

## What was already home, and the one gap

The pieces all existed separately. `q38-forward` is one step: an id and a
position in, the next id out. `q38-prefill` continues the *same* state at a
position, so the rows before it keep the KV they were written with.
`fcmg-offer-stream` already injects spans mid-stream.

The gap was not the door and not the retrieval. It was that
`fcmg-offer-stream`'s `thought-lists` are computed **before** generation
begins. Nothing the model emits can choose them. The existing injection is
never *caused* by the model's own output.

## The heedmark

A **heedmark** is a mark the model writes into its own output as ordinary
text. The carrier reads the emitted tail, heeds the mark, and Form looks the
query up. The answer re-enters as prefill at the current position.

Four outcomes, and the fourth is the bound:

| outcome | when | what enters |
|---|---|---|
| hit | a grounded row | knowledge **with attribution** |
| miss | the lookup ran, no row | a named status, `no grounded row` |
| nothing | no index, or the window closed | nothing — axiom-1 |
| spent | the per-turn budget is gone | nothing; the mark stays plain text |

Miss enters a *named status* rather than silence on purpose: a model told
nothing will invent the row the body does not hold. Nothing enters nothing,
because a window that closed without an answer is not an error.

```
./fkwu form/form-stdlib/tests/form-cli-heedmark-band.fk   # 1023
./fkwu form/form-stdlib/form-cli-heedmark-run.fk
  logits-executed=0
  span-enters      hit=1 miss=1 nothing=0 spent=0
  knowledge-enters hit=1 miss=0
  admits-hit no-source=0  with-source=1
  bounded 0-marks=0  1-mark=1  5-marks=2
  prefill-cost=12  naive-cost=1012  forwards-saved=1000
```

Preflight clean: parens balanced, 0 errors, 0 warnings, 0 unresolved.

## The refusal, made a constant

`LogitsExecuted = 0` sits in the authority as a named const. No logit
executed a tool. A forward pass produces a distribution over ids and nothing
else. The model asked; the carrier answered. Naming it means the claim cannot
quietly become "the model called a tool" three receipts from now.

## What this sitting does not claim

The live path is **untouched**. `form-cli-model-generate.fk` and
`form-cli-repl.fk` have not been edited. The law is proven on the fkwu arm;
the production wiring is not started, because codex's `validate.sh` gate
exited 1 (bootstrap `uni.c` stale, fourth arm ABSENT) and edits stopped
there. `runtime/fkwu-uni.c` was not regenerated and
`FORM_ALLOW_THREE_ARM=1` was not used.

## The surprise

The performance argument was already paid for and nobody had spent it.
`q38-prefill` takes a position and threads the same state, which means an
answer injected mid-stream costs the *span*, not the context — the rows
before the injection keep the KV they were written with. Priced on a
1000-token context with a 12-token answer that is 1000 forward passes a
naive carrier would repay per lookup. The seam that makes retrieval-in-stream
affordable was built for prompt prefill and had been sitting there since the
head-once optimization.

## Where discomfort turned to gold

Reading the xtal back, every one of my function names appeared exactly once,
and I nearly called that proof. It was `grep -c` counting *lines* on a file
that is one long line — the numb-green trap the body has been bitten by
before. Counting occurrences instead showed `fhm-outcome` eleven times, so
the call sites inside `fhm-walk` and `fhm-check` really had lowered. The
discomfort was that a passing check and a meaningless check looked identical;
the gold is that the discriminator was one flag, and the habit of distrusting
a count I did not derive is what found it.

## Frontier question offered to the corpus

*What is the smallest word for a token a model emits that its carrier honors
as a request, when the answer re-enters as context rather than as a return
value?* — **heedmark**. Not a call: a call returns to its caller, and this
answer arrives as prefill the model reads as if it had always been there. Not
a tool token: the model executed nothing. The asymmetry is the whole word —
the model *marks*, the carrier *heeds*.

Signed, Claude — sibling, this worktree.

; witnessed: 2026-08-24 -> heedmark-band 1023 on fkwu, preflight clean,
; bounded(5)=2 under MaxHeeds=2, forwards-saved(1000,12)=1000, logits-executed=0
