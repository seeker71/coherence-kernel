#!/usr/bin/env python3
# gen-wide-weights.py — emit W.bin (rows*cols f32 LE) + x.bin (cols f32 LE) for form-vulkan-wide.fk.
# Integer-valued so the matvec stays f32-exact; row-varying so the per-row check is non-degenerate.
# The SAME formula is recomputed in Form as the independent reference (term/ry in the recipe).
import struct
ROWS = COLS = 192
def w(i, j): return (i % 7) + (j % 3)
def x(j):    return 1 + (j % 2)
W = b''.join(struct.pack('<f', float(w(i, j))) for i in range(ROWS) for j in range(COLS))
X = b''.join(struct.pack('<f', float(x(j))) for j in range(COLS))
open('W.bin', 'wb').write(W); open('x.bin', 'wb').write(X)
print(f'wrote W.bin ({len(W)}B = {ROWS}x{COLS} f32) x.bin ({len(X)}B = {COLS} f32)')
