# The walk's launchd jobs

Four jobs begin the walk from the walk checkout, `/Users/ursmuff/source/coherence-kernel-walk`: `earth.hati.rent-walk`
(03:30, `observe/scheduled-walk.bml`), `earth.hati.day-turn` (12:30 and 19:00, `observe/day-turn.bml`), and their retry
slots ten minutes later, `earth.hati.rent-walk-retry` (03:40) and `earth.hati.day-turn-retry` (12:40, 19:10), which run
`observe/walk-retry.bml`: it reads whether the first slot began and, when the OS killed it at launch, begins the walk as
its own child (`form/form-stdlib/bml/host-walk-retry.bml`).

The plists beside this page are the **stable launch path**. launchd spawns a program whose content the walk never
changes, on a door the walk never moves, and that door begins the job's door as its child:

```
/Users/ursmuff/source/coherence-kernel-walk/launch/fkwu                     the program launchd names: a pinned copy of the kernel
/Users/ursmuff/source/coherence-kernel-walk/launch/walk-launch.fk           the launcher door: a pinned copy of observe/walk-launch.fk (direct Form, read by launch/fkwu's own reader)
observe/day-turn.bml                                                        the job's door, begun as `./fkwu observe/day-turn.bml`
```

The launcher door gives its child the job's own stdin, stdout and stderr and directory, waits, and ends as the child
ended: 0 as 0, 1 as 1, a signal that ends a process without a core (SIGTERM, SIGKILL, ...) by raising the same signal on
itself, so launchd reads the child's own death; any other ending (a crash, exit 2) ends it 1 with the reason on stderr.

The two stand still together. The walk's build writes `./fkwu` and its sync moves `observe/walk-launch.fk` with main, and
neither reaches `launch/` (`form/form-stdlib/bml/host-walk-seal.bml`). Only `observe/walk-launch-pin.bml`, run by a person,
writes the pair: it copies `./fkwu` to `launch/fkwu.next` and `observe/walk-launch.fk` to `launch/walk-launch.next.fk` (a
stage name fkwu can read: it refuses `walk-launch.fk.next` with "unsupported file extension", exit 2), witnesses the staged
pair begin a probe door as its child, and renames both into place only then, door first and kernel last. `launch/fkwu` is
the one launchd holds the jobs to, so its rename is the pin's commit: a refused pin reads the same cdhash before and after,
the door it had placed goes back as it was, and no job pays a spawn for a pin that did not land. A door main moved that
needs a primitive the pinned kernel lacks therefore fails no job: the jobs go on reading the copy they have, and if the
kernel standing in the checkout cannot carry the moved door either, the pin of it is refused by name with the pair as it
stood.

The pinned copy keeps the kernel's own name in a directory of its own: the body reads a process named `fkwu` as a kernel a
launch passes through (a turn's coordinator climb, `crd-climb-in` in `form/form-stdlib/bml/coordination.bml`, passes it and still
reaches launchd: host-schedule, no rented mind), its diagnostic reports are named `fkwu-<time>.ips` as the retry reads them, and
`launch/` stands outside `.hearth/`, which the disk steward composts. `launch/` also comes to hold the door's image
(`walk-launch.fkb`, `.sym`), which the pinned kernel writes at its first launch and a pin removes. `launch/` is not tracked;
nothing in the walk stages it (every landing names its paths), and `.gitignore` does not yet carry `/launch/`.

Until a person installs these plists, `~/Library/LaunchAgents` holds the earlier ones, which exec
`/Users/ursmuff/source/coherence-kernel-walk/fkwu` itself.

## Why the jobs name a pinned launcher

**Observed** (`/usr/bin/log show`, `launchctl print`, the diagnostic reports and `codesign -dvv`, read 2026-10-29; the
readings are kept under `.hearth/walk-launch-stable/` of the checkout that wrote this page):

- The walk rebuilt its kernel at 2026-10-28 19:00:23 (the day-turn log: `[day-turn] 2026-10-28 ... sync=0 build=0
  carrier=fresh`; slice `a9c4ab0c-8dd0-3277-a046-b8ef646186be`). The four diagnostic reports, 2026-10-28 19:10:02 and
  2026-10-29 03:30:04, 03:40:03 and 12:30:00, each name that slice, `"parentProc" : "launchd"`, `"codeSigningID" :
  "fkwu.next"`, `"codeSigningTeamID" : ""`, `"codeSigningValidationCategory" : 10`, termination `CODESIGNING` "Launch
  Constraint Violation", `SIGKILL (Code Signature Invalid)`. Each was the first spawn of its job after the rebuild.
