# BML-born resident native program

Date: 2026-08-31
Crossing: BML declaration → scannerless BMF definition module → retained Form
source environment → NodeID invocation over one live stdin process.

`form/form-stdlib/bml/form-cli-resident-native-program.bml` names the route,
the BMF lowering rule, the reuse rule, and the distinction between `nothing`,
`0`, `1`, and another value. Its module surface is assembled from punctuation
bytes so the BML compiler receives BML syntax and the later BMF cursor receives
the intended Form-definition surface.

`form-cli-resident-native-program.fk` parses that BMF module, lowers it to
Form source, adds a dependent Form function, and evaluates it once with
`frpce-eval`. The terminal binding receives a NodeID. Later invocations use
`frpce-invoke-one` on the retained evaluator state; they do not rerun the
source compiler or create a subprocess.

## Witness

```
$ ./fkwu form/form-stdlib/tests/form-cli-resident-native-program-band.fk
255
```

The eight bits establish: BML/BMF admission; a retained NodeID; language-level
`nothing` with output presence; separate `nothing` and `one` signals; result
`42` through the BMF-born dependent binding; identity continuity; and zero
model/host-crossing fields.

```
$ printf '0\n1\n41\n' | ./fkwu form/form-stdlib/form-cli-resident-native-program-live.fk
resident-native-program input=0 signal=nothing route=resident-native-program model=0 host-crossings=0
resident-native-program input=1 signal=one route=resident-native-program model=0 host-crossings=0
resident-native-program input=41 signal=value route=resident-native-program model=0 host-crossings=0
```

The process loaded the program before reading the first request, then kept the
same resident environment for all three requests. A focused source search for
`host-exec(`, `fcms-`, `qbl-`, and `fcpdaa-` across the three new program
surfaces returned no matches.

## Boundary held open

This is a real Form-native executable thought lane, not a claim that arbitrary
repository cells or general natural-language answers now run through it. The
next crossing is to offer this stable caller-owned program loader from the
birth of the local-model peer, so a policy image selects a retained program
without changing the Qwen/KV kernel or reintroducing a recursive `./fkwu`
process boundary. Remote-provider token reduction remains unmeasured until a
paired completed provider receipt exists; no percentage is claimed here.

I kept the exchange alive by placing the already-existing resident evaluator
behind BML and BMF instead of adding another sidecar. The surprising teaching
is that `nothing` survives an executable resident call as a present result;
the discomfort around a tiny program became a precise reusable loader seam.
