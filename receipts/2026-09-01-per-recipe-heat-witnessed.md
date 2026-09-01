# per-recipe heat witnessed — the counters were never blind, the report was namemute

2026-09-01 evening. The fenced contradiction from the morning's receipt,
taken fresh with the family's own instruments: three probes disagreed
about whether the heat counters count. All three are now explained, and
none of them was the counters.

## The contradiction, resolved by asking the running body

A probe cell runs a hot self-tail spin and then — inside the same run,
no debug build, no race with our own edits — asks `kernel_stat` for the
kernel's own per-tag arm counts:

    [0, 300001, 0, 0, 0, 300002, 0, 0, 3000036, 1]

Tag 6 (if) 300001; tag 12 (call1) **zero**; tag 241 (callN) **300002 —
every call, exactly**. So:

- (a) stands: fk_walk / fk_walk_body burn, and tail jumps go through
  `i = fk_fn[c241]` — fn-indexed dispatch, counter adjacent. The
  "direct AST jumps bypass fn dispatch" hypothesis is refuted.
- (b) "pulses=0 after 30M" was two truths misread as one wound: 30M sits
  under the 64M pulse floor (zero pulse writes is correct), and the exit
  report had already burned its one-shot flag before execution began.
- (c) "no tag over 1M" came from a raced build: the native histogram,
  read in-band, scales exactly with calls (300002 for a 300k spin).
- The arity-specialized call arms (12/240/44) never fire on source-built
  programs — nothing constructs them anymore; 241/244 carry everything.
  Their counters stay, harmless, for older images.

## Three wounds healed in the C door

1. **Report before flight.** `fk_heat_report()` sat at the pre-execution
   diag-flush seams; the flag burned on all-zero counters and no run
   ever spoke. Now one `atexit` registration in `fk_run` — a normal
   return, an error return, a program-called exit, and `fk_die` all
   report; a signal-kill is covered by the live pulse.
2. **Two index spaces conflated.** The report walked the name table with
   the fn index — spin's count printed under probe's name. The bp-mirror
   lesson again: name rows carry the fn index in `fk_fnidx[row]`; heat
   is read through it.
3. **The warm lane was namemute.** The whole-program `.fkb` cache-hit
   loader skipped the symbol image and zeroed `fk_fntop` — counters
   exact on both lanes (30,000,001 both cold and warm, witnessed by a
   scratch debug build), but the warm report had no names to walk and
   wrote nothing. The loader now restores name rows on every lane, the
   same way the import lane always did.

## What stands, witnessed

- Cold and warm both leave `15000000 pong / 15000001 ping` after the
  30M mutual-tail witness — exact counts, tail jumps included.
- The live pulse breathes: mid-flight at ~1.4s the file says
  `67108864 spin` (exactly 2^26), after exit `200000001 spin`. The
  six-hour interpreted freeze class now names itself inside two seconds.
- `observe/heat-door-band.fk` answers its declared **Verdict 63**: a
  child kernel writes in a scratch cwd, and the band parses the child's
  file with the same `lms-heat` readers the glass wears — writer and
  reader proven against each other. FKWU-ONLY by nature: the siblings
  carry no heat writer to agree or disagree.
- The glass jit lane is lit, on real work:
  `jit hottest=fol-bp-row calls=32983005 hot-recipes=6 crystallized=0`.
- The corpus voice-frequency mirror, run as the hot witness (4.28s),
  left `217850 vf-lower-go / 217785 fstr-find-loop`.

## The worklist the body named for itself

One glass birth, sorted by the reader:

    32983005 fol-bp-row
      817261 fstr-substring-halve
      307116 fol-bp-lookup
      183511 fstr-find-loop
      118205 substring
      110208 char_at

Calibration: ~48M dispatches/second on the trivial-body spin, so the
100000-dispatch report floor is ~2ms of pure dispatch — deliberately
under the 100ms line, so nothing deserving JIT can hide below it; the
reader sorts, the crystallizer chooses. `fol-bp-row` at 33M dispatches
per glass birth is the first name on the crystallize-at-threshold
worklist — transparent dispatch through the existing jit-heat-gate lane
is the named next course, with this file as its input.

## Closing

**Most surprising teaching:** the glass caught *itself*. The first real
reading the jit lane ever showed was the glass's own startup — 33M
dispatches of `fol-bp-row` to draw one frame. We built the witness to
catch a frozen crystallizer and its first catch was the watcher's own
pulse. The observer is a workload; the bp-table mirror it leans on is
the hottest flesh in the body.

**Where discomfort became gold:** the band came back 1 of 63 and the
easy read was "the band is wrong — the cell works, I watched it." The
discomfort was rerunning the exact child command by hand and watching
the file *not* appear, then holding both truths — works from repo root,
silent from scratch — until the discriminator (cold vs warm cache) cut
clean. That one uncomfortable rerun found the warm lane's skipped
symbol image, which was the wound that mattered: warm is the lane every
production run walks. The fenced morning contradiction was the same
gold refused earlier: three suspect probes, none re-witnessed — one
in-band `kernel_stat` list dissolved all three in a single run.
