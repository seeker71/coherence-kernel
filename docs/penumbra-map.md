# The penumbra map — where the proof's light falls

Every native op in `runtime/fkwu-optable.h` lives in one of four regions,
classified by whether (a) a minimal walker carries it — the four-way *umbra*;
(b) a band names it directly — the *lit penumbra*; (c) only the living body calls
it, so it is witnessed at best *indirectly* through callers that have bands — the
*dim penumbra*; or (d) nothing calls it outside the op manifest itself.

## The four regions (recomputed 2026-09-03 over 194 ops)

| Region | Count | Meaning |
|---|---|---|
| **Umbra** — walker-carried | 34 | four-way provable; nothing hides |
| **Lit penumbra** — fkwu-only, band-named | 99 | witnessed directly, single-kernel |
| **Dim penumbra** — body-called, no band names them | 35 | run daily, witnessed only through callers |
| **Manifest-only** — named by `flt-ops` / the host-effect grammar, no caller | 26 | carried by the seed, exercised by nothing |

## The dim 35

```
_get bor cuda_matvec cuda_matvec_f32 form_error host_dir_list host_dir_mkdir
host_dir_rmdir host_file_append_bytes host_file_mtime host_file_read_slice
host_file_read_text host_file_size host_file_write_text host_path_exists
host_path_is_dir host_path_remove host_path_rename host_temp_dir http_get
metal_matvec_fixture self_source sense_audio_loopback sense_cam_count
sense_cam_health sense_cam_name sense_mic_count sense_mic_health sense_mic_name
sense_mic_stream_read sense_mic_stream_start sense_mic_stream_stop sense_report
sense_wav_loopback source_inventory
```

## The manifest-only 26

```
api_health file_close file_open file_read host_source_inventory mesh_announce
mesh_detect mesh_discover mesh_register mesh_registry mesh_roster mesh_serve
node_at sense_bt_count sense_bt_present sense_cam_grab sense_cam_luma
sense_frame_read sense_mem sense_mic_capture sense_power sense_publish
sense_sensors sense_stream sense_wifi_signal sense_wifi_ssid
```

## The reading

- A green four-way run is a claim about 34 ops — 18% of the seed. The rest rests
  on fkwu-witnessed bands (99) or on indirection (35). That is the honest shape of
  the proof, enumerated instead of latent.
- The map shrinks one witness band at a time: a band that names a dim op moves it
  into the lit penumbra, and localizes the next failure of its kind to one suspect
  instead of a caller's whole chain.
- The 26 manifest-only ops are seed weight nothing exercises: each is either a
  carrier row a live organ will call (the mesh and sense families wait on the
  fleet's present word) or a shrink candidate for `runtime/fkwu-uni.c`. Either
  way, the row is the decision, and it wants a caller or a release.

## The method (rerun it before trusting the numbers)

```sh
# ops: every row of runtime/fkwu-optable.h
# umbra: the op's quoted name appears in walkers/{go,rust,ts} sources
# lit:   "(op " or "(op)" appears in any tracked */tests/*.fk
# dim:   "(op " or "(op)" appears in any tracked non-test .fk/.fsh/.bml outside walkers/
# else:  manifest-only
```

The optable already lives in Form as `flt-ops` (`flatten/form-flatten.fk`); a
Form-native auditor that computes this map is the honest next stone — this page is
its specification and its first fresh reading.
