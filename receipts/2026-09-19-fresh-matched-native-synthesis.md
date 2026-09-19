# A fresh comparison, including its correction cost

Codex, 2026-09-19. The three provider executions below occurred on September
17 at `8be6efdc4`. Their retained contracts, answers and accounting were checked
again after rebasing onto `7f64081e7`; no new provider process was opened.

## What moved

Three new assessments covered a measured profile choice, the native final-answer
selector, and a fictional deadline dialogue with a finite translation codebook.
The original manifest and rubric were frozen before either answer existed.
The baseline received the original sources and a native tool for calculation
and checking. Synthesis received those sources plus the same native observations
up front. Both provider processes were owned and invoked inside Form. Neither
route generated with Qwen in this comparison.

This compares two workflows with different presentation and tool access. Equal
CLI flags do not establish identical backend model identity. It does not isolate
model intelligence or establish native-only parity.

Original goals, documents, source checks, report checks and word bounds were
preserved. The synthesis prompt excluded expected assertions and earlier model
answers. The baseline completed two native tool commands; the synthesis event
inventory contained messages only. All three retained answers passed the
original assertions and word bounds, and all three processes released.

## All provider attempts

Cached input is already included in input. Reasoning is already included in
output. Each distinct provider usage event is counted once.

| Route | Measured execution ms | Input | Cached input | Uncached input | Output | Reasoning within output | Input + output |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| Baseline, native tools available | 53,482 | 73,981 | 58,368 | 15,613 | 2,154 | 259 | 76,135 |
| First native-grounded synthesis | 29,609 | 23,126 | 10,624 | 12,502 | 956 | 194 | 24,082 |
| Synthesis refinement | 24,594 | 23,492 | 10,624 | 12,868 | 946 | 167 | 24,438 |
| Synthesis including refinement | 54,203 | 46,618 | 21,248 | 25,370 | 1,902 | 361 | 48,520 |

The first synthesis saved 52,053 input-plus-output tokens against the baseline.
After the refinement, the saving was 27,615; uncached input was **9,757 higher**,
and measured execution was **721 ms longer**. Summing all three provider
processes gives 124,655 input-plus-output tokens. No price ratio follows from
these totals. The execution sum excludes coordinator inspection, writing and
time between attempts; it is not whole-session latency.

The completed coordinating turn containing these experiments was separately
observed by `observe/form-cli-turn-cost-run.bml`:

- Turn `01a0afaf-533d-7521-9bf3-49322954a570`, 29 model calls.
- Input 5,630,258; cached 5,513,472; uncached 116,786.
- Output 34,812, including 20,738 reasoning; input-plus-output 5,665,070.
- Unattributed tokens 0. Separate provider subprocesses and the current
  September 19 continuation are excluded by this meter.

Coordination cost dominates these three small tasks. This is adverse evidence
against claiming efficient whole-session operation from the synthesis row alone.

## What the answers established

Both initial answers calculated the requested deltas correctly, read the
selector states correctly, preserved the explicit deadline and distinguished
an absent reply from rejection. Neither invented a recipient motive or a
missing German translation.

Both initial recommendations blurred completion recovery with format recovery.
The baseline also described local observation-prefill IDs as coordinating
input. The first synthesis suggested checker **or** semantic review, leaving
the original checker's continuing authority unclear.

Native execution provided the discriminating observation: a completed non-JSON
final answer returns through `model-final`, with zero injected observations and
no reserved generation stage. Separately, the ordinary controller accepted the
same checked report and entered repair after one measured number was changed.
The refinement received these observations and explicit token-counter scope;
it received no earlier answer text.

The refined answer distinguished local injected IDs from coordinating-provider
tokens and retained the caller's checker. Some wording still grouped completion
with formatting capacity. Its dialogue draft was more concise, but its explanation
derived draft/send authorization from the limits of reversibility. Authorization
came from the requester. That causal error remains visible even though the
structured fields passed. Human resonance remains unassessed.

The useful surprise was that a native counterexample improved a specific
explanation while the new answer introduced a different reasoning defect.
The correction cost also removed the original speed advantage. A single green
check or a cheaper first answer cannot stand in for the whole outcome.

## Retained evidence and checks

Private root: `.hearth/response-parity/fresh-matched-v1/`.

- Original manifest SHA-256:
  `7157b129d2ce0dd2d0da306050004fb1945a2990836f55242799b7b9db30d543`.
- `baseline-result.json`, `hybrid-result.json`, `timing.json`, `audit.json`.
- `refinement/manifest.json`, `native-evidence.json`, `result.json`, `receipt.json`.
- `close-audit.json` rechecks refinement source preservation, original checks,
  word bounds, distinct usage events and the cumulative arithmetic.
- Both synthesis replays returned zero new processes and the original usage
  event. The September 19 close audit returned 1, exit 0.

Baseline provider task: `01a0afb8-92da-7540-aeff-7e6dbf68f947`.
Initial synthesis task: `01a0afb9-5f33-7870-b354-10adf9e97c5f`.
Refinement task: `01a0afc0-e9d5-7483-b621-47ff2cb55a0f`.

All assessment answers remain outside training. A lookup for a guessed source
path failed; the source was then located. The refinement cell initially failed
compilation on `nsm_n`; changing it to the existing `nsm-n` passed fresh
compilation before inference. Those attempts remain part of the work.

The public reserve help and native coding documentation now state the observed
trigger: completion and JSON validity are separate; the ordinary parser and
caller checks retain their work. No reserve runtime behavior changed here.

Panel reading on September 19: counsel reported **orphans 0** and **11/12 lanes
unobserved** with no standing hearth. The glass's first frame arrived in 34 ms.
These readings describe their own instruments, not answer quality.

The next quality observation needs to preserve authorization as a supplied
fact while examining the actual prose. The accounting also points toward
consolidating native work and shortening coordinator context before claiming
whole-session efficiency.
