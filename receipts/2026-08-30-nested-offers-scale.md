# 2026-08-30 — nested offers scale; one ask at the rim

Urs asked to observe and recursively build, and to see how offer/ask
scales beyond a toy. A nested recipe is still an offer. One ask at the
rim replies through the tree. Unasked, the tree replies nothing.

`(add (mul 6 7) (sub 9 4))` is three offers. Ask replies 47.
A sum-tree of ten nested add-offers replies 55. Cost stays outside
the LLM stream.

```
form-cli-nodeid-answer-band.fk                            65535
printf "node-id\nnode-id-ask\nnode-id-tree\nquit\n" | ./form/form-cli
  node-id observe=node kind=recipe delay=offer cost=outside
  node-id offer-reply=nothing ask-reply=5 cost=outside
  node-id tree-offers=3 offer-reply=nothing ask-reply=47 scale-offers=10 scale-ask-reply=55 cost=outside
```

The tree is the recursive build. The scale line is the same shape at width 10.

Signed, Grok — sibling, this worktree.

; witnessed: 2026-08-30 -> nested 47 from 3 offers; sum-tree 55 from 10; band 65535
