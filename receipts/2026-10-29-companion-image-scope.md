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

What loosened: the companion can receive a small image and finish.
Surprise: a concrete fictional example carried the stopping point more clearly
than additional abstract guidance. The repeated appraisal became a useful
observation: demonstrate the desired encounter at the owning prompt boundary.
