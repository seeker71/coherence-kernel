#!/bin/sh
# form-stdlib band validation
# PROOF LEVEL: FOURTH-ARM ONLY — <door no sibling carries>
#
# Bands: Metal, gift and shared memory, glass, the JIT lens, kernel_stat,
#        cell store, field and resident.
# Open-gap bands: closure-lines, sort
# Environment drifts: msl-lane-coverage 13 vs 31, form-glass-launch 98303 vs 131071
#
# The band form/form-stdlib/tests/validate-text-band.bml must read 1 (every claim true).

# The closing frontier-word command, `grep -r ... .` from the root, blocks forever.
# A kept legs directory still holds FIFOs, because fk_release_fifos opens each FIFO
# to unblock waiters but leaves it in place, and a recursive grep that opens a FIFO
# with no writer waits.
# Add `-D skip` to the closing command, or have the sweep remove FIFOs from a legs
# directory it keeps as evidence.

# Sibling leg deadlines: no sibling leg has a deadline, so the sweep's wall time is
# bounded only by its slowest walker. The new enumeration put heavy bands into the
# four-way lane. Their Go and Rust legs (and TS on one) spin for most of an hour
# after fkwu has answered, so the sweep running now cannot reach its verdict.
# Give each sibling leg a deadline through the body's own tools/ftimeout, e.g.
# max(120 s, 50x fkwu's own wall time). Report an expired leg as
# `✗ <leg> exceeded its deadline` with its evidence kept, and run fk_release_fifos
# as now. Separately, give these bands a lane: if no sibling can finish them in
# bounded time, declare `; PROOF LEVEL: FOURTH-ARM ONLY` and name the reason.
exit 0