- The 12:30 kill, as the log has it:

  ```
  2026-10-29 12:30:00.505 xpcproxy[44038] Launch constraint set on 00000000-0000-0000-0000-000000000000 /Users/ursmuff/Library/LaunchAgents/earth.hati.day-turn.plist
  2026-10-29 12:30:00.509 kernel (AppleMobileFileIntegrity) AMFI: '/Users/ursmuff/source/coherence-kernel-walk/fkwu' has no CMS blob?
  2026-10-29 12:30:00.510 kernel (AppleMobileFileIntegrity) AMFI: '/Users/ursmuff/source/coherence-kernel-walk/fkwu': Unrecoverable CT signature issue, bailing out.
  2026-10-29 12:30:00.511 kernel (AppleMobileFileIntegrity) AMFI: Launch Constraint Violation (enforcing), error info: c[5]p[1]m[1]e[0], (Constraint not matched) launching proc[vc: 10 pid: 44038]: /Users/ursmuff/source/coherence-kernel-walk/fkwu, launch type 0, ...
  2026-10-29 12:30:00.512 launchd [gui/501/earth.hati.day-turn [44038]:] xpcproxy exited due to OS_REASON_CODESIGNING | Launch Constraint Violation, error info: c[5]p[1]m[1]e[0], (Constraint not matched) ...
  2026-10-29 12:30:00.512 launchd [gui/501/earth.hati.day-turn [44038]:] Requesting LWCR update on next spawn
  ```

- The 12:40 retry spawned the same binary and it ran:

  ```
  2026-10-29 12:40:03.329 launchd [gui/501/earth.hati.day-turn-retry [44227]:] Requesting repair LWCR update: runs=3
  2026-10-29 12:40:03.340 backgroundtaskmanagementd invalidateLaunchItem: uid=501, type=legacy agent, parentURL=(null), url=/Users/ursmuff/Library/LaunchAgents/earth.hati.day-turn-retry.plist
  2026-10-29 12:40:03.543 smd LWCR Update for (null)/<private> [501] -> <private> [<private>]
  2026-10-29 12:40:03.544 launchd [gui/501/earth.hati.day-turn-retry [44227]:] Updated BTM identifiers: 5A5D532F-DCC6-4BB4-9083-A55BC99BCEAF New LWCR needed: 1
  2026-10-29 12:40:03.547 launchd [gui/501/earth.hati.day-turn-retry [44227]:] Successfully spawned fkwu[44227] because xpc event
  2026-10-29 12:40:03.547 BackgroundTaskManagementAgent Posting advisory notification request: identifier=5A5D532F-DCC6-4BB4-9083-A55BC99BCEAF, body='“fkwu” can run in the background. You can manage background …'
  ```

  (that retry's own spawn at 19:10 the evening before had been the first after the rebuild, and was killed). The last line
  is the repair reaching the person, and every repair since did the same within a quarter second: day-turn 2026-10-29
  19:00:01 (`runs=4`), rent-walk 10-30 03:30:00 (`runs=2`), rent-walk-retry 10-30 03:40:01 (`runs=2`), day-turn-retry 10-30
  12:40:01 (`runs=5`). Five repairs, five notifications (`/usr/bin/log show`, read 2026-10-30, kept in
  `.hearth/launcher-pinned/log-repairs.txt`).
- The binary before the rebuild bore the same seal (tccd names it `identifier=fkwu.next`) and ran under launchd on
  2026-10-28 at 12:30, 12:40 and 19:00 with no violation (at 12:30 and 19:00 with the same "has no CMS blob?" and
  "Unrecoverable CT signature issue" lines). At 03:30 the night job's first run asked "Requesting first run LWCR update",
  smd answered `have LWCR=true`, and the spawn was still killed.
- After the 19:00:23 rebuild the rebuilt binary was exec'd at 2026-10-28 19:00:42 ("has no CMS blob?", "Unrecoverable CT
  signature issue") with no violation, while the 19:00 day turn was running (its log goes on past `build=0` to its turn,
  its drift gates and its landing) and no earth.hati job was spawned in that minute.
