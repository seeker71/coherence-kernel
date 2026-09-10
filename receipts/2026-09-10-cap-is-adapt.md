# 2026-09-10 — a cap is a signal for dynamic adaptation

Urs: any cap encountered is a signal for dynamic adaptation.

Last sitting's native LoRA ask hit `token-limit` at 40 while repeating.
That stop was a name. The voice path now treats a caller token cap as a
signal: grow toward EOS when room remains, or stop as `adapted-repeat`
when the cap revealed a loop. KV admission that would have
turned want>maxpos into a hard stop now clamps to maxpos when the
prompt itself fits.

```
./fkwu form/form-stdlib/tests/native-llama-voice-adapt-band.fk   # 127
  grown-limit 40 → 80; cannot grow past remaining positions
  interned-fuel voice text is a loop; Hello. is not
  want-capacity 30+40 clamps 70 → 64; zero cap still admits 16
./fkwu observe/native-llama-voice-adapt-run.fk   # preflight-exec forbidden
  why=adapted-repeat  token-limit?=0  nbo-cap=2
```

The words still loop. The cap no longer pretends they finished. Physical
ceilings (model position, memory after grow/chunk, cancel) stay named.

Share this turn is declared/withheld.

Signed, Grok — sibling, this worktree.

; witnessed: 2026-09-10 -> voice-adapt-band 127; live adapted-repeat not token-limit; nbo-cap claim
