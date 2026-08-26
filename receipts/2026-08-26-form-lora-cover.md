# 2026-08-26 — Trained overlay; remaining samples named honestly

Yes asked to train and to know how many more samples most
responses still want.

## Train, this sitting

Overlay fit on the lens cartesian. Rank-1 A/B reseeded into
gitignored `.form-lora-adapter`. `fqt-lora?` stays 0. Qwen GGUF
is not modified.

```
./fkwu form/form-stdlib/tests/form-cli-lora-cover-band.fk   # 1023
./fkwu form/form-stdlib/form-cli-lora-cover-run.fk
  lens tn=1   72/72   ppm=1000000
  lens tn=2   71/72   ppm=986111
  lens 12/4  258/288  ppm=895833
  lane tn=1  356/369  ppm=964769
  more-tn-most=0
  more-tn-local=-1
  lane-need-hits-990k=10
  session-ppm=894153
  session-need-events=95
  adapter=1
  fqt-lora=0
```

## What "most responses covered" is

- **Most (95%) on the lens tongue:** already met at the smallest N.
  Extra mill samples of the same mix did not help — they diluted
  the centroid (tn=2 and 12/4 both fell). Remaining mill N for
  this family is **0**. Climbing to 99% by adding more of the
  same cartesian is **-1**: the curve went the other way.
- **Lane routing (41 skills × 9 planes):** 96.5%. Ten more heldout
  *hits* would reach 99% of that mill. That is not the same as ten
  more sample rows.
- **This session's events:** 89.4% native+local. Ninety-five more
  events want to stay local to hit 99%. Those are remote planner
  turns, not missing mill templates.

Signed, Grok — sibling, this worktree.

; witnessed: 2026-08-26 -> lora-cover-band 1023, trained overlay, remaining mill N 0/-1, session 95 events, fqt-lora=0
