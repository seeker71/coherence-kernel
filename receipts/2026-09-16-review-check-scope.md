# Check scope and the full response comparison

Codex, 2026-09-16. Continues `2026-09-16-native-review-evidence.md`.

## Local explanation

Shared review guidance now states what format acceptance establishes and keeps
total attempts distinct from the final result. The same Qwen Q8 explanation
request and paired source evidence completed in 195837 ms with 429 generated
IDs, zero injected IDs, one passing format check and release 1. The preceding
run took 186270 ms with 391 generated IDs.

Reading the answer against the source found the intended improvement: it no
longer claims that format acceptance proves behavior, and describes zero final
counterexamples after three search attempts. It also describes the preserved
telemetry path more accurately. This is one observed response improvement, not
a general semantic-quality guarantee. The raw answer stays unchanged in
`.hearth/response-parity/scope-explain-report.json`.

## Fresh three-task session

The native driver uses the original frozen promotion, controller-review and
grounded-dialogue requests. It adds the existing caller-owned behavioral checker
and native structural repair to the controller task. Evaluation recall and
learning remain excluded. Request hashes preserve the comparison identity;
model selection and stronger checks are explicit differences. The provider
baseline's retained controller answer also passed the same strengthened check.
That recheck made no new provider call.

The exploratory Q4 run was interrupted during its first task after two wrong
promotion reports. Its last progress boundary recorded 728054 ms, 410 generated
IDs and 246 injected IDs; the later reply recorded two failed checks. The process
ended with SIGTERM/exit 143 after correlated branch action 1. These are separate
observation boundaries. No completed session or native release receipt is claimed.
The exact wrong answers and interruption remain retained under `scope-full-q4`
and in the referenced reply files. Q4's prior kernel improvement was not evidence
that it would outperform Q8 in this session.

The fresh Q8 run uses the same three tasks:

- Promotion ended at attention in 492677 ms, with 829 generated IDs and 1452
  injected IDs, release 1 and a failed final decision check. The answer treated
  broader quality evidence as an additional promotion prerequisite absent from
  the supplied rule.
- Controller review completed in 220721 ms, with 306 generated IDs, no injected
  IDs and one accepted native repair. The repaired code passed the original
  contract and the stronger behavioral check. Its method-summary explanation
  does not yet provide the requested full causal account; code verification
  and answer quality remain distinct.
- Grounded dialogue completed in 335620 ms, with 399 generated IDs, no injected
  IDs, release 1 and passing report checks. The answer correctly leaves absence
  distinct from rejection. It blurs the teaching about preserving words while
  changing their usage into word replacement, omits the concrete eight-hex
  anchor and its distinction from composition-based identity, and describes
  full kernel identity as aspirational. Its nearest-anchor proposal has no
  supplied proximity method. These are qualitative limitations despite the
  boolean checks passing. The retained baseline gives the actual anchor,
  absent German mapping, identity distinction and a more developed human example.

After promotion's local release, native Form invoked one authorized provider
resource with a fresh evaluation identity. The corrected report promotes under
the supplied loss rule while retaining unknown broader quality and usage. It
passed the unchanged checks. The provider process took 11261 ms and reported
16498 input tokens, including 10624 cached, and 179 output tokens. Both the failed
native report and the assisted report remain separate. This provider call ran
inside Form; Codex did not answer that resource outside the native invocation.

The provider resource ran while the local controller task continued; its time
is not added to the full native session's wall time. All three original request
hashes match the baseline, and both arms' actual reports were read against the
previously frozen rubric. The comparison is:

| Observation | Native Q8 plus Form-owned assistance | Retained provider baseline |
| --- | ---: | ---: |
| Session wall time | 1049419 ms | 258074 ms |
| Original contract passes | 3, including 1 assisted | 3 |
| Provider input tokens | 16498 | 454009 |
| Of those, cached input | 10624 | 432896 |
| Of those, uncached input | 5874 | 21113 |
| Provider output tokens | 179 | 5726 |

The local model generated 1534 IDs and received 1452 feedback IDs. Those are
native tokenizer counts, separate from rented usage. The Q8 driver made no
provider calls itself; the optional Form-owned resource made one. The coordinator
invoked that native resource door after the failed case's release; this experimental
driver does not yet orchestrate the optional resource itself. Failed local
results remain retained. The native side used fewer provider tokens in this
comparison, but took longer and still provided weaker controller and dialogue
explanations. All contract checks passing is not overall parity. The native
run's wall time also excludes the interrupted Q4 experiment and development
coordination. No human resonance preference is inferred.

Private records are `scope-full-q8/`, `scope-assistance.json`,
`scope-baseline-controller-check.json`, `scope-comparison.json` and their logs.

Development coordination is additional cost. The goal counter moved from
4369763 at the start of this comparison movement to 4612445 before its final
receipt: 242682 coordinator tokens, of which 129036 belong to the final
continuation. These are intermediate counter readings, not final costs or
provider attribution. The transcript meter separately read 1068063 cumulative
session output tokens. Those differently scoped counters are not added to each
other. The provider-token table excludes this substantial development overhead.

## Verification and continuity

Policy band 65535, native repair band 8388607, clean preflights and drift gates
8191 all exited 0. Counsel reported orphans 0 and 11/12 lanes unobserved without
a standing hearth. The native guide was read at start and before this landing.
No runtime dependency or C-seed change was introduced.

The prior evidence-retention teaching completed. After the assessment released
its final model, this movement's verified teaching of check scope was submitted
through the native session-learning door. The worker completed round 28 with
pending 0; promotions remain 4 and serving generation 5 is unchanged. This is
the native Llama adapter learner, not a Qwen weight update. Training did not
overlap the measured local run. Assessment answers remain excluded from training.

The useful surprise is the difference between two improvements: paired evidence
helped attribution, and explicit check scope corrected a separate overclaim.
The difficult result is also concrete: improved wording did not resolve the
promotion decision. Two next repairs are concrete: carry the verified native
proposal into the model's complete causal explanation before finishing the
ordinary review, and resolve the repeated addition of an unstated promotion
prerequisite. The successful separate explanation run is evidence for the first
path, not proof that the ordinary review already carries it. Timing still needs
improvement as well. The full objective stays open.
