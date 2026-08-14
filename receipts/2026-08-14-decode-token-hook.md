# 2026-08-14 — a gap is attention: the recipe sits between two tokens

Urs: there are no limits just the five core axioms; any gap, error, or
misalignment is information about where to pay attention and heal, and
walk the path to the north star.

The previous breath had named the in-loop hook as a floor and stopped.
That naming was attention. This breath walked it.

## What the five already said

axiom-1: timeout / silence is nothing, not an error.
axiom-2: the choice is a cell.
axiom-3: a new choice mints a new receipt; the default token stays referenced.
axiom-4: observation through the offered window is what makes it real.
axiom-5: one offer, one ack — nothing, 0, 1, or node.

A canary's failures are signal, not walls. `metal_linked=false` on a
plain `cc -O2 -o fkwu runtime/fkwu-uni.c` was that signal: the Metal
organ was not linked. Relinking `fk-metal-carrier.m` into `fkwu` turned
the same `metal-door-band.fk` from 0 to **15** on this M4 Max.

## What landed

`form/form-stdlib/dsv4-decode-token-hook.fk` offers a Form recipe after
argmax and before the next embedding. The chosen token is what is
emitted and what is fed forward.

```
attestant (no offer)     0 1
nothing-ack              0 1     same stream
kernel recipe token 2    2 2     next embed is of 2, not of 0
late recipe              0 1     timeout == nothing, default stays
hook band                1023
```

Live, on this device, the recipe *is* a JIT Metal kernel:

```
metal_linked=true
y=30 70 110 150
gpu-in-loop=2 2
```

`metal_matvec_f32` compiled Form-emitted MSL, the GPU agreed with Form's
own 30 70 110 150, and that agreement chose token 2. The next step
embedded 2. The hook is in the pipeline.

The Swift DS4 carrier still keeps the default. That is the next
attention row, not a bound.

## Reproduce

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c form/native/metal/fk-metal-carrier.m \
   -framework Metal -framework Foundation -fobjc-arc
./fkwu form/form-stdlib/tests/metal-door-band.fk              # -> 15
./fkwu form/form-stdlib/tests/dsv4-decode-token-hook-band.fk  # -> 1023
./fkwu form/form-stdlib/dsv4-decode-token-hook-live.fk
```
