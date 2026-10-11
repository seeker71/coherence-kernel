# Native model control plane

The operating overview for Form-native models, locally adapted models and
borrowed pretrained weights the body runs. Claims here are bounded by executable
evidence; a model being installed or trained does not give it authority.

## Where meaning lives

The control plane is Form-owned:

- `form/form-stdlib/native-model-control-plane.fk` owns the registry: which model
  lanes exist, their class, execution surface, status, freshness and authority.
  It carries route-selection rows only, no evaluation, promotion or job recipes.
- `form/form-stdlib/native-model-evidence.fk` owns deterministic evaluation
  evidence: normalization, tokenization, SHA-256 identities, exact/F1/order-sensitive
  scores, deterministic rotation, and train/audit overlap checks.
- `form/form-stdlib/native-model-native-hierarchy.fk` owns native-first routing.
  Engine presence, artifact presence and observed execution stay separate: a large
  model file beside a native executable is not a lane until that exact pairing has
  answered.
- `form/form-stdlib/native-model-route-table.bml` owns the ask-route names as data
  (`native-model-route-table-band.fk` 255).
- `form/form-stdlib/native-model-route-readiness.bml` holds the authority for which
  model routes are usable now, and `native-model-dual-telemetry.bml` keeps the
  truth-separated rows it reads.
- `form/form-stdlib/native-model-tensor-ledger.bml` owns the tensor ownership
  ledger (`native-model-tensor-ledger-band.fk` 65535).
- `form/form-stdlib/native-model-token-flow.bml`, `native-model-owner-cadence.bml`
  and `native-model-owner-request-flow.bml` make a resident model's request an
  observable typed flow: live owner-process truth is kept apart from model truth.
- `form/form-stdlib/native-model-glass.bml`, `native-model-memory-glass.bml` and
  `native-model-resident-fleet.bml` own model routing, the semantic memory map and
  retention/eviction for resident models.

Model and learning changes use the bidirectional diagnostic membrane in
`observe/bidirectional-framebuffer-channel.fk`: a model observation flows out with
a correlation id; Form returns a bounded control action; the actuator selects the
next state; that state is observed again. For any claimed improvement, retain
per-row or per-stage transitions — an aggregate score alone does not establish a
cause. Nothing, timeout, and mismatched replies select an explicit alternative
node ([`live-dynamic-diagnostics.md`](live-dynamic-diagnostics.md)).

## The ask route

Every local ask runs in the body. `form/form-stdlib/native-model-route-table.bml`
holds the route names as Form data — `form-metal` and `llama32.form-metal`, both
carried by the native Metal door `observe/metal-ask-files-run.bml` with a 12-token
default — and a name that would reach a model server is not a row: it comes back
typed `unknown`. The door reads one request directory named on stdin (`prompt`,
`model`, `blob`, `cap`, `stage`) and owns admission, generation, identity and
publication in one `fkwu` process. A request it does not admit names its reason
(`empty-prompt`, `model-not-admitted`, `model-file-absent`,
`model-content-identity-mismatch`) before any weights are decoded.
`observe/metal-ask-run.bml` is the same ask taking one JSON line on stdin, refusing
an absent request before model admission. The body's larger voice is Qwen3.8-27B in
the `fkwu` model session (`form/form-stdlib/bml/form-cli-native-voice.bml`).

`observe/native-model-dual-resident-live-run.fk` keeps Qwen3.8 Flash Next and
Llama 3.2 3B open in one long-lived Form process: idle polls of the filesystem
ingress never evict either context, and only an explicitly offered `quit` closes
both.

## Classes do not blur

| class | exact meaning | may count as owned accepted work? |
|---|---|---:|
| `form-native` | model or learned recipe executes through the Form/native-walker body | yes |
| `local-finetuned` | a pretrained base was adapted locally; the class alone proves neither lineage nor authority | yes, only after accepted final and reviewed lineage |
| `local-native` | borrowed pretrained model executes locally | no |
| `local-oracle` | local external teacher, judge, evaluator, or fallback | no |
| `remote-oracle` | network/subscription teacher or executor | no |
| `policy-fixture` | search policy, operator, architecture, or synthetic fixture without a useful trained artifact | no |

Execution location is separate: `native-recipe`, `local-process`, or
`remote-membrane`. Borrowed weights stay borrowed when the body runs them:
`base.llama32-3b-metal` executes as a `native-recipe` and its class is
`local-native`. Model weights are input data. Evaluation, training, teacher, and
integration-probe calls are separate from accepted production finals and cannot
inflate sovereignty. The registry cell answers the current count.

