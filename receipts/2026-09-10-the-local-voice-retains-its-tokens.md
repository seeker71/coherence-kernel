# The local voice retains its tokens

Codex, 2026-09-10.

`nlg-emit` passed a scalar token ID to list concatenation. The text could grow
while the replay history stayed at the prompt. Under context pressure, the
native Llama voice therefore rebuilt attention from the wrong history.

The live emitter now uses `nlg-remember`, which appends a singleton list. The
new native history band first refused with eight failed checks and exit 1.
After the repair all ten checks passed, verdict 1023, exit 0. It covers zero,
repeated IDs, prompt immutability, 1024 emissions, the retained tail, and actual
two-layer Metal prefill: selected candidates and every retained key/value match
an independent expected-ID replay. All fixture buffers are released. This is
an execution fixture, not a language-quality benchmark or a whole live utterance.

Fresh preflight and execution also passed voice adaptation 65535, tokenizer 31,
native session coding 511, and session adapter embodiment 127. The standing
Glass ear frame was 34 ms old, sequence 262516, with 46 rows and ear.stands=1.
No microphone text was read. The worktree hearth answered no-standing-hearth;
source inspection supplied the repair. No C growth, foreign runtime, model
download or new resident was needed.

The verified teaching also completed a native Llama 3B learning round. Adam
continued at step 2 and updated 56 LoRA pairs; the candidate was saved privately.
Teaching loss moved 4.640418 -> 4.572080. The two held-out rows moved
2.024727 -> 2.032962 and 3.647666 -> 3.646079, so promotion remained zero.
The worker exited 0, with no pending rows. This is a persisted learning step,
not a serving adapter or a Qwen weight update. All 13 drift gates passed.

The surprise: the existing green adaptation band did not exercise emitted-ID
retention. The memory loss became a regression test that reaches actual
attention buffers. The exchange stayed alive by putting the correction in the
emitter, not leaving it only in this explanation. Repetition policy and the
loss of older prompt context under sliding remain separate work; this change
does not claim general autonomy or session equivalence.
