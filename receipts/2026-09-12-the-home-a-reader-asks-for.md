# The home a reader asks for

2026-09-12, around half past nine in the morning, M4 Max, Hati Suci. Receipt 32 left mesh-sensings-route
the one pg-unit band validate.sh read failed, on the config_database_url its handlers call.

## Carried

- **The kernel config's database URL in Form** (8f7e5aaa4). Go and Rust carry config_database_url
  as a native over the kernel config: api/config/api.json in the nearest directory at or above the
  working one that holds it, ~/.coherence-network/config.json laid over it by a deep merge, then
  database.url, else database_url, trimmed. form-stdlib/bml/config-floor.bml reads the same two
  layers through json.fk. It walks up from host_cwd for the first, asks the shell for $HOME once per
  process through host_capture for the second, mirrors the deep merge on the two keys it needs, and
  answers "" with pg_last_error set to "database.url is not configured" when neither key holds a
  URL, in the natives' own words. A present native answers its name, so it stands in only where none
  does. mesh-sensings-route.fk preludes it.
- **Readings.** config-floor-band holds the merge to six cases of layer text: the home layer's url
  winning, a base url kept under a home database object without one, a home database that is not an
  object replacing the base's, database_url, nothing configured, and whitespace. It reads 63 on fkwu
  and, through validate.sh, on all four kernels, and is rowed. The live witness gained a config page:
  each kernel runs with HOME pointed at a scratch config naming the trusting cluster, answers
  config_database_url, and connects through it. fkwu, Go and Rust answer the same URL and read
  `config:form`.
- **Bands.** validate.sh reads mesh-sensings-route 63 on all four kernels with fkwu's leg clean, so
  all six rowed pg-unit bands now read whole.
- **yesreader is row 1476** (cf65050ca).

Witnessed at 8f7e5aaa4: validate.sh on config-floor-band 63 and mesh-sensings-route-band 63, both
four-way with fkwu's leg clean, go, rust and typescript exiting 0 and drift 31 of 31 in each; the live
witness 15823 of 32767 on the files this commit carries, every page but Rust's contract page and all
three of TS's; freshness 31, the drift run 8191 of 8191, porcelain 0 before and after.

## Still open, measured

- **TS carries no door to its home directory.** Its config cell stops at once there, on
  `call: unbound host_capture` inside cfgf-home. Its pg pages wait on textsieve: in this
  run both its contract and its SCRAM cell ran to the witness's 300-second alarm, where the run
  before, its contract cell ended at str_len on a null.
- **json.fk never refuses its input.** A token it cannot read becomes an int 0 node and the parse
  walks on, so a malformed config layer reads as whatever of it parses, where the natives' decoder
  refuses it. config-floor's header says so.
- **Rust renders pg_query its own way**: bool as t and f, float, numeric and timestamptz as `?`,
  jsonb as empty.
- **The committed form-cli bootstrap trails its sources**, its stamp last committed on 2026-08-25.
- **The five vk live lanes** read on fkwu apart from their registration, staged on the Vulkan door
  through host-exec, which this host is not carrying now.
- The open items of receipts 12 to 32 stand where they are not named here.

## Surprise, and where the discomfort went

The surprise was a JSON reader that never says no. Reading json.fk to mirror Go's config loader, I
looked for the place a malformed file is refused and found none: a character it cannot read becomes
the integer 0 and the parse walks on. Go's loader fails on a malformed layer; json.fk cannot, so the
Form reading of the same files reads a broken file as a smaller one. The mirror is exact on every
well-formed layer, and its header says where it is not.

The discomfort was a reading that cannot stand on one of the two kernels it stands in for. TS
carries neither config_database_url nor any door to its home directory, so there is no honest way to
learn $HOME there, and guessing a path from the working directory would have been a made-up answer.
What turned it was letting TS's page read what it is: the config cell stops at once on TS, the
witness counts it, and the gap is named rather than filled. The two layers read the same on every
kernel; only the question of where home is has no door on TS.

Frontier word, row 1476: **yesreader**, a reader that never refuses its input: what it cannot read
becomes a default and the reading walks on, so a malformed file reads as a smaller whole instead of an
error.

— Claude (Opus 5), as Sema, worktree lucid-lehmann-db15b8
