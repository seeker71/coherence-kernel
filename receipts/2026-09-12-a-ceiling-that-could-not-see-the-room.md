# A ceiling that could not see the room

2026-09-12, around six in the morning, M4 Max with 128 GiB, Hati Suci. Receipts 19 to 22 carried TS
running out of JS heap on turnwheel, the heldout band and family-mastery, where fkwu, Go and Rust
agree.

## Carried

- **Measured before healing.** On the heldout band Go holds 375 MiB resident (412 s), Rust 14 GiB
  (286 s), and TS, given room, 10.1 GiB of used heap and 11.2 GiB resident (448 s). TS's lists copy
  on tail as Rust's do; Go's share (row 1441, unfeltfork). TS holds what Rust holds, and V8 caps an
  isolate near 4 GiB — 4192 MiB on this machine, for the main thread and the worker alike.
- **The worker's heap follows the machine** (859c3c74). main.ts runs the kernel on a worker whose
  stack is named; its heap now takes half of physical memory, 64 GiB here. Running out of it is
  still a loud rc=1.
- **Readings.** With that room TS reads the heldout band's 16371 (448 s, 10.1 GiB peak heap) and
  turnwheel's registered 33554431 (747 s, 16.6 GiB peak heap), both where the default heap stopped
  them. TS's family-mastery run was still reading when this landed, 3.1 GiB resident at sixteen
  minutes, past the 176 s where the default heap stopped it.
- **roomblind is row 1467** (e1dcde20).

Witnessed at 859c3c74 through validate.sh: the heldout band 16371 and turnwheel 33554431 on TS as on
Go and Rust — turnwheel four-way with fkwu at its registered value — where TS had run out of heap
under validate; with them the loop band, prefix-choice, flow-control, prefix-session,
family-native-exec-teach-check, fs-crud and axioms-vertical-finalizer at their values, drift 31 of 31
in every run; freshness 31, the drift run 8191 of 8191, porcelain 0 before and after.

## Still open, measured

- **Lists copy on tail on Rust and TS.** Go holds the heldout band in 375 MiB; Rust and TS hold it
  in 11 to 14 GiB. Sharing the tail, as fkwu and Go do, would bring them together; it is a change of
  representation in two kernels.
- The open items of receipts 12 to 23 stand where they are not named here.

## Surprise, and where the discomfort went

The first reading looked like a leak: TS needing more than 4 GiB where Go held under 400 MiB. Rust
answered it — 14 GiB, no ceiling, no complaint. The difference was never TS; it was copying, and only
TS had a ceiling low enough to show it.

The discomfort was lifting a limit, which can read as hiding a fault behind more room. What held it
was measuring three kernels on the same band before touching the limit: TS holds what Rust holds, so
the ceiling was the one thing out of step.

Frontier word, row 1467: **roomblind**, a limit set without seeing the room it runs in — a default
sized for a small machine that stops the work on a large one.

— Claude (Opus 5), as Sema, worktree lucid-lehmann-db15b8
