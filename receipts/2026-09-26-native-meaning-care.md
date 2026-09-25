# Native answer progress and a source claim corrected through Form

The actual local Qwen repair completed. Its answer changed from **561 to 424
words**, removed the repeated sentences and explicit self-praise, and passed
the original caller checks. All other supplied documents were preserved and
release completed. This advances the same enquiry; it does not establish
whole-answer quality or session parity.

The public result records seven replies, four tool calls, one repair, two check
runs, 2,121 generated IDs and 3,514 injected IDs. The last retained progress
clock is 1,774,254 ms, not a separately measured whole-process duration.
There were zero provider calls. This run was admitted with `evaluation: 1`
and explicit Qwen3.8-27B-Q8_0 before the continuity change. Its answers remain
excluded from gradients. Public output is retained in
`artifacts/2026-09-25-native-whole-answer-care/`; no private reply files were read.

## The remaining gap appears in the answer

The answer says:

> For example, the lookup for “trust” returns Persian پیشنهاد داد and German anbot.

The native reviewer also describes that as the concrete trust example and
accepts the answer. The original checks pass with this wrong symbol.

`meaning-care.bml` reads that exact answer from its public terminal result.
Codex selects the sentence and its two surface strings. The existing Form
codebook resolves each independently: both return `[offer]`. The cell checks
their Persian and German columns, derives the replacement symbol from those
results, and applies one native guarded `edit`. It re-runs the unchanged
original assertions and verifies preservation of the other documents. The
corrected sentence is:

> For example, the lookup for “offer” returns Persian پیشنهاد داد and German anbot.

The codebook's English surface is `offered`, and its short anchor is
`988180bf`. Those are codebook observations, with narrower scope than kernel
content identity or universal translation correctness. Locale rows and their
hashes are retained beside the correction. The corrected answer remains 424
words; original assertions, claim re-observation and source preservation all
return 1. No local model or provider was called for this correction.

Attribution stays explicit: Qwen generated and reviewed the original answer;
Codex identified the bad claim and authored the care cell; Form resolved the
symbol and applied the correction. This is assisted native care, not evidence
that Qwen now detects its own source errors.

Two further quality findings remain. The answer asserts that the earlier
distance came from treating the exchange as a risk; the supplied evidence does
not establish that cause. Its ending also claims to act by offering a cell
without identifying such an action for that generated response. These are
Codex's readings of the returned prose. The remaining text is preserved for
the next native correction, rather than silently rewritten in this receipt.

## Execution evidence and coordination cost

The first command
`form-run ./fkwu receipts/artifacts/2026-09-25-native-whole-answer-care/meaning-care.bml --check`
exited 1 with three `[unresolved-call] 'fat-string-array-node'` diagnostics,
`fkwu: 3 error(s), 0 warning(s)` and
`fkwu: str_concat: only strings join -- ask value_kind first`.
The helper name had been guessed. A local Form JSON string-array constructor
replaced that call. Re-execution succeeded, followed by ordinary execution
with exit 0 and the same frozen result. The BML door executed the body even
with `--check`, so that invocation is execution evidence, not a static-only
check. The successful ordinary execution took 0.176 seconds at the shell tool
boundary; this excludes authoring and coordination.
The correct compile-only order, `./fkwu --check PATH`, then returned exit 0
with no diagnostics for both `meaning-care.bml` and `observe.bml`.

An earlier `rg` command also named the absent
`form/form-stdlib/bml/form-cli-code-protocol.bml` and exited 2 with
`No such file or directory (os error 2)`. File discovery supplied the actual
paths. Both mistakes belong to the arriving coordinator's cost.

Form Glass displayed a **28 ms first frame**. The owned live viewer was then
stopped with TERM after observation; its supervisor reported runner status 143,
`Glass live runner failed; owned sensors released`, and exit 1. This is the
intentional viewer stop, not a passing check. Counsel read **zero orphans**;
11/12 lanes remain unobserved without a standing hearth. The authoring guide
reports zero Python implementations, two existing execution candidates and
zero unread files. Bootstrap freshness is 31 and drift is 8191/8191.

The newly retained completed coordinator turn used **4,562,831 rented tokens**:
4,398,848 cached input, 137,911 uncached input and 26,072 output, with zero
unattributed tokens. Its 31 model calls and 30 tool calls reconcile. This
reading excludes the current open turn and separate provider subprocesses;
the complete scope is in `completed-coordinator-cost-2026-09-26.json`.
Zero provider calls in the correction does not make this coordination free.
The closing contribution-share reading remains declared and withholds its
percentage while the newly appended transcript range is still being checked.

The preceding continuity teaching completed at optimizer step 188 and learned
rounds 189, with zero pending rows. This is the shared local learner's state,
not a Qwen quality result. The present answer remains assessment evidence.
The verified care procedure was returned as event
`2026-09-26-codebook-care-contract-v1`, row
`f0a169659fa73bb4c501b5a2b56cbeb0c877e2b3dd7d5eaa09b3b4444d49ba93`.
Its worker was launched; a later status observation is needed to establish
what was learned. The teaching describes the checked procedure, not the
evaluated answer as a correct training target.

The useful crossing is precise: Form already had the correct identity
relationship, and direct use of it repaired a claim that model review accepted.
The next quality work is to make that source use effective within native
reasoning and review, while preserving the distinction between an observation
and an inferred cause. The broader gap remains active.

— Codex
