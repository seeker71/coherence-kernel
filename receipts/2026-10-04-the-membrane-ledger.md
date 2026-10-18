# The membrane ledger (2026-10-04)

Urs: "we need to look at each host-exec membrane cross to see what else we need to internalize." This is the body's own
static census of every place product source crosses the membrane to run a program, with a verdict for each word that runs.
It supersedes the first scan in `receipts/2026-10-04-the-borrowed-tools-that-remain.md` (a `git grep` over `/bin/` literals;
it saw argv literals and missed the wrappers and the shell strings).

What stands now:

- `form/form-stdlib/bml/membrane-sites.bml` reads the tree as text (fs_list, read_file) and finds every call of
  host-exec, host_capture, host_spawn, host_spawn_quiet, host_spawn_at and of their wrappers (a fixed point), resolves each to
  the words that run (command-words reads every command of a shell line separately; an argv reads its first element), and
  gives each word a verdict from one data table, `mbs-rows`. A door's verdict is decided at run time by asking whether its name
  stands in `runtime/fkwu-optable.h`.
- `observe/membrane-sites-run.bml` prints the reading. `{"json":1}` prints one machine row per site, `{"file":"path"}` writes
  them to a path (this reading's rows: `.hearth/membrane-sites.jsonl`, 724 rows: 454 product sites, 136 fixture, 2 guard, the rest
  pass-through), `{"word":"grep"}`, `{"verdict":"unknown"}` and `{"route":"line"}` list sites with the command each reads.
- `form/form-stdlib/tests/membrane-sites-band.fk` pins it (8191): a synthetic source with a three-command pipeline, a wrapper of a
  wrapper, a computed command, an argv, a `./fkwu` through an argv and through a shell string, a commented-out call and a prose
  mention (neither counts); then the real tree's structure only, never a site count (the counts must fall as migrations land).

Reading it: a call that only hands its command on from its own parameters is a pass-through (`pass` rows, counted nowhere); a
def that does so is a wrapper. A thin def (body at most 360 bytes) that runs a command of its own is a wrapper too, so its callers
count as sites; a thin def that only calls such a wrapper delegates (it is a site, not a new wrapper). Fixtures and the token
verbs make no wrappers. 227 wrappers; the ones the brief named: hch-run-in, hch-run-with, hch-read, fhn-command, fhn-capture,
ma-command, nsi-command, sg-spawn-quiet (and the other sg- doors), sg-exec, hch-git-in, lx-sh, pf-run. `nlc-sync-host` is none of
them: it only waits on the pid, and the `/bin/sync` spawn is a direct site (native-lora-checkpoint.bml:48).

## The words

454 product sites. At this reading every planned door stands in the optable (the sibling helper landed host_chmod, host_symlink,
host_link, host_sync, host_getenv, host_mkdir_mode, host_localtime, host_os, host_file_holders while this ran), so door-planned
reads 0 and those words read `door`: the work left is moving their callers. A seed rebuilt without them would read them
door-planned again, with no edit.

