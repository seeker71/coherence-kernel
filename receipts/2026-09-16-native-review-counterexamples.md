# A completed native report still needs behavioral evidence

Signed: Codex, 2026-09-16.

## The question and the observed floor

This movement tests the unresolved controller review and grounded dialogue from
the retained response comparison. All new generations use local Qwen through
Form, with provider calls disabled and evaluation content excluded from learning.
The current runtime differs from the older baseline, so comparisons with that
baseline do not isolate one intervention.

Giving the first reasoning reply 4,096 tokens produced different outcomes. The
controller review exhausted all 4,096 without a closing answer boundary, in
1,009,052 ms. It retained the candidate, released the model and reported attention;
there was no final report. The dialogue completed in 917,918 ms, with 3,232
generated IDs, 75 injected IDs, two checks and release confirmed. Its answer
used the supplied vocabulary and distinguished a short symbol anchor from
kernel identity. Human preference and resonance remain unmeasured.

The coordinator initially called `locale-rows=nothing` an invented detail.
That accusation was wrong: the supplied vocabulary reading contained that
literal. The codebook carries the symbol, its English phrase and its anchor,
with absent locale rows. In this codebook `nl` means the English natural-language
phrase, not Dutch. Reading all supplied evidence corrected the coordinator's
assessment. The local answer deserves credit for evidence it actually used.

## Completion and correctness separated

A private experiment reserved a final-answer phase: 512 reasoning IDs, followed
by an explicit controller-inserted closing boundary when the model had not
closed it, then up to 1,024 answer IDs. The last pending generated ID was committed
exactly once before continuing. The boundary intervention was recorded as
`controller-close=1`; incomplete output remained ineligible for action.

This completed in 349,573 ms with 936 generated IDs, two injected IDs, one check
and release confirmed. It repaired completion, not the proposal's semantics.
The experiment is not a production default.

The next ordinary-generation replay supplied 4,778 additional bytes: the exact
model-feedback callee and its model-session implementation. It completed in
320,888 ms with 307 generated IDs, no injected IDs and release confirmed. The
answer still proposed an early finish at the caller's turn budget. The original
field/source checks accepted it; those checks did not establish preservation of
other failure paths.

## An executed counterexample

A native BML witness compiles the frozen original loop and advance function
alongside the inspected proposal. Controlled pure dependencies supply checkpoint
success/failure and model liveness. This is executable branch evidence, not
fault injection into real disk or GPU operations.

| Case | Original outcome | Proposed outcome | Feedback injections |
| --- | --- | --- | --- |
| Already complete | Continue/complete state retained | Same | 0 → 0 |
| Budget reached | Caller budget reached | Same | 1 → 0 |
| Checkpoint refused at budget | Checkpoint write refused | Caller budget reached | 1 → 0 |
| Model offline at budget | Model offline | Caller budget reached | 1 → 0 |
| Ordinary continuation | Continue | Same | 1 → 1 |

The candidate was retained in all five cases, yet two failure reasons were
masked. The witness returned **two preservation counterexamples**, exit 0.
A true `preserves_candidate` field therefore did not establish the broader
behavioral claim. The proposal was not installed.

The original loop hash is
`20fb2c282eb411d2f5f1fa68c503d6ae39246c32613aff379a9008d0f9a67b78`;
the disproven proposal hash is
`25a3348a338cd3de6d7241774bca2d76299b600f7ff57f4992bfe8116860f149`.
The successful witness hash is
`7db1c023fb384832165b8860fb582cf74a71817ce85a54b26e7c2d4f5caddf63`.

Adding the prior report and actual counterexamples as ordinary documents did
not repair the answer. That replay took 372,977 ms, generated 307 IDs and
returned the same report byte-for-byte. A further attempt routes those same
executed failures through the existing native failed-check transition, entering
the controller's repair state. That run completed in **392,876 ms**, with 309
generated IDs, no injected IDs and release confirmed. Its explanation accurately
described both failures, but its proposed function remained byte-identical.
The rebuilt witness had the same hash and reproduced both counterexamples.

