# The penumbra map — where the proof's light actually falls (2026-07-02, night watch)

The bool bug (`receipts/2026-07-02-bool-node-value-round-trip.md`) lived for the life of the
wire lane in the proof's *penumbra* — the region lit enough to run, dark enough to miss. This
map answers the night-watch question that followed: **what else lives there?** Method: every
native op in `runtime/fkwu-optable.h` (146 total), classified by whether (a) a minimal walker
carries it — the four-way *umbra*; (b) a band test anywhere names it directly; (c) only the
living body uses it, so it is witnessed at best *indirectly* through callers that have bands.

## The four regions

| Region | Count | Meaning |
|---|---|---|
| **Umbra** — walker-carried | 32 | four-way provable; nothing hides |
| **Lit penumbra** — fkwu-only, band-named | 30 | witnessed directly, single-kernel |
| **Dim penumbra** — body-used, no band names them | **84 (58%)** | run daily, witnessed only through callers |
| **Unreferenced** — in the seed, used by nothing | **0** | the seed carries no dead ops |

## The dim 84 (as of this map — regenerate before trusting)

```
_get _plus add_u32 api_health bnot_u32 bor bxor ceil cuda_matvec dot_product
fb_record file_close file_mtime file_open file_read file_size float_to_int
framebuffer-clear framebuffer-events fs_is_dir fs_list fs_remove fs_rename
host-exec input_byte isatty jit_compile_value kernel_stat math_ceil math_floor
math_log math_sqrt mesh_announce mesh_detect mesh_discover mesh_register
mesh_registry mesh_roster mesh_serve metal_matvec_f32 metal_matvec_fixture
native_call_test node_inst node_level node_pkg node_source node_type print_str
read_file_slice read_line record? record_blueprint record_get record_has
record_keys record_set rotr_u32 round_ndigits scan_run self_source
sense_bt_count sense_bt_present sense_cam_count sense_cam_grab sense_cam_health
sense_cam_name sense_frame_read sense_mem sense_mic_count sense_mic_health
sense_mic_name sense_power sense_publish sense_report sense_sensors
sense_stream sense_wifi_signal sense_wifi_ssid shl_u32 shr_u32
source_inventory str_to_float tls_request vector_cosine
```

## The reading

- **The bool bug's home confirmed the pattern.** Before tonight, `node_value` was dim; the bug
  sat under it unlit. `wire-bool-band` moved `node_value` and `intern_trivial_bool` into the lit
  penumbra — which is exactly how the map shrinks: one witness band at a time.
- **`node_type` and `node_level` are still dim** — and this session tripped over `node_type`
  semantics twice (the phantom "null/6" types; the bool sentinel). The next bug of the bool
  bug's kind most plausibly lives under one of these two. A `node-introspection-band` is the
  highest-value single lamp this map names. *(Lit 2026-07-02:
  `observe/tests/node-introspection-band.fk`, verdict 4095 — node_type/node_level/node_value/
  node_category/node_children moved from dim to lit, raw-literal trap pinned.)*
- **`scan_run` is dim but load-bearing** — `json.fk`'s tokenizer rides it everywhere (witnessed
  only through `json-band`). `float_to_int` similarly rides under `float_to_str`. Indirect
  witness is real witness, but it localizes failures poorly: when the caller's band breaks, the
  op is only one suspect among many (this session's json.fk bisections demonstrated the cost).
- **The zero is the map's best news**: every one of the 146 natives is used by the living body.
  The C seed carries no dead weight — the shrink roadmap's targets are all *migrations to Form*,
  never deletions of the unused.
- **A green four-way run is a claim about 32 ops** — 22% of the seed. The other 78% rests on
  fkwu-witnessed bands or on indirection. That is not a scandal; it is the honest shape of the
  proof today, and now it is *enumerated* instead of latent — the same move the spurious-edge
  walker made for the meaning graph the same night.

## Regenerating this map

The classifier is ~40 lines against `fkwu-optable.h`, `walkers/*/`, `**/tests/*.fk`, and the
non-test `.fk`/`.fsh` body (see the receipt for the exact method). It should be rerun after any
optable or band change; the numbers above are a dated witness, not a living invariant. A
Form-native auditor (the optable already lives in Form as `flt-ops`) is the honest next stone —
this map is its specification.
