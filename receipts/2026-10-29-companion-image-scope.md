# Receive the offered image at its own scale

The native companion added appraisal and explanation to a standalone poetic
statement. Abstract guidance about brevity still produced that extra layer.
`observe/companion-run.bml` now gives a small fictional example of receiving an
image in its own poetic register and ending there. Interpretation and play
retain their invitation scopes; practical help retains its existing guidance.

Observed through the current `cp-packet` and `fcnv-ask`, with empty memory,
using fkwu/Metal and the registry's Qwen3.8-27B Q8 weights. The private observation
entry reused the source packet verbatim and omitted the normal conversation
append. The personal encounter and its responses remain local, outside training
and this receipt. The final response was one short image echo, with no added
appraisal, interpretation or question. No exact personal wording is encoded in
the implementation. A preparation attempt with an incomplete packet was stopped
and excluded from response evidence; its trace remains local.

Separate fictional observations use an image absent from the prompt example:

| Invitation | Actual native answer |
|---|---|
| My thoughts are little boats today. | Little boats on the water of your mind, drifting where the current takes them. |
| Offer one possible interpretation of: My thoughts are little boats today. | Perhaps they are drifting gently, untethered from the heavy current of the day, finding a quiet rhythm in the water. |
| Let's play: my thoughts are little boats today. Add one playful sentence. | I’ll tie a bright ribbon to the mast of the one that looks most ready to sail. |

These are small observed responses, not a measure of a person's vitality or a
general quality score. The offered-image reply extends the metaphor; the
requested interpretation is explicitly tentative, and invited play adds one
fictional action. Held-out fixtures were not
read or used. Existing grounding and authored-move bands remain separate from
generated-prose evidence.

Verification: `./fkwu --check observe/companion-run.bml` and `git diff --check`
pass. Completed native observations report `complete=1`, `release_ok=1`,
`provider_calls=0` and `model_server_calls=0`. Source repair and judgment used
rented OpenAI gpt-6-astra reasoning; repair-specific rental totals are not
separately metered. The generated answers are native; quality judgments carry
no native sufficiency verdict or grounding NodeID.

Landing exposed one design-document link to an unimplemented last-block test.
The design now names that reproduction band as planned; the focused link
observation reports 1,131 claims and zero broken links. No trainer capability
was added or claimed by this repair.

What loosened: the companion can receive a small image and finish.
Surprise: a concrete fictional example carried the stopping point more clearly
than additional abstract guidance. The repeated appraisal became a useful
observation: demonstrate the desired encounter at the owning prompt boundary.

## Return-path care after integration

A concurrent voice-timing change landed during publication. The final native
encounter then reached decode checkpoints and failed before returning its answer:

`fkwu: str_eq: only strings and nothing compare as strings -- ask value_kind first, or use value_eq -- in q38oo-phase? (form/form-stdlib/bml/qwen38-open-observation.bml)`

The timing reader scanned the whole framebuffer as though every category were
a string. It now uses value equality for phase recognition and accepts only
integer clocks held in node children. Unrelated structured events, scalar
events, missing clocks and malformed clocks leave valid spans readable.
The existing native observation band reproduces the failure before repair and
returns its full verdict 2047 afterward. Independent review found no further
issue in this scoped change. No C runtime change was needed.

The actual fresh encounter was then run again through the repaired reader:
it returned the same short image echo with 706 prompt IDs, 5 generated IDs,
`complete=1`, `release_ok=1`, `provider_calls=0` and
`model_server_calls=0`. The answer and successful return are both observed.