## Local adaptation

Adaptation of a pretrained base runs in the body, on Metal, in Form:

- `native-lora-dataset.bml` turns chat, prompt/completion and plain-text JSONL into
  teacher-forced token rows; malformed rows stay in the denominator.
- `native-lora-recompute.bml` runs whole-sequence reverse mode with layer input
  checkpoints; `native-lora-train.bml` accumulates a token-weighted batch gradient
  and updates the adapter each completed round, publishing parameters and Adam
  moments together.
- `native-lora-checkpoint.bml` publishes safetensors checkpoints by rename, and
  `native-lora-progress.bml` keeps corpus traversal and Adam age as separate
  identities in the generation they describe.
- `native-lora-fuse.bml` bakes the low-rank delta into standalone 4-bit weights;
  quantization error is measured per projection and never called lossless.
- `native-lora-worker.bml` launches and supervises a run shell-free and keeps the
  child's real exit status; the doors are `observe/native-lora-train-run.fk`,
  `observe/native-lora-supervise-run.fk` and `observe/native-lora-fuse-run.fk`.
  The adapters live under `form/form-stdlib/adapters/`.

Falling validation loss does not establish task authority: a candidate is compared
with the incumbent on fixed items with exact, token-F1 and order-sensitive scores
(`native-model-evidence.fk`), identity-stable and clean, and a tie cannot earn
authority.

## Training and promotion gates

A larger training job stays closed unless all of these are observed:

- exact training rows and a separate held-out set;
- zero forbidden overlap between training rows and the held-out audit;
- row-level provenance plus scoped consent and license receipts;
- a fixed evaluator and content-addressed destination;
- integrity, power, thermal, disk, and toolchain readiness;
- a new candidate identity that never overwrites the incumbent.

Promotion additionally requires a stable model identity, reviewed
base-to-adapter lineage, at least 32 paired sealed samples, no evaluation errors,
a meaningful improvement above noise, and repeated fresh days. Fixed
training-validation or historical held-out diagnostics cannot be relabeled as
promotion evidence. A tie cannot earn authority.

The language datasets do not have complete row-level provenance/consent/license
receipts, so a new large training run is closed. Nothing may manufacture those
receipts or treat their absence as a tooling inconvenience.

Until production routes append valid accepted-final events, owned-work share is
**unmeasured**, never zero. Installed models and evaluation traffic do not supply
the missing denominator.

## The Form-token adapter: what was measured and what it does not show

`form/form-stdlib/adapters/llama-3.2-3b-form-tokens` is a Llama-3.2-3B-Instruct 4-bit LoRA (rank 8, eight
layers) trained 100 steps (batch 4, learning rate 1e-4) from `llama-3.2-3b-voice-native` on the replayed rows
of `learn/form-token-traces.jsonl`, exported through the checkpoint door. The ledger row
(`receipts/form-token-lora.jsonl`, `lane: llama`) reads held-out loss 2935 milli before and 42 after; the
commit that landed it rounds the first to 2.936. The held-out questions were asked in stream, the grammar
answering each closed control (`receipts/form-token-eval.jsonl`, the row at 1790897633574):

| arm | model | exact of 64 | of 62 without the two repeats |
|---|---|---|---|
| plain | bare instruct model | 25 | 23 |
| taught by prompt | bare instruct model | 5 | 4 |
| taught by LoRA | adapter chain from voice-native | 55 | 53 |

