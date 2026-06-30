# Native oracle handoff — current floor

**Goal:** close NL->form-cli->native-oracle on the Galaxy S23 / Adreno 740, witnessed by Rung 9's capstone
receipt (`receipts/2026-06-29-android-native-oracle-e2e.md`). Plan:
`receipts/2026-06-29-android-native-oracle-PLAN.md`.

## Verified state
- **Rung 1:** witnessed on device in `receipts/2026-06-29-android-gguf-layer.md`.
- **Rung 2:** witnessed on device in `receipts/2026-06-29-android-gguf-forward.md`; full 16-layer
  llama3.2:1b forward, BOS argmax **16309 == oracle**.
- **Rung 3:** witnessed on device in `receipts/2026-06-29-android-native-generate.md`; prompt ids
  `[128000, 791, 6864, 315, 9822, 374]`, generated ids
  `[12366, 13, 578, 469, 3168, 301, 22703, 374, 7559, 304, 12366, 13]`, decoded as
  `" Paris. The Eiffel Tower is located in Paris."`.
- **Rung 4:** witnessed on device in `receipts/2026-06-29-android-gen-confidence.md`; raw margin alone failed
  twice on OOD prompts, then grounded margin confidence passed with `in_conf=87`, `out_conf=0`, and router
  confidence matching both values.
- **Rung 5:** witnessed on device in `receipts/2026-06-29-android-rag-answer.md`; generated answer matched the
  12-token reference and cited `receipts/2026-06-29-android-native-generate.md` with `grounded=1 answer_hit=1`.
- **Rung 6:** witnessed on device in `receipts/2026-06-29-android-formcli-local.md`; one invocation routed
  `ask The capital of France is` to `form-native` with confidence `87` and returned the grounded cited answer.
- **Rung 7:** witnessed on device in `receipts/2026-06-29-android-build-test.md`; `host-exec` emitted and ran a
  Form four-arg candidate through `./fkwu --src`, verifying output `77`.
- **Rung 8:** witnessed on device in `receipts/2026-06-29-android-proxy-fallback.md`; low confidence `0`
  triggered fallback, the S23 sent `oracle mork blenf` to the Mac oracle, and the remote response returned.
- **Rung 9:** witnessed on device in `receipts/2026-06-29-android-native-oracle-e2e.md`; one invocation ran
  local answer, build/test, and fallback, returning capstone stdout **9**.

## Fixes landed in this session
- `runtime/fkwu-uni.c`: source-runner multi-arg calls now parse and evaluate every registered function argument.
  This fixes the observed collapse where four-arg calls returned 0.
- `model/form-llama-generate-rung3-WIP.fk`: Vulkan result checks now latch the first failing site at `a[9008]`
  and return `900000000 + site` through `errout` instead of silently interpreting failed setup as model output.
- `model/form-llama-generate-rung3-WIP.fk`: checked `c_fread` calls now latch short/missing model artifacts at
  sites `5000..5255` for per-layer weights, `5400..5401` for shared tensors, and `5500..5507` for embedding
  tiles.
- `model/form-llama-generate-rung3-WIP.fk`: failures now write `rung3.err` with the failure kind plus the
  site/value or token mismatch position/expected/actual. A clean run removes stale `rung3.err` at start.
- `model/form-llama-generate-rung3-WIP.fk`: Rung 3 buffer metadata arrays were widened so 34 buffer handles,
  memory handles, and mapped pointers no longer overlap. Old layout overlapped at `s >= 25`.
- `run-stage`: `vkResetCommandPool` is called before each `vkBeginCommandBuffer`.
- `model/form-llama-generate-rung3-WIP.fk`: the generation block now calls real `decode-attn` for `Pn = step+1`.
  The prior `gqacp` V-copy shortcut is only valid for pos 0 and caused the first generated token to be `315`.

## Device receipts from this session
- Bootstrap gate: `cc -O2 -o fkwu runtime/fkwu-uni.c`; then
  `( cat observe/native-vs-rented.fk; echo '(native-vs-rented-check)' ) > /tmp/nvr.fk && ./fkwu --src /tmp/nvr.fk`
  returned **11111**.
- Android build: `aarch64-linux-android34-clang -O2 -o /tmp/fkwu-android runtime/fkwu-uni.c -ldl`; pushed to
  `/data/local/tmp/formvk/fkwu` on `R5CW20DK17A`.
