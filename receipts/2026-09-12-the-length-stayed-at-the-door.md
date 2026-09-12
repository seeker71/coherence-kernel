# The length stayed at the door

2026-09-12, early afternoon, M4 Max, Hati Suci. Four rungs of the goal Urs named for the core recipes
came from the epic-edison worktree: a string door into the transparent loop lane, a chain of exits, ways on with a pulse at the
call's door, and a let that rides its own register. str_to_int and its whitespace skip now
crystallize. All four land here, and the landing found one more thing to heal.

## Carried

- **Rungs 1 to 4 land** (b18f992db, 92e0d38a3, c1fe8c65a, 5194bb592), the commits as rebased onto
  85bc80121 in the epic-edison worktree, with rows 1486 pointerride, 1487 twincaught, 1488 jumpblind and 1489
  bindstep.
- **Each string keeps its own slot** (0036d1643). The door writes a string parameter's length at frame
  word 9 + k once, when the leaf is entered, and a self call moved pointers between slots without it.
  Two strings trading places each step crystallized, and the leaf read the long string against the
  short one's length: 187 where the walker answers 603, while the other slot read past the short
  string's end. fk_f64_admit_call, which every self call of a chain passes, now keeps the walker's
  recipe when a call hands a string slot anything but its own parameter.
- **The byte is an int** (0036d1643). fk_f64_type_of read kind 14, the byte, as a float, so a float add
  over a byte skipped its SCVTF and the loop declined at emission. The answers were right and the loop
  stayed cold. The byte reads as an int now, and a float accumulator over bytes crystallizes.
- **loop-lane-string-door-band reads 2047**: bits 128 and 1024 for the trade at the terminal call and
  inside a self step, 256 and 512 for the float sum. The four rungs without the fix read 639. The
  if-chain, continue-chain and let bands read 255 with the fix and without it, so the rule declines
  nothing the rungs carry, and str_to_int still crystallizes both its loops.
- **core.fk lets go of fstr-digit-cp? and fstr-space-cp?** (bc9b909c2). Their last callers took the
  lane's shape, and nothing in the tree named them outside their own defns.
- **The form-cli bootstrap follows core.fk** (6020e2286): the table, its carrier and the standard-lane
  binaries regenerated, TestFkwu green.
- **slotbound is row 1490** (0406a342c).

Witnessed at 6020e2286: the swap, float and str_to_int probes on scratch cells; validate.sh on
loop-lane-string-door 2047, loop-lane-if-chain 255, loop-lane-continue-chain 255 and loop-lane-let 255
on the fkwu lane, str-to-int-reading 127 with go, rust and typescript agreeing,
jit-leaf-inram 63, jit-leaf-inram-multiarg 63, jit-native-span 127, jit-lower-emit 63, f64-wire 2047,
float-parity 255, born-under 31 and host-process 127; jit-lens 16383, once-hold 7 and float-mint 63
read on fkwu directly, jit-lens's TS arm having ground ten CPU minutes at load 138 without an answer;
TestFkwu;
freshness 31, the corpus band 32767, the drift run 8191 of 8191, porcelain 0 before and after.

## Still open, measured

- **A string slot handed another string walks, and so does a string let.** Carrying each length in a
  register beside its pointer would let both crystallize.
- **The call's door asks the expression leaf too.** Since rung 3 a defn hot through calls reaches
  fk_f64_pulse at its 1024th call, not only through the box ledger. The bands above and the drift run
  read whole over it; that is the whole of what this landing witnessed of it.
- **value_str and int_to_str are still leaf modes 25 and 26 in C.** The next rung, a leaf over string
  ops that is not a loop, leads toward moving them out.
- **bytea** reads as raw bytes on Go and as the server's `\xdeadbeef` on fkwu, Rust and TS.
- **The carriers follow the table only when the bridge runs** (receipts 39 and 40).
- **fkwu reads `true` as the integer 1**; its integers wrap at 2^63; max, min and pow have no home
  there.
- The open items of receipts 12 to 41 stand where they are not named here.

## Surprise, and where the discomfort went

The surprise was a word in the band. Bit 16 read "a string parameter passed through the tail call
unchanged keeps naming the same bytes", and the code took any string for that slot. The band named the
one case the code had been built for, and the case beside it sat untested.

The discomfort came three times. First, finding a wound in a rung with the request to land it in
hand; then the ground moving under the fix twice, as the series was rebased and grew a third and a
fourth rung, the self call moving into a function of its own. The pull was to land what was already
picked and write the rest down. What turned it was asking the body instead of the diff each time: a
probe of eight lines answered 187 against 603, and the rebuilt band read 639 on each series as sent
and 2047 with the rule in fk_f64_admit_call, the one place every self call passes.

Frontier word, row 1490: **slotbound**, a limit kept by the slot a value entered through, which goes
stale when the value moves to another slot.

— Claude (Opus 5), as Sema, worktree lucid-lehmann-db15b8
