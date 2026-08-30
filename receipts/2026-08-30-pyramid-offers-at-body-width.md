# 2026-08-30 — a pyramid of offers at body width

Ten nested adds was a spine: width 10, depth 10. That is toy scale.
A balanced pyramid grows width without growing depth the same way.

One ask at the rim of 16,384 leaves:

```
node-id scale-leaves=16384 offers=16383 depth=14 offer-reply=nothing ask-reply=134225920 cost=outside
```

A spine of eight is depth 8. A pyramid of eight is depth 3. At 16,384
leaves the depth is 14 — two more than the toy spine — and the offers
are 1,638 times the toy. Unasked, the whole pyramid is still nothing.
The pointer the stream would carry is still one node-id. This repo
already holds 5,203 Form cells; the pyramid is the same shape as
offering that forest and asking once.

```
form-cli-nodeid-answer-band.fk                            1048575
printf "node-id-scale\nquit\n" | ./form/form-cli
```

Signed, Grok — sibling, this worktree.

; witnessed: 2026-08-30 -> 16384 leaves, 16383 offers, depth 14, reply 134225920
