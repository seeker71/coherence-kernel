# Report meaning and repair

Signed: Codex, 2026-09-16.

The same frozen promotion-review request now completes with the correct decision
and no repair attempts. This is a development-case improvement, with the
original request, checks and failed answer retained. Whole-session parity
remains open.

## The observed cause

The native answer correctly read all three improving loss rows, then retained
the serving model because general quality parity was unproven. It had turned a
limit on a broader claim into another condition for promotion. The supplied
policy kept those claims separate.

The next native reply guessed that the failure meant the JSON wrapper was wrong.
The old feedback named neither the check nor the difference between successful
tool execution and a mismatched result. The exact retained replies made these
two problems visible; the earlier shape-only readings could not establish them.

The repair is in Form/BML:

- Review guidance connects the decision conditions to the observed facts and
  keeps limits on broader claims separate. It no longer introduces the older
  wrapper as a competing instruction; that interface remains supported.
- Report-check feedback names the tool, query arguments and actual result. It
  distinguishes tool failure from stdout mismatch and withholds the caller's
  expected stdout. This does not hide expected literals a caller might choose
  to put inside a query; the observed case uses field projections only.
- A read-only review can submit a corrected report directly from repair. The
  same source and report checks run again. The diagnosis/replan path remains
  available. Coding repair still requires its implementation work.

These changes were authored by the rented coordinator from the actual local
failure. The comparison answers were excluded from training, and the expected
decision was not added to the model prompt or failure feedback.

## Re-observation

Both runs used the identical frozen request and four-reply limit:

| Reading | Before | After |
| --- | ---: | ---: |
| Completed behavior checks | 0 | 1 |
| Repair attempts | 2 | 0 |
| Generated token IDs | 731 | 221 |
| Injected token IDs | 939 | 729 |
| Elapsed milliseconds | 479,467 | 241,543 |

The earlier run ended at its reply limit with status `attention`; its runner
exit zero meant the assessment was retained. The repaired run read both source
documents, ran native verification and submitted its report. It promoted the
candidate, kept broader quality unproven and rented usage unknown, and preserved
held-out targets outside training. Its next action recorded the promotion while
proposing wider assessment before claiming general parity.

The earlier run overlapped the previous local learner. These elapsed values
describe the actual attempts; they are not an isolated speed comparison.
Two changes to review behavior landed together, so this observation does not
isolate their individual causal contributions. The successful run did not need
the new repair-submission path; native boundary tests establish that path.

The longer mixed cohort was deliberately interrupted, exit 143, after its
unresolved repair cycles. Its interruption decision and unfinished cases remain
beside its frozen manifest. The help-editing and grounded-dialogue cases now run
in a separate native session. The help case's source and preservation assertion
were refreshed together from current bytes before that session began; the
dialogue request stayed unchanged. Neither result is supplied in advance.

## Verification, learning and cost

Preflight closed the request and policy bands. Native validation passed request
255, policy 65535, session 1 and CLI 67108863. Tests establish that an immediate
report correction reruns source checks, rejects another false report, preserves
documents and cannot complete coding repair. Diagnostics retain actual output
while withholding the caller's expected stdout. The effectful runner passed
compile-only checking. The native authoring guide retained its zero-Python
reading and two voice invocation candidates.

The preceding verified controller teaching completed local learning round 8.
Promotions remained 4 and serving generation 5 stayed in place. That learner is
Llama 3B; no Qwen weight update is claimed. A separate verified teaching about
the new report-repair checks was retained as `report-repair-contract-verified-v1`.
Its worker overlapped the remaining development cases. Assessment answers remain
excluded from learning targets.

The meter settled the previous completed coordinator turn, id
`01a0a7e8-96a5-7310-9249-10816e5e986e`: **68 provider calls**, input **10,159,235**,
including cached input **9,803,392** and uncached input **355,843**; output
**87,682**, including reasoning **57,569**. Reported total **10,274,125** includes
an explicitly unattributed **27,208**. Subset quantities are not added again.
That is substantial rented coordination cost. The goal counter separately read
915,848 at this turn's observation; it is not the provider-total ledger.

Panel reading for that settled turn: native/local/remote boundary events
**37/72/68**, normalized **21/41/38**. This is event share, not semantic credit.
Counsel still read **orphans 0**, with **11/12** resident lanes unobserved. The
native assessment executor made zero provider calls; that fact excludes the
coordinator and the previously measured provider baseline. Overall cost savings
are not established by a zero in that narrower scope.

The useful teaching is precise: uncertainty about a broad claim can coexist
with a supported decision now. The uncomfortable failure became actionable when
the actual answer and the check's meaning were visible together. That correction
now shapes the next native use while the broader enquiry stays open.

## Local evidence

- Before: `.hearth/response-sessions/7a7d7cf1eaa6bf2f724b726a8d2610f5a7a91493a88771341b4f97dfa039889a-55858-1789526813185/`
- After: `.hearth/response-sessions/7a7d7cf1eaa6bf2f724b726a8d2610f5a7a91493a88771341b4f97dfa039889a-59797-1789527358969/`
- Exact replies: `.hearth/code-memory/replies/55858-1789526993419.txt`, `55858-1789527100766.txt`, `59797-1789527600375.txt`
- `.hearth/response-parity/diagnostic-replay-process.log`
- `.hearth/response-parity/repaired-transfer-process.log`
- `.hearth/response-parity/remaining-session-manifest.json`
- `.hearth/response-parity/remaining-session-process.log`
- `.hearth/response-parity/report-repair-embody.log`
- `.hearth/response-parity/share-settle.log`
