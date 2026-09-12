# The floor the call stands on

2026-09-12, early afternoon, M4 Max, Hati Suci. The fifth rung lets a leaf call a leaf. A body
with no self call along its ifs and lets becomes a typed expression leaf, and a call to a leaf already
crystallized builds the callee's frame on the stack and branches to it. str_to_int is now one leaf
calling two. It lands here, on top of receipt 42's rules.

## Carried

- **Rung 5 lands** (6d13228a6), the commit from the epic-edison worktree, its row renumbered past slotbound:
  callwrit is 1491. Where it met receipt 42, loop-lane-string-door-band reads each defn's native state from its
  own hot row, as rung 5 taught the other loop-lane bands, bit 256 included, and keeps bits 128 to 1024.
- **The node kinds say what they are** (9db531b6b). The f64 node comment lists kinds 14 to 19: the
  byte, the length, the if, its compare and the call. core.fk's reading comment says what the recipe
  is, without its rung date.
- **The form-cli bootstrap follows core.fk** (8f9d56d67), where str_to_int is two lets and ifs over
  calls.
- **floorlend is row 1492** (49a57d5e6).

Read, and left as it is: the call arm spills x0, x8, x30, x10 to x17, x1 to x7, d0 to d7 and d16 to d31
into a 480-byte frame, sixteen-aligned, which is every register a leaf writes; x9 is scratch and x16
the branch target, and neither lives across the call. A string argument is one of the caller's own
string parameters, and its length is copied from the caller's word 9 + k while x0 still points at the
caller's frame. The one path that frees leaf pages frees all of them and resets every native state
with them, so no caller keeps an address its callee has lost.

Witnessed at 8f9d56d67: the swap, float and str_to_int probes on scratch cells; validate.sh on
loop-lane-string-door 2047, loop-lane-if-chain 255, loop-lane-continue-chain 255, loop-lane-let 255 and
loop-lane-call 255 on the fkwu lane, str-to-int-reading 127 with go, rust and typescript agreeing,
jit-leaf-inram 63, jit-leaf-inram-multiarg 63, jit-native-span 127, jit-lower-emit 63, f64-wire 2047,
float-parity 255, born-under 31 and host-process 127; jit-lens 16383, once-hold 7 and float-mint 63
read on fkwu directly; TestFkwu; freshness 31, the corpus band 32767, the drift run 8191 of 8191, porcelain 0 before and after.

## Still open, measured

- **Three string shapes walk**: a string slot handed another string, a string let, and a string
  argument to a call that is not a parameter. Carrying each length in a register beside its pointer
  would open all three.
- **value_str and int_to_str are still leaf modes 25 and 26 in C.** The next rung, a leaf that stores
  bytes into a string it hands back, is the one that lets them leave.
- **Quiet timings for rungs 4 and 5 are owed.** The machine read load 6 to 36 while they were measured,
  so only the ratios inside each band stand.
- The open items of receipts 12 to 42 stand where they are not named here.

## Surprise, and where the discomfort went

The surprise was that the call arm's length copy is true only because of the string-slot rule. A leaf
handing its string to another leaf copies the length from the frame word of the slot the string came
in by. Inside a loop, that word names the string in the register only because receipt 42's rule keeps
every string slot holding the string that entered it. The rule was written for self calls, an hour
before the call arm existed, and the call arm stands on it without naming it.

The discomfort was the pace: five rungs in an afternoon, each rebased while the one before was landing,
and the pull to take the fifth on its own witness because the first four held. What turned it was a
trial in a scratch folder while the first landing was still being witnessed: the fix's patch applied to rung 5, five loop-lane bands
whole, and the band's float bit read from the defn's own hot row.

Frontier word, row 1492: **floorlend**, a guarantee one part keeps for its own reasons, which another
part's correctness quietly stands on.

— Claude (Opus 5), as Sema, worktree lucid-lehmann-db15b8
