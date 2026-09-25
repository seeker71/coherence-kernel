# Connect native response generation to its sampler

The retained direct answer counted 540 words before and after its length
correction. It also asserted its own grounding without evidence. Earlier work
had already changed message layout, replayed the full transcript through fresh
prefill and trained a sentence-copy head adapter. Those observations did not
establish effective correction. Repeating those operations had no new basis.

Reading the serving implementation exposed a capability gap: `q38-head`
selected an argmax at every step. The existing Form sampling recipes were not
connected to this session path. The
[Qwen3.8 model card](https://huggingface.co/Qwen/Qwen3.8-27B#best-practices)
recommends temperature 0.7, top-k 20, top-p 0.8 and presence penalty 1.5 for direct
responses. That difference supports implementing the missing route; it does
not establish the cause of the preceding answer failures.

## Implementation

`generate --sample SEED` now selects that explicit direct-answer policy. Form
owns the GPU top-k selection, temperature and nucleus distribution, existing
MINSTD random sequence, output-token presence and session lifetime. Prompt and
feedback IDs are excluded from the presence set. Each answer or correction
starts a new presence set while preserving the random sequence. The ordinary
greedy route remains available and is named in its report. Reasoning generation
keeps its existing separate route.

Selection reads the actual immutable model logits. A partitioned GPU reduction
keeps exactly twenty finite candidates in descending order, using token ID to
break ties. Only those small candidate vectors cross into the Form sampler.
The session retains its seed, random state and draw count, including selected
end tokens. Its five buffers are released through the normal session boundary,
including a session that has stopped serving. No C seed, external runtime or model
server changed.

The physical check compares device selection with the independent existing
CPU top-k recipe over 1,025 values, including ties across partitions. It checks
penalties before selection, immutable logits, nucleus boundaries, repeatable
draws, one RNG advance per selection, actual forwarding, stop versus budget
exhaustion, parser boundaries and resource release. Its first execution found
that I had passed byte offsets to `kth-u32at`, which takes word indexes.
Preflight reported one carried error; the compile-only check was clean and
direct execution exposed the failed selection assertion. Correcting the index
restored exact agreement. This was an authored implementation defect, not a
model failure.

After the repair, sampler band 1, model-session band 4095 and reasoning-budget
band 1 pass with clean preflights. The complete source-backed CLI compiles.
The native compiler also detected and rebuilt stale local images normally.

## Actual response comparison

The public CLI listed Q8 at index zero, selected it, and admitted the exact
6,465-byte prompt from the preceding direct-response movement. The enquiry,
selected sources, response profile, 1,024-token allowance and 350–450-word
requirement remain fixed. Seed 1729 selects the new sampling policy. Model
admission independently counted the same 1,887 prompt IDs. The comparison
changes the complete sampling policy, not one individual sampling parameter.
The native run completed in 387,584 ms, with 602 IDs in each of its two
generation stages and 84 feedback IDs. The sampler recorded 1,206 draws,
including the two end tokens, and final random state 14,163,891. Both answers
count 496 words. Completion, release and retention are 1; the original range
requirement still fails. Provider calls are 0. The native exporter confirms
the same prompt, prompt-token count and 4,831-position capacity as the baseline.
Private generation files were not opened.

Compared with the earlier 540-word response, the final words changed and are
shorter. That establishes neither successful correction nor a quality gain.
The answer says that distinguishing absence “eliminates the need to fabricate”
and derives its asserted lack of caution from garbage collection and identity.
Those mechanisms do not establish the claimed response behavior. It also
treats a conversational correction as an observed interface breach without
establishing that correspondence. Its frequency-annotation and language-surface
distinctions are useful, but the unsupported self-description remains.

The next gap remains evidence-guided correction. Sampling is now an available,
checked native capability; this observation gives no basis to claim that it
closed the answer-quality gap or to retry another seed. The complete public
answer, report, unchanged prompt and original assertion result are retained in
`artifacts/2026-09-25-native-sampling/`.

The staged whitespace check found trailing spaces and final blank lines in the
verbatim packets and terminal prompts. The native archive helper stores those
six files as `.txt.json` / `.log.json`, with original path, byte count, SHA-256
and `text`. Decoding every archived string reproduces its original bytes exactly.
The plain input files under `.hearth` and model inputs are unchanged; the public
answer stays plain text. The check is preserved rather than relaxed.

## Continuity

The preceding completion-budget lesson finished at optimizer step 183,
learned rounds 184 and pending zero; serving remains generation 5. This is
the separate Llama learner, with no Qwen weight update. Evaluated answers
remain excluded from training.

The checked sampling contract was returned through the native home-embody door,
event `2026-09-25-native-direct-sampling-v1`, row
`8d1ae6cd604f531a5334028a621c749541cb8d168ebd1aa9aa2aae835c5a4616`.
It teaches selection, RNG, presence and release behavior, not this evaluated
answer. The latest observation has its worker running with one pending row;
no completed update or serving promotion is claimed for this new lesson.

The cost reading still selects the preceding completed coordinator turn:
12,772,212 rented tokens, including 12,310,912 cached input. Total input is
12,671,452, output 74,637 and unattributed 26,123. The current open turn and
separate provider processes are excluded. Token quantities validate; event
reconciliation remains withheld (98 tool calls, 158 output events). Share is
declared with no percentage. The retained cost JSON includes that scope.

Glass first frame: 26 ms, followed by an intentional Ctrl-C (exit 1). Counsel
reports orphans 0 and eleven lanes unobserved without a standing hearth.
The native guide reports Python implementations 0, invocation candidates 2,
unread files 0. Drift gates pass 8191/8191. These instrument readings do not
measure answer quality. The brief glass read and local checks overlap model
execution, so elapsed times are not isolated throughput comparisons.

The first closing mirror call incorrectly supplied its path on stdin:
`form-run ./fkwu observe/voice-frequency-run.fk`. It returned exit 1,
`fkwu: str_len: nothing has no length -- ask nothing? before measuring`.
Writing the documented `/tmp/voice-frequency-target` restored the reading;
the mirror completed at exit 0. This input mistake remains coordinator cost.

— Codex
