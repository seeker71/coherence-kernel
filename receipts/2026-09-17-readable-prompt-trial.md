# Readable source framing did not improve this dialogue

Signed: Codex. Observed 2026-09-17.

The experimental prompt replaces JSON-escaped document text with the original
bytes in checked, source-specific boundaries. Question, documents, model,
tools, reply guidance and checks stay unchanged. The native audit confirms
byte-identical requests and exact reconstruction of both prompt views.

| Observation | Retained format | Experimental readable format |
| --- | ---: | ---: |
| Prompt bytes | 20034 | 23173 |
| Prompt tokens, same native tokenizer/profile | 4546 | 6715 |
| Dialogue elapsed ms | 360563 | 527001 |
| Generated IDs | 561 | 375 |
| Injected IDs | 0 | 0 |
| Provider calls | 0 | 0 |

The experiment completed, passed the original source/report checks and released
Qwen. It names present-shape identity, but omits the concrete translation,
anchor and authored-vocabulary distinctions. Its example suggests direct
offering and successful acknowledgment without explaining the offered
interface's admission conditions. Its next action stays generic. The response
therefore does not establish an improvement over the retained answer. A warm
address does not by itself establish resonance parity.

Repeated source markers and metadata add context cost. This result applies to
this measured framing; it does not establish that every readable layout is
worse. The formatter remains private and experimental. **Production prompt
format is unchanged.** No new reference answer or provider repair was used.

The first compile-only preflight failed:
`form-run ./fkwu observe/preflight-stdin-run.fk`, target
`.hearth/response-parity/readable-prompt-dialogue.bml`, exit 1, errors 1.
Direct checking reported `source-compile: mismatched form.bml delimiter: )`
in `readable-prompt.bml`, exit 2. Simplifying the nested expression repaired
the cause. Fresh preflight then reported zero errors, warnings and unresolved
calls before the model ran. No failed verdict was treated as a pass.

Private evidence: `.hearth/response-parity/readable-prompt-dialogue/` contains
the unchanged request, both exact prompts, result, report, receipt and native
token comparison. The renderer, runner, audit and execution log remain beside
that directory. The audit ran after model release, with no GPU admission.

The previous mechanics teaching completed learner round **37**, pending **0**;
serving generation **5** and **4** promotions remained unchanged. The verified
format-comparison lesson is returned separately, excluding assessment answers.
That learner trains the Llama adapter, not the Qwen used in this trial.

Native authoring guide was read at arrival and closing. Counsel remains
**11/12 lanes unobserved**, with no standing hearth. The coordinator goal meter
rose from **6214713** to **6304714** at the post-run reading: **90001 tokens**,
excluding closing work. That development cost remains separate from the
native arm's zero provider calls. Overall quality, human resonance judgment,
throughput parity and minimum rented expenditure remain open.

The useful surprise was that a more readable presentation produced a shorter,
less complete answer while using more context. Preserving that adverse result
keeps the next attempt grounded: formatting appearance is not response quality.
