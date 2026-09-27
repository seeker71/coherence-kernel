# The penumbra map — where the proof's light falls

Every native op in `runtime/fkwu-optable.h` lives in one of four regions,
classified by whether (a) the minimal walkers carry it — the four-way *umbra*;
(b) a band names it directly — the *lit penumbra*; (c) only the living body calls
it, so it is witnessed at best *indirectly* through callers that have bands — the
*dim penumbra*; or (d) nothing calls it outside the op manifest itself.

The map is a reading, not a score: it says where a failure of a given op would
be caught, and where it would hide. The counts below come from the method at the
end, run over the current tree; rerun it before trusting them.

## The four regions (259 ops)

| Region | Count | Meaning |
|---|---|---|
| **Umbra** — walker-carried | 33 | four-way provable; nothing hides |
| **Lit penumbra** — fkwu-only, band-named | 147 | witnessed directly, single-kernel |
| **Dim penumbra** — body-called, no band names them | 58 | run daily, witnessed only through callers |
| **Manifest-only** — named by the op manifest / the host-effect grammar, no caller | 21 | carried by the seed, exercised by nothing |

## The dim 58

```
_get api_health bor cuda_matvec cuda_matvec_f32 field_reset file_close
file_open file_read gift_roster_register host_cpu_busy_us host_cpu_us
host_dir_list host_dir_rmdir host_disk_stat host_file_append_bytes
host_file_mtime host_file_read_slice host_file_read_text host_file_size
host_file_write_text host_gpu_busy_us host_gpu_utilization host_load_avg
host_nice host_path_exists host_path_is_dir host_path_remove host_path_rename
host_processes host_spawn host_vm_stat http_get kernel_page_bury
kernel_page_ended kernel_roster_adopt metal_matvec_fixture node_at self_source
sense_audio_loopback sense_cam_count sense_cam_health sense_cam_name
sense_mic_count sense_mic_health sense_mic_name sense_mic_stream_read
sense_mic_stream_start sense_mic_stream_stop sense_report
sense_speaker_stream_start sense_wav_loopback shm_read shm_write
source_inventory terminal_cols terminal_lines value-kind
```

## The manifest-only 21

```
host_source_inventory kernel_hot kernel_roster_forget mesh_announce mesh_detect
mesh_discover mesh_register mesh_registry mesh_roster mesh_serve sense_bt_count
sense_bt_present sense_cam_grab sense_cam_luma sense_mem sense_mic_capture
sense_power sense_publish sense_sensors sense_wifi_signal sense_wifi_ssid
```

## The reading

- A green four-way run is a claim about 33 ops — 13% of the seed. The rest rests
  on fkwu-witnessed bands (147) or on indirection (58). That is the honest shape
  of the proof, enumerated instead of latent.
- The map shrinks one witness band at a time: a band that names a dim op moves it
  into the lit penumbra, and localizes the next failure of its kind to one suspect
  instead of a caller's whole chain. The host-effect family (`host_*`, `sense_*`)
  is most of the dim region.
- The 21 manifest-only ops are seed weight nothing exercises: each is either a
  carrier row a live organ will call (the mesh and sense families wait on the
  fleet's present word) or a shrink candidate for `runtime/fkwu-uni.c`. Either
  way, the row is the decision, and it wants a caller or a release.

## The method

The op surface lives in Form as `form/form-stdlib/native-op-manifest.fk`;
`flatten/` generates `runtime/fkwu-optable.h` from it. The walkers each carry a
generated list of every reserved head (`walkers/go/reserved_heads.go`,
`walkers/rust/src/reserved_heads.rs`, `walkers/ts/reserved-heads.ts`), so a name
in that list says nothing about whether the walker evaluates it; those files are
left out of the umbra test.

```text
ops:   every row name of runtime/fkwu-optable.h
umbra: the quoted name "op" appears in each of walkers/go, walkers/rust and
       walkers/ts, outside their reserved-heads list
lit:   "(op " or "(op)" in any tracked */tests/*.fk or *.fsh,
       or a call op( in any tracked */tests/*.bml
dim:   the same patterns in any tracked non-test .fk/.fsh/.bml outside walkers/
else:  manifest-only
```

The method is a handful of `git grep` passes and runs in seconds. A Form-native
auditor that computes this map from the manifest rows is the next stone; this
page is its specification.
