# Reference reuse is not a decoding loop

The native voice now checks consecutive repetition at its active tail. A second
mention of a filename, shared sentence opening, or ordinary reference no longer
rejects the next predicted token. Earlier repetition no longer poisons every
later alternative.

The witness is bounded to 512 bytes, with periods of 1–256 bytes. It requires
at least 24 repeated bytes, at least three copies for short periods and two for
periods of 24 bytes or more. ASCII case is folded; other bytes stay intact.
Whitespace-only formatting is not a loop. This is a decoding heuristic, not
semantic understanding; longer or nonconsecutive loops remain outside it, and
intentional consecutive repetition can still trigger it.

The same 24 public cases run against the exact voice source from `73b6f3b9`
failed 13 rows and exited 1. The changed source passes all 24, exit 0:

```
./fkwu form/form-stdlib/tests/native-voice-repeat-band.fk   # 16777215
./fkwu form/form-stdlib/tests/native-llama-voice-adapt-band.fk  # 65535
./fkwu form/form-stdlib/tests/native-tokenizer-choice-band.fk  # 127
./fkwu form/form-stdlib/tests/native-session-code-band.fk      # 511
```

The new cases include every emitted prefix of the configuration-file example,
Portuguese and Han references, actual native tokenizer decoding and candidate
selection, short/long/UTF-8 loops, formatting, alternatives and EOS. Preflight
is clean. An initial fixture put raw spaces into the tokenizer's byte alphabet;
the fixture was repaired through `nlt-symbol`, without changing the tokenizer.

The older adaptation band's sentence-frame and arbitrary echo assertions were
testing the false-positive policy itself. They now require references to pass
and consecutive loops to adapt; context growth, retention and EOS checks stay.
The live observation door also stops treating the words “children”, “employees”
or “friends” as evidence of poor output. It reports actual stopping reason and
explicitly leaves semantic quality unmeasured. No new full-model quality claim
or 95% session-equivalence claim follows from these mechanism tests.

Glass read 163 rows, 583 unread, 61 asks and 46 recipes in 123 ms; all 24 held
findings fit its 100×30 view. That is a bounded observation, not all organs healthy.

The surprising lesson was that the attempted loop escape was also trapped by
old text. The discomfort became a concrete repair when a green suppression
test met ordinary useful language. The exchange stays alive in the native
selector and its regression cases, not in a claim of better fluency.

Signed, Codex.
