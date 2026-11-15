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
exit 0