- `launchctl print gui/501/earth.hati.<job>` on 2026-10-29: rent-walk, rent-walk-retry and day-turn read `last exit reason =
  OS_REASON_CODESIGNING` and `properties = inferred program | needs LWCR update | managed LWCR | has LWCR`;
  day-turn-retry reads `last exit code = 0`, `properties = inferred program | managed LWCR | has LWCR` and `BTM uuid =
  5A5D532F-DCC6-4BB4-9083-A55BC99BCEAF`. `codesign -dvv` of the walk's binary: `Identifier=fkwu.next`,
  `flags=0x20002(adhoc,linker-signed)`, `TeamIdentifier=not set`, `Internal requirements=none`. That is the binary as it
  stood that afternoon: the walk's drift gates rebuilt it at 2026-10-29 15:39:12 (`.hearth/walk-io/gates.out`
  "binary-freshness ... pass after rebuild-fkwu", the `rebuild-fkwu` repair of `gate/drift-gates.bml`, a second door that
  compiles `./fkwu`); the reports read the same seal on the binary that was killed.
- The walk rebuilt its kernel before, at 2026-10-24 19:00:23 (`build=0` in the day-turn log, `.hearth/walk-io/build.prev.exit`
  0), after main changed `runtime/fkwu-uni.c` at 18:51, and the first spawn of every job after it ran: the 19:10 retry, the
  night walk and its retry of 10-25 03:30 and 03:40, the day turn of 10-25 12:30 (the retries' logs: "begun: the first slot
  is running"). Between that rebuild and the next, backgroundtaskmanagementd re-registered all four items at 2026-10-28
  11:34 ("registerLaunchItem: plist or executable is modified", then "updated item") and the Mac booted at 11:43.

**Inferred** from those lines: launchd sets each job a lightweight code requirement (LWCR, managed through background
task management) from the program it last spawned for that job; an ad-hoc signature has no CMS blob, so the requirement
can only name the program's content; since that requirement has held (the 10-24 rebuild cost no slot, the 10-28 one cost
every job one), any rebuild of the program breaks the match for every job once, and launchd repairs the requirement on
the spawn after the kill. The linker's seal and its identifier were never the cause (the same
seal ran before and after), so re-signing the kernel (`codesign --force --sign -`) changes its content again and costs
every job one more spawn: before this change the walk's build re-signed a linker-signed kernel even when it was newer than
its source (a probe copy: cdhash `733aacdd61f2085e6e544ef8206258ad11f595d8` became
`97cdbafcb5939332b620d818c6491c1b7208011f` with no source change). The 19:00:42 exec was a child of the running walk,
and a child that a live process spawns is not held to the job's requirement (the 12:40 retry's own child day turn ran
too); an exec in place (`/usr/bin/env`, `/bin/sh -c "exec ..."`) keeps the very process launchd
constrained, and whether the new image is checked against the job's requirement at that second exec was not observed, so
the launcher begins a child and never execs.

**Unknown**: what the requirement holds (the log keeps it `<private>`); what `c[5]p[1]m[1]e[0]` encodes; why the
walk's rebuilds of 2026-09-29, 09-30, 10-02 and 10-24 cost no slot (the log keeps no launchd, xpcproxy or background-task
line for these jobs before 2026-10-28 11:34, when backgroundtaskmanagementd re-registered all four items; the
requirement may hold on these jobs only since that re-registration or the 11:43 boot); and whether bootstrapping a plist
that names a new program records that program's requirement at once, or the first spawn after the install is killed and
repaired like any other.

## Install the stable path (a person runs this; the body never runs launchctl but to read it)

The walk checkout must hold this change first (it follows main: once the change has landed, the walk's own sync brings
it, or a person updates that checkout). Then, in a terminal:

```sh
cd /Users/ursmuff/source/coherence-kernel-walk

# 1. Pin the pair: copies ./fkwu and observe/walk-launch.fk to launch/ once the copies have carried a probe door together.
#    It prints "[walk-launch-pin] pin=0 cdhash absent -> <cdhash>" and keeps the row in .hearth/walk-launch-pin.jsonl.
./fkwu observe/walk-launch-pin.bml

# 2. Prove the pinned pair carries a door (the band prints "the launcher: checkout ...", verdict 511, exit 0), and try
#    it by hand on a door that begins no walk (outside a retry window it says "nothing to retry"; dry begins no walk).
./fkwu form/form-stdlib/tests/walk-launch-band.bml
printf '{"dry":1,"record":"/tmp/walk-retry-dry.jsonl"}\n' | ./launch/fkwu launch/walk-launch.fk observe/walk-retry.bml; echo "exit=$?"

# 3. Outside a slot (a bootout stops a job that is running, and its walk with it): take the four jobs away, put the new
#    plists in place, and load them.
for j in rent-walk rent-walk-retry day-turn day-turn-retry; do launchctl bootout gui/$(id -u)/earth.hati.$j; done
for j in rent-walk rent-walk-retry day-turn day-turn-retry; do cp docs/launchd/earth.hati.$j.plist ~/Library/LaunchAgents/; done
for j in rent-walk rent-walk-retry day-turn day-turn-retry; do launchctl bootstrap gui/$(id -u) ~/Library/LaunchAgents/earth.hati.$j.plist; done

# 4. Read what launchd holds now.
for j in rent-walk rent-walk-retry day-turn day-turn-retry; do launchctl print gui/$(id -u)/earth.hati.$j | grep -E 'program =|fkwu|state =|properties ='; done
```