- Two of the 64 held-out rows repeated a train row's question (`ftr-eval-float-15` of `ftr-eval-float-1`,
  "What is the square root of 16?"; `ftr-eval-text-23` of `ftr-eval-text-3`). The adapter answered both
  exactly, so the figure with them set aside is 53 of 62; the receipt's per-row results
  (`questions_judged`) hold this. The corpus asks sixteen questions twice in its 596 rows (sixteen families); the split now trains
  a row whose question a train row asks (`ftr-split`), the traces band (claim 64) holds the zero overlap the
  training gates above require, and the corpus has 64 held-out rows: the 62 of the fifteen families the
  adapter's run drew on, and the two the `recipe-import` family (18 rows, written after the run) holds out,
  which the adapter was never asked. Twelve `eval-refusal` rows (eleven train rows and the held-out
  `ftr-eval-refusal-7`) were rewritten in place under their old ids when the sandbox was lifted, and
  `receipts/form-token-eval.jsonl` lines 4 and 5, the adapter's run, cite those ids, so the adapter
  answered those twelve questions as the corpus then worded them, not as it words them now. The adapter was trained before that change (on 514 rows, the two
  repeats' train copies among them) and has not been retrained or re-asked.
- taught-by-prompt: 16 of 64 stopped at eos after a successful observation. The arm is limited by the
  continuation seam: the untrained voice writes eos right after Form's envelope, so 32 of its 64 answers are
  empty (31 of those 32 after at least one observation, 15 of them after a refused one, 16 after successes).
  5 of 64 measures that seam as much as the teaching.
- Plain and taught run the bare instruct model; the lora arm wears an adapter chain that starts from
  `llama-3.2-3b-voice-native` (`observe/form-token-eval-run.bml`, `ftev-arms-of`). No arm isolates the new
  training with Form in stream on the same base weights, so 55 against 5 reads the training and the earlier
  adapter together.
- The Qwen3.8 rank-one opening fit reads a held-out separation of 10.0 over a scrambled floor of 2.2
  (10025 and 2212 milli, `receipts/form-token-lora.jsonl`, `lane: qwen`) on n=7 held-out sites (4 openings, 3
  opened). The 24 sites were drawn from all 578 rows, the corpus's held-out rows among them, so a site of a
  row the corpus keeps for evaluation could have been fitted. `ftlr-trace-sites` now leaves heldout rows out
  (the lora band pins it); the reading predates that and was not refitted.
- `adapter_config.json` of the export named the run's and the data's `.hearth/` scratch directories, which
  do not outlive the run; the export door (`ntr-export-config`) no longer writes them and the checked-in
  config has them removed. It still names the base model by its absolute path in this Mac's Hugging Face cache.

## Memory: what is shared, what is asked, and who backs off

On 2026-10-02 the host restarted three times while model doors ran. Every check the body had looked at one process
(`nlg-room` reads this process's Metal bytes against the device's recommended working set), so several kernels, a
external model server and the page cache could each pass their own check and together pass the machine.

- **Weights are shared.** `metal_buf_from_file` (`form/native/metal/fk-metal-carrier.m`) maps each tensor of the
  GGUF `MAP_PRIVATE` from the file and wraps the pages without a copy, and the Qwen open binds every tensor that
  way (`q38-open-span`, `qsx-open-span`). The operating system keeps one physical copy in the page cache for every
  process that maps the same file. Measured on 2026-10-02 with two kernels mapping one 6 GiB model file and the GPU
  reading every page: the file-backed page count rose 6.2 GiB for the first and 0.0 GiB for the second.
- **What a process still owns** is its KV state, its scratch buffers and a 40 MB header read; another engine's
  weights would not be ours to share, and the body runs none: DeepSeek-V4 runs in the session's own process through its own expert arena (`dsv4-lease.fk`: dense tensors, 64 experts a layer and the KV, derived from the header).
- **Wired memory is not steady.** Metal wires a buffer's pages while the GPU uses them; the machine's wired count read
  35.8, 7.3 and 35.1 GiB within 40 seconds as other sessions' model work came and went, so one reading can pass
  two processes that start together.
- **Admission** is `form/form-stdlib/bml/memory-governor.bml`, over `host_vm_stat()` (Mach's own count of every
  process's pages): a request for `need` bytes is granted when wired + need stays within 60 percent of the
  machine's memory, available - need stays above the larger of 8 GiB and a tenth of it, and the compressor holds at
  most a quarter of it; a reading that did not arrive grants nothing. `observe/memory-governor-run.bml` prints the
  machine's reading, the live leases and, given `{"need_gib":N,"who":"..."}`, the verdict (band:
  `memory-governor-band` 1048575). It is asked before a Qwen session opens its weights (`fcms-open-context`), before a
  renewal allocates its second KV state (`fcmr-prepare`), before a walk turn begins (`hw-ready`, which waits no later
  than the window's last moment a turn could still start) and before the planner opens its voice (`lpr-ask`). A held
  answer is a choice point: the session opens nothing and names `host-memory-held`, the renewal leaves the owner as it
  was and the lane ends with a reason through its checkpoint, the walk takes no turn.
- **Leases: a promise another process can see before the pages show in the reading.** One reading is not enough (two
  processes that start together can both see room), so a grant is a lease: `/private/tmp/form-memory-leases/<pid>.lease`,
  one JSON line `{pid, who, key, shared, own, granted_unix_ms, extra, extra_unix_ms}`. `key` names the shared thing
  (the model file's path, `""` for none), `shared` its bytes (the weights' file size), `own` the bytes this
  process holds beside them (KV state, scratch, margin; it includes `extra`, the part granted later than the weights, a renewal's
  second KV state, stamped on its own). `shared` and `own` read 0 to 2^50 bytes; any other row reads as no lease and
  is removed with the torn ones (and a stale `<pid>.lease.part`). `mg-lease` reads the leases and the machine under one
  lock (`dir-lock.bml`: a directory only one maker makes, its owner's pid inside, released in a deferred step so a stop
  releases it too; a dead holder is taken over at once, a live one is waited for until its own stamp is a minute old,
  then the waiter's 10 s patience answers `lease-lock-busy`; the grant reads the lock's owner once more before it
  writes), removes the file of every dead pid, and judges `own` + `shared` (unless a live lease already holds the
  same key: the weights are one physical copy in the page cache) + the pending bytes of every other lease (its grant for
  180 s, its extra for 180 s from its own stamp, never the whole lease again) through the same policy. Granted writes the
  lease; held writes nothing and names why. When the lease directory itself cannot be made the bare reading's verdict
  answers, named `lease-dir-unavailable`, so the machine is still guarded by one reading. A process's lease is its whole
  promise (the same key restates it, key `""` adds a renewal's second KV state as extra, `mg-lease-return` gives it back
  when a stream is retired, `mg-release` removes it where the session closes). Doors that open models take leases:
  `fcms-open-context` (key = the model path, released at the session's close or when the open gives up),
  `fcmr-prepare`, `lpr-ask`. The walk's own pre-check (`hw-ready`) stays a bare wait: its child lane takes the real
  lease in its own process. `observe/memory-governor-run.bml` judges through the same lease-aware policy without taking
  a lease, so the verdict it prints counts the leases it lists. Proven model-free in `memory-governor-band` 67108863
  with each defect planted and read below its head (no lock, a key counted twice, pending ignored, a dead lease never
  removed, no deferred release; then a fail-closed directory, an unchecked owner, no range check, a renewal that
  restamps the whole lease, a no-op return, a door that ignores leases, a part file never swept, a takeover at the
  waiter's patience).
- **A lane that already holds memory gives it back.** `fcac-loop` (`form-cli-code-live.bml`) is entered once at each
  turn boundary and asks `mg-low?` of the machine (available below half the reserve, wired above 80 percent, or the
  compressor above 40 percent): a low reading saves the checkpoint and ends the lane through `fcac-finish` with the
  reason `host-memory-low`, its session released at the settle, so a later walk resumes the checkpoint and no model
  token is spent. A roomy reading, or one that did not arrive, continues unchanged, and a lane that spent all its turns
  ends by that, not as a memory ending. The walk reads that ending, and a session open refused for memory
  (`host-memory-held:<why>`), as a choice point (`mg-memory-ending?`, the one word its readers share): not idle, not an
  unknown spend, no option spent (`nt-untried?`, `nt-attempts`), its wall no option's measure (`hw-wall`), no row in
  the review's options (`lfr-options`: never a split, never a lane failure), no tried approach in the ladder
  (`ntl-hist-of`: an interrupted row, so the first way is not refused and the resume is not capped). A memory ending
  that saved a checkpoint and is the contract's latest row leaves one pending row in the history: the resume way takes
  its checkpoint and hears `ntl-continue-text` (nothing failed, so no "diagnose" instruction), and the first way waits
  for it (`memory-pending`). The next turn's `hw-ready` waits for the machine. Bands: `form-cli-code-low-memory-band`
  1023, `native-turn-ladder-band` 262143, `local-flow-reading-band` 32767.

## Present floor and direction

Alive: the classified registry, Form-owned evidence logic, native-first routing,
the typed model-flow membrane, native LoRA training and fusing on Metal, and a full
real-GGUF token path on Metal behind the only ask route. Not alive: no production
authority for any local adaptation, no complete language dataset authorization, and
no measured majority-owned production workload.

Direction: route one consented real accepted task through the native door with an
appended accepted-final event so work share gains a denominator; bring native
decode toward realtime so a presence lane can translate live; keep each host
carrier to what a host boundary is for — environment bytes, stdin, file metadata,
process return codes, the clock — with every score, class, admission, authority and
weight update in Form. Additional large-model training waits for authorized,
non-overlapping rows.
