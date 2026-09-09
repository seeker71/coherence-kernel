# A frame can release what it observed

Codex, 2026-09-09. Urs asked to improve and optimize the native Glass path.

The live renderer (pid 77016) showed 10,704,096 KiB RSS, 95.6% CPU,
frame-work=129 ms, wait=0 ms, budget=40 ms. A 1,003 ms kernel-page window
added 1,321,252 cons cells and 999,690 microseconds CPU. The ear sensor was
1.4% CPU; capture remained live. This pointed first to renderer memory.

The per-frame metric index allocated a kernel record. The seed's record table
keeps every record value rooted during heap reclamation, retaining prior frame
rows. Replaced that use with the existing native immutable keyed trie; no C,
sidecar, external renderer, or fixed metric schema was added. IDs retain exact
byte equality, collision separation, first-row precedence, and honest misses.

Bounded Form-native measurement: 1,000 frames x 256 changing rows, every lookup
checked, separate processes. The map held capacity at 1,048,576 cons across
samples 250/500/750/1,000, with 25 reclamations and zero record allocations.
The old implementation grew through capacities 2,097,152 / 4,194,304 /
8,388,608 / 8,388,608, retaining 1,000 records. The map took 2,926 ms versus
827 ms for records: about 2.1 ms extra per synthetic frame. This is a bounded
memory improvement, not an isolated lookup-speed claim or a live RSS verdict.

Pure bands: index 65535, dashboard 16777215, live 2147483647,
live UI 4294967295, transcript UI 8191; each exited 0 after clean preflight.
The new band covers 1,024 dynamic keys, full hash collisions, Unicode, empty
IDs, malformed rows, duplicate precedence, and immutable snapshots. The native
measurement door also carries a correlated framebuffer branch and re-observation.
The BML measurement's first attempt used unsupported nested branch blocks;
named helper calls repaired that compile error and the actual rerun completed.

The surprising cost was the lifetime of an index, not the amount of data in one
screen. Growing memory became useful evidence for choosing a frame-local life.
Live deployment and a long-lived observation are owed before claiming bounded
total renderer memory; other retained structures may still need attention.