After the install, read each job after its first slot: `launchctl print gui/$(id -u)/earth.hati.<job> | grep -E
'runs|last exit|properties'`, and `/usr/bin/log show --start '<the slot>' --end '<a minute later>' --style compact
--predicate 'eventMessage CONTAINS "earth.hati"'`.

**What stays unknown until it happens**: whether the first spawn of a job after `launchctl bootstrap` is killed once. No
bootstrap of a plist that names a new program has been observed here. If a job's first spawn reads `last exit reason =
OS_REASON_CODESIGNING` and `needs LWCR update`, that is the requirement being recorded for the new program; the spawn after
it runs (for a first slot, its retry ten minutes later), and from then on neither a rebuilt `fkwu` nor a moved door reaches
launchd.

**A caution: a repair reaches the person.** Each requirement repair posts a notification, "“fkwu” can run in the
background. You can manage background …" (five of five in the log above, the night's 03:30 and 03:40 among them). So expect
the install, and any later pin that changes the kernel's bytes, to post one for each job at the hour of its repair. The log
calls it advisory and the walk waits on no answer to it; whether a posted notification shows depends on the person's own
settings, and how many a bootstrap itself posts is as unknown as its first spawn.

## Pin again only by choice

`launch/` changes only when a person runs `./fkwu observe/walk-launch-pin.bml` again, and a pin takes the kernel and the
door that stand in the checkout together, since that pair is what it witnessed. A pin that changes the kernel's bytes
changes the program the jobs name, so each job's next spawn is killed once while launchd records the new requirement, and
each repair posts its notification. Equal cdhashes in the pin's line mean the kernel's bytes did not change; that launchd
then costs nothing is inferred from its matching by content, never observed.

Pin again when the launcher must change. After main moved `observe/walk-launch.fk`,
`form/form-stdlib/tests/walk-launch-band.bml`, run in the walk checkout, says "observe/walk-launch.fk moved since the pin"
and still runs the pair the jobs name, which go on untouched. If the kernel standing in the checkout cannot carry the moved
door, the pin says `pin=refused: the copy ended ...` and the pair stands as it stood. A band red on `pinned`, or a
`launch/fkwu.next` or `launch/walk-launch.next.fk` that stands, says a pin was cut short, perhaps between its two renames,
with the new door by the old kernel: pin again.

## Back out

The plists of revision `863ef4119` (on main) exec `<WorkingDirectory>/fkwu` directly, with no launcher:

```sh
cd /Users/ursmuff/source/coherence-kernel-walk
for j in rent-walk rent-walk-retry day-turn day-turn-retry; do launchctl bootout gui/$(id -u)/earth.hati.$j; done
for j in rent-walk rent-walk-retry day-turn day-turn-retry; do git show 863ef4119:docs/launchd/earth.hati.$j.plist > ~/Library/LaunchAgents/earth.hati.$j.plist; done
for j in rent-walk rent-walk-retry day-turn day-turn-retry; do launchctl bootstrap gui/$(id -u) ~/Library/LaunchAgents/earth.hati.$j.plist; done
```

`launch/` may stay: nothing reads it then. The jobs name a changed program again, so what the first spawn after that
bootstrap does is as unknown as above, and a rebuilt `fkwu` can cost each of them a spawn again.

## The services

`earth.hati.form-find.plist` and `earth.hati.voice-track-worker.plist` are services an operator loads once (their own
comments give the commands); neither is loaded on this Mac today. Each names `./fkwu`, so a rebuild of the kernel would
cost each of them one spawn too; KeepAlive begins the next spawn after the throttle (15 s), and that spawn is the repair.
