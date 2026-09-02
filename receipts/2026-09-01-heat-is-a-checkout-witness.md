# 2026-09-01 — heat is a checkout witness, while Form owns the meaning

The glass had no grounded answer to the simple question: *which recipe
is actually holding the machine?*  It could show a process or a duration,
but it could not name the work.  This movement makes the temporary checkout
seed expose one compact fact: per-function dispatch counts from the source
walker.

`runtime/fkwu-uni.c` now carries a parallel counter array through the same
allocate-copy-swap growth as the source function image.  The widening is
overflow-checked before byte multiplication; all six new arrays are obtained
before an old one is released; allocation failure frees its temporary rows;
and the heat row is released with the rest.  A hot report is atomically renamed
into `.fkwu-heat.<pid>`.  The BML reader selects the living board PID, so the
glass's own short process cannot erase or impersonate the resident's snapshot.
During an enduring run the counter has a 64M-dispatch pulse; at terminal return
it writes the last hot snapshot.

This is explicitly a **checkout-witness repair**, not new kernel meaning.
The C seed only counts and carries `count symbol` rows.  The authored
interpretation is in `form/form-stdlib/lane-motion.bml`: it reads the current
snapshot, finds the hottest row, and places `hot`, `hotname`, and `hotn` in
the existing lane sample.  `form/form-stdlib/tests/lane-motion-band.fk`
proves the BML reader with a three-row fixture.

## Witness

After rebuilding the one local `fkwu` from this source:

```
bootstrap/ground.fk                                      -> 42
form-stdlib/tests/binary-freshness-band.fk               -> 31
form-stdlib/tests/lane-motion-band.fk                    -> 1023
form-cli-peer-contribution-turnwheel-band.fk             -> 16777215
```

The full four-way turnwheel returned `16777215` and its exact PID row
`.fkwu-heat.85004` contained a source-walker snapshot including:

```
22866588 append-1
11863199 map
11753823 append-list
```

After that turnwheel returned, a separate short `fkwu bootstrap/ground.fk`
returned `42` and a byte comparison proved it had left
`.fkwu-heat.85004` unchanged.  That establishes the per-process noninterference
the live glass needs, as well as the terminal snapshot and BML projection.  It
does **not** yet establish a pulse seen before a terminal return; the next
honest witness is a controlled run that crosses 64M dispatches while the glass
observes a new heat-file stamp.

`runtime/tests/fkwu-source-dependency-growth.sh` is the companion capacity
witness: it exercises 160 source dependencies so the former narrow function
image cannot hide behind the heat work.

## Shrink path

The C row disappears when the native Form walker/JIT emits the same
NodeID/function-identity dispatch observation directly as a typed stage
frame.  `lane-motion.bml` remains its sole reader and glass projection; the
checkout file becomes unnecessary rather than a second runtime interface.
No model, network, HTTP server, or sidecar enters this path.

## What changed in the exchange

I kept the exchange alive by converting a silent high-CPU interval into
observable named work and by refusing to call an old snapshot current.
The surprising teaching is that an empty report is meaningful: it is the
attestation that this run had no hot function, not missing instrumentation.
The discomfort was touching the C seed; it became bounded gold by naming its
removal route and putting all interpretation in BML.

; witnessed: 2026-09-01 -> BML lane heat parser 1023; source turnwheel
; 16777215; exit heat snapshot named append-1/map/append-list; checkout-seed
; shrink route explicit

## Addendum: warm burns burn by name (2026-09-02)

The practice witness found two wounds in one line and both are closed.
The writer indexed the symbol arrays with the fn index — spaces that
coincide on fresh compiles and divorce under image remapping — so cold
runs MISATTRIBUTED heat (an inner fn's 80M dispatches printed under its
caller's name) and warm runs printed nameless counts. The writer now
walks the symbol table and follows fk_fnidx; any hot fn no symbol names
prints fn#N — counted work is never blank. And the whole-image loader
restored what it had always been handed: fk_fkb_restore_symbol_image
reads the name records the artifact already carries instead of
skipping them (pre-v3 artifacts restore nothing and keep the honest
fallback). Witnessed: the same 80,000,400-dispatch burn now reads
"burn" cold AND warm. Bands: lane-motion 1023, glass 16777215,
freshness 31. Corpus row 1217.
