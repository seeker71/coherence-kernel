# The Form tokens: where they stand and where they go

What stands, measured on 2026-10-09 and 10 (every number is a reading the body holds):

* **Nine tokens, one list.** recipe-birth, recipe-exec, choice, eval, knowledge-query, node-get, node-make, node-select and
  recipe-import live once in `form-token-kinds.bml`. The grammar dispatches them, the census counts them and the trainer
  admits them from that list; its order is the Qwen head's control-ID rows (first PAD row 248077, verbs 248078 to 248086).
  Four copies had drifted: the trainer's lacked recipe-import, so the adapter never held the ninth token.
* **A voice that writes them.** The 3B voice (native LoRA, in stream) writes eight of the nine; at generation 50 of the
  rebalanced corpus it wrote them more evenly (effective kinds 3.84 to 4.40) and answered fewer right (55 of 64 to 39 of
  72). The streams showed why: taught mass moves into neighbours. `choice` stood in for `recipe-exec` (9 expected, 0
  written), `node-select` was written three times over.
* **A meter.** `observe/form-token-usage-run.bml` reads an evaluation row kind by kind: taught, expected, written,
  substituted, kinds used, each family's answers, and the advice its rules give (under-written, over-written).
* **A loop.** `observe/form-token-cycle-run.bml` walks refresh, census, GPU gate, train, re-ask, meter in one process.

## The expansion, in the order the evidence asks for it

1. **Contrast before volume.** A verb the voice confuses with its neighbour needs rows where only the neighbour is right.
   recipe-both (each of two recipes run with recipe-exec) and eval-reuse (a name, and `it`, read by the next control)
   are the first two such families; the meter's advice names the next from each run.
2. **One closed loop per feature.** A feature enters the voice's flow as a family in the corpus, a line in the meter and a
   claim in a band, and is judged by the same cycle: the number of controls of that kind, the share the answer used, the
   refusals Form gave. A feature with no family has no meter; one with no meter has no loop.
3. **The Qwen head.** The 27B lane writes `<|form:` as seven tokens and a verb as more. The reserved PAD rows give each
   Form token one id (opening 248077, verb 248078 to 248086). The head adapter (`observe/form-token-lora-run.bml`, lane
   `qwen`) fits the opening toward that id; wearing it in the code lane is the step that makes the engineering voice a
   Form-token voice, and the first one whose crossings the ledger (`.hearth/form-tokens/crossings.jsonl`, empty on every
   checkout today) would record.
4. **The model's own envelope.** `<tool_call>`, `</tool_call>`, `<tool_response>` and `</tool_response>` are single
   vocabulary tokens (248058, 248059, 248066, 248067) where the lane spells them as text; `<|form:tool|>` is fifteen
   tokens a call. Feeding the model its own ids is cheaper and what it was trained on; the control scanner and the
   injection path are the two places to change.
5. **Features the voice cannot yet reach.** The verb table is what eval, nodes, recipes and knowledge can do. Candidates
   by what the body already runs natively: the file and clock doors through eval (needs rows whose answers do not move),
   the workspace scopes (`global:` and `local:`), recipe libraries beyond training-cost. Each enters as a family only
   when its answers are stable for the oracle to check twice.

## What would make each step honest

* Held-out questions stay out of training and the meter says so: the 2026-10-02 row's written verbs equal its expected
  verbs in every kind, which a held-out set should not do; the next run's split is read before its numbers are trusted.
* A change in the corpus is a hypothesis until the cycle has re-asked the same questions at the same round.
* A GPU run waits for a quiet GPU: the day turn holds it from 12:30 and 19:00, so the cycle starts after 21:30.
