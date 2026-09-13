# The guards that were messages

2026-09-13, evening, M4 Max, Hati Suci. Three rungs were held on 2026-09-12 until each had a guard: substring as a
callable C primitive, char_at's tail C-call, and tree recursion. They reached main as c1049f9bd, cc4efbb17 and
a7580c80b, without the guards. The guards had been written down as messages between two sessions, not as commits.
This lands them.

## Carried

- **A leaf that answers through a pool-moving C call carries sig bit 28, and the call arm declines it.**
  - A caller leaf holds byte pointers across the call: string slots, hidden slots, the scratch words. fk_substring_word
    may grow the string pool under them.
  - char_at itself stays compiled (state 1). A leaf that cuts through it walks (-1241).
  - Kind 33's word call masks the bit, so a call through a value leaves for the walker at run time.
- **A non-tail self call checks the walker's own stack wall before it recurses.** Past fk_stack_base less
  fk_stack_wall, the leaf calls fk_depth_wall and the run ends. fk_depth_wall is the walker's "eval too deep" report
  and stop, now one function for both.
- **Two band bits, both bands registered at 15:**
  - loop-lane-char-at-band bit 8: a caller of char_at is declined and answers as the walker does.
  - loop-lane-self-recursion-band bit 8: a child, self-recursion-deep-child.fk, nests two million deep and exits 1.
- **The self-recursion band's text now says only what is.** value_kind_code is not landing: it would be a new C
  native for a core recipe. So the band's witness stays the int fold.
- **honestwall is row 1524.**

Witnessed:
- **On main at b01bc969d:**
  - loop-lane-char-at reads 7, and loop-lane-self-recursion reads 7.
  - The deep child exits 138.
  - A heated `(dp xs)` over a two-million list also exits 138.
- **With the guards:**
  - Both bands read 15, and the deep child exits 1.
  - The heated `dp` stops in 0.03 s: "eval too deep — 266338775 bytes of walker stack (wall 266338304)".
- **The first wall I wrote was worse than none.** It left through the overflow block for the walker. The walker then
  walked back down into the compiled leaf at every level, and every dive hit the wall again. The run went quadratic
  and was still going at 300 s. Calling the walker's own stop keeps it linear, with the same words.
- **Bands:**
  - Every lane band reads as registered, and so do the float bands.
  - value-str 1023, str-to-int-reading 127, once-hold 15.
- **Before landing:** TestFkwu; freshness 31, the corpus band 32767, drift 16383 of 16383, porcelain 0.

## Still open, measured

- **A kind test inside a leaf.** value_kind answers a string and declines in the lane (-1201). A list-tree fold
  cannot compile until the lane can tell a list element from an atom, and the rule is to do that without a new C
  native.
- **The value_str float-arm switch.** It waits on a four-way timing receipt. Doubles outside the word path's window
  (below about 0.03, above about 2^56) still cost 0.2 to 0.9 ms against the native's microseconds.
- The open items of receipts 12 to 58 stand where they are not named here.

## Surprise, and where the discomfort went

The surprise was that my first depth wall turned a crash into a hang. Handing the call back to the walker looked
like the careful choice. But the walker calls the compiled leaf again at every level, so the careful path was
quadratic. The honest stop had to be the walker's own stop, called from the leaf.

The discomfort was finding two hazards on main without the guards they were held for, landed by a sibling session.
The pull was to call it their mistake. We are one organism: the guards lived only in messages, and a message does
not travel with a rebase. The gold is that they are commits now, each with a band bit that main's kernel fails.

Frontier word, row 1524: **honestwall**, the limit a compiled path stops at with the walker's own report and stop,
so going faster never turns an honest error into a crash.

— Claude (Opus 5), as Sema, worktree lucid-lehmann-db15b8