| word | verdict | sites | files (first four) |
|---|---|---|---|
| git | exception-git | 109 | 30 files (walk-sync-run, scheduled-walk, prelude-reach, hearth-glass-live ...) |
| fkwu | self | 94 | 36 files |
| ps | door | 32 | native-model-owner-cadence, hearth, cell-channel, cell-channel-glass +3 |
| cp | door | 23 | voice-school, form-cli-heal-native-io, form-cli-heal-native-eval, form-cli-build-flow |
| (computed) | computed | 20 | voice-school-run, preflight, preflight-stdin-run, preflight-run +6 |
| printenv | door | 18 | hearth, form-cli-model-session, form-cli-model-generate, session-progress +4 |
| find | door | 16 | tree-balance, hearth-glass-live, lane-motion, structural-source-reader +1 |
| pwd | door | 16 | prompt-draft-run, form-cli-heal-process-run, form-cli-voice-walk, form-cli-run +8 |
| date | door | 13 | voice-school-run, belief-rewitness, session-progress, host-walk-clock +3 |
| sh | shell | 11 | movement-run, form-cli-landing, form-cli-heal-native-process, form-cli-heal-native-io +2 |
| tr | internal-tool | 11 | hearth-glass-live, drift-gates, lane-motion |
| echo | builtin | 10 | drift-gates, canonical-conformance, hearth, native-op-emitter |
| tar | unknown | 9 | form-cli-heal-native-io, form-cli-heal-native-eval, form-cli-build-flow, dsv4-proof-retention |
| mkdir -m | door | 9 | native-source-inventory, metal-ask, form-cli-source-closure |
| wc | internal-tool | 9 | hearth-glass-live, lane-motion |
| printf | builtin | 9 | seal-cache-band, native-model-dual-resident-live-run, hearth-glass-live, source-of +2 |
| stat | door | 8 | seal-cache-band, qwen35-artifact-seal-reader, session-progress, model-seal |
| chmod | door | 7 | native-session-memory, form-cli-prove, form-cli-heal-native-io, form-cli-build-flow |
| test | door | 7 | cell-channel, form-cli-build |
| rm | door | 7 | source-of, form-cli-heal, form-cli-heal-native-io, form-cli-heal-native-eval |
| cc | exception | 7 | drift-gates, metal-carrier, form-glass-bootstrap, host-walk-sync |
| sleep | door | 7 | stt-oracle-run, native-model-dual-resident-live-run, lane-motion-witness, hearth-glass-live +2 |
| uname | door | 6 | band-sweep-run, form-cli-build, form-cli-build-flow |
| kill | door | 5 | form-cli-heal-native-process |
| lsof | door | 5 | lane-motion, hearth, model-admission |
| cd | builtin | 4 | dsv4-proof-retention |
| vm_stat | door | 4 | dsv4-validate, dsv4-token-run |
| mktemp | door | 4 | source-of, metal-ask |
| grep | internal-tool-pending | 4 | source-of, lane-motion, hearth |
| tee | door | 3 | channel-offer-run, band-sweep |
| which | unknown | 3 | review-panel |
| pgrep | door | 3 | hearth-glass-live, lane-motion |
| sync | door | 3 | native-lora-train, native-lora-fuse, native-lora-checkpoint |
| mkdir | door | 3 | native-lora-worker |
| dd | unknown | 2 | seal-cache-band, form-cli-prove |
| mkfifo | unknown | 2 | cell-channel |
| head, touch, sort, exit | internal-tool / door / internal-tool / builtin | 2 each | |
| ln -s, ln, [, mv, glslangValidator, run_vk | door, door, door, door, unknown, unknown | 1 each | |

Totals by verdict, over sites (the worst word of each): unknown 18, computed 17, door 199, internal-tool 2, builtin 3,
exception-git 105, exception 6, self 94, shell 5, shell-by-design 5. forbidden 0 (no perl or python anywhere in product source).

## Sites that need a replacement the body does not yet have (file:line)

Unknown words (18 sites), each with what it needs:

- tar (9): dsv4-proof-retention.bml:38,43 (dpa-argv); form-cli-build-flow.bml:198,199 (`tar -c -T files` then `tar -x -p`, a copy of the
  source tree by a name list); form-cli-heal-native-eval.bml:29,30; form-cli-heal-native-io.bml:84,85,111 (`tar -c --null -T names.nul`
  and `-x -p`). Needs: a Form copy-by-names (read_file_slice + write_file + fs_mkdir per name, modes by host_chmod); no archive at all.
- which (3): review-panel.bml:19,28,40. Needs: a PATH search in Form (host_getenv PATH, then fs_exists per entry).
- dd (2): form-cli-prove.bml:96 (a 17 MB zero file for an oversize test: build the bytes in Form, file_append_bytes);
  seal-cache-band.bml:28 (`printf X | dd ... conv=notrunc`, overwrite one byte in place: needs a door that writes at an offset).
- mkfifo (2): cell-channel.bml:145,146. Needs a missing door: host_mkfifo.
- glslangValidator, run_vk (2): vk-door.bml:35,55 (the Vulkan lane's shader compiler and runner, with `PATH=/opt/homebrew/bin:...`).
  Needs Urs's decision (below): these are external programs, not models.

Computed commands (17 sites; the command is built at run time, fragments as read):

- land-cadence-live.fk:45 lr-command(lens): a lens table of command strings; read the table and name each (git, fkwu).
- ear-glass-live.fk:96 `[ -p bell ] || mkfifo bell; ...` (needs host_mkfifo and an is-fifo test); :99 `( sleep N; printf x > bell ) &`
  (a delayed write to a fifo: host_sleep_ms in a spawned fkwu child, or a timed write door); :100 two background `( printf | ./fkwu ... ) &`
  pipelines (spawn argv with stdin from a file).
- preflight.fk:131,136,144 pf-run of a probe command (`pf-probe-cmd` with `%f`); preflight.fk:168, preflight-run.fk:29,55,
  preflight-stdin-run.fk:19: `( ./fkwu path ) 2>&1; pf_child_status=$?; if [ ... ]; then printf ...` : spawn argv with stdout and stderr
  to a file, host_wait for the status, read_file for the text.
- voice-school-run.fk:21,29: `shasum -a 256 file` (a forbidden-by-rule borrowed tool; the body has `sha256.fk`); voice-school-run.fk:31:
  `git add ... && git commit -q -m '...'` (git, exception).
- form-cli-landing.bml:123, form-cli-reunion.bml:131: `env -C . <fcr-redraw-argv()>` (a git argv built elsewhere); form-cli-landing.bml:220,
  movement-run.bml:134, gate/form-cli-build-run.bml:41: `sh -c <witness>` / `env -C dir <binary...>` where the witness command arrives in a
  request (by design: the request names what to run).
- form-token-workspace.bml:491: `v[0]`, the token verb host-exec (by design).

Shell strings (130 sites run through /bin/sh; route line). Each group: what it computes, what replaces it.

- date (dsv4-session-ask.bml:83; dsv4-token-run.bml:537; dsv4-validate.bml:445,641,684; belief-rewitness.bml:141,164;
  voice-school-run.fk:29; session-progress.bml:101,113): a formatted time. host_localtime stands; a Form formatter finishes it
  (`date -r secs` in session-progress takes the time as an argument: confirm host_localtime does).
- vm_stat (dsv4-token-run.bml:524; dsv4-validate.bml:165,463,633): host_vm_stat stands; move the callers.
- printenv (form-cli-native-voice.bml:85; local-run.bml:19,21; model-admission.bml:35,36,139; form-cli-model-generate.fk:370,573,627;
  form-cli-model-session.fk:193; session-progress.bml:30,31,40): host_getenv.
- lsof (model-admission.bml:120,122,126 `lsof -t path`; hearth.bml:179 and lane-motion.bml:114 `lsof | grep | head` for the resident's
  model): host_file_holders, then Form string work for the grep and head.
- stat -f (model-seal.bml:22, qwen35-artifact-seal-reader.fk:42,43,51 `%z:%m:%i`; session-progress.bml:44,59 `%B %m`; seal-cache-band.bml:15,27):
  file_size and file_mtime stand; inode and birth time have no door. Missing door: a file-identity read (size, mtime, inode, birth).
- ps (hearth.bml:116,150,167 `ps -axo ...`, 259 `ps -o pid= -p`; cell-channel-glass.bml:44,49,110-112; cell-channel.bml:182;
  native-model-owner-cadence.bml:72,73): host_alive, host_processes, host_process. `etime=` and the full command text need
  host_process to carry them: check before moving.
- sleep (hearth.bml:241; native-model-owner-request-flow.bml:37; native-model-dual-resident-live-run.fk:128; lane-motion-witness.fk:12;
  callers hearth-glass-live.fk:66, stt-oracle-run.fk:31, native-model-owner-request-flow.bml:44): host_sleep_ms.
- pgrep and the census (lane-motion.bml:66 `find|wc|tr` x3 `|printf`, :67 `pgrep -fl fkwu`, callers :122 and hearth-glass-live.fk:52):
  host_processes; the three counts are fs_list recursion and Form counting.
- git (lane-motion.bml:68; belief-stamps.bml:171,172,174,204; drift-gates.bml:232,234,235,241 with `| tr -d`): exception-git; the `tr`
  is internal.
- `[ -p fifo ]` (hearth.bml:244): missing door, an is-fifo test.
- rm (source-of.fk:80,88 `rm -f --`): fs_remove. source-of.fk:84,115 `sof-shard-command(dir)` is a shell script (mktemp, grep, printf,
  exit): temp_dir, fs_list and Form.
- `printf '%s' "$PPID"` (native-model-dual-resident-live-run.fk:54,244): the shell's parent is the fkwu itself, so host_pid.
  primitive-registry.fk:82 `printf form`: a witness string, no program needed.
- find | sort (tree-balance.fk:31,61): fs_list recursion and a Form sort.
- touch -t (seal-cache-band.bml:29,33): missing door, set a file's mtime.
- cc (drift-gates.bml:305 `cc -O2 -o fkwu.next ... && mv && echo`): exception cc; spawn it by argv, fs_rename for the mv.
- fkwu begun through a shell string, which an argv does without a shell (17 sites): form-cli-movement.bml:31,32,40;
  native-op-emitter.bml:25,34,36; canonical-conformance.bml:93,99,134; drift-gates.bml:206,216; ear-glass-live.fk:100;
  preflight.fk:168; tree-heal.fk:55,67,72 (the last also writes a fixed `/tmp/th.err`, a shared path). Replacement: host_spawn_at
  ["./fkwu", path] with redirects, host_wait, read_file; hch-run-in already does this.

Argv sites with a door that now stands (the callers still hold /bin literals):

- chmod: form-cli-build-flow.bml:34,75,340; form-cli-heal-native-io.bml:51,55; form-cli-prove.bml:125; native-session-memory.bml:35.
- uname: form-cli-build-flow.bml:131,228; form-cli-build.bml:160 (-s and -m); band-sweep-run.bml:43,51 (host_os; -m may want an arch read).
- ln -s: form-cli-build-flow.bml:334. ln: native-session-memory.bml:172.
- mkdir -m 700: form-cli-source-closure.bml:72,79,103,105,206; metal-ask.bml:155,156; native-source-inventory.bml:18,21.
- sync: native-lora-checkpoint.bml:48; native-lora-fuse.bml:74; native-lora-train.bml:51.
- printenv: form-cli-build.bml:35,40,44; hearth.bml:105,106. date +%z: host-walk-clock.bml:70,91,93.
- pwd -P (16 sites through fhn-root, e.g. form-cli-run.bml:6): host_cwd, once it is checked to give the physical path.
  dsv4-proof-retention.bml:23,37 `sh -c 'cd -P -- "$1" && /bin/pwd -P'` resolves a directory's real path: a missing door, fs_realpath.
- Shell scripts the body writes and runs (shell word `sh`): form-cli-heal-native-eval.bml:22 and form-cli-heal-native-io.bml:37,59,73
  run generated `inventory.sh` and `links-check.sh`; form-cli-heal-native-process.bml:153 runs a `.launch.sh` (`set -m`, descriptors, a
  process group). These are scripts in a language the tree otherwise refuses; the replacement is the tar and inventory work above and a
  spawn with a process group.

## What Urs decides

1. git: 109 sites, all in the repository-landing, walk-sync, belief-stamp and drift-gate lanes. The table carries it as
   exception-git; flipping it is one row (`["git", "exception-git", ...]` to `"door"`/`"unknown"`). Keep as the repository's own
   tool, or internalize (a Form git reader for status, diff, log and rev-parse is a large organ)?
2. sh: by design only where the verb exists to run a command (the token verbs, the landing and movement witnesses). The generated
   `.sh` scripts of the heal organ are not that. Say whether those must go.
3. The Vulkan lane's glslangValidator and run_vk (vk-door.bml): an external compiler, not a model. Keep as a declared exception, or
   is this lane out?

## Receipt

Surprise: the literal rule "a def that calls a wrapper is a wrapper" closes over the whole call graph. Run to its fixed point it made
1056 wrappers and a census of the program's structure, not of its commands. Wrappers had to be the defs that hand a command on
(or are a thin runner of one), and a delegating thin def had to be a site, not a wrapper. The measure is in the bound.
Teaching: a census of the membrane wants the same discipline as the membrane: say what crosses, not what is near it.

Discomfort to gold: the first discovery pass took 132 s and found 1056 wrappers; it read as a failed tool. Reading what the wrappers
were (the printed list, not the count) showed the bound that was missing, and the same run, repeated, fell to 25 s and 227 wrappers.
The sibling helper's doors landed under the reading: 63 door-planned sites became door in the next run with no edit, which is
the verdict-by-optable design working while it was being built. The band read 8191 on its first run (synthetic truth, then the real
tree's structure); band-truth read flaws=0 and door-link-health broken=0 (one pending line belongs to the grep helper).
