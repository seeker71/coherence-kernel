# The borrowed tools that remain (2026-10-04)

Urs's rule, stated three times in the voice session: only Form code, no external tool or program, external models fine.
This is the scan of the merged tree (`origin/main` + the native voice and lowering work) for a program the body
borrows through `host_spawn_at` / `host_spawn_quiet` / `hch-run-in` / `fhn-command` / `host-exec`. The voice lane,
the GPU lease, the track supervisor and the host walk's deadline carry none of these any more (`host_signal` replaced
`/bin/kill -9`). What is left belongs to the lanes below; the sibling campaign that removes the shell
(`spawn-guard.bml`, the structural gate allowlist) is the natural owner, and each row names the door it is missing.

Re-scan:

```sh
git grep -n -E 'host_spawn_at\(\["(/bin|/usr/bin)/[a-z]+"|host_spawn_quiet\(\["(/bin|/usr/bin)/|\["/bin/(kill|sh|sync|mkdir|chmod|ln|cp|mv|rm|cat)"' -- 'form/form-stdlib/*.bml' 'form/form-stdlib/*.fk' 'form/form-stdlib/bml/*.bml' 'observe/*' 'gate/*' ':!form/form-stdlib/tests'
```

| Tool | Where | The door the body lacks |
|---|---|---|
| `/bin/kill -<sig>` of a group (`-pid`) | `bml/form-cli-heal-native-process.bml` (`fhn-signal`, `fhn-signal-descendant`) | `host_signal` takes one pid above zero; a process-group signal is a separate decision (pid < 0) |
| `/bin/sh script`, `/bin/ps -e -o pid=,ppid=` | `bml/form-cli-heal-native-process.bml` (`set -m` launch script, `fhn-child-tree`) | `host_processes` already lists kernels; the launch script places descriptors and a process group |
| `/bin/cp`, `/bin/rm -rf`, `/bin/chmod`, `/bin/ln -s` | `bml/form-cli-build-flow.bml`, `form-cli-heal-native-eval.bml`, `form-cli-heal-native-io.bml`, `form-cli-prove.bml`, `native-session-memory.bml` (`/bin/chmod 700`, `/bin/ln`), `voice-school.bml` (`/bin/cp`) | a file copy through `read_file_slice` + `write_file` is expressible now; `chmod` and `ln` have no door; `fs_rmdir` is recursive and removes a tree already |
| `/bin/mkdir [-m 700]` | `bml/form-cli-source-closure.bml`, `bml/metal-ask.bml`, `bml/native-source-inventory.bml`, `native-lora-worker.bml` (twice) | `fs_mkdir` makes one level; the mode (700) has no door |
| `/bin/sync` | `native-lora-checkpoint.bml`, `native-lora-fuse.bml`, `native-lora-train.bml` | an `fsync` / `sync` door |
| `/usr/bin/cc` | `metal-carrier.bml` (`mca-build`) | none: the C compiler is the one external tool the seed needs to be BUILT (AGENTS.md says so; `band-sweep-run.bml` repeats it) |
| `/usr/bin/tee` | `observe/channel-offer-run.bml` | a file tee is two writes |
| `/bin/sh -c` for `host-exec` | `bml/form-token-verbs.bml` (the `host-exec` verb) | by design: the verb exists to run a command; its guard (`spawn-guard.bml`) reads the argv or the script before a child begins |
| `git` | `native-session-sources.bml`, `host-child.bml` (`hch-git-argv`) | none wanted: the repository's own tool |

**Shell strings** (a command line given to `host-exec` or `host_capture`, which a scan of `host_spawn` argv does not see):
37 product files run one. Re-scan with `git grep -n -E 'host-exec\(|host_capture\(' -- 'form/form-stdlib/*.bml' 'form/form-stdlib/*.fk' 'form/form-stdlib/bml/*.bml' 'observe/*' 'gate/*' 'learn/*' ':!form/form-stdlib/tests'`.
Many already have a Form door and would be a one-line change: `sleep N` -> `host_sleep_ms` (`hearth.bml`), `vm_stat` -> `host_vm_stat`
(`dsv4-validate.bml`), `ps -o pid= -p` -> `host_alive` and `ps -axo ...` -> `host_processes` (`hearth.bml`,
`form-cli-response-session.bml`, `dsv4-oracle-mapped.bml`), `date` -> `now_unix_ms` plus a Form formatter. The rest want a
door that does not exist: `printenv` (an environment read: `local-run.bml`, `model-admission.bml`, `hearth.bml`), `lsof -t`
(`model-admission.bml`), `mktemp` (`metal-ask.bml`). Healed here because it failed a band on a clean origin and was one of
these: `symbol-voice.bml` found a corpus word's sentence with `host-exec("grep -B2 | head | grep -o | tr")` against the
corpus's old multi-line layout; it reads the corpus as text now (`symbol-voice-band` 63; it read 31 on a clean checkout).

Not listed because they are fixtures that name a tool on purpose: `form-cli-landing-band`, `host-doors-band`,
`host-walk-band.bml` (it uses `/usr/bin/true`, `/bin/sh -c "trap '' TERM ..."`, `/bin/cat`, `/bin/cp`, `/bin/date`
and its deaf child is the only one: on Darwin a SIGSTOPped child still dies to SIGTERM, so `host_signal` cannot
build one in pure Form).

What this session closed: `host_signal pid sig` (leaf-door mode 34) and its callers — `voice-track.bml` (`vtr-kill9`),
`host-child.bml` (the deadline's survivors), `form-token-workspace.bml` (`ftw-stop-child`) — and the two bands'
`/bin/sleep` and `/bin/sh` fixtures (`voice-track-reap-band`, `gpu-lease-band` now spawn a Form sleeper).
