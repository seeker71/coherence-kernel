# The kind the frame already knew

2026-09-13, evening, M4 Max, Hati Suci. int_to_str is a core recipe in BML. It asks `(value_kind n)` and hands
anything that is not an int to value_str. The lane had no arm for value_kind, which answers a string, so int_to_str
walked for every number (-1201). Receipt 59 left this open: a kind test inside a leaf, without a new C native.

## Carried

- **A kind test over a typed operand folds when the leaf compiles.** Inside a leaf an int parameter is an int and a
  float parameter is a float, so the frame already carries the kind. When str_eq compares `(value_kind x)` with a
  string literal and x's type is known, the lane answers 0 or 1 as a constant. It compares the literal's bytes with
  the kind's name at compile time.
  - The operand is admitted once to learn its type. The lane's state is restored before the constant is pushed, so
    that admission leaves no code behind.
  - A parameter passes the fold through to its argument, because the argument stands where the parameter stood.
  - A word-typed operand does not fold. Its kind is known only at run time, and the leaf declines as before.
- **An int's "int" and "record" stay the walker's.** Record r and the integer -r are one word, and value_kind answers
  "record" for that word once r records exist (kindshadow, row 1525, found by a sibling session the same evening).
  For an int operand the lane folds every other name to 0, which holds either way. It leaves "int" and "record" to
  the walker, whose answer depends on how many records the run has minted.
- **A constant condition admits only the arm it takes.** If a condition folds to a constant, either directly or
  through a compare of two int constants, the lane compiles the taken arm alone. int_to_str's value_str arm is dead
  for an int, so it is never admitted.
- **A compare of two int constants is itself a constant**, as a condition and as a value.
- **loop-lane-kind-fold-band, registered at 31:**
  - bit 1: int_to_str, driven hot on ints, reads state 1 and gives its cold answers over edge ints;
  - bit 2: a kind test over a typed parameter reads state 1 and answers right for a float, an int, a string and a
    list;
  - bit 4: a leaf whose dead arm holds value_str compiles under a constant condition;
  - bit 8: int_to_str of a float, a string and a list still answers as value_str writes them;
  - bit 16: once a record is minted, a hot kind test of -1 against "int" and against "record" answers as the walker
    does.
- **knownkind is row 1526.**

Witnessed:
- **The band reads 31 here.**
  - On the runtime of b01bc969d it reads 24: all three leaves read -1201, and the walker answers bit 16.
  - The fold as I first committed it read 15. Its bit-16 sum was 4977 where the walker gives 3000: 1023 cold calls
    answered "record", then every hot call answered "int".
- **int_to_str over three million ints:** 1.02 s on b01bc969d's runtime and 0.04 to 0.05 s here, with the same sum
  (19888896), over two rounds.
- **A probe over both kernels** gives identical answers for 0, -42, 1234567890123, -9, 2.5, "abc" and a list.
- **Bands:**
  - Every lane band reads as registered, and so do the float bands.
  - value-str 1023, str-to-int-reading 127, once-hold 15.
- **Before landing:** TestFkwu; freshness 31, the corpus band 32767, drift 16383 of 16383, porcelain 0.

## Still open, measured

- **kindshadow itself.** value_kind still answers "record" for a small negative int once records exist. The sibling
  receipt names a repair, records in their own parity-distinguished band, and holds it for Urs. The lane agrees with
  the walker either way.
- **A kind test over a word-typed value.** A list element read inside a leaf is a word, and its kind is known only at
  run time, so `(value_kind (head xs))` still declines. A list-tree fold waits on a run-time kind read that is not a
  new C native.
- **The value_str float-arm switch.** It waits on a four-way timing receipt. Doubles outside the word path's window
  (below about 0.03, above about 2^56) still cost 0.2 to 0.9 ms against the native's microseconds.
- The open items of receipts 12 to 59 stand where they are not named here.

## Surprise, and where the discomfort went

The surprise came twice. First, the fold needed no new knowledge. The lane already typed every slot; it had simply
never let a string question see the type. Then, while I was landing it, main moved with a sibling's receipt saying a
fold like this could not simply be shipped. My committed fold was that fold, and for -1 it answered "int" where the
walker answers "record".

The discomfort was that the lane was right and still wrong. "int" is the truer answer, but a lane that disagrees with
the walker is a bug whichever side tells the truth. The gold is a narrower fold: fold only the names whose answer no
run can change. int_to_str lost nothing, since it asks only "string", "list" and "float". And the sibling's row, which
held its teaching without a band, now has a band bit that the first fold fails.

Frontier word, row 1526: **knownkind**, a value's kind the compiled frame already knows, answered when the leaf
compiles instead of asked on every call.

— Claude (Opus 5), as Sema, worktree lucid-lehmann-db15b8
