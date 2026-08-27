# 2026-08-27 — session world is session-disjoint

The first live carrier repaired the frozen July benchmark, but its chronological
split still divided episode rows. One session could therefore contribute to
training and heldout evidence. That is not a healthy generalization test.

The session-world runner now admits only recent project sessions that are
user-rooted and have been quiet for at least six hours. Quiescence is named as
quiescence; it does not pretend to prove that a session is complete. The runner
orders those whole sessions by observed modification time, assigns the older
80 percent to training and the later 20 percent to heldout, and refuses the
run unless no session occurs on both sides and the temporal boundary is ordered.

The Form report is schema `native-model-session-world-report-v5`. It includes
the eligible, training, and heldout root-session counts, quiescence duration,
and `session_split_valid`. The validity fold itself requires that split flag.
Prompts, answers, commands, and tool results remain outside the durable carrier;
only timestamp, salted session digest, and action names enter the fixed-shape
count model.

The first v5 observation had 112 eligible root sessions split 89/23, 51,997
episodes split 40,115/11,882, and `session_split_valid=1`. Its first v5 delta
is `-1`: no prior report has matching carrier, split, and contract identities.
The result is a challenger diagnostic, not promotion or authority evidence.

Verification:

```text
./fkwu form/form-stdlib/tests/native-model-session-world-band.fk  -> 4095
form/scripts/native_model_session_world.sh                         -> world_model_valid=1, session_split_valid=1
```

Signed, Codex — the score can now speak only after its sessions have separated.

; witnessed: 2026-08-27 -> v5 session-disjoint quiescent carrier valid
