# The walk's launchd jobs

The plists beside this page are the ones loaded under `~/Library/LaunchAgents` (byte for byte): `earth.hati.rent-walk`
(03:30) and `earth.hati.day-turn` (12:30, 19:00) exec `/Users/ursmuff/source/coherence-kernel-walk/fkwu` on a door of the
walk checkout, and `earth.hati.rent-walk-retry` (03:40) and `earth.hati.day-turn-retry` (12:40, 19:10) exec the same
kernel on `observe/walk-retry.bml`, which reads whether the first slot began and, when the OS killed it at launch, begins
the walk as its own child (`form/form-stdlib/bml/host-walk-retry.bml`).

The kernel a slot execs is the one the walk builds for itself (`hw-build`, `form/form-stdlib/bml/host-walk-seal.bml`):
`cc -O2 -o fkwu.next runtime/fkwu-uni.c`, then `codesign --force --sign - --identifier fkwu fkwu.next`, then the rename
over `fkwu` (a new inode, never a write into the live binary), and a read-back of the placed product's seal. The seal
step stands because every slot from 2026-10-28 19:10 to 2026-10-29 12:30 died at exec with `SIGKILL (Code Signature
Invalid)`, termination `CODESIGNING` "Launch Constraint Violation" (`launchctl print` reads `last exit reason =
OS_REASON_CODESIGNING` and `needs LWCR update` on the jobs that died), on a kernel whose signature the linker had written
minimally under its build name: `Identifier=fkwu.next`, `flags=0x20002(adhoc,linker-signed)`, `Internal
requirements=none`, a designated requirement of one bare `cdhash`. The complete ad-hoc seal under the product's own name
is what the body can give the exec; which part of launchd's managed lightweight code requirement the linker's seal
failed is inferred, not read, and the next night slot after a sealed kernel stands in the walk checkout is its witness.