The failed behavior check was supplied at repair entry; the subsequent submission
still used the original field/source assertions. Those assertions accepted the
report again. The next implementation seam is therefore explicit: retain actual
behavioral checks in the repair acceptance contract and rerun them on every
proposal. Understanding a diagnostic in prose did not establish its repair.
A correlated abstain action 3 retained the adverse result and declined to install
the proposal. Re-observation still counted two counterexamples.

This is a frozen review assessment. The current production `fcac-advance` already
routes the exhausted-budget case through `fcac-loop` without injecting feedback.
The disproven candidate did not replace it. No generation default or runtime
implementation changed in this movement.

## Preparation failures and scope

The first manifest helper called nonexistent `json-node-number`; preflight
reported the unresolved call, exit 1. Using the existing `json-node-int`
repaired it. A comparison helper then tried to concatenate an absent final
report; retaining explicit report absence repaired that observation.

The first behavior builder failed with a mismatched BML delimiter, exit 2.
A native list join replaced the fragile nested expression. A premature dependent
run then failed because its source did not exist. The generated witness next
lacked the core prelude and failed on unresolved `sum`, exit 1. Adding the
required prelude produced the concrete five-case result. No failed run's printed
value was accepted as a pass; the unsuccessful source is retained privately.

The long reasoning run's quiet interval was also inspected: the owned process
was executing and waiting on Metal, and its sparse generation checkpoints later
advanced. A correlated continue action preserved that run. Quiet output was not
treated as terminal completion or proof of a stall.

Private evidence lives under `.hearth/response-parity/` in `quality-focus-*`,
`quality-bounded-controller.*`, `quality-callee-*`, `quality-behavior-*`,
`quality-counterexample-*` and `quality-repair-state.*`. The exact dialogue
comparison is available locally as `quality-dialogue-comparison.md`. Private
reasoning and assessment answers are not published or used as training targets.

## Cost, embodiment and the next attempt

The panel reports **11/12 lanes unobserved**, with no standing hearth; it cannot
support an all-good verdict. The share reader reports declared/unmeasured and
withholds a percentage. The native authoring guide reports zero Python
implementations and two invocation candidates in `voice-say.bml`, outside this
movement.

At the recorded intermediate snapshot, the goal counter rose from 3,492,551 to
3,873,490: **380,939 coordinator tokens**, subtracted by native Form. That cost is
substantial and does not meet the minimal-rental aim. It is not final-turn usage
or an API input/output breakdown. The transcript meter separately reports
899,518 cumulative output tokens, decided through byte 45,060,375 of 45,060,407.
These scopes are not added. The new local trials made zero provider calls;
the coordinator remained rented throughout.

A verified method teaching was submitted as session
`native-review-verification-method-v1`, event
`verified-check-scope-and-correction-v1`. It covers check scope, persistent repair
acceptance and source verification, without supplying assessment answers or
reference patches. The existing learning integration trains a Llama 3B adapter;
it does not update the Qwen weights used in these trials. Retention is confirmed;
the worker is running with one pending example, completed round 22, four prior
promotions and serving generation 5. The teaching door remains attached until
the supervised worker exits. A new adapter and response gains remain unproven.

`git diff --check` is clean. The landing drift door passes all 13 lanes:
`pass=8191 full=8191 refused=0`, exit 0. This commit records the observations;
it does not install a failed proposal or change the assessment acceptance rules.

The surprising teaching is that the model can correctly describe why its own
proposal fails while returning the same proposal. The difficulty became useful
when execution exposed that difference, and when the coordinator's mistaken
critique was corrected against the complete source. This movement keeps both
arms accountable. Native answer quality, human resonance, throughput parity and
minimum rented cost remain open; the next attempt belongs at the persistent
behavioral acceptance boundary.
