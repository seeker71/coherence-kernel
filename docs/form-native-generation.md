# Native direct generation

The source-backed CLI runs the local model in `fkwu` with its in-process Metal
carrier. Discover the available model index before selecting it:

```sh
form-run ./fkwu form/form-stdlib/form-cli-repl.fk
```

```text
models /Users/ursmuff/models/qwen38-27b
use 0
generate --sample 1729 --tokens 1024 --words 350:450 --prompt-file enquiry.txt
quit
```

Use the index returned by `models`. Put `--prompt-file` last; the file's complete
bytes become the enquiry. `--words` checks whitespace-delimited words, including
headings, and permits one correction in the same session. Read the returned
answer and its completion, range and release fields separately.

`--sample SEED` enables the Qwen3.8 direct-answer policy: temperature 0.7,
top-k 20, top-p 0.8 and presence penalty 1.5. These settings follow the
[model author's non-thinking recommendation](https://huggingface.co/Qwen/Qwen3.8-27B#best-practices).
Choose an integer seed from 1 through 2147483646. Omitting the option
preserves greedy decoding. This option currently serves direct answers;
`--reasoning` remains a separate mode.

The GPU selects exactly twenty finite candidates after subtracting the presence
penalty. Ties use ascending token ID. Form applies temperature and softmax,
keeps the smallest sorted prefix reaching top-p, and draws with its existing
MINSTD generator. Each emitted token is marked once for presence; prompt and
feedback IDs are excluded. Presence starts afresh for each answer or length
revision; the random sequence continues across the correction.

The public report names the policy, initial seed, final random state and draw
count, including a selected end token. Aggregate diagnostics count non-argmax
draws, single-candidate nuclei and total nucleus candidates. The peak probability
sum is conditional on the selected top twenty before nucleus filtering; it is
not semantic confidence. The report retains both the pre-correction snapshot
and final cumulative counters. Their difference describes the correction stage.
`length_revision_change` distinguishes `not-requested`, `unchanged` and `changed`
by comparing answer bytes in the executing controller. No private text or token
sequence is exposed by these fields.

A fixed seed makes the draws repeatable
for the same logits; it is not a guarantee of identical floating-point model
execution across hardware. The sampler owns five scratch buffers, released
alongside the session even after a failure. Nonfinite candidate shortages,
device errors and unfinished generation remain visible failures.

The native sampler changes token selection. Answer fidelity, instruction
following and usefulness are assessed from the returned words; sampling alone
establishes none of them. No weight update or provider call is involved.