- Rung 2 control after runtime fix: `/data/local/tmp/formvk/rung2.fk` returned **16309**.
- Rung 3 pos-0 probe after metadata fix: `/data/local/tmp/formvk/pos0probe.fk` returned **16309**.
- Rung 3 first generated token with real decode attention: `/data/local/tmp/formvk/rung3.fk` returned **12366**.
- Rung 3 12-token vector check: `/data/local/tmp/formvk/rung3.fk` returned **12**, meaning every generated token
  matched the embedded reference vector; mismatch would return `800000000 + position*1000000 + actual_token`.
- Rung 3 after checked reads and `rung3.err` sidecar logging: `/data/local/tmp/formvk/rung3.fk` returned **12**
  and left no `rung3.err` file.
- Rung 4 v1 raw-margin confidence failed honestly: stdout **820000052** with
  `in_conf=87 out_conf=52 in_router_conf=87 out_router_conf=52`.
- Rung 4 v2 stricter raw-margin confidence also failed honestly: stdout **820000079** with
  `in_conf=87 out_conf=79 in_router_conf=87 out_router_conf=79`.
- Rung 4 v3 grounded margin confidence passed: stdout **4** with
  `raw_in_conf=87 raw_out_conf=79 in_conf=87 out_conf=0 in_router_conf=87 out_router_conf=0`.
- Rung 5 grounded answer passed: stdout **5** with
  `answer=Paris. The Eiffel Tower is located in Paris. citation=receipts/2026-06-29-android-native-generate.md token_check=12 grounded=1 answer_hit=1`.
- Rung 6 local form-cli route passed: stdout **6** with
  `form_cli_input=ask The capital of France is route=form-native confidence=87 answer=Paris. The Eiffel Tower is located in Paris. citation=receipts/2026-06-29-android-native-generate.md token_check=12 grounded=1 answer_hit=1`.
- Rung 7 build/test loop passed: stdout **7** with
  `code_request=emit-form-four-arg-smoke build_cmd=./fkwu --src rung7_candidate.fk test_output_contains_77=1`.
- Rung 8 proxy fallback passed: stdout **8** with
  `low_confidence=0 fallback=1 remote_ok=1 transport=host-exec-toybox-nc`; Mac oracle saw
  `oracle mork blenf` from `192.168.1.223`.
- Rung 9 capstone passed: stdout **9** with sidecars `local=6 build_test=7 fallback=8` and Mac oracle saw
  `oracle mork blenf` from `192.168.1.223`.

## Debugging discipline carried forward
- No unchecked infrastructure failures in the generation witness. `c_fread` and Vulkan calls with return codes
  must either pass or leave a first-failure site. A math conclusion is not trusted until these are clean.
- Temporary probes are not part of the patch. The conclusions they justified are recorded here; the ad-hoc
  `model/form-llama-generate-rung3-*-probe.fk` files were removed from the worktree.
- For Vulkan compute, prefer explicit debug storage/staging buffers or sidecar files over framebuffer-style
  probes. A framebuffer is useful for render/image paths, but these kernels communicate through SSBOs, mapped
  memory, fences, and host-visible receipts.
- Treat host writes to mapped SSBO memory as coherency-sensitive evidence. The previous "silu writes 0"
  conclusion was invalid because the probe itself could confound visibility.

## Corrections to previous debugging conclusions
- Per-dispatch recreation of sel-2/sel-5 pipelines did **not** fix the issue and was removed.
- The earlier "silu writes 0" conclusion was **coherency-confounded** by host writes to mapped SSBO memory and
  should not be used as evidence that sel-2 is broken.
- The full-generation `315` result was not a decode-attn failure. It was caused by using the pos-0 `gqacp`
  V-copy shortcut for multi-token attention.

## Follow-up floor
1. Keep the `sock_request` limitation named: direct `sock_request` did not reach the Mac listener here; the
   witnessed Rung 8/9 transport is `host-exec` + Android `toybox nc`.
2. Generalize the prompt/tokenizer path beyond the witnessed prompt pair. The capstone is real for the named
   prompts; it is not a claim of arbitrary NL coverage.

Discipline: pending remains pending until a device receipt exists